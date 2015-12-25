#include "Precompiled.h"
#include "Input/InputManager.h"
#include "Config/ConfigHelper.h"
#include "MessageInterface.h"
#include "Message/Dispatcher.h"
#include "STLHelper.h"
#include "StringHelper.h"
#include "Logger/Log.h"
#include "Asset/IAsset.h"
#include "File/AssetLoadSystem.h"
#include "File/VirtualFileSystem.h"
#include <tinyxml.h>
#include <algorithm>
#include <map>
#include <stack>
#include <string>
#include <vector>
#include <istream>
#include <ostream>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>

namespace eg {

namespace
{
    class KeyBindingsFactory;

    class KeyBinding : public IAsset
    {
    public:
        static const guid_t kType = 0xec14c2da;

        KeyBinding(uint64_t id, const std::string& name);

        // from IAsset
        virtual uint64_t GetId() const;
        virtual const std::string& Name() const;
        virtual const std::string& Fqn() const;
        virtual Locker_t Lock();
        virtual guid_t Type() const;
        virtual void HandleMessage(const Message &msg);

        const std::string& Source() const {return mSource;}

    private:
        uint64_t mId;
        std::string mName;
        std::string mSource;
        std::string mFqn;

        friend KeyBindingsFactory;
        void load(std::istream& stream);
        void save(std::ostream& stream) const;

        static const uint32_t version = 1;
    };

    KeyBinding::KeyBinding(uint64_t id, const std::string& name)
    : mId(id)
    , mName(name)
    {
    }


    uint64_t KeyBinding::GetId() const
    {
        return mId;
    }


    const std::string& KeyBinding::Name() const
    {
        return mName;
    }

    const std::string& KeyBinding::Fqn() const
    {
        return mFqn;
    }

    IAsset::Locker_t KeyBinding::Lock()
    {
        return Locker_t();
    }


    guid_t KeyBinding::Type() const
    {
        return KeyBinding::kType;
    }


    void KeyBinding::HandleMessage(const Message &msg)
    {
        msg;
    }

    void KeyBinding::save(std::ostream& stream) const
    {
        log_trace(tr_input) << "Saving '" << Name() << "' <KeyBinding> asset. version = " << KeyBinding::version;

        stream << std::noskipws;
        stream << mSource;
    }

    void KeyBinding::load(std::istream& stream)
    {
        mSource.clear();
        stream >> std::noskipws;
        std::copy(std::istream_iterator<char>(stream), std::istream_iterator<char>(), std::back_inserter(mSource));
    }


    //-------------------------------------------------------------------------------------
    // this handles key binding (action/input) file
    class KeyBindingsFactory
    {
        // no copy semantics
        KeyBindingsFactory(const KeyBindingsFactory&);
        KeyBindingsFactory& operator=(const KeyBindingsFactory&);

    public:
        KeyBindingsFactory(AssetLoadSystem& als)
        : als(als)
        {
            als.RegisterFactory(KeyBinding::kType, KeyBindingsFactory::New,
                                                   KeyBindingsFactory::Load,
                                                   KeyBindingsFactory::Save,
                                                   KeyBindingsFactory::Query);
        }

        ~KeyBindingsFactory()
        {
            als.UnregisterFactory(KeyBinding::kType);
        }

        static IAsset *New(const std::string& name, uint64_t asssetId)
        {
            IAsset *pAsset = new KeyBinding(asssetId, name);
            return pAsset;
        }

        static boost::function<void ()> assetLoaded;
        static void Load(AssetLoadSystem::IAsset_t asset, VirtualFileFactory::istream_t stream, const std::string& name)
        {
            name;
            boost::shared_ptr<KeyBinding> scriptAsset = boost::dynamic_pointer_cast<KeyBinding>(asset);
            if (scriptAsset)
            {
                if (stream && eg::stream::size(*stream))
                {
                    scriptAsset->load(*stream);
                    scriptAsset->mFqn = name;

                    assetLoaded();
                }
            }
        }

        static void Save(AssetLoadSystem::IAsset_t asset, VirtualFileFactory::ostream_t stream, const std::string& name)
        {
            name;
            // we can catch ex::serialize exception here if needed.
            // AssetLoadSystem does this already.
            boost::shared_ptr<KeyBinding> scriptAsset = boost::dynamic_pointer_cast<KeyBinding>(asset);
            if (scriptAsset && stream)
            {
                scriptAsset->save(*stream);
            }
        }

        static std::vector<std::string> Query(AssetLoadSystem::IAsset_t asset, const VirtualFileSystem& vfs)
        {
            vfs;
            const std::string& name = asset->Name();
            // here we control the order of iteration over streams for usage in Load(...) and Save(...) methods
            std::vector<std::string> files;
            std::string file = name;
            file += ".xml";
            files.push_back(file);
            return files;
        }

