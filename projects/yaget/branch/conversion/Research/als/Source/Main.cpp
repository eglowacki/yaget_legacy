// Pong game main entry point
#include "Main.h"
#include "Logger/Log.h"
#include "Registrate.h"
#include "Input/InputManager.h"
#include "File/DiskFileFactory.h"
#include "File/VirtualFileSystem.h"
#include "File/AssetLoadSystem.h"
#include "IdGameCache.h"
#include "Timer/Clock.h"
#include "App/AppUtilities.h"
#include "App/MainConsole.h"

#include "FooAsset.h"
#include "Script/ScriptAsset.h"

#include <boost/filesystem.hpp>
#include <boost/signals.hpp>
#include "vld.h"



using namespace eg;
namespace bfs = boost::filesystem;


std::vector<std::string> getTraceModules()
{
    std::vector<std::string> traceModules;
    //traceModules.push_back(tr_util);
    traceModules.push_back("main");
    traceModules.push_back("script");
    traceModules.push_back(tr_input);
    return traceModules;
}
logs::LoggerInit loggerInit("", logs::kDest_file|logs::kDest_cout|logs::kDest_debug, 0, true, getTraceModules());



namespace eg { namespace config {

    //! Helper function to add new factory to VirtualFileSystem
    template <typename T>
    inline void AddVirtualFile(VirtualFileSystem& vfs, const VirtualFileSetting& settings)
    {
        VirtualFileSystem::Factory_t factory(new T(settings.type, settings.priority, settings.folders));
        vfs.RegisterFactory(settings.type, factory);
    }

}} // namespace config // namespace eg



bool quitApp = false;
void onQuit(const std::string& actionName, uint32_t timeStamp, int32_t mouseX, int32_t mouseY)
{
    quitApp = true;
}


void gameLoop(InputManager& input)
{
    ClockManager clock;
    Clock main_time(clock);

    HANDLE hStdInput = ::GetStdHandle(STD_INPUT_HANDLE);
    INPUT_RECORD inputRec = {0};

    MainConsole::ClearConsole();
    MainConsole::SetColor(FOREGROUND_BLUE|FOREGROUND_INTENSITY);
    MainConsole::ConPrintAt(0, 0, "Welcome to als test.");
    MainConsole::SetColor(FOREGROUND_GREEN);
    MainConsole::ConPrintAt(0, 1, "ESC to quit.");
    MainConsole::gotoXY(0, 2);

    while (!quitApp)
    {
        app::sleep(0.3);
        clock.FrameStep();
        float deltaTime = static_cast<float>(main_time.GetFrameDuration());
        double currTime = main_time.GetTime();

        float fps = main_time.GetFrameRate();
        std::string fps_text = "FPS: " + boost::lexical_cast<std::string>(fps) + "               ";
        MainConsole::ConPrintAt(0, 2, fps_text);
        MainConsole::gotoXY(0, 3);

        input.Tick(deltaTime);

        inputRec.EventType = 0;
        DWORD numRead = 0;
        DWORD numPendingInputs = 0;
        ::GetNumberOfConsoleInputEvents(hStdInput, &numPendingInputs);
        if (numPendingInputs)
        {
            if (::ReadConsoleInput(hStdInput, &inputRec, 1, &numRead))
            {
            }
            else
            {
                log_error << "Could not read input from Console Application.";
            }
        }

        switch (inputRec.EventType)
        {
        case KEY_EVENT:
            {
                bool altPressed = (inputRec.Event.KeyEvent.dwControlKeyState & LEFT_ALT_PRESSED) ? true : false;
                if (inputRec.Event.KeyEvent.dwControlKeyState & RIGHT_ALT_PRESSED)
                {
                    altPressed = true;
                }

                bool ctrlPressed = (inputRec.Event.KeyEvent.dwControlKeyState & LEFT_CTRL_PRESSED) ? true : false;
                if (inputRec.Event.KeyEvent.dwControlKeyState & RIGHT_CTRL_PRESSED)
                {
                    ctrlPressed = true;
                }

                bool shiftPressed = (inputRec.Event.KeyEvent.dwControlKeyState & SHIFT_PRESSED) ? true : false;
                uint32_t flags = altPressed ? InputTypes::kButtonAlt : 0;
                flags |= ctrlPressed ? InputTypes::kButtonCtrl : 0;
                flags |= shiftPressed ? InputTypes::kButtonShift : 0;


                if (inputRec.Event.KeyEvent.bKeyDown)
                {
                    flags |= InputTypes::kButtonDown;
                }
                else
                {
                    flags |= InputTypes::kButtonDown;
                }

                int keyValue = input.MapKey(inputRec.Event.KeyEvent.uChar.AsciiChar);
                InputManager::Key *pKey = new InputManager::Key(static_cast<const uint32_t>(currTime * 1000.0f), flags, keyValue);
                input.ProcessInput(pKey);
            }

            break;
        }
    }
}


int main()
{
    log_trace("main") << "Starting als test...";

    std::string workingDir = registrate::GetExecutablePath() + "/Assets";
    bfs::path base_asset_path(workingDir);
    bfs::path asset_path(base_asset_path);

    log_trace("main") << "Folders:\n\tAsset Path: '" << asset_path << "'.";
    bfs::create_directories(asset_path);

    // rough order of initialization
    IdGameCache idCache;    // manages all unique id's
                            //
    VirtualFileSystem vfs;  // abstructs acces to persistent resources
    config::VirtualFileSetting setting;
    setting.folders.push_back(asset_path.string());

    setting.type = "foo";
    config::AddVirtualFile<DiskFileFactory>(vfs, setting);

    setting.type = "lua";
    config::AddVirtualFile<DiskFileFactory>(vfs, setting);

    setting.type = "xml";
    config::AddVirtualFile<DiskFileFactory>(vfs, setting);

    AssetLoadSystem als(vfs, idCache);  // provides assets backed by persistent storage
    FooAsset::Register(als);
    ScriptAsset::Register(als);

    InputManager input(als, "KeyBindings");    // provides all input and handling
    InputManager::ActionListener_t action_quit = input.ListenForAction("App.Quit", onQuit);

    ::SetConsoleTitle("Asset Load System [Test]");

    hAsset<FooAsset> brickAsset("Brick", als);
    FooAsset::Lamp oldColor = brickAsset->Color();
    FooAsset::Lamp newColor = static_cast<FooAsset::Lamp>((oldColor+1) % 3);
    brickAsset->SetColor(newColor);
    brickAsset->SetCountry("USA Hello");
    brickAsset.save();

    hAsset<ScriptAsset> wallAsset("Wall", als);
    wallAsset->Execute();

    gameLoop(input);

    int z = 0;
}