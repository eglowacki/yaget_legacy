// Main Unit Test file
//#include "BaseDLLs.h"
#include "BaseRegistrate.h"
#include "Config/InitSystem.h"
#include "Config/ConfigHelper.h"
#include "App/MainConsole.h"
#include <wx/utils.h>
#include <wx/init.h>
#include <wx/log.h>
#include <UnitTest++.h>

using namespace eg;

std::string MyFunction()
{
    return "[UnitTest]";
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
