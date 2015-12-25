#include "Precompiled.h"
//#include "vld.h"
#include "ComponentsModule.h"
#include "MessageInterface.h"
#include "Registrate.h"
#include "Plugin/ModuleHelper.h"
//#include "BaseDLLs.h"
#include "Logger/Log.h"
#include <wx/dynlib.h>
#include <boost/shared_ptr.hpp>

WXDLL_ENTRY_FUNCTION();


namespace eg {

IMPLEMENT_DYNAMIC_CLASS(ComponentsModule, wxModule)


ComponentsModule::ComponentsModule()
{
}


ComponentsModule::~ComponentsModule()
{
}


bool ComponentsModule::OnInit()
{
    wxLogTrace(TR_COMPONENTS, "ComponentsModule Initialized.");
    module::ActivateComponents("Components");
    return true;
}


void ComponentsModule::OnExit()
{
    wxLogTrace(TR_COMPONENTS, "ComponentsModule Shutdown.");
}


} // namespace eg//
