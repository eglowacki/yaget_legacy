#include "Precompiled.h"
#include "ReplicateModule.h"
#include "MessageInterface.h"
#include "Plugin/ModuleHelper.h"
#include <wx/dynlib.h>
//#include <wx/ipc.h>

WXDLL_ENTRY_FUNCTION();


namespace eg {

IMPLEMENT_DYNAMIC_CLASS(ReplicateModule, wxModule)


ReplicateModule::ReplicateModule()
{
}


ReplicateModule::~ReplicateModule()
{
}


bool ReplicateModule::OnInit()
{
    wxLogTrace(TR_REPLICATE, "ReplicateModule Initialized.");
    module::ActivateComponents("Replicate");
    return true;
}


void ReplicateModule::OnExit()
{
    wxLogTrace(TR_REPLICATE, "ReplicateModule Shutdown.");
}


} // namespace eg