        AssetLoadSystem& als;
    };

    boost::function<void ()> KeyBindingsFactory::assetLoaded;

    class ISData;
    InputActionCallback_t DefaultInputActionCallback;


    //! This is returned to user which allows to clean up listener for input on exit
    class ActionListenerIml : public InputManager::ActionListener
    {
    public:
        ActionListenerIml(boost::signals::connection connection, const std::string& actionName, ISData *pInputData)
        : mConnection(connection)
        , mActionName(actionName)
        , mpInputData(pInputData)
        {
        }

        virtual ~ActionListenerIml();

    private:
        boost::signals::connection mConnection;
        std::string mActionName;
        ISData *mpInputData;

    };

    /*
    struct ActionScriptCallback
    {
        ActionScriptCallback()
        {
        }

        ActionScriptCallback(const std::string& name, bool useAsScript) :
            mName(name),
            mObjectNode((useAsScript ? "" : name), 0)
        {
        }

        void operator()(const std::string& actionName, uint32_t timeStamp, int32_t mouseX, int32_t mouseY) const
        {
            if (mObjectNode.GetNode())
            {
                wxASSERT_MSG(0, "Broken, ObjectMananger disabled");
                // we have object node to load
                //eg::GetObjectManager().CreateObject(mObjectNode.GetNode());
            }
            else
            {
                // load our script file and let it execute
            }
        };

        std::string mName;
        eg::ObjectIncludeNode mObjectNode;
    };
    */

    //! This is used to return true if key is down
    struct ActionKeyDownCallback
    {
        ActionKeyDownCallback() :
            mActionActive(new bool)
        {
            *(mActionActive.get()) = false;
        }

        void operator()(const std::string& actionName, uint32_t /*timeStamp*/, int32_t /*mouseX*/, int32_t /*mouseY*/) const
        {
            if (boost::algorithm::icontains(actionName, "$DOWN$"))
            {
                *(mActionActive.get()) = true;
            }
            else if (boost::algorithm::icontains(actionName, "$UP$"))
            {
                *(mActionActive.get()) = false;
            }
        };

        bool IsDown() const
        {
            return *(mActionActive.get());
        }

        // we use shared bool pointer here, because
        // function object is copied and we need to have access to
        // this bool value when instantiating this object for the first time
        mutable boost::shared_ptr<bool> mActionActive;
    };


    // handy defines for this file
    typedef InputManager::Input Input;
    typedef std::vector<Input *> vInput_t;
    typedef vInput_t::iterator it_vInput;
    typedef vInput_t::const_iterator cit_vInput;

    /*!
    One registered action by the user
    */
    class Action
    {
    public:
        Action(Input *Source, const char *name, const char *contextName, bool compareAsOr)
        : mHead(Source)
        , mCurrent(Source)
        , mValid(false)
        , mCompareAsOr(compareAsOr)
        , mName(name)
        , mTime(0)
        , mPos(0, 0)
        , mContextName(contextName ? contextName : "")
        {
        }

        ~Action()
        {
            delete mHead;
        }

        bool Test(const Input *input)
        {
            /*
            In some situation where there are several inputs generated in one frame
            and one of the first was correct, the next ones have ability
            to make this invalid input and we do not want that,
            since all the actions are reset every frame
            */
            if (mValid)
            {
                return true;
            }

            if (mCompareAsOr)
            {
                Input *currInput = mCurrent;
                do
                {
                    if (mCurrent->mType == input->mType)
                    {
                        if (input->Is(currInput))
                        {
                            makeValid(input);
                            break;
                        }
                    }

                    currInput = currInput->mNext;
                } while (currInput);
            }
            else
            {
                if (mCurrent->mType == input->mType)
                {
                    // we need to test if current is part or whole of the user Input
                    if (input->Is(mCurrent))
                    {
                        // if we have more inputs in the chain, go to next one
                        if (mCurrent->mNext)
                        {
                            mCurrent = mCurrent->mNext;
                        }
                        else
                        {
                            makeValid(input);
                        }
                    }
                    else
                    {
                        // this Input is the same type but different key, so let's
                        // reset the Current pointer (IOW, we are rewinding
                        // our Action to beginning state.
                        mValid = false;
                        mCurrent = mHead;
                    }
                }
            }

            return mValid;
        }

        //! Allows to trigger any callbacks attached to this action
        void Trigger()
        {
            mValid = true;
            // and use the time of that last Input as this Action time
            // \todo we need current time
            //mTime = input->mTimeStamp;
        }

        uint32_t Time() const {return mTime;}
        void Invalidate() {mValid = false;}
        const Vector2& Pos() const {return mPos;}

        bool IsValid(const std::string& currContextName) const
        {
            // short circuit this if valid is already true, but we need to
            // check our context
            if (mValid && !currContextName.empty() && !mContextName.empty())
            {
                return currContextName == mContextName;
            }

            return mValid;
        }

