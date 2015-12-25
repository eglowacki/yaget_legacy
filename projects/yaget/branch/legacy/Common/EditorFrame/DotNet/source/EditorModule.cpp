#include "Precompiled.h"
//#include "vld.h"
#include "EditorModule.h"
#include "EditorFrame.h"
#include "MessageInterface.h"
#include "Message/Dispatcher.h"
//#include "BaseDLLS.h"
#include <wx/dynlib.h>
#include <boost/shared_ptr.hpp>

WXDLL_ENTRY_FUNCTION();


namespace eg {

IMPLEMENT_DYNAMIC_CLASS(EditorModule, wxModule)


EditorModule::EditorModule()
{
}


EditorModule::~EditorModule()
{
}


bool EditorModule::OnInit()
{
    Message(mtype::kRegisterPlugin, std::string("EditorFrame")).Send();

    wxLogTrace(TR_EDITOR, "EditorModule Initialized.");
    return true;
}


void EditorModule::OnExit()
{
    wxLogTrace(TR_EDITOR, "EditorModule Shutdown.");
}


} // namespace eg//
