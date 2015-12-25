#include "Precompiled.h"
#include "Renderer.h"
#include "ComponentEnumsRender.h"
#include "IComponentView.h"
#include "Interface/ICollision.h"
#include "Config/ConfigHelper.h"
#include "IObjectManager.h"
#include "IComponentModel.h"
#include "Math/Matrix.h"
#include "MessageInterface.h"
#include "Input/InputManager.h"
#include "ErrorHandler.h"
#include "File/VirtualFileSystem.h"
#include "Message/Dispatcher.h"
#include "YagetVersion.h"
#include "Widgets/Presenters.h"
#include <wx/confbase.h>
#include <wx/dynload.h>
#include <wx/dir.h>
#include <wx/filename.h>
#include <ximage.h>
#include <d3dx9math.h>
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
//#define WIN32_LEAN_AND_MEAN
//#define WIN32_EXTRA_LEAN
#include <windows.h>


namespace {

    #include "Render/MissingGeometry.h"
    #include "Render/MissingTexture.h"
    #include "Render/MissingMaterial.h"
    #include "Render/MissingShader.h"

    struct MatchViewToken
    {
        MatchViewToken(WXWidget widgetOwner) : mWidgetOwner(widgetOwner)
        {
        }

        template <typename T>
        bool operator() (const T& value)
        {
            return value.mWidgetOwner == mWidgetOwner;
        }

        WXWidget mWidgetOwner;
    };

} // namespace