    private:
        void makeValid(const Input *input)
        {
            // otherwise, set Valid to TRUE, since we reached the end of chain
            mValid = true;
            // and use the time of that last Input as this Action time
            mTime = input->mTimeStamp;

            // \note what about updating mouse position regardless of what input type
            // we are processing. What we would need is global mouse cursor position, most
            // likely driven from main window/frame.
            if (InputManager::tMouse == input->mType)
            {
                mPos = (static_cast<const InputManager::Mouse *>(input))->mPos;
            }
            else
            {
                mPos.set(0, 0);
            }
        }

        Input *mHead;
        Input *mCurrent;
        bool mValid;
        //! if this is true, then compare current event with any of list inputs
        bool mCompareAsOr;
        //! Used in saving/loading Actions to/from file.
        std::string mName;
        uint32_t mTime; ///< At what time this Action became Valid.
        Vector2 mPos;
        //! which context this action is valid with
        std::string mContextName;
    };


    //! loosely based on the pimple pattern
    class ISData
    {
    public:
        typedef std::map<std::string, Action *> mAction_t;
        typedef mAction_t::iterator it_mAction;
        typedef mAction_t::const_iterator cit_mAction;

        ISData(Dispatcher& disp, AssetLoadSystem& als, const std::string fileKeyBinds, uint32_t currentTimeSlice = 0xFFFFFFFF)
        : mCurrentIndex(0)
        , mCurrentTimeSlice(currentTimeSlice)
        , mbProcessingInput(false)
        , mALS(als)
        , mKeyBindingsFactory(als)
        , mKeyBindings(fileKeyBinds, als)
        , mDispatcher(disp)
        {
            mKeyBindingsFactory.assetLoaded = boost::bind(&ISData::LoadConfigFiles, this);
            log_trace(tr_input) << "Created Input Device.";
        }

        ~ISData()
        {
            // unbind all actions which where auto loaded
            for (std::vector<std::string>::const_iterator it = mBindActions.begin(); it != mBindActions.end(); ++it)
            {
                UnregisterAction((*it).c_str());
            }

            DeleteClear(mPendingInputs);
            DeleteClear(mCurrentInputs);
            DeleteMapClear(mActionList);
        }

        void SetFilter(InputManager::Filter filter)
        {
            mInputFilter = filter;
        }

        void ProcessInput(InputManager::Input *input)
        {
            assert(input);
            if (mInputFilter)
            {
                if (!mInputFilter(*input))
                {
                    delete input;
                    return;
                }
            }
            if (LockPendingInputs())
            {
                /*
                struct Finder
                {
                    bool operator()(const Input *elem1) const
                    {
                        return *elem1 == *elem2;
                    }

                    Input *elem2;
                };
                // let see if we already have this same input
                Finder finder;
                finder.elem2 = input;
                vInput_t::const_iterator it = std::find_if(mPendingInputs.begin(), mPendingInputs.end(), finder);
                if (it == mPendingInputs.end())
                {
                    mPendingInputs.push_back(input);
                }
                */
                mPendingInputs.push_back(input);
                UnlockPendingInputs();
            }

            mDispatcher[mtype::kInput](Message(mtype::kInput, static_cast<const InputManager::Input *>(input)));
        }

        //! Called by the framework before any of the Tick(...) are called for the current frame.
        void Tick(float /*deltaTime*/)
        {
            mbProcessingInput = true;
            //float deltaTime  msg.GetValue<float>();
            // Time to clean up pending inputs:
            if (LockPendingInputs())
            {
                // grab all the pending inputs which are older in time then current time slice
                // into local lists std::string, CAction
                // copy and remove all the Input.mTimeStamp < mCurrentTimeSlice
                vInput_t pending(mPendingInputs);
                mPendingInputs.clear();
                UnlockPendingInputs();

                UpdateCurrentInputs(pending);
            }

            // now we are ready to update action list callbacks if there are any
            typedef std::map<std::string, eg::InputActionCallback_t>::const_iterator cit_ICL;
            for (cit_ICL it = mInputCallbackList.begin(); it != mInputCallbackList.end(); ++it)
            {
                const std::string& actionName = (*it).first;
                // if we have an action, let'c call callback
                if (IsAction(actionName.c_str()))
                {
                    Vector2 actionPos = ActionPos(actionName.c_str());
                    uint32_t actionTimeStamp = ActionTime(actionName.c_str());
                    (*it).second(actionName, actionTimeStamp, actionPos.x, actionPos.y);
                }
            }

            // walk over all of listeners and check to see if that action was triggered
            for (Listeners_t::const_iterator it = mListeners.begin(); it != mListeners.end(); ++it)
            {
                const std::string& actionName = (*it).first;
                if (IsAction(actionName.c_str()))
                {
                    Vector2 actionPos = ActionPos(actionName.c_str());
                    uint32_t actionTimeStamp = ActionTime(actionName.c_str());
                    (*(*it).second)(actionName, actionTimeStamp, actionPos.x, actionPos.y);
                }
            }

            mbProcessingInput = false;
            for (std::vector<std::string>::const_iterator it = mActionsToDelete.begin(); it != mActionsToDelete.end(); ++it)
            {
                UnregisterAction((*it).c_str());
            }
            mActionsToDelete.clear();

            for (std::vector<std::string>::const_iterator it = mListenersToDelete.begin(); it != mListenersToDelete.end(); ++it)
            {
                RemoveListener(*it);
            }
            mListenersToDelete.clear();
        }

