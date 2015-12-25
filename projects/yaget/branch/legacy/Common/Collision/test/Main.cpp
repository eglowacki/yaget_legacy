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

#include <UnitTest++.h>

using namespace eg;

extern int dummyValue;
std::string MyFunction()
{
    std::string configFile;
    configFile += "[Plugins/Prerequisite]\n";
    configFile +=     "Objects = required\n";
    configFile += "[Plugins/Dynamic]\n";
    configFile +=     "CollisionHavok = required\n";
    configFile += "[Logs/Output]\n";
    configFile +=     "Output = Console\n";
    configFile += "[Logs]\n";
    //configFile +=     "lua = all\n";
    //configFile +=     "Script = all\n";
    configFile +=     "Utility = all\n";
    configFile +=     "Objects = all\n";
    //configFile +=     "Components = all\n";
    //configFile +=     "Replicate = all\n";
    //configFile +=     "Renderer = all\n";
    //configFile +=     "RendererResource = all\n";
    //configFile +=     "SimVisualizer = all\n";
    configFile +=     "Collision = all\n";
    //configFile +=     "Editor = all\n";
    return configFile;
}

eg::FunctionSetter functionSetter(&MyFunction);


int main(int argc, char* argv[])
{
    StartUnitTestSystem unitTestSystem(argc, argv);
    unitTestSystem.Begin();

    int result = UnitTest::RunAllTests();

    unitTestSystem.End();
    return result;
}
