#include "RendererPlugin.h"
#include "File/AssetLoadSystem.h"
#include "Logger/Log.h"
#include "Script/ScriptAsset.h"
#include "App/AppUtilities.h"
#include "UIAsset.h"
#include "ShaderAsset.h"
#include "DXErrorHandler.h"


#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include "vld.h"

using namespace eg;

std::vector<std::string> getTraceModules()
{
    std::vector<std::string> traceModules;
    traceModules.push_back(tr_script);
    traceModules.push_back(tr_render);
    traceModules.push_back("main");
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
    const int kNumTries = 5;
} // namespace


namespace eg {
namespace script
{
    extern void init_renderer(lua_State * /*L*/, RendererPlugin * /*renderer*/);
} // namespace script


RendererPlugin::RendererPlugin(IAsset::Token script, AssetLoadSystem& als)
: mCurrentView((view_t)-1)
, mD3D(Direct3DCreate9(D3D_SDK_VERSION))
, mErrorTries(kNumTries)
, mAls(als)
{
    if (!mD3D)
    {
        using namespace boost;
        throw ex::plugin(str(format("Could not create DirectX9 object, SDK version: %1%.") % D3D_SDK_VERSION).c_str());
    }

    ScriptAsset::RegisterBinds(script, boost::bind(&script::init_renderer, _1, this));
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


IRendererPlugin::view_t RendererPlugin::NewView(app::handle_t handle)
{
    if (!mDevice.device)
    {
        if (!createDevice(handle))
        {
            return (view_t)-1;
        }
    }

    view_t viewToken = mSwapChains.size();
    mSwapChains.push_back(SwapChain());
    SwapChain& swapChain = mSwapChains[viewToken];
    swapChain.handle = handle;
    if (swapChain.create(mDevice.device))
    {
        return viewToken;
    }
    return (view_t)-1;
}


void RendererPlugin::DestroyView(view_t viewToken)
{
    if (isValidView(viewToken))
    {
        mSwapChains[viewToken] = SwapChain();
        if (mCurrentView == viewToken)
        {
            mCurrentView = (view_t)-1;
        }
    }
    else
    {
        log_error << "ViewToken: " << viewToken << " is invalid.";
    }
}


math::Rect_t RendererPlugin::GetViewSize(view_t viewToken) const
{
    if (isValidView(viewToken))
    {
        const SwapChain& swapChain = mSwapChains.at(viewToken);
        return swapChain.getViewSize();
    }

    return v4::ZERO();
}


math::Rect_t RendererPlugin::ResizeView(view_t viewToken)
{
    math::Rect_t viewRect = v4::ZERO();
    if (isValidView(viewToken))
    {
        HRESULT hr = mDevice.device->SetRenderTarget(0, mDevice.renderSurface);
        HR_REPORT_INVALID(hr);
        mDevice.renderSurface = 0;
        hr = mDevice.device->SetDepthStencilSurface(mDevice.depthSurface);
        HR_REPORT_INVALID(hr);
        mDevice.depthSurface = 0;

        SwapChain& swapChain = mSwapChains[viewToken];
        swapChain.buffer = 0;
        swapChain.dsBuffer = 0;
        if (swapChain.create(mDevice.device))
        {
            hr = mDevice.device->GetDepthStencilSurface(&mDevice.depthSurface);
            HR_REPORT_INVALID(hr);
            hr = mDevice.device->GetRenderTarget(0, &mDevice.renderSurface);
            HR_REPORT_INVALID(hr);
        }

        if (mCurrentView == viewToken)
        {
            mCurrentView = (view_t)-1;
        }
        viewRect = GetViewSize(viewToken);
    }

    return viewRect;
}


bool RendererPlugin::activateView(view_t viewToken)
{
    if (!isValidView(viewToken))
    {
        return false;
    }

    if (mCurrentView == viewToken)
    {
        return true;
    }

    SwapChain& swapChain = mSwapChains[viewToken];
    if (swapChain.activate(mDevice.device))
    {
        mCurrentView = viewToken;
        return true;
    }

    mCurrentView = (view_t)-1;
    return false;
}


void RendererPlugin::RenderView(view_t viewToken, uint32_t clearFlags, const Color4_t& clearColor, boost::function<void()> callback)
{
    if (!mErrorTries)
    {
        return;
    }

    if (activateView(viewToken))
    {

        // we are ready to render our view now
        HRESULT hr = mDevice.device->BeginScene();
        if (FAILED(hr))
        {
            HR_PRINT(hr);
            mErrorTries--;
            return;
        }

        if (clearFlags)
        {
            uint32_t bufferFlags = 0;
            bufferFlags |= (clearFlags & IRendererPlugin::Color) ? D3DCLEAR_TARGET : 0;
            bufferFlags |= (clearFlags & IRendererPlugin::ZBuffer) ? D3DCLEAR_ZBUFFER : 0;
            bufferFlags |= (clearFlags & IRendererPlugin::Stencil) ? D3DCLEAR_STENCIL : 0;

            DWORD bufferColor = D3DCOLOR_COLORVALUE(clearColor.x, clearColor.y, clearColor.z, clearColor.w);

            hr = mDevice.device->Clear(0, NULL, bufferFlags, bufferColor, 1.0f, 0);
            if (FAILED(hr))
            {
                HR_PRINT(hr);
                mErrorTries--;
                return;
            }
        }

        // call render callback here
        if (callback)
        {
            callback();
        }

        hr = mDevice.device->EndScene();
        if (FAILED(hr))
        {
            HR_PRINT(hr);
            mErrorTries--;
            return;
        }

        SwapChain& swapChain = mSwapChains[viewToken];
        swapChain.buffer->Present(NULL, NULL, NULL, NULL, 0);

        mErrorTries = kNumTries;
    }
}


bool RendererPlugin::createDevice(app::handle_t handle)
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
    DWORD behaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;//|D3DCREATE_MULTITHREADED;

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
        presentParam.BackBufferWidth = 4;
        presentParam.BackBufferHeight = 4;
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
                HRESULT result = mD3D->GetAdapterIdentifier(adapter, 0, &identifier);
                HR_REPORT_INVALID(result);
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

    HRESULT hr = mD3D->CreateDevice(adapterToUse,
                                    deviceType,
                                    static_cast<HWND>(handle),
                                    behaviorFlags,
                                    &presentParam,
                                    &mDevice.device);
    if (FAILED(hr))
    {
        HR_PRINT(hr);
        return false;
    }

    hr = mDevice.device->GetDepthStencilSurface(&mDevice.depthSurface);
    HR_REPORT_INVALID(hr);
    hr = mDevice.device->GetRenderTarget(0, &mDevice.renderSurface);
    HR_REPORT_INVALID(hr);

    mShaderFactory = ShaderAsset::Register(mAls, mDevice.device);

    try
    {
        mVertexBuffer.reset(new VertexBuffer(mDevice.device, 8 * 1024 * 1024));
    }
    catch (const ex::bad_init& ex)
    {
        log_error << ex.what();
        return false;
    }

    hr = mDevice.device->SetRenderState(D3DRS_LIGHTING, FALSE);
    HR_REPORT_INVALID(hr);

    mDevice.presentParams = presentParam;

    mUI = UIAsset::Register(mAls);
    if (!UIAsset::Attach(mUI, mDevice.device, reinterpret_cast<_D3DPRESENT_PARAMETERS *>(&mDevice.presentParams)))
    {
        log_error << "Could not initialize UI subsytem";
        mUI.reset();;
    }

    return true;
}


uint32_t RendererPlugin::Generate(VertexBuffer::Generator_t generator, size_t vertexSize, D3DPRIMITIVETYPE type, D3DVERTEXELEMENT9 *layout)
{
    return mVertexBuffer->Generate(generator, vertexSize, type, layout);
}


void RendererPlugin::Generate(uint32_t token)
{
    mVertexBuffer->Generate(token);
}


void RendererPlugin::Render(uint32_t token)
{
    mVertexBuffer->Render(token);
}


math::Rect_t RendererPlugin::SwapChain::getViewSize() const
{
    RECT rect;
    GetClientRect(static_cast<HWND>(handle), &rect);
    math::Rect_t viewRect;
    viewRect.set(static_cast<float>(rect.left), static_cast<float>(rect.top), static_cast<float>(rect.right), static_cast<float>(rect.bottom));

    return viewRect;
}


bool RendererPlugin::SwapChain::create(CComPtr<IDirect3DDevice9> device)
{
    D3DPRESENT_PARAMETERS presentParams;
    ZeroMemory(&presentParams, sizeof(presentParams));
    presentParams.BackBufferFormat = D3DFMT_UNKNOWN;
    presentParams.BackBufferCount = 1;
    presentParams.EnableAutoDepthStencil = FALSE;
    presentParams.MultiSampleType = D3DMULTISAMPLE_NONE;
    presentParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
    presentParams.hDeviceWindow = static_cast<HWND>(handle);
    presentParams.Windowed = TRUE;

    HRESULT hr = device->CreateAdditionalSwapChain(&presentParams, &buffer);
    if (FAILED(hr))
    {
        HR_PRINT(hr);
        *this = SwapChain();
        return false;
    }

    ZeroMemory(&presentParams, sizeof(presentParams));
    hr = buffer->GetPresentParameters(&presentParams);
    if (FAILED(hr))
    {
        HR_PRINT(hr);
        *this = SwapChain();
        return false;
    }

    hr = device->CreateDepthStencilSurface(presentParams.BackBufferWidth, presentParams.BackBufferHeight, D3DFMT_D24S8, presentParams.MultiSampleType, 0, FALSE, &dsBuffer, NULL);
    if (FAILED(hr))
    {
        HR_PRINT(hr);
        *this = SwapChain();
        return false;
    }

    return true;
}


bool RendererPlugin::SwapChain::activate(CComPtr<IDirect3DDevice9> device)
{
    CComPtr<IDirect3DSurface9> backBuffer;
    HRESULT hr = buffer->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &backBuffer);
    if (FAILED(hr))
    {
        HR_PRINT(hr);
        errorTries--;
        return false;
    }

    hr = device->SetRenderTarget(0, backBuffer);
    if (FAILED(hr))
    {
        HR_PRINT(hr);
        errorTries--;
        return false;
    }

    hr = device->SetDepthStencilSurface(dsBuffer);
    if (FAILED(hr))
    {
        HR_PRINT(hr);
        errorTries--;
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

    hr = device->SetViewport(&vp);
    if (FAILED(hr))
    {
        HR_PRINT(hr);
        errorTries--;
        return false;
    }

    return true;
}

} // namespace eg


//-------------------------------------------------------------------------------------
// yaget plugin exported functions
extern "C" __declspec(dllexport) IPluginObject *get_plugin(const Version& version, const Environment *environment)
{
    version;
    if (!rendererPlugin)
    {
        IAsset::Token script = environment->get<IAsset::Token>("Script");
        AssetLoadSystem& als = environment->get_ref<AssetLoadSystem>("als");

        rendererPlugin.reset(new RendererPlugin(script, als));
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