        /*! Extract all the input which happen this frame. Call it once
        and then NextInput() until returns NULL. */
        void LockQue()
        {
            mCurrentIndex = 0;
            LockCurrentInputs();
        }

        //! Return next input in the queue and increment the queue counter.
        const InputManager::Input *NextInput()
        {
            if ( mCurrentIndex < (int)mCurrentInputs.size() )
            {
                Input *RequestedInput = mCurrentInputs[mCurrentIndex];
                mCurrentIndex++;
                return RequestedInput;
            }

            return 0;
        }

        void UnlockQue()
        {
            UnlockCurrentInputs();
        }

        //! Register user action by name
        void RegisterAction(const char *actionName, const Input *defaultInput, const char *contextName, eg::InputActionCallback_t inputActionCallback, bool compareAsOr)
        {
            // before we register new action, let's make sure that we do not have one yet.
            cit_mAction it = mActionList.find(actionName);
            // if we do not have Action, then let's create one
            // if the Action is already created, do nothing.
            // This also means that we can not change bind defaultInput to new one
            // without first Unregistering that Action.
            if (it == mActionList.end())
            {
                mActionList.insert(std::make_pair(actionName, new Action((Input *)defaultInput, actionName, contextName, compareAsOr)));

                if (inputActionCallback)
                {
                    mInputCallbackList.insert(std::make_pair(actionName, inputActionCallback));
                }
            }
            else
            {

            }
        }

        InputManager::ActionListener_t ListenForAction(const std::string& actionName, eg::InputActionCallback_t inputActionCallback)
        {
            Listeners_t::iterator it = mListeners.find(actionName);
            if (it == mListeners.end())
            {
                it = mListeners.insert(std::make_pair(actionName, new ListenerCallback_t)).first;
            }

            boost::signals::connection connection = (*it).second->connect(inputActionCallback);

            InputManager::ActionListener_t listenerHandle(new ActionListenerIml(connection, actionName, this));
            return listenerHandle;
        }

        void RemoveListener(const std::string& actionName)
        {
            if (mbProcessingInput)
            {
                mListenersToDelete.push_back(actionName);
            }
            else
            {
                // we need to check to see if there are any other listeners attached
                // and if not, then we remove entire entry for it
                //mListeners.erase(actionName);
                Listeners_t::iterator it = mListeners.find(actionName);
                if (it != mListeners.end())
                {
                    if ((*it).second->empty())
                    {
                        mListeners.erase(it);
                    }
                }
                else
                {
                    log_error << "Trying to remove listener '" << actionName << "' but it does not exist.";
                }
            }
        }

        //! Will remove action Name from this UserInput registered Actions.
        void UnregisterAction(const char *actionName)
        {
            if (mbProcessingInput)
            {
                mActionsToDelete.push_back(actionName);
            }
            else
            {
                cit_mAction it = mActionList.find(actionName);
                if (it != mActionList.end())
                {
                    delete it->second;
                    mActionList.erase(actionName);
                }
                std::map<std::string, eg::InputActionCallback_t>::iterator itc = mInputCallbackList.find(actionName);
                if (itc != mInputCallbackList.end())
                {
                    mInputCallbackList.erase(actionName);
                }
            }
        }

        //! Trigger any callbacks attached to this action
        //! \note this method returns right a way and trigger action will
        //! not happen to the end of frame.
        void TriggerAction(const char *actionName)
        {
            cit_mAction it = mActionList.find(actionName);
            if (it != mActionList.end())
            {
                // we have this action, let's make it
                // to generate trigger.
                Action *pAction = (*it).second;
                pAction->Trigger();
            }
        }

