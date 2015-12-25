#include "Precompiled.h"
#pragma warning(push)
#pragma warning (disable : 4512)  // '' : assignment operator could not be generated
#pragma warning (disable : 4244)  // '' : conversion from 'type' to 'type', possible loss of data
#include "Registrate.h"
#include "Widget/WidgetAsset.h"
#include "StringHelper.h"
#pragma warning(pop)
#include <wx/filename.h>
#include <map>
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
//! Make window function defines go away
#undef GetObject


namespace
{
    std::string GetFullExecutable()
    {
        static bool found = false;
        static std::string path;

        if (!found)
        {
    #ifdef __WXMSW__

            char buf[512];
            *buf = '\0';
            GetModuleFileName(NULL, buf, 511);
            path = buf;

    #elif defined(__WXMAC__)

            ProcessInfoRec processinfo;
            ProcessSerialNumber procno ;
            FSSpec fsSpec;

            procno.highLongOfPSN = NULL ;
            procno.lowLongOfPSN = kCurrentProcess ;
            processinfo.processInfoLength = sizeof(ProcessInfoRec);
            processinfo.processName = NULL;
            processinfo.processAppSpec = &fsSpec;

            GetProcessInformation( &procno , &processinfo ) ;
            path = wxMacFSSpec2MacFilename(&fsSpec);
    #else // any other platform
            wxString argv0 = wxTheApp->argv[0];

            if (wxIsAbsolutePath(argv0))
            {
                path = argv0;
            }
            else
            {
                wxPathList pathlist;
                pathlist.AddEnvList(wxT("PATH"));
                path = pathlist.FindAbsoluteValidPath(argv0);
            }

            wxFileName filename(path);
            filename.Normalize();
            path = filename.GetFullPath();
    #endif
            found = true;
        }

        return path;
    }

    typedef std::map<std::string, boost::any> RegisteredObjects_t;
    RegisteredObjects_t mRegisteredObjects;

    struct CleanupCheck
    {
        CleanupCheck()
        {
        }

        ~CleanupCheck()
        {
            if (!mRegisteredObjects.empty())
            {
                wxLogWarning("There are %d Registered Objects left on exit.", mRegisteredObjects.size());
            }
        }
    };

    CleanupCheck cleanupCheck;

    //eg::GetCustomConfigStream_t GetCustomConfigStreamPF = 0;

    typedef std::map<std::string, guid_t> RegisteredWidgets_t;
    RegisteredWidgets_t RegisteredWidgets;


} // namespace



namespace eg {
namespace registrate {


__declspec(dllexport) boost::any Get(const char *name)
{
    name;
    return boost::any();
}


__declspec(dllexport) void Set(const char *name, boost::any value)
{
    name;
    value;
}


__declspec(dllexport) boost::any GetObject(const char *pName)
{
    if (!IsNull(pName))
    {
        std::string name(pName);
        RegisteredObjects_t::const_iterator it = mRegisteredObjects.find(name);
        if (it != mRegisteredObjects.end())
        {
            return (*it).second;
        }
    }

    return boost::any();
}


__declspec(dllexport) void RegisterObject(boost::any object, const char *pName)
{
    if (!IsNull(pName))
    {
        std::string name(pName);
        if (object.empty())
        {
            wxASSERT(mRegisteredObjects.find(name) != mRegisteredObjects.end());
            mRegisteredObjects.erase(name);
        }
        else
        {
            mRegisteredObjects[name] = object;
        }
    }
}


__declspec(dllexport) std::string GetAppName()
{
    wxFileName fileName(GetFullExecutable());
    return std::string((const char *)fileName.GetName().c_str());
}


__declspec(dllexport) std::string GetExecutablePath()
{
    wxFileName fileName(GetFullExecutable());
    return std::string((const char *)fileName.GetPath().c_str());
}



} // namespace registrate


/*
FunctionSetter::FunctionSetter(GetCustomConfigStream_t functionPointer)
{
    GetCustomConfigStreamPF = functionPointer;
}

std::string GetCustomConfigStream()
{
    if (GetCustomConfigStreamPF)
    {
        return GetCustomConfigStreamPF();
    }

    return "";
}
*/


namespace widgets
{


__declspec(dllexport) guid_t GetWidgetType(const std::string& name)
{
    RegisteredWidgets_t::const_iterator it = RegisteredWidgets.find(name);
    if (it != RegisteredWidgets.end())
    {
        return (*it).second;
    }

    return 0;
}


__declspec(dllexport) void AddWidgetType(const std::string& name, guid_t type)
{
    RegisteredWidgets.insert(std::make_pair(name, type));
}


__declspec(dllexport) void RemoveWidgetType(const std::string& name)
{
    RegisteredWidgets.erase(name);
}


} // namespace widgets

} // namespace eg
