// Pong game main entry point
#include "Precompiled.h"
#include "Main.h"
#include "Logger/Log.h"
#include "Timer/ClockManager.h"
#include "IdGameCache.h"
#include "Registrate.h"
#include <set>
#include <queue>
#include <boost/foreach.hpp>
#pragma warning(push)
#pragma warning (disable : 4244)  // '' : conversion from '' to ''
#include <boost/thread/condition.hpp>
#include <boost/thread/thread.hpp>
#pragma warning(pop)
#include "vld.h"

using namespace eg;

namespace eg
{
    ClockManager clock;
    typedef std::stringstream sstring;
} // namespace eg

namespace
{
    void sleep(uint32_t miliseconds)
    {
        boost::xtime xt;
        boost::xtime_get(&xt, boost::TIME_UTC);
        xt.nsec += miliseconds * 1000000.0;
        boost::thread::sleep(xt);
    }

    void sleep(double seconds)
    {
        uint32_t duration = static_cast<uint32_t>(seconds * 1000.0);
        sleep(duration);
    }

    void busy(double seconds)
    {
        double duration = eg::clock.GetRealTime() + seconds;
        while (eg::clock.GetRealTime() < duration) {}
    }

    struct task_timer
    {
        task_timer(const ClockManager& clock, const char *msg)
        : mClock(clock)
        , mStartTime(clock.GetRealTime())
        , mMsg(msg)
        {}

        task_timer(const ClockManager& clock, const std::string& msg)
            : mClock(clock)
            , mStartTime(clock.GetRealTime())
            , mMsg(0)
            , mText(msg)
        {}

        ~task_timer()
        {
            if (mMsg || !mText.empty())
            {
                double delta = duration();
                log_trace("timer") << "Finished [" << (mMsg ? mMsg : mText) << "]. It took: " << delta << " seconds.";
            }
        }

        double duration() const {return mClock.GetRealTime() - mStartTime;}

    private:
        task_timer(const task_timer&);
        task_timer& operator =(const task_timer&);

        const ClockManager& mClock;
        double mStartTime;
        const char *mMsg;
        std::string mText;
    };

    std::vector<std::string> getTraceModules()
    {
        std::vector<std::string> traceModules;
        //traceModules.push_back(tr_util);
        traceModules.push_back("main_thread");
        traceModules.push_back("timer_thread");
        traceModules.push_back("timer");
        traceModules.push_back("save");
        traceModules.push_back("sleep");
        return traceModules;
    }
    logs::LoggerInit loggerInit("", logs::kDest_file|logs::kDest_cout|logs::kDest_debug, 0, true, getTraceModules());

    struct Node
    {
        Node(uint64_t id, uint64_t root_id) : id(id), root_id(root_id) {}
        Node() : id(0), root_id(0) {}
        uint64_t id;
        uint64_t root_id;

        bool operator <(const Node& node) const
        {
            return id < node.id;
        }
    };

    const double kInterval = 20;


    struct ChangelistQueue
    {
        typedef std::map<uint64_t, std::set<Node> > Areas_t;
        typedef std::pair<uint64_t, double> AreaTimer_t;

        ChangelistQueue()
        : mQuit(false)
        , mExit(false)
        , mLastTimerValue(0)
        {
        }

        ~ChangelistQueue()
        {
            stop();
            boost::mutex::scoped_lock lock(the_mutex);
            if (!mExit)
            {
                exit_condition_variable.wait(lock);
            }

            log_trace("timer") << "Changelist stopped";
        }

        void stop()
        {
            log_trace("timer") << "Changelist stop requested: exit: " << logs::boolean(mExit) << ", quit: " << logs::boolean(mQuit);
            boost::mutex::scoped_lock lock(the_mutex);
            if (!mExit && !mQuit)
            {
                mQuit = true;
                the_condition_variable.notify_one();
                lock.unlock();
            }
        }