namespace eg {


IMPLEMENT_USER_EXPORTED_PLUGGABLE_CLASS(Renderer, IRenderer)

CComPtr<IDirect3DDevice9> Renderer::mD3DDevice;

Renderer::Renderer()
: mCurrViewToken(-1)
, mNewFont("Lucida Console", 10, false)
{
    wxLogTrace(TR_RENDER, "Renderer Created.");
    Dispatcher& disp = REGISTRATE(Dispatcher);
    mShutdownConnection = disp[mtype::kShutdownBegin].connect(boost::bind(&Renderer::onShutdown, this, _1));
}


Renderer::~Renderer()
{
    mShutdownConnection.disconnect();
    registrate::RegisterObject(boost::any(), "Widgets.TrianglePresenter");
    registrate::RegisterObject(boost::any(), "Widgets.LinePresenter");
    registrate::RegisterObject(boost::any(), "Renderer");
    mD3DDevice = 0;
    wxLogTrace(TR_RENDER, "Renderer Deleted.");
}


const std::string& Renderer::GetName() const
{
    static std::string staticName("Renderer");
    return staticName;
}


void Renderer::onShutdown(Message& /*msg*/)
{
    mMissingTexture = "";
    mMissingGeometry = "";
    mMissingMaterial = "";
    mMissingShader = "";
}


IPluginObject::eInitReturnCode Renderer::OnInit(int32_t initPass)
{
    if (initPass == 0)
    {
        //__asm int 3;

        wxASSERT(mD3DDevice == 0);

        RenderInitData renderInitData;
        Message(mtype::kGetRendererInitData, &renderInitData).Send();
        if (!renderInitData.mWidgetHandle)
        {
            wxLogError("Renderer Plugin did not receive valid WidgetHandle.");
            return IPluginObject::ircError;
        }

        D3DPRESENT_PARAMETERS presentParam;
        ZeroMemory(&presentParam, sizeof(presentParam));

        // init in full screen mode or use window
        presentParam.Windowed = renderInitData.mFlags & IRenderer::rfFullScreenMode ? FALSE : TRUE;
        presentParam.PresentationInterval = renderInitData.mFlags & IRenderer::rfFreeRefreshMode ? D3DPRESENT_INTERVAL_IMMEDIATE : D3DPRESENT_INTERVAL_DEFAULT;

        // we only use size if we are in full screen mode
        if (presentParam.Windowed == FALSE)
        {
            // use default values if there is none set
            presentParam.BackBufferWidth = renderInitData.mResX ? renderInitData.mResX : 800;
            presentParam.BackBufferHeight = renderInitData.mResY ? renderInitData.mResY : 600;
        }

        presentParam.BackBufferCount = 1;
        presentParam.SwapEffect = D3DSWAPEFFECT_DISCARD;
        presentParam.EnableAutoDepthStencil = TRUE;
        presentParam.AutoDepthStencilFormat = D3DFMT_D24S8;
        // this will be adjusted below if we are in full screen mode
        presentParam.BackBufferFormat = D3DFMT_UNKNOWN;

        UINT adapterToUse = D3DADAPTER_DEFAULT;
        D3DDEVTYPE deviceType = D3DDEVTYPE_HAL;
        DWORD behaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
        if (presentParam.Windowed == TRUE)
        {
            presentParam.hDeviceWindow = static_cast<HWND>(renderInitData.mWidgetHandle);
            // we only allow any kind of debugging in window mode only
            if (0)//eg::config::ReadBool(wxConfigBase::Get(false), _T("Debug/Render/DebugShaders")))
            {
                deviceType = D3DDEVTYPE_REF;
                behaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
                presentParam.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

                wxLogTrace(TR_RENDER, "Using Debug flags for Vertex and Pixel Shaders... accept very slow performance.");
            }
            else
            {
                for (UINT adapter = 0; adapter < RenderModule::D3D->GetAdapterCount(); adapter++)
                {
                    D3DADAPTER_IDENTIFIER9 identifier;
                    HRESULT result = RenderModule::D3D->GetAdapterIdentifier(adapter, 0, &identifier);
                    if (HR_IS_VALID(result))
                    {
                        wxLogTrace(TR_RENDER, "Adapter id: %d, Name: '%s', Driver: '%s', Description: '%s'", adapter, identifier.DeviceName, identifier.Driver, identifier.Description);
                        std::string description(identifier.Description);
                        if (boost::algorithm::iequals(description, std::string("NVIDIA PerfHUD")))
                        {
                            wxLogTrace(TR_RENDER, "Creating device for use with NVIDIA PerfHUD.");
                            adapterToUse = adapter;
                            deviceType = D3DDEVTYPE_REF;
                            break;
                        }
                    }
                }
            }
        }

        if (deviceType == D3DDEVTYPE_HAL)
        {
            // let's find out which monitor we are going to use
            //adapterToUse = GetValidMonitor();
            if (presentParam.Windowed == FALSE)
            {
                if (checkDeviceType(adapterToUse, D3DFMT_X8R8G8B8, D3DFMT_A8R8G8B8))
                {
                    presentParam.BackBufferFormat = D3DFMT_A8R8G8B8;
                }
            }
        }

        //if (eg::config::ReadBool(wxConfigBase::Get(false), _T("Render/Multhitreaded")))
        {
            behaviorFlags |= D3DCREATE_MULTITHREADED;
        }

        HRESULT result = RenderModule::D3D->CreateDevice(adapterToUse,
                                                         deviceType,
                                                         static_cast<HWND>(renderInitData.mWidgetHandle),
                                                         behaviorFlags,
                                                         &presentParam,
                                                         &mD3DDevice);
        if (!HR_IS_VALID(result))
        {
            return IPluginObject::ircError;
        }

        D3DVIEWPORT9 vp;
        vp.X      = 0;
        vp.Y      = 0;
        vp.Width  = renderInitData.mResX;
        vp.Height = renderInitData.mResY;
        vp.MinZ   = 0.0f;
        vp.MaxZ   = 1.0f;

        mD3DDevice->SetViewport(&vp);
        mD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

        SwapChain swapChain;
        mD3DDevice->GetSwapChain(0, &swapChain.mSwapChain);
        mD3DDevice->GetDepthStencilSurface(&mMainDepthSurface);
        mD3DDevice->GetRenderTarget(0, &mMainRenderSurface);
        swapChain.mWidgetOwner = renderInitData.mWidgetHandle;
        mViewTokens.push_back(swapChain);

        // create and register all widget presenters
        registerPresenters();

        // Before we go any further, let' make sure that we have all default assets loaded
        setDefaultAssets();

        registrate::RegisterObject(static_cast<IRenderer *>(this), wxT("Renderer"));
        //! We are going to blast this message to all created components
        REGISTRATE(IObjectManager).BroadcastEvent(Message(mtype::kRenderValid));
        Message(mtype::kRendererVideoModeChanged, &renderInitData).Send();

        wxLogTrace(TR_RENDER, "Renderer Plugin Initialized at Pass %d.", initPass);
    }

    return mD3DDevice ? IPluginObject::ircDone : IPluginObject::ircError;
}


void Renderer::registerPresenters()
{
    //! \note how do we allow dynamic/data configuration of presenters?
    mLinePresenter.reset(new widgets::LinePresenter);
    registrate::RegisterObject(static_cast<widgets::LinePresenter *>(mLinePresenter.get()), "Widgets.LinePresenter");

    mTrianglePresenter.reset(new widgets::TrianglePresenter);
    registrate::RegisterObject(static_cast<widgets::TrianglePresenter *>(mTrianglePresenter.get()), "Widgets.TrianglePresenter");
}


void Renderer::setDefaultAssets()
{
    //-------------------------------------------------------------------------------------
    /*
    // texture asset
    const int kSizeX = 8;
    const int kSizeY = 8;
    CxImage image(kSizeX, kSizeY, 24, CXIMAGE_SUPPORT_TGA);
    BYTE *pPixels = image.GetBits();
    for (int y = 0; y < kSizeY; y++)
    {
        for (int x = 0; x < kSizeX; x++)
        {
            pPixels[(((y*kSizeX)+x)*3)] = 255;  // b
            pPixels[(((y*kSizeX)+x)*3)+1] = 0;  // g
            pPixels[(((y*kSizeX)+x)*3)+2] = 0;  // r
        }
    }

    BYTE *buffer = 0;
    long size;
    if (image.Encode(buffer, size, CXIMAGE_SUPPORT_TGA))
    {
        VirtualFileSystem& vfs = REGISTRATE(VirtualFileSystem);
        if (VirtualFileFactory::ostream_t ostream = vfs.AttachFileStream("Missing.tgad"))
        {
            ostream->write((const char *)buffer, size);
        }
        else
        {
            wxLogError("Could not get 'Missing.tgad' stream for default asset.");
        }
        image.FreeMemory(buffer);
    }
    */
    VirtualFileSystem& vfs = REGISTRATE(VirtualFileSystem);
    if (!vfs.IsFileStreamExist(missingasset::GeometryTexture))
    {
        if (VirtualFileFactory::ostream_t ostream = vfs.AttachFileStream(missingasset::GeometryTexture, true))
        {
            ostream->write((const char *)MissingTexture_abRawBlock, MissingTexture_BLOCK_SIZE);
        }
        else
        {
            wxLogError("Could not get %s' stream for default asset.", missingasset::GeometryTexture);
        }
    }

    mMissingTexture = missingasset::GeometryTexture;
    //-------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------
    // geometry asset
    if (!vfs.IsFileStreamExist(missingasset::Geometry))
    {
        if (VirtualFileFactory::ostream_t ostream = vfs.AttachFileStream(missingasset::Geometry, true))
        {
            ostream->write((const char *)MissingGeometry_abRawBlock, MissingGeometry_BLOCK_SIZE);
        }
        else
        {
            wxLogError("Could not get '%s' stream for default asset.", missingasset::Geometry);
        }
    }

    mMissingGeometry = missingasset::Geometry;
    //-------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------
    // material asset
    if (!vfs.IsFileStreamExist(missingasset::Material))
    {
        if (VirtualFileFactory::ostream_t ostream = vfs.AttachFileStream(missingasset::Material, true))
        {
            ostream->write((const char *)MissingMaterial_abRawBlock, MissingMaterial_BLOCK_SIZE);
        }
        else
        {
            wxLogError("Could not get '%s' stream for default asset.", missingasset::Material);
        }
    }

    mMissingMaterial = missingasset::Material;
    //-------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------
    // shader asset
    if (!vfs.IsFileStreamExist(missingasset::Shader))
    {
        if (VirtualFileFactory::ostream_t ostream = vfs.AttachFileStream(missingasset::Shader, true))
        {
            ostream->write((const char *)MissingShader_abRawBlock, MissingShader_BLOCK_SIZE);
        }
        else
        {
            wxLogError("Could not get '%s' stream for default asset.", missingasset::Shader);
        }
    }

    mMissingShader = missingasset::Shader;
    //-------------------------------------------------------------------------------------
}


void Renderer::Reset()
{
    // give everybody a chance to reset themselves
    Message(mtype::kRendererVideoModeReset).Send();

    mMainDepthSurface = 0;
    mMainRenderSurface = 0;
    mLineRenderer = 0;
    mTextRenderer = 0;

    typedef SwapChains_t::iterator it_SC;
    for (it_SC it = mViewTokens.begin(); it != mViewTokens.end(); ++it)
    {
        SwapChain& swapChain = *it;
        swapChain.mDepthStencilTexture = 0;
        swapChain.mSwapChain = 0;
    }

    // recreate device here
    RenderInitData renderInitData;
    Message(mtype::kGetRendererInitData, &renderInitData).Send();
    wxASSERT(renderInitData.mWidgetHandle);

    D3DPRESENT_PARAMETERS presentParam;
    ZeroMemory(&presentParam, sizeof(presentParam));

    // init in full screen mode or use window
    presentParam.Windowed = renderInitData.mFlags & IRenderer::rfFullScreenMode ? FALSE : TRUE;
    presentParam.PresentationInterval = renderInitData.mFlags & IRenderer::rfFreeRefreshMode ? D3DPRESENT_INTERVAL_IMMEDIATE : D3DPRESENT_INTERVAL_DEFAULT;

    // we only use size if we are in full screen mode
    if (presentParam.Windowed == FALSE)
    {
        // use default values if there is none set
        presentParam.BackBufferWidth = renderInitData.mResX ? renderInitData.mResX : render::DefaultResX;
        presentParam.BackBufferHeight = renderInitData.mResY ? renderInitData.mResY : render::DefaultResY;
    }

    presentParam.BackBufferCount = 1;
    presentParam.SwapEffect = D3DSWAPEFFECT_DISCARD;
    presentParam.EnableAutoDepthStencil = TRUE;
    presentParam.AutoDepthStencilFormat = D3DFMT_D24S8;
    // this will be adjusted below if we are in full screen mode
    presentParam.BackBufferFormat = D3DFMT_UNKNOWN;

    UINT adapterToUse = D3DADAPTER_DEFAULT;
    D3DDEVTYPE deviceType = D3DDEVTYPE_HAL;
    /*DWORD behaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;*/
    presentParam.hDeviceWindow = static_cast<HWND>(renderInitData.mWidgetHandle);
    if (presentParam.Windowed == TRUE)
    {
        // we only allow any kind of debugging in window mode only
        if (eg::config::ReadBool(wxConfigBase::Get(false), _T("Debug/Render/DebugShaders")))
        {
            presentParam.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
        }
    }

    if (deviceType == D3DDEVTYPE_HAL)
    {
        // let's find out which monitor we are going to use
        //adapterToUse = GetValidMonitor();
        if (presentParam.Windowed == FALSE)
        {
            if (checkDeviceType(adapterToUse, D3DFMT_X8R8G8B8, D3DFMT_A8R8G8B8))
            {
                presentParam.BackBufferFormat = D3DFMT_A8R8G8B8;
            }
        }
    }


    HRESULT hr = mD3DDevice->Reset(&presentParam);
    if (!HR_IS_VALID(hr))
    {
        return;
    }

    D3DVIEWPORT9 vp;
    vp.X      = 0;
    vp.Y      = 0;
    vp.Width  = renderInitData.mResX;
    vp.Height = renderInitData.mResY;
    vp.MinZ   = 0.0f;
    vp.MaxZ   = 1.0f;

    mD3DDevice->SetViewport(&vp);
    mD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

    wxASSERT(!mViewTokens.empty());

    SwapChain& swapChain = *mViewTokens.begin();
    mD3DDevice->GetSwapChain(0, &swapChain.mSwapChain);
    mD3DDevice->GetDepthStencilSurface(&mMainDepthSurface);
    mD3DDevice->GetRenderTarget(0, &mMainRenderSurface);
    swapChain.mWidgetOwner = renderInitData.mWidgetHandle;

    // we only allow extra swap chain if we are in window mode
    wxASSERT((presentParam.Windowed == FALSE && mViewTokens.size() == 1) || presentParam.Windowed == TRUE);

    for (it_SC it = mViewTokens.begin() + 1; it != mViewTokens.end(); ++it)
    {
        SwapChain& swapChain = *it;
        updateSwapChain(swapChain);
    }

    Message(mtype::kRendererVideoModeChanged, &renderInitData).Send();

    wxLogTrace(TR_RENDER, "Renderer Plugin Reset Successfully.");
}


bool Renderer::checkDeviceType(UINT adapter, D3DFORMAT displayFormat, D3DFORMAT backBuferFormat)
{
    HRESULT hr = RenderModule::D3D->CheckDeviceType(adapter, D3DDEVTYPE_HAL, displayFormat, backBuferFormat, FALSE);
    return HR_IS_VALID(hr);
}


bool Renderer::BeginScene() const
{
    if (mD3DDevice)
    {
        HRESULT result = mD3DDevice->BeginScene();
        return HR_IS_VALID(result);
    }

    return false;
}


void Renderer::EndScene() const
{
    if (mD3DDevice)
    {
        mD3DDevice->EndScene();
    }
}


void Renderer::renderLines() const
{
    // Before calling this method, BeginScene has to be called.
    if (!mLines.empty())
    {
        if (!mLineRenderer)
        {
            HRESULT result = D3DXCreateLine(mD3DDevice, &mLineRenderer);
            HR_IS_VALID(result);
        }

        if (mLineRenderer)
        {
            mD3DDevice->SetTransform(D3DTS_WORLD, (D3DXMATRIX *)m44::Identity().GetF());
            mD3DDevice->SetTransform(D3DTS_VIEW, (D3DXMATRIX *)m44::Identity().GetF());
            mD3DDevice->SetTransform(D3DTS_PROJECTION, (D3DXMATRIX *)m44::Identity().GetF());

            mLineRenderer->Begin();
            for (std::vector<Line>::const_iterator it = mLines.begin(); it != mLines.end(); ++it)
            {

                const Color4_t& lineColor = (*it).mBegColor;
                D3DCOLOR color = D3DCOLOR_COLORVALUE(lineColor.x, lineColor.y, lineColor.z, lineColor.w);

                if ((*it).mUseMatrix)
                {
                    D3DXVECTOR3 points[2];
                    points[0].x = (*it).mBegPos.x;
                    points[0].y = (*it).mBegPos.y;
                    points[0].z = (*it).mBegPos.z;
                    points[1].x = (*it).mEndPos.x;
                    points[1].y = (*it).mEndPos.y;
                    points[1].z = (*it).mBegPos.z;
                    mLineRenderer->DrawTransform(points, 2, (D3DXMATRIX *)((*it).mMatrix.GetF()), color);
                }
                else
                {
                    D3DXVECTOR2 points[2];
                    points[0].x = (*it).mBegPos.x;
                    points[0].y = (*it).mBegPos.y;
                    points[1].x = (*it).mEndPos.x;
                    points[1].y = (*it).mEndPos.y;

                    mLineRenderer->Draw(points, 2, color);
                }
            }

            mLineRenderer->End();
        }
        mLines.clear();
    }
}


void Renderer::renderText() const
{
    // Before calling this method, BeginScene has to be called.
    if (!mText.empty())
    {
        if (!mTextRenderer || mNewFont.bCreate)
        {
            std::string fontName = mNewFont.Name;
            int size = mNewFont.Size;
            UINT bold = mNewFont.bBold;
            mNewFont.bCreate = false;
            mTextRenderer = 0;

            HRESULT result = D3DXCreateFont(mD3DDevice, size, 0, bold, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_DONTCARE, fontName.c_str(), &mTextRenderer);
            HR_IS_VALID(result);
        }

        if (mTextRenderer)
        {
            RECT rc;
            for (std::vector<Text>::const_iterator it = mText.begin(); it != mText.end(); ++it)
            {
                rc.left = (*it).mPosX;
                rc.top = (*it).mPosY;
                rc.right = 0;
                rc.bottom = 0;
                D3DCOLOR color = D3DCOLOR_COLORVALUE((*it).mColor.x, (*it).mColor.y, (*it).mColor.z, (*it).mColor.w);

                mTextRenderer->DrawText(NULL, (*it).mText.c_str(), -1, &rc, DT_CALCRECT, color);
                mTextRenderer->DrawText(NULL, (*it).mText.c_str(), -1, &rc, 0, color);
            }
        }

        mText.clear();
    }
}


void Renderer::Flip() const
{
    if (mD3DDevice)
    {
        if (!mLines.empty() || !mText.empty())
        {
            BeginScene();
            renderLines();
            renderText();
            EndScene();
        }

        HRESULT result;
        if (mCurrViewToken > -1 && mCurrViewToken < static_cast<int32_t>(mViewTokens.size()))
        {
            result = mViewTokens[mCurrViewToken].mSwapChain->Present(NULL, NULL, NULL, NULL, 0);
        }
        else
        {
            result = mD3DDevice->Present(NULL, NULL, NULL, NULL);
        }

        HR_IS_VALID(result);
    }
}


void Renderer::Tick(float /*deltaTime*/)
{
}


void Renderer::updateSwapChain(SwapChain& swapChain)
{
    D3DPRESENT_PARAMETERS presentParam;
    ZeroMemory(&presentParam, sizeof(presentParam));

    presentParam.hDeviceWindow = static_cast<HWND>(swapChain.mWidgetOwner);
    presentParam.Windowed = TRUE;
    presentParam.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
    presentParam.BackBufferCount = 1;
    presentParam.SwapEffect = D3DSWAPEFFECT_DISCARD;
    presentParam.BackBufferFormat = D3DFMT_UNKNOWN;
    if (config::ReadBool(_T("Debug/Render/DebugShaders")))
    {
        presentParam.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
    }

    swapChain.mDepthStencilTexture = 0;
    swapChain.mSwapChain = 0;
    HRESULT hr = mD3DDevice->CreateAdditionalSwapChain(&presentParam, &swapChain.mSwapChain);
    if (HR_IS_VALID(hr))
    {

        ZeroMemory(&presentParam, sizeof(presentParam));
        hr = swapChain.mSwapChain->GetPresentParameters(&presentParam);
        wxSize size(presentParam.BackBufferWidth, presentParam.BackBufferHeight);
        hr = mD3DDevice->CreateTexture(size.x,
                                       size.y,
                                       1,
                                       D3DUSAGE_DEPTHSTENCIL,
                                       D3DFMT_D24S8,
                                       D3DPOOL_DEFAULT,
                                       &swapChain.mDepthStencilTexture,
                                       0);
    }
}


/*
IRenderer::ViewToken Renderer::CreateViewToken(const wxWindow *pWindow)
{
    wxASSERT(pWindow);
    return createViewToken(pWindow->GetHandle());
}
*/


IRenderer::ViewToken Renderer::CreateViewToken(void *winh)
{
    wxASSERT_MSG(!mViewTokens.empty(), "CreateViewToken called before any 3D Device created.");

    // if this is the main window, it has a always the same index
    if (mViewTokens[0].mWidgetOwner == winh || !winh)
    {
        IRenderer::ViewToken viewToken;
        viewToken.index = 0;
        return viewToken;
    }

    // find existing swap chain for this window
    int32_t indexVT = FindSwapChain(winh);
    if (indexVT == -1)
    {
        SwapChain swapChain;
        swapChain.mWidgetOwner = winh;
        mViewTokens.push_back(swapChain);
    }

    indexVT = FindSwapChain(winh);
    wxASSERT(indexVT > -1 && indexVT < static_cast<int32_t>(mViewTokens.size()));
    SwapChain& swapChain = mViewTokens[indexVT];
    updateSwapChain(swapChain);

    IRenderer::ViewToken viewToken;
    viewToken.index = indexVT;
    return viewToken;
}


bool Renderer::UseViewToken(ViewToken viewToken)
{
    if (mCurrViewToken == viewToken.index)
    {
        return true;
    }

    if (IsViewTokenValid(viewToken) && viewToken.index < static_cast<int32_t>(mViewTokens.size()))
    {
        // this is our main view token own by main ap window
        if (viewToken.index == 0)
        {
            wxASSERT(mMainRenderSurface);
            wxASSERT(mMainDepthSurface);

            HRESULT hr;
            hr = mD3DDevice->SetRenderTarget(0, mMainRenderSurface);
            HR_IS_VALID(hr);
            hr = mD3DDevice->SetDepthStencilSurface(mMainDepthSurface);
            HR_IS_VALID(hr);
        }
        else
        {
            HRESULT hr;
            // set back buffer (color buffer)
            CComPtr<IDirect3DSurface9> pBackBuffer;
            hr = mViewTokens[viewToken.index].mSwapChain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
            HR_IS_VALID(hr);
            hr = mD3DDevice->SetRenderTarget(0, pBackBuffer);
            HR_IS_VALID(hr);

            // set depth/stencil buffer
            CComPtr<IDirect3DSurface9> pDepthBuffer;
            mViewTokens[viewToken.index].mDepthStencilTexture->GetSurfaceLevel(0, &pDepthBuffer);
            hr = mD3DDevice->SetDepthStencilSurface(pDepthBuffer);
        }

        mCurrViewToken = viewToken.index;
        return true;
    }

    return false;
}


/*
const Texture::Info& Renderer::GetBackBufferInfo() const
{
static Texture::Info backBufferInfo;
if (mCurrViewToken == -1 || mCurrViewToken == 0)
{
    // just grab the main render surface which is always created
    D3DSURFACE_DESC desc;
    HRESULT hr = mMainRenderSurface->GetDesc(&desc);
    if (SUCCEEDED(hr))
    {
        backBufferInfo.mWidth = desc.Width;
        backBufferInfo.mHeight = desc.Height;
    }
}
else
{
    wxASSERT(mCurrViewToken < static_cast<int32_t>(mViewTokens.size()));
    CComPtr<IDirect3DSurface9> pBackBuffer;
    HRESULT hr = mViewTokens[mCurrViewToken].mSwapChain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
    if (SUCCEEDED(hr))
    {
        D3DSURFACE_DESC desc;
        hr = pBackBuffer->GetDesc(&desc);
        if (SUCCEEDED(hr))
        {
            backBufferInfo.mWidth = desc.Width;
            backBufferInfo.mHeight = desc.Height;
        }
    }
}

return backBufferInfo;
}
*/

const Version Renderer::GetVersion() const
{
    return YagetVersion;
}


int32_t Renderer::FindSwapChain(const WXWidget& widgetOwner) const
{
    SwapChains_t::const_iterator it = std::find_if(mViewTokens.begin(), mViewTokens.end(), MatchViewToken(widgetOwner));
    if (it != mViewTokens.end())
    {
        return it - mViewTokens.begin();
    }

    // there is no swap chain for this widget created
    return -1;
}


void Renderer::DrawLine(const Vector3& begPos, const Vector3& endPos, const Color4_t& begColor, const Color4_t& endColor, const Matrix44 *pMatrix)
{
    mLines.push_back(Line(begPos, endPos, begColor, endColor, pMatrix));
}


void Renderer::Print(int32_t x, int32_t y, const std::string& text, const Color4_t& color)
{
    mText.push_back(Text(x, y, text, color));
}


collision::Response Renderer::GetHitObjects(uint64_t viewObjectId, const Vector2& mousePos, bool bGetAllObjects) const
{
    if (ICollision *pICollision = registrate::p_cast<ICollision>("Collision"))
    {
        if (IComponentView *pView = component::get<IComponentView>(viewObjectId))
        {
            collision::Request request;
            request.ray = pView->GetRayAtCursor(mousePos);
            request.allHits = bGetAllObjects;
            return pICollision->HitObjects(request);
        }
    }

    return collision::Response();
}


collision::Response Renderer::GetHitObjects(const std::string& viewObjectName, const Vector2& mousePos, bool bGetAllObjects) const
{
    ComponentListCallback<IComponentView> cameras;
    while (IComponentView *pView = cameras.Next())
    {
        if (pView->OI().Name == viewObjectName)
        {
            return GetHitObjects(pView->OI().Id, mousePos, bGetAllObjects);
        }
    }

    return collision::Response();
}


void Renderer::SetFont(const std::string& fontName, size_t size, bool bBold)
{
    mNewFont.Name = fontName;
    mNewFont.Size = size;
    mNewFont.bBold = bBold;
    mNewFont.bCreate = true;
}

} // namespace eg

