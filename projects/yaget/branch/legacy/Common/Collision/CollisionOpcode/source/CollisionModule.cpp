#include "Precompiled.h"
#include "vld.h"
#include "CollisionModule.h"
#include "MessageInterface.h"
#include "Plugin/ModuleHelper.h"
//#include "BaseDLLs.h"
#include <wx/dynlib.h>
#include <wx/ipc.h>

WXDLL_ENTRY_FUNCTION();


namespace eg {

IMPLEMENT_DYNAMIC_CLASS(CollisionModule, wxModule)


CollisionModule::CollisionModule()
{
}


CollisionModule::~CollisionModule()
{
}


bool CollisionModule::OnInit()
{
    wxLogTrace(TR_COLLISION, "CollisionModule Initialized.");
    module::ActivateComponents("Collision");
    Message(mtype::kRegisterPlugin, std::string("Collision")).Send();
    return true;
}


void CollisionModule::OnExit()
{
    wxLogTrace(TR_COLLISION, "CollisionModule Shutdown.");
}

//! This is just here to satisfy linker
__declspec(dllexport) wxFrame *GetMainUserFrame()
{
    return 0;
}

} // namespace eg