        void operator()()
        {
            log_trace("timer") << "Executing Changelist thread function";

            while (true)
            {
                {
                    boost::mutex::scoped_lock lock(the_mutex);
                    if (mChangedAreas.empty())
                    {
                        // we need to check for quit request
                        // which this allows is to let all jobs finished
                        if (mQuit)
                        {
                            log_trace("timer") << "Changelist Quiting...";
                            mExit = true;
                            exit_condition_variable.notify_one();
                            return;
                        }
                        log_trace("timer") << "Changelist sleeping...";
                        the_condition_variable.wait(lock);
                        log_trace("timer") << "Changelist wakup";
                    }
                }

                // --- lock block
                boost::mutex::scoped_lock lock(the_mutex);
                if (mChangedAreas.empty())
                {
                    continue;
                }

                //task_timer taskTimer(eg::clock, "Changelist Execution");
                AreaTimer_t areaTimer = mChangedAreas.front();
                if (timeExpired(areaTimer) || mQuit)
                {
                    mChangedAreas.pop();
                    Areas_t::iterator it = mChaneglist.find(areaTimer.first);
                    if (it != mChaneglist.end())
                    {
                        mCurrentTransactions[areaTimer.first] = (*it).second;
                        mChaneglist.erase(it);
                    }
                    else
                    {
                        log_error << "There is no record for areaId: " << areaTimer.first << " in changelist.";
                    }

                    const std::set<Node>& nodeList = mCurrentTransactions[areaTimer.first];
                    lock.unlock();
                    // --- lock block

                    //log_trace("timer") << "Time Expired AreaId: " << areaTimer.first;

                    std::set<uint64_t> files;
                    for (std::set<Node>::const_iterator it = nodeList.begin(); it != nodeList.end(); ++it)
                    {
                        files.insert((*it).id % 100);
                    }
                    saveAreaNodes(nodeList, files);

                    // --- lock block
                    lock.lock();
                    mCurrentTransactions.erase(areaTimer.first);
                    // --- lock block
                }
                else
                {
                    //lock.unlock();
                    // we need to kick sleep in
                    double more_sleep = areaTimer.second - eg::clock.GetRealTime();
                    double start_sleep = eg::clock.GetRealTime();

                    if (more_sleep > 0)
                    {
                        the_condition_variable.timed_wait(lock, boost::posix_time::milliseconds(more_sleep * 1000.0));
                        log_trace("sleep") << "Slept for (current/requested) " << eg::clock.GetRealTime() - start_sleep << "/" << more_sleep << " for AreaId: " << areaTimer.first << ". Quit: " << logs::boolean(mQuit) << ", Exit: " << logs::boolean(mExit);
                    }
                }
            }
        }

        std::set<Node> findChangelist(uint64_t areaId) const
        {
            boost::mutex::scoped_lock lock(the_mutex);
            Areas_t::const_iterator it = mCurrentTransactions.find(areaId);
            if (it != mCurrentTransactions.end())
            {
                return (*it).second;
            }

            it = mChaneglist.find(areaId);
            if (it != mChaneglist.end())
            {
                return (*it).second;
            }

            return std::set<Node>();
        }

        void push(const std::set<Node>& nodes)
        {
            //task_timer taskTimer(eg::clock, "Adding Node batches");
            size_t counter = 0;
            boost::mutex::scoped_lock lock(the_mutex);
            BOOST_FOREACH(const Node& node, nodes)
            {
                counter += push_node(node) ? 1 : 0;
            }
            if (counter)
            {
                log_trace("timer") << "New Change AreaId: " << (*nodes.begin()).root_id << " with " << nodes.size() << " nodes into queue.";
                the_condition_variable.notify_one();
            }
        }

        void push(const Node& node)
        {
            boost::mutex::scoped_lock lock(the_mutex);
            if (push_node(node))
            {
                log_trace("timer") << "New Change AreaId: " << node.root_id << " for NodeId: " << node.id;
                the_condition_variable.notify_one();
            }
        }