        //! Will return TRUE if User Input matched this actionNamemNext in the last frame.
        bool IsAction(const char *actionName) const
        {
            // first we need to check to see if we key repeat input requested
            std::map<std::string, ActionKeyDownCallback>::const_iterator cit = mInputKeyDownList.find(actionName);
            if (cit != mInputKeyDownList.end())
            {
                return (*cit).second.IsDown();
            }

            cit_mAction it = mActionList.find(actionName);
            // we find the type, so let's just return the handle
            if (it != mActionList.end())
            {
                // extract current context name if any
                std::string currContextName;
                if (!mContextStack.empty())
                {
                    currContextName = mContextStack.top();
                }

                return (*it).second->IsValid(currContextName);
            }

            return false;
        }

        uint32_t ActionTime(const char *actionNameNext) const
        {
            cit_mAction it = mActionList.find(actionNameNext);
            // we find the type, so let's just return the handle
            if (it != mActionList.end())
            {
                return (*it).second->Time();
            }

            return 0;
        }

        Vector2 ActionPos(const char *actionNameNext) const
        {
            cit_mAction it = mActionList.find(actionNameNext);
            // we find the type, so let's just return the handle
            if (it != mActionList.end())
            {
                return (*it).second->Pos();
            }

            return v2::ZERO();
        }

        void PushContext(const std::string& newContextName)
        {
            mContextStack.push(newContextName);
        }

        std::string PopContext()
        {
            std::string topOfContext;
            if (!mContextStack.empty())
            {
                topOfContext = mContextStack.top();
                mContextStack.pop();
            }

            return topOfContext;
        }

        int MapKey(int value) const
        {
            // this will translate OS generated key code
            // into our own Input types
            std::map<int, int>::const_iterator it = mKeyMap.find(value);
            if (it != mKeyMap.end())
            {
                // we do allow specifying -1 as to use the original OS value
                if ((*it).second != -1)
                {
                    return (*it).second;
                }
            }

            return value;
        }

        void LoadBindings(const std::string& name)
        {
            mKeyBindings *= name;
            //LoadConfigFiles();
        }

