// Unit test for Kosole object
#include "Registrate.h"
#include "Config/Console.h"
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <UnitTest++.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef GetObject

using namespace eg;
namespace bfs = boost::filesystem;

// just to satisfy linker for the time being
__declspec(dllexport) std::string registrate::GetExecutablePath()
{
    char buf[512] = {'\0'};
    GetModuleFileName(0, buf, 511);
    bfs::path test_path = buf;
    std::string appPath = test_path.string();
    boost::ierase_last(appPath, test_path.filename());
    if (!appPath.empty() && (*appPath.rbegin() == '/' || *appPath.rbegin() == '\\'))
    {
        appPath.erase(appPath.size()-1);
    }
    return appPath;
}

__declspec(dllexport) std::string registrate::GetAppName()
{
    char buf[512];
    *buf = '\0';
    GetModuleFileName(0, buf, 511);
    bfs::path app_path = buf;
    std::string appName = app_path.stem();
    return appName;
}


__declspec(dllexport) boost::any registrate::GetObject(char const *)
{
    return boost::any();
}


struct KonsoleEcho
{
    void operator()(config::KosoleResult result, const std::string& command_name, const std::string& command_output_string)
    {
        CHECK_EQUAL(Result, result);
        CHECK_EQUAL(Name, command_name);
        CHECK_EQUAL(Out, command_output_string);
    }

    config::KosoleResult Result;
    std::string Name;
    std::string Out;
};


struct CommandTest
{
    std::string operator()(const std::vector<std::string> & parameters, config::KosoleResult& result)
    {
        CHECK_EQUAL(tokens.size(), parameters.size());
        CHECK(std::equal(tokens.begin(), tokens.end(), parameters.begin()));

        result = Result;
        return Out;
    }

    std::vector<std::string> tokens;
    std::string Out;
    config::KosoleResult Result;
};



TEST(Console)
{
    config::Konsole konsole;
    CommandTest commandTest;
    KonsoleEcho konsoleEcho;
    konsole.sigCommandEcho.connect(boost::ref(konsoleEcho));

    //------------------------------------------------------------------------------------
    // test well formed command with parameters
    commandTest.tokens.push_back("/CommandTest");
    commandTest.tokens.push_back("1");
    commandTest.tokens.push_back("true");
    commandTest.tokens.push_back("Garden Tree");
    commandTest.tokens.push_back("false");
    commandTest.tokens.push_back("Molten Rock");
    konsole.AddItem("CommandTest", boost::ref(commandTest));

    commandTest.Out = "ABCD";
    commandTest.Result = config::krError;

    konsoleEcho.Result =  commandTest.Result;
    konsoleEcho.Name = commandTest.tokens[0];
    konsoleEcho.Out = "ABCD";
    CHECK(konsole.Execute("/CommandTest 1 true \"Garden Tree\" false \"Molten Rock\""));

    //------------------------------------------------------------------------------------
    // test bad formed command with parameters, which should trigger 
    // built-in /help command
    konsoleEcho.Result =  config::krHelp;
    konsoleEcho.Name = "/help";
    konsoleEcho.Out = "Valid commands:\n\t/CommandTest";
    CHECK(konsole.Execute("CommandTest 1 true \"Garden\" Tree false Molten Rock\"") == false);
    CHECK(konsole.Execute("") == false);

    //------------------------------------------------------------------------------------
    // test just the command name
    commandTest.tokens.clear();
    commandTest.tokens.push_back("/CommandTest");
    konsoleEcho.Result =  commandTest.Result;
    konsoleEcho.Name = commandTest.tokens[0];
    konsoleEcho.Out = "ABCD";
    CHECK(konsole.Execute("/CommandTest"));

}
