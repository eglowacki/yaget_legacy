#include "Precompiled.h"
#include "ManagedLoader.h"
#include "IEditorMessages.h"
#include "Message/Dispatcher.h"
#include <BugslayerUtil.h>

namespace
{
} // namespace


namespace eg {


ManagedLoader::ManagedLoader(const char *pName, IManagedInterface *pMI) :
    mpInterface(pMI),
    mName(pName ? pName : "ManagedLoader")
{
}


ManagedLoader::~ManagedLoader()
{
    delete mpInterface;
}


wxWindow *ManagedLoader::Init(wxWindow *pParentWindow)
{
    if (mpInterface)
    {
        if (mpInterface->Init())
        {
            // nothing to do here...
        }
        else
        {
            // we could not initialize our panel
            // we still keep this object around, but it should have negligible impact on memory
            delete mpInterface;
            mpInterface = 0;
        }
    }

    return 0;
}



const char *ManagedLoader::GetName() const
{
    return mName.c_str();
}


namespace managed {

void LogTrace(const std::string& text)
{
    DEJA_TRACE("EditorNET", text.c_str());
}


void LogMessage(const std::string& text)
{
    DEJA_TRACE("EditorNET", text.c_str());
}


void LogWarning(const std::string& text)
{
    DEJA_WARNING("EditorNET", text.c_str());
}


void LogError(const std::string& text)
{
    DEJA_ERROR("EditorNET", text.c_str());
    //wxLogError(text.c_str());
}


} // namespace managed
} // namespace eg