    private:
        void saveAreaNodes(const std::set<Node>& nodeList, const std::set<uint64_t>& files)
        {
            files;
            sstring msg;
            msg << "Saving Area Nodes: " << (*nodeList.begin()).root_id << " with " << nodeList.size() << " changed nodes";
            task_timer taskTimer(eg::clock, msg.str());
            BOOST_FOREACH(const Node& node, nodeList)
            {
                //if (node.root_id == 200)
                //{
                //    busy(0.0001);
                //}
            }
        }

        bool timeExpired(const AreaTimer_t& areaTimer) const
        {
            //double delta = areaTimer.second - eg::clock.GetRealTime();
            //log_trace("sleep") << "TimeExpired " << logs::boolean(delta < 0) << ", delta = " << delta;
            return areaTimer.second < eg::clock.GetRealTime();
        }

        bool push_node(const Node& node)
        {
            bool newEdit = false;
            Areas_t::iterator it = mChaneglist.find(node.root_id);
            if (it == mChaneglist.end())
            {
                it = mChaneglist.insert(std::make_pair(node.root_id, std::set<Node>())).first;
                // trigger timer for this area
                newEdit = true;
            }

            std::set<Node>& changes = (*it).second;

            std::set<Node>::iterator it_n = changes.find(node);
            if (it_n == changes.end())
            {
                changes.insert(node); // potential node copy
            }
            else
            {
                // there is already change of this node
                // so we need to combine it together
                // add, change (replace), dirty (combine fields), remove (set flag only)
                Node& newNode = *it_n;
                newNode = node;
            }

            if (newEdit)
            {
                bool empty = mChangedAreas.empty();
                double currtime = eg::clock.GetRealTime();
                if (mLastTimerValue > 0)
                {
                    // let's check to see if current changelist request is too close
                    // to current time
                    if (currtime < mLastTimerValue + kInterval)
                    {
                        currtime = mLastTimerValue;
                    }
                }

                double wakeupTime = currtime + kInterval;
                mLastTimerValue = wakeupTime;
                log_trace("timer") << "New Changed Area: " << node.root_id << ", CurrTime: " << currtime << ", wakupTime: " << wakeupTime;
                mChangedAreas.push(std::make_pair(node.root_id, wakeupTime));
                if (empty)
                {
                    return true;
                }
            }

            return false;
        }

        // which area has changes and when to save it
        std::queue<AreaTimer_t> mChangedAreas;
        Areas_t mChaneglist;
        Areas_t mCurrentTransactions;
        bool mQuit;
        bool mExit;
        double mLastTimerValue;

        mutable boost::mutex the_mutex;
        boost::condition the_condition_variable;

        boost::condition exit_condition_variable;
    };
} // namespace


int main()
{
    log_trace("main_thread") << "Starting thread test...";
    IdGameCache idCache;
    eg::clock.GetRealTime();

    ChangelistQueue changelistQueue;

    boost::thread changelistThread(boost::ref(changelistQueue));

    for (int j = 1; j < 4; j++)
    {
        std::set<Node> nodes;
        for (int i = 0; i < 10000; i++)
        {
            Node node(idspace::get_burnable(idCache), j * 100);
            nodes.insert(node);
        }
        changelistQueue.push(nodes);
        busy(1);
    }

    log_trace("main_thread") << "Added all nodes.";

    /*
    Node sNode(123456789, 200);
    changelistQueue.push(Node(idspace::get_burnable(idCache), 200));
    changelistQueue.push(Node(idspace::get_burnable(idCache), 200));
    changelistQueue.push(Node(idspace::get_burnable(idCache), 200));
    changelistQueue.push(Node(idspace::get_burnable(idCache), 200));
    changelistQueue.push(Node(idspace::get_burnable(idCache), 200));
    */
    //changelistQueue.stop();
    task_timer taskTimer(eg::clock, "Shutting down");
    //busy(60);
    changelistQueue.stop();

    //changelistThread.join();

    log_trace("main_thread") << "Finished thread test.";
}