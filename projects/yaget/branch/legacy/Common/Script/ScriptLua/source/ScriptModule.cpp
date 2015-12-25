#include "Precompiled.h"
//#include "vld.h"
#include "ScriptModule.h"
#include "MessageInterface.h"
#include "Plugin/ModuleHelper.h"
//#include "BaseDLLs.h"
#include <wx/dynlib.h>
#include <wx/ipc.h>

WXDLL_ENTRY_FUNCTION();


namespace eg {

IMPLEMENT_DYNAMIC_CLASS(ScriptModule, wxModule)


ScriptModule::ScriptModule()
{
}


ScriptModule::~ScriptModule()
{
}


bool ScriptModule::OnInit()
{
    wxLogTrace(TR_SCRIPT, "ScriptModule Initialized.");
    module::ActivateComponents("Script");
    Message(mtype::kRegisterPlugin, std::string("Script")).Send();
    return true;
}


void ScriptModule::OnExit()
{
    wxLogTrace(TR_SCRIPT, "ScriptModule Shutdown.");
}

/*
//! This is just here to satisfy linker
__declspec(dllexport) wxFrame *GetMainUserFrame()
{
    return 0;
}
*/

} // namespace eg
