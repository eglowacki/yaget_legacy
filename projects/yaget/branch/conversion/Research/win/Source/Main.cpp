// Pong game main entry point
#include "Logger/Log.h"
#include "Timer/Clock.h"
#include "App/AppUtilities.h"
#include "File/DiskFileFactory.h"
#include "App/MainConsole.h"
#include "Profiler/Profiler.h"
#include "Plugin/IPluginObject.h"

#include "ObjectID.h"
#include <boost/tokenizer.hpp>

#include <boost/filesystem.hpp>
#include "vld.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "App/AppWindow.h"


using namespace eg;
namespace bfs = boost::filesystem;


std::vector<std::string> getTraceModules()
{
    std::vector<std::string> traceModules;
    traceModules.push_back(tr_script);
    traceModules.push_back(tr_plugin);
    traceModules.push_back(tr_input);
    //traceModules.push_back(tr_asset);
    traceModules.push_back("main");
    return traceModules;
}
logs::LoggerInit loggerInit("", logs::kDest_file|logs::kDest_debug, 0, true, getTraceModules());

namespace eg {

//extern void RegisterWindowTypes(HINSTANCE hInstance);
//extern void WindowLoop(InputManager& input, ClockManager& clock, boost::function<void (const Clock&)> callback, HINSTANCE hInstance, int iCmdShow);

} // namespace eg


HINSTANCE eg::app::Window::Instance = 0;
eg::app::Context *eg::app::Window::Context = 0;
std::list<app::Window *> app::Window::Windows;
bool app::Window::Active = false;

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

    class Win
    {
    public:
        Win(const std::string& assetPath)
        : mContext(boost::bind(&on_vfs, _1, assetPath))
        , mAssetFactory(ScriptAsset::Register(mContext.als, mContext.dispatcher, mContext.input, mContext.pluginManager))
        , mProfiler(mContext.input)
        , mWindow("Main")
        {
        }

        void Run(HINSTANCE /*hInstance*/, int /*iCmdShow*/)
        {
            //std::string startScript = util::getAppName();
            //hAsset<ScriptAsset> bootstrap(startScript, mContext.als);
            //RegisterWindowTypes(hInstance);
            //WindowLoop(mContext.input, mContext.clock, boost::bind(&Win::onFrame, this, _1), hInstance, iCmdShow);
        }

    private:
        void onFrame(const Clock& clock)
        {
            Prof(GameLoop);
            float deltaTime = static_cast<float>(clock.GetFrameDuration());
            mContext.input.Tick(deltaTime);

            // now send frame tick message;
            mContext.dispatcher[mtype::kFrameTick](Message(mtype::kFrameTick, deltaTime));

            mProfiler.Tick(deltaTime);
        }

        app::Context mContext;
        IAsset::Token mAssetFactory;
        Profiler mProfiler;

        app::Window mWindow;
    };

} // namespace

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*szCmdLine*/, int iCmdShow)
{
    log_trace("main") << "Starting win.";

    std::string workingDir = util::getAppPath() + "/../" + "Assets";
    bfs::path base_asset_path(workingDir);
    bfs::path asset_path(base_asset_path);

    log_trace("main") << "Folders:\n\tAsset Path: '" << asset_path << "'.";
    bfs::create_directories(asset_path);

    Win app(asset_path.string());
    app.Run(hInstance, iCmdShow);

    return 0;
}
