// Pong game main entry point
#include "Main.h"
#include "Logger/Log.h"
#include "Timer/Clock.h"
//#include "Registrate.h"
#include "App/AppUtilities.h"
#include "File/VirtualFileSystem.h"
#include "File/DiskFileFactory.h"
#include "File/AssetLoadSystem.h"
#include "Script/ScriptAsset.h"
#include "IdGameCache.h"
#include "Input/InputManager.h"
#include "Message/Dispatcher.h"
#include "App/MainConsole.h"
#include "Profiler/Profiler.h"
#include "Plugin/PluginManager.h"
#include "Plugin/IPluginObject.h"

#include "ObjectID.h"
#include <boost/tokenizer.hpp>

#include <boost/filesystem.hpp>
#include "vld.h"


using namespace eg;
namespace bfs = boost::filesystem;


std::vector<std::string> getTraceModules()
{
    std::vector<std::string> traceModules;
    traceModules.push_back(tr_script);
    traceModules.push_back(tr_plugin);
    //traceModules.push_back(tr_input);
    //traceModules.push_back(tr_asset);
    traceModules.push_back("main");
    return traceModules;
}
logs::LoggerInit loggerInit("", logs::kDest_file|logs::kDest_debug, 0, true, getTraceModules());

namespace eg {

extern void ConsoleLoop(InputManager& input, ClockManager& clock, boost::function<void (const Clock&)> callback);

} // namespace eg


namespace
{
    void on_vfs(VirtualFileSystem& vfs, const std::string& assetPath)
    {
        config::VirtualFileSetting setting;
        setting.folders.push_back(assetPath);

        setting.type = "lua";
        config::AddVirtualFile<DiskFileFactory>(vfs, setting);

        setting.type = "xml";
        config::AddVirtualFile<DiskFileFactory>(vfs, setting);
    }

    class Ups
    {
    public:
        Ups(const std::string& assetPath)
        : mIdCache()
        , mDispatcher()
        , mVfs(boost::bind(&on_vfs, _1, assetPath))
        , mAls(mVfs, mIdCache, mDispatcher)
        , mInput(mDispatcher, mAls, "KeyBindings")
        , mClock()
        , mPluginManager(0)
        , mAssetFactory(ScriptAsset::Register(mAls, mDispatcher, mInput, mPluginManager))
        , mKeys(256)
        , mProfiler(mInput)
        {
            mInput.SetFilter(boost::bind(&Ups::onInputFilter, this, _1));
            mPluginLoad = mInput.ListenForAction("Plugin.Load", boost::bind(&Ups::loadPlugin, this, _1, _2, _3, _4));
        }

        void Run()
        {
            std::string startScript = util::getAppName();
            hAsset<ScriptAsset> bootstrap(startScript, mAls);
            ConsoleLoop(mInput, mClock, boost::bind(&Ups::onFrame, this, _1));
        }

    private:
        void onFrame(const Clock& clock)
        {
            Prof(GameLoop);
            float deltaTime = static_cast<float>(clock.GetFrameDuration());
            mInput.Tick(deltaTime);

            // now send frame tick message;
            mDispatcher[mtype::kFrameTick](Message(mtype::kFrameTick, deltaTime));

            mProfiler.Tick(deltaTime);
        }

        bool onInputFilter(const InputManager::Input& input)
        {
            if (const InputManager::Key *key = dynamic_cast<const InputManager::Key *>(&input))
            {
                if ((key->mFlags & InputTypes::kButtonDown) && mKeys[key->mValue])
                {
                    return false;
                }

                mKeys[key->mValue] = (key->mFlags & InputTypes::kButtonUp) == 0;
            }
            return true;
        }

        void loadPlugin(const std::string& /*actionName*/, uint32_t /*timeStamp*/, int32_t /*mouseX*/, int32_t /*mouseY*/)
        {
            static bool firstTime = true;
            if (firstTime)
            {
                firstTime = false;
                IPluginObject *plugin = mPluginManager.ptr("dyn");
            }
            else
            {
                mPluginManager.unload("dyn");
            }
        }

        IdGameCache mIdCache;
        Dispatcher mDispatcher;
        VirtualFileSystem mVfs;
        AssetLoadSystem mAls;
        InputManager mInput;
        ClockManager mClock;
        PluginManager mPluginManager;
        IAsset::Token mAssetFactory;
        std::vector<uint8_t> mKeys;
        Profiler mProfiler;
        InputManager::ActionListener_t mPluginLoad;
    };

} // namespace


int main()
{
    log_trace("main") << "Starting ups.";

    std::string workingDir = util::getAppPath() + "/../" + util::getAppName() + "_Assets";
    bfs::path base_asset_path(workingDir);
    bfs::path asset_path(base_asset_path);

    log_trace("main") << "Folders:\n\tAsset Path: '" << asset_path << "'.";
    bfs::create_directories(asset_path);

    Ups app(asset_path.string());
    app.Run();
}