    private:
        //! Load all the configuration files needed by input system
        void LoadConfigFiles()
        {
            mBindActions.clear();
            mKeyMap.insert(std::make_pair(38, 255));
            mKeyMap.insert(std::make_pair(40, 254));
            mKeyMap.insert(std::make_pair(39, 253));
            mKeyMap.insert(std::make_pair(37, 252));

            //mKeyBindings = "KeyBindings";


            /*
            <KeyBindings>
                <!-- name required, context optional -->
                <Action name="Camera.Move" context="">
                    <device>mouse</device>          <!-- required (mouse, key) -->
                    <modifiers>ctrl</modifiers>     <!-- optional (ctrl, shift, alt) -->
                    <type>move</type>               <!-- required (move, up, down) -->
                    <button>left</button>           <!-- optional (left, right, middle) -->
                </Action>

                <!-- name required, context optional -->
                <Action name="Camera.Type.Next" context="">
                    <device>key</device>            <!-- required (mouse, key) -->
                    <modifiers>ctrl</modifiers>     <!-- optional (ctrl, shift, alt) -->
                    <type>down</type>               <!-- required (move, up, down) -->
                    <key>13</key>                   <!-- required (ascii value) -->
                </Action>
            </KeyBindings>

            //logs_not_impl("Init/Input/KeyBindings");
            std::string keyBindingsFileName; //= eg::config::ReadString("Init/Input/KeyBindings");
            //eg::config::ExpandMacros(keyBindingsFileName);
            if (keyBindingsFileName.empty())
            {
                return;
            }
            */

            TiXmlDocument doc(mKeyBindings->Name());
            if (doc.Parse(mKeyBindings->Source().c_str()))
            {
                //wxLogTrace(tr_util, "KeyBindings '%s' file loaded and processing...", keyBindingsFileName);
                TiXmlHandle docHandle(&doc);
                int i=0;
                while (true)
                {
                    TiXmlElement *child = docHandle.FirstChild("KeyBindings").Child("Action", i).ToElement();
                    if (!child)
                    {
                        break;
                    }
                    ++i;

                    TiXmlHandle actionHandle(child);
                    const char *pName = child->Attribute("name");
                    const char *pContext = child->Attribute("context");
                    if (TiXmlElement *xmlNode = actionHandle.FirstChildElement("device").ToElement())
                    {
                        TiXmlText *nodeType = actionHandle.FirstChildElement("type").FirstChild().ToText();
                        assert(nodeType);
                        uint32_t inputType = xmlConvertType(nodeType);
                        assert(inputType);

                        TiXmlText *nodeModifiers = actionHandle.FirstChildElement("modifiers").FirstChild().ToText();
                        uint32_t modifiersKey = xmlConvertModifiers(nodeModifiers);
                        inputType |= modifiersKey;

                        const std::string& device = xmlNode->FirstChild()->Value();
                        if (boost::iequals(device, "mouse"))
                        {
                            TiXmlText *nodeButton = actionHandle.FirstChildElement("button").FirstChild().ToText();
                            uint32_t button = xmlConvertButton(nodeButton);

                            InputManager::Mouse *pInput = new InputManager::Mouse(inputType);
                            if (button)
                            {
                                pInput->mButtons[button] = true;
                            }

                            UnregisterAction(pName);
                            RegisterAction(pName, pInput, pContext, DefaultInputActionCallback, false);
                            log_trace(tr_input) << "Registered Action '" << pName << "' with context '" << (IsNull(pContext) ? "None" : pContext) << "' using Mouse as an initial binding.";
                            log_trace(tr_input) << "   Type '" << nodeType->Value() << "', Modifiers '" << (nodeModifiers ? nodeModifiers->Value() : "None") << "' Buttons '" << (nodeButton ? nodeButton->Value() : "None") << "'.";
                        }
                        else if (boost::iequals(device, "key"))
                        {
                            TiXmlText *nodeKey = actionHandle.FirstChildElement("key").FirstChild().ToText();
                            const std::string& keyValues = nodeKey->ValueStr();
                            /*
                            InputManager::Key *pInput = 0;
                            for (std::string::const_iterator it = keyValues.begin(); it != keyValues.end(); ++it)
                            {
                                uint32_t keyValue = boost::lexical_cast<uint32_t>(*it);
                                if (pInput)
                                {
                                    InputManager::Input *lastInput = pInput;
                                    while (lastInput->mNext)
                                    {
                                        lastInput = lastInput->mNext;
                                    }

                                    lastInput->mNext = new InputManager::Key(inputType, keyValue);
                                }
                                else
                                {
                                    pInput = new InputManager::Key(inputType, keyValue);
                                }
                            }
                            */

                            bool multiKey = false;

                            uint32_t keyValue = boost::lexical_cast<uint32_t>(keyValues);
                            InputManager::Key *pInput = new InputManager::Key(inputType, keyValue);
                            if (keyValue >=65 && keyValue <=90)
                            {
                                multiKey = true;
                                InputManager::Key *smallKey = new InputManager::Key(inputType, keyValue+32);
                                pInput->mNext = smallKey;
                            }

                            UnregisterAction(pName);
                            RegisterAction(pName, pInput, pContext, DefaultInputActionCallback, multiKey);
                            log_trace(tr_input) << "Registered Action '" << pName << "' with context '" << (IsNull(pContext) ? "None" : pContext) << "' using Key as an initial binding.";

                            std::string charValue = nodeKey ? nodeKey->Value() : "";

                            int asciiValue = boost::lexical_cast<int>(charValue);
                            char letter = (char)asciiValue;
                            log_trace(tr_input) << "   Type '" << nodeType->Value() << "', Modifiers '" << (nodeModifiers ? nodeModifiers->Value() : "None") << "' Key (" << asciiValue << "/'" << letter << "').";
                        }
                        else
                        {
                            log_error << "Invalid device type '" << device << "' in '" << pName << "' Action in '" << mKeyBindings->Name() << "' file.";
                        }

                        mBindActions.push_back(pName);
                    }
                    else
                    {
                        log_error << "There is no device section in '" << pName << "' Action in '" << mKeyBindings->Name() << "' file.";
                    }
                }

                log_trace(tr_input) << "KeyBindings '" << mKeyBindings->Name() << "' file processed with " << mBindActions.size() << " bindings.";
            }
            else
            {
                log_error << "Could not load key bindings file: \n" << mKeyBindings->Name() << "(" << doc.ErrorRow() << ") : error: " <<  doc.ErrorDesc();
            }
        }

        static uint32_t xmlConvertModifiers(const TiXmlText *pNode)
        {
            const std::string& value = pNode ? pNode->Value() : "";
            // \note do we need to provide ability to have multiple modifiers
            // 'ctrl, shift'
            if (value == "ctrl")
            {
                return InputTypes::kButtonCtrl;
            }
            if (value == "shift")
            {
                return InputTypes::kButtonShift;
            }
            if (value == "alt")
            {
                return InputTypes::kButtonAlt;
            }

            return 0;
        }

        static uint32_t xmlConvertType(const TiXmlText *pNode)
        {
            const std::string& value = pNode ? pNode->Value() : "";
            if (value == "up")
            {
                return InputTypes::kButtonUp;
            }
            if (value == "down")
            {
                return InputTypes::kButtonDown;
            }
            if (value == "move")
            {
                return InputTypes::kMouseMove;
            }

            log_error << "xmlConvertType did not have valid Value: '" << value << "'.";
            return 0;
        }

        static uint32_t xmlConvertButton(const TiXmlText *pNode)
        {
            const std::string& value = pNode ? pNode->Value() : "";
            if (value == "left")
            {
                return InputTypes::kMouseLeft;
            }
            if (value == "right")
            {
                return InputTypes::kMouseRight;
            }
            if (value == "middle")
            {
                return InputTypes::kMouseMiddle;
            }

            log_error << "xmlConvertType did not have valid Value: '" << value << "'.";
            return 0;
        }

