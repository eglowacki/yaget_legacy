// Pong game main entry point
#include "Logger/Log.h"
#include "Timer/Clock.h"
#include "App/AppUtilities.h"
#include "App/AppWindow.h"
#include "File/DiskFileFactory.h"
#include "File/AssetLoadSystem.h"
#include "Script/ScriptAsset.h"
#include "Profiler/Profiler.h"
#include "Plugin/IPluginObject.h"
#include "Shiny.h"

#include <boost/tokenizer.hpp>

#include <boost/filesystem.hpp>
#pragma warning(push)
#pragma warning (disable : 4512)  // '' : assignment operator could not be generated
    #include <boost/program_options.hpp>
#pragma warning(pop)
#include "vld.h"



using namespace eg;
namespace bfs = boost::filesystem;
namespace po = boost::program_options;


namespace eg { namespace script {
    extern void init_window(lua_State * /*L*/);
}} // namespace script // namespace eg

HINSTANCE app::Window::Instance = 0;
app::Context *app::Window::Context = 0;
std::list<app::Window *> app::Window::Windows;
bool app::Window::Active = false;


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

} // namespace


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR szCmdLine, int /*iCmdShow*/)
{
    log_trace("main") << "Starting boot.";

    std::vector<std::string> args = po::split_winmain(szCmdLine);

    std::string scriptName = util::getAppName();
    po::options_description desc("Allowed options");
    try
    {
        desc.add_options()
            ("help", "this help message")
            ("script,s", po::value<std::string>(&scriptName), "startup script name")
            ;

        po::variables_map vm;
        po::store(po::command_line_parser(args).options(desc).run(), vm);
        po::notify(vm);

        if (vm.count("help"))
        {

            log_trace("main") << "Yaget Boot\nCopyright Edgar Glowacki (Yaget) 2009";
            log_trace("main") << desc;
            std::cout << "Yaget Boot\nCopyright Edgar Glowacki (Yaget) 2009";
            std::cout << desc << "\n";

            std::stringstream text;
            text << "Copyright Edgar Glowacki (Yaget) 2009\n";
            text << "cmd [" << szCmdLine << "]\n";
            text << desc << "\n";
            MessageBox(NULL, (LPCTSTR)text.str().c_str(), TEXT("Boot options help"), MB_OK);
            return 1;
        }
    }
    catch (std::exception& e)
    {
        log_error << "error: " << e.what();
        log_trace("main") << "Yaget Boot\nCopyright Edgar Glowacki (Yaget) 2009";
        log_trace("main") << desc;

        std::cerr << "error: " << e.what() << "\n";
        std::cout << "Yaget Boot\nCopyright Edgar Glowacki (Yaget) 2009";
        std::cout << desc << "\n";

        std::stringstream text;
        text << "Copyright Edgar Glowacki (Yaget) 2009\n";
        text << "cmd [" << szCmdLine << "]\n";
        text << desc << "\n";
        MessageBox(NULL, (LPCTSTR)text.str().c_str(), TEXT("Boot options help"), MB_OK);
        return 1;
    }

    app::Context context(boost::bind(&on_vfs, _1, util::getAppPath()));
    context.environment.vars["vfs"] = &context.vfs;
    context.environment.vars["loader"] = &context.pluginManager;

    app::Window::Initialize(hInstance, &context);
    // let's also register with script our Window bindings
    ScriptAsset::RegisterBinds(context.script, script::init_window);

    log_trace("main") << "Starting auto executing script '" << scriptName << "'...";
    hAsset<ScriptAsset> bootstrap(scriptName, context.als);
    bootstrap->Execute();

    return 0;
}
