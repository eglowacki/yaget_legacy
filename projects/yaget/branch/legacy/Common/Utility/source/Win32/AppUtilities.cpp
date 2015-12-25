#include "Precompiled.h"
#include "App/AppUtilities.h"
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace bfs = boost::filesystem;

namespace eg { namespace util
{

void sleep(uint32_t miliseconds)
{
    ::Sleep(miliseconds);
}


// just to satisfy linker for the time being
std::string getAppPath()
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

std::string getAppName()
{
    char buf[512] = {'\0'};
    GetModuleFileName(0, buf, 511);
    bfs::path app_path = buf;
    std::string appName = app_path.stem();
    return appName;
}


std::string getAppFullPath()
{
    return getAppPath() + file::sep + getAppName();
}

}} // namespace util // namespace eg

