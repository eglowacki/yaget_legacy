#include "Precompiled.h"
//#include "vld.h"
#include "RenderModule.h"
#include "MessageInterface.h"
//#include "BaseDLLs.h"
#include <wx/dynlib.h>
#include <wx/ipc.h>

WXDLL_ENTRY_FUNCTION();


namespace eg {

IMPLEMENT_DYNAMIC_CLASS(RenderModule, wxModule)

CComPtr<IDirect3D9> RenderModule::D3D;


RenderModule::RenderModule()
{
}


RenderModule::~RenderModule()
{
}


bool RenderModule::OnInit()
{
    D3D.Attach(::Direct3DCreate9(D3D_SDK_VERSION));
    if (D3D)
    {
        //Message(mtype::kRegisterPlugin, std::string("Renderer")).Send();
        wxLogTrace(TR_RENDER, "RenderModule Initialized.");
    }

    return D3D != 0;
}


void RenderModule::OnExit()
{
    D3D = 0;
    wxLogTrace(TR_RENDER, "RenderModule Shutdown.");
}


} // namespace eg
