#include "RendererPlugin.h"
#include "Logger/Log.h"
#include "DXErrorHandler.h"
#include "Script/ScriptAsset.h"
#include "App/AppUtilities.h"
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/algorithm/string.hpp>
#include "vld.h"

using namespace eg;

std::vector<std::string> getTraceModules()
{
    std::vector<std::string> traceModules;
    traceModules.push_back(tr_script);
    traceModules.push_back(tr_render);
    return traceModules;
}

std::string getLogName()
{
    return util::getAppFullPath() + ".renderer.log";
}
logs::LoggerInit loggerInit(getLogName(), logs::kDest_file|logs::kDest_debug, 0, true, getTraceModules());

namespace
{
    boost::shared_ptr<RendererPlugin> rendererPlugin;
} // namespace


        //void init_renderer(lua_State *L)

namespace eg {

namespace script
{
    extern void init_renderer(lua_State * /*L*/);
} // namespace script

RendererPlugin::RendererPlugin(const Environment *environment)
: mD3D(Direct3DCreate9(D3D_SDK_VERSION))
, mEnvironment(environment)
{
    if (mEnvironment)
    {
        IAsset::Token script = mEnvironment->get<IAsset::Token>("Script");
        ScriptAsset::RegisterBinds(script, script::init_renderer);
    }

    log_trace(tr_render) << "RendererPlugin object created.";
}


RendererPlugin::~RendererPlugin()
{
    log_trace(tr_render) << "RendererPlugin object deleted.";
}


const Version RendererPlugin::GetVersion() const
{
    return YagetVersion;
}


IRendererPlugin::view_t RendererPlugin::NewView(handle_t handle)
{
    if (!mD3DDevice)
    {
        if (!createDevice(handle))
        {
            return (view_t)-1;
        }
    }

    return (view_t)-1;
}


void RendererPlugin::DestroyView(view_t /*handle*/)
{
}


bool RendererPlugin::createDevice(handle_t handle)
{
    bool bFullScreen = false;
    D3DPRESENT_PARAMETERS presentParam;
    ZeroMemory(&presentParam, sizeof(presentParam));

    presentParam.Windowed = !bFullScreen;
    presentParam.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    presentParam.BackBufferCount = 1;

    presentParam.SwapEffect = D3DSWAPEFFECT_DISCARD;
    presentParam.EnableAutoDepthStencil = TRUE;
    presentParam.AutoDepthStencilFormat = D3DFMT_D24S8;
    presentParam.BackBufferFormat = D3DFMT_UNKNOWN;

    UINT adapterToUse = D3DADAPTER_DEFAULT;
    D3DDEVTYPE deviceType = D3DDEVTYPE_HAL;
    DWORD behaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING|D3DCREATE_MULTITHREADED;

    if (bFullScreen)
    {
        // in full screen mode
        // use default values if there is none set
        presentParam.BackBufferWidth = 1024;
        presentParam.BackBufferHeight = 768;

        //if (checkDeviceType(adapterToUse, D3DFMT_X8R8G8B8, D3DFMT_A8R8G8B8))
        //{
        //    presentParam.BackBufferFormat = D3DFMT_A8R8G8B8;
        //}
    }
    else
    {
        presentParam.hDeviceWindow = static_cast<HWND>(handle);
        // we only allow any kind of debugging in window mode only
        if (0)//eg::config::ReadBool(wxConfigBase::Get(false), _T("Debug/Render/DebugShaders")))
        {
            deviceType = D3DDEVTYPE_REF;
            behaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
            presentParam.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

            log_trace(tr_render) << "Using Debug flags for Vertex and Pixel Shaders... expect low performance.";
        }
        else
        {
            for (UINT adapter = 0; adapter < mD3D->GetAdapterCount(); adapter++)
            {
                D3DADAPTER_IDENTIFIER9 identifier;
                /*HRESULT result =*/ mD3D->GetAdapterIdentifier(adapter, 0, &identifier);
                log_trace(tr_render) << "Adapter id: " << adapter << ", Name: '" << identifier.DeviceName << "', Driver: '" << identifier.Driver << "', Description: '" << identifier.Description << "'";
                std::string description(identifier.Description);
                if (boost::algorithm::iequals(description, "NVIDIA PerfHUD"))
                {
                    log_trace(tr_render) << "Creating device for use with NVIDIA PerfHUD.";
                    adapterToUse = adapter;
                    deviceType = D3DDEVTYPE_REF;
                    break;
                }
            }
        }
    }

    HRESULT result = mD3D->CreateDevice(adapterToUse,
                                         deviceType,
                                         static_cast<HWND>(handle),
                                         behaviorFlags,
                                         &presentParam,
                                         &mD3DDevice);

    if (FAILED(result))
    {
        HR_PRINT(result);
        return false;
    }

    RECT rect;
    GetClientRect(static_cast<HWND>(handle), &rect);

    D3DVIEWPORT9 vp;
    vp.X      = 0;
    vp.Y      = 0;
    vp.Width  = rect.right;
    vp.Height = rect.bottom;
    vp.MinZ   = 0.0f;
    vp.MaxZ   = 1.0f;

    result = mD3DDevice->SetViewport(&vp);
    HR_CHECK_VALID(result);
    result = mD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    HR_CHECK_VALID(result);
    return true;
}


} // namespace eg


//-------------------------------------------------------------------------------------
// yaget plugin exported functions
extern "C" __declspec(dllexport) IPluginObject *get_plugin(const Version& version, const Environment *environment)
{
    version;environment;
    if (!rendererPlugin)
    {
        rendererPlugin.reset(new RendererPlugin(environment));
    }

    return rendererPlugin.get();
}


extern "C" __declspec(dllexport) void free_plugin(IPluginObject *plugin)
{
    if (rendererPlugin.get() == plugin)
    {
        rendererPlugin.reset();
    }
    else
    {
        log_error << "Could not free plugin '" << plugin->GetName();
    }
}
