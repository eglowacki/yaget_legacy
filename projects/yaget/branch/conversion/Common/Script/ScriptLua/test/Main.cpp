// Objects Unit Test file
//#include "BaseDLLs.h"
#include "BaseRegistrate.h"
#include "Config/InitSystem.h"
#include "Config/ConfigHelper.h"
#include "App/MainConsole.h"
#include "IObjectManager.h"
#include <wx/utils.h>
#include <wx/init.h>
#include <wx/log.h>

/*
#include <pantheios/pantheios.hpp>
#include <pantheios/implicit_link/core.h>
#include <pantheios/implicit_link/util.h>
#include <pantheios/implicit_link/fe.N.h>
#include <pantheios/implicit_link/be.N.h>
#include <pantheios/implicit_link/bec.file.h>
#include <pantheios/implicit_link/bec.fprintf.h>
#include <pantheios/implicit_link/bec.WindowsDebugger.h>
#include <pantheios/implicit_link/bec.WindowsMessageBox.h>

#include <pantheios/frontends/fe.N.h>
#include <pantheios/backends/be.N.h>
#include <pantheios/backends/bec.file.h>
#include <pantheios/backends/bec.fprintf.h>
#include <pantheios/backends/bec.WindowsDebugger.h>
#include <pantheios/backends/bec.WindowsMessageBox.h>
*/

#include <UnitTest++.h>


/*
const pan_fe_N_t  PAN_FE_N_SEVERITY_CEILINGS[]  =
{
    { 4,  PANTHEIOS_SEV_ERROR    }      // Terminates the array; sets the default ceiling to 'notice'
    , { 0,  PANTHEIOS_SEV_DEBUG    }    // Terminates the array; sets the default ceiling to 'notice'
};

pan_be_N_t  PAN_BE_N_BACKEND_LIST[] =
{
    PANTHEIOS_BE_N_STDFORM_ENTRY(1, pantheios_be_file, 0)
    , PANTHEIOS_BE_N_STDFORM_ENTRY(2, pantheios_be_fprintf, 0)
    , PANTHEIOS_BE_N_STDFORM_ENTRY(3, pantheios_be_WindowsDebugger, 0)
    , PANTHEIOS_BE_N_STDFORM_ENTRY(4, pantheios_be_WindowsMessageBox, 0)
    , PANTHEIOS_BE_N_TERMINATOR_ENTRY
};

PANTHEIOS_EXTERN_C const char PANTHEIOS_FE_PROCESS_IDENTITY[] = "UT_ScriptLua";
*/


using namespace eg;

std::string MyFunction()
{
    std::string configFile;
    configFile += "[Plugins/Prerequisite]\n";
    configFile +=     "Objects = required\n";
    configFile += "[Plugins/Dynamic]\n";
    configFile +=     "Components = required\n";
    configFile +=     "ScriptLua = required\n";
    configFile += "[VFS Factory/DiskFile]\n";
    configFile +=     "lua = ../../Assets/Scripts\n";
    configFile += "[Debug/Script]\n";
    configFile +=     "Debug = false\n";
    configFile +=     "ExtendedDebug = false\n";
    configFile +=     "ExecutionMaxTime = 3\n";
    configFile += "[Logs/Output]\n";
    configFile +=     "Output = Console, File\n";
    configFile += "[AssetLoadSystem]\n";
    configFile +=     "LoadAsync = false\n";
    return configFile;
}

eg::FunctionSetter functionSetter(&MyFunction);



int main(int argc, char* argv[])
{
    //pantheios_be_file_setFilePath("log.local", PANTHEIOS_BE_FILE_F_TRUNCATE, PANTHEIOS_BE_FILE_F_TRUNCATE, PANTHEIOS_BEID_LOCAL);
    //pantheios::log_DEBUG("500 Hello Again and again!");
    //pantheios::log_ERROR("We should see this in Dialog Box");
    {
        StartUnitTestSystem unitTestSystem(argc, argv, true);
        unitTestSystem.Begin();

        int result = UnitTest::RunAllTests();

        unitTestSystem.End();
    }
    return 0;
}