        // this is our key map translation
        // where first is OS generated key
        // and second is our Input type key
        std::map<int, int> mKeyMap;

        class TestActionFO
        {
        public:
            TestActionFO(const Input *source) : mSource(source) {}

            void operator()(std::pair<std::string, Action *> actionEntry)
            {
                // m_Source is the actual key generated by the user, so we testing
                // each Action against it.
                actionEntry.second->Test(mSource);
            }

        private:
            const Input *mSource;
        };

        void operator=(const ISData& source);

        bool LockPendingInputs()
        {
            //! \todo Add code to lock the PendingInput list.
            return true;
        }

        void UnlockPendingInputs()
        {
            //! \todo Add code to unlock the PendingInput list.
        }

        bool LockCurrentInputs()
        {
            return true;
        }

        void UnlockCurrentInputs()
        {
        }

        void UpdateCurrentInputs(vInput_t& inputSource)
        {
            LockCurrentInputs();

            // delete all the current Inputs first from the Current Input list.
            DeleteClear(mCurrentInputs);
            // copy all the Inputs in the InputSource to our Current list of inputs
            std::copy(inputSource.begin(), inputSource.end(), std::back_inserter(mCurrentInputs));

            // now, we need to make sure that all Actions are reset before we test it for current input.
            std::for_each(mActionList.begin(), mActionList.end(), ResetValidActionFlag);
            it_vInput it_curr = mCurrentInputs.begin();
            while (it_curr != mCurrentInputs.end())
            {
                std::for_each(mActionList.begin(), mActionList.end(), TestActionFO(*it_curr));
                ++it_curr;
            }

            UnlockCurrentInputs();
        }

        mAction_t mActionList;              // all user registered action map
        vInput_t mPendingInputs;            // pending input accumulated from os source
        vInput_t mCurrentInputs;            // current input list for this frame time slice
        int mCurrentIndex;                  // used with NextInput() method
        const uint32_t mCurrentTimeSlice;   // grab all the inputs which where generated before this time
                                            // it defaults to 0xFFFFFFFF which disregards the time test

        std::stack<std::string> mContextStack;

        static void ResetValidActionFlag(std::pair<std::string, Action *> actionEntry)
        {
            actionEntry.second->Invalidate();
        }

        // callback for registered input actions
        std::map<std::string, eg::InputActionCallback_t> mInputCallbackList;
        // used to keep track if key is down or not
        std::map<std::string, ActionKeyDownCallback> mInputKeyDownList;
        //! List of actiosn to delete on the next OnTimer call
        std::vector<std::string> mActionsToDelete;
        std::vector<std::string> mListenersToDelete;
        //! This is set to true, when OnTimer is called and garbage collection is differed
        volatile bool mbProcessingInput;

        typedef boost::signal<void (const std::string& /*actionName*/, uint32_t /*timeStamp*/, uint32_t /*mouseX*/, uint32_t /*mouseY*/)> ListenerCallback_t;
        typedef std::map<std::string, boost::shared_ptr<ListenerCallback_t> > Listeners_t;

        Listeners_t mListeners;

        //! All bind actions created from configuration key binding xml file
        std::vector<std::string> mBindActions;
        AssetLoadSystem& mALS;
        KeyBindingsFactory mKeyBindingsFactory;
        hAsset<KeyBinding, lod_type_bloc> mKeyBindings;
        InputManager::Filter mInputFilter;
        Dispatcher& mDispatcher;
    };


    ActionListenerIml::~ActionListenerIml()
    {
        mConnection.disconnect();
        mpInputData->RemoveListener(mActionName);
    }

} // namespace


InputManager::InputManager(Dispatcher& disp, AssetLoadSystem& als, const std::string& fileKeyBinds)
: mData(new ISData(disp, als, fileKeyBinds))
, mMousePosition(v2::ZERO())
{
}


InputManager::~InputManager()
{
    delete mData;
}


void InputManager::SetFilter(Filter filter)
{
    mData->SetFilter(filter);
}


void InputManager::Tick(float deltaTime)
{
    mData->Tick(deltaTime);
}


void InputManager::LoadBindings(const std::string& name)
{
    mData->LoadBindings(name);
}


void InputManager::RegisterAction(const char *actionName, const Input *defaultInput, const char *contextName, bool compareAsOr)
{
    RegisterAction(actionName, defaultInput, contextName, DefaultInputActionCallback, compareAsOr);
}


void InputManager::RegisterAction(const char *actionName, const Input *defaultInput, const char *contextName, InputActionCallback_t inputActionCallback, bool compareAsOr)
{
    if (actionName && defaultInput)
    {
        mData->RegisterAction(actionName, defaultInput, contextName, inputActionCallback, compareAsOr);
    }
}


void InputManager::RegisterAction(const char *actionName, const unsigned char value, const char *contextName)
{
    RegisterAction(actionName, value, contextName, DefaultInputActionCallback);
}


void InputManager::RegisterAction(const char *actionName, const unsigned char value, const char *contextName, InputActionCallback_t inputActionCallback)
{
    RegisterAction(actionName, new InputManager::Key(0, InputTypes::kButtonUp, value), contextName, inputActionCallback, false);
}


InputManager::ActionListener_t InputManager::ListenForAction(const std::string& actionName, InputActionCallback_t inputActionCallback)
{
    return mData->ListenForAction(actionName, inputActionCallback);
}

void InputManager::TriggerAction(const char *actionName)
{
    if (actionName)
    {
        mData->TriggerAction(actionName);
    }
}


void InputManager::UnregisterAction(const char *actionName)
{
    if (actionName)
    {
        return mData->UnregisterAction(actionName);
    }
}


void InputManager::PushContext(const std::string& newContextName)
{
    mData->PushContext(newContextName);
}


std::string InputManager::PopContext()
{
    return mData->PopContext();
}


bool InputManager::IsAction(const char *actionName) const
{
    if (actionName)
    {
        return mData->IsAction(actionName);
    }

    return false;
}

uint32_t InputManager::ActionTime(const char *actionName) const
{
    if (actionName)
    {
        return mData->ActionTime(actionName);
    }

    return 0;
}


Vector2 InputManager::ActionPos(const char *actionName) const
{
    if (actionName)
    {
        return mData->ActionPos(actionName);
    }

    return v3::ZERO();
}


void InputManager::ProcessInput(Input *input)
{
    if (input)
    {
        mData->ProcessInput(input);
    }
}


void InputManager::LockQue()
{
    mData->LockQue();
}

void InputManager::UnlockQue()
{
    mData->UnlockQue();
}

const InputManager::Input *InputManager::NextInput()
{
    return mData->NextInput();
}


int InputManager::MapKey(int value) const
{
    return mData->MapKey(value);
}

void InputManager::SetCurrentMousePos(const Vector2& pos)
{
    mMousePosition = pos;
}

const Vector2& InputManager::GetCurrentMousePos() const
{
    return mMousePosition;
}


/*
const char *InputManager::Input::Print() const
{
    gTextBuffer[0] ='\0';
    int shift = mFlags & InputTypes::kButtonShift; if ( shift ) shift = 'S'; else shift = 's';
    int ctrl = mFlags & InputTypes::kButtonCtrl; if ( ctrl ) ctrl = 'C'; else ctrl = 'c';
    int caps = mFlags & InputTypes::kButtonCaps; if ( caps ) caps = 'L'; else caps = 'l';
    int down = mFlags & InputTypes::kButtonDown; if ( down ) down = 'D'; else down = 'd';
    int numlock = mFlags & InputTypes::kButtonNumLock; if ( numlock ) numlock ='N'; else numlock = 'n';

    gTypeName = "Input";
    if (tKey == mType)
        gTypeName = "Key     ";
    else if (tMouse == mType)
        gTypeName = "Mouse   ";
    else if (tJoyPad == mType)
        gTypeName = "JoyPad  ";
    else if (tJoystick == mType)
        gTypeName = "Joystick";
    else if (tCustum == mType)
        gTypeName = "Custom  ";

    sprintf(gTextBuffer, ":%8u: '%s', F: [%8X] [%c%c%c%c%c] ",mTimeStamp, gTypeName.c_str(), mFlags, shift, ctrl, caps, down, numlock);
    return gTextBuffer;
}

const char *InputManager::Key::Print() const
{
    Input::Print();
    char *CurrPtr = gTextBuffer + strlen(gTextBuffer);
    sprintf(CurrPtr, " V: '%c', [%3d]", Value, Value);
    return gTextBuffer;
}

const char *InputManager::Mouse::Print() const
{
    Input::Print();
    char *CurrPtr = gTextBuffer + strlen(gTextBuffer);
    //int move = mFlags&IS_MOUSE_MOVE; if ( move )  move = 1;
    int l = mButtons[InputTypes::kMouseLeft]; if ( l ) l = 1;
    int r = mButtons[InputTypes::kMouseRight]; if ( r ) r = 1;
    int m = mButtons[InputTypes::kMouseMiddle]; if ( m ) m = 1;

    sprintf(CurrPtr, " P: (%4d, %4d), B: [%d%d%d]", (int)mPos.x, (int)mPos.y, l, m, r);
    return gTextBuffer;
}

const char *InputManager::JoyPad::Print() const
{
    Input::Print();
    return gTextBuffer;
}
*/

} // namespace eg
