#include "Precompiled.h"
#include "ComponentCamera.h"
#include "RenderContext.h"
#include "Renderer.h"
#include "Timer/ClockManager.h"
#include "IEditorMessages.h"

//#include <AssetResource/AssetResourceFactory.h>
//#include <BoolControlValidator.h>
//#include <Components/IComponentEntity.h>
//#include <Components/IComponentModel.h>
//#include <IGameGui.h>
//#include <IMaterialResource.h>
//#include <IShaderResource.h>
//#include <ITextureUserResource.h>
//#include <ObjectData/ObjectDataParser.h>
//#include <ObjectMessage.h>

namespace
{
    //! mpData will point to Vector3
    const guid_t kEditorUpdatePostProcessShader = 0x11297145;

    /*
    struct RenderCallback : public eg::EnumComponentSafeCallback<eg::IComponentModel>
    {
        RenderCallback(eg::IRenderContext *pRenderContext) :
            mpRenderContext(pRenderContext)
        {
        }

        virtual bool ProcessComponent(CompType_t *pComponent)
        {
            //! If camera id is 0, then all the object will alwyas be rendered with it.
            //! if component render id is 0, then it will render with all cameras.
            //! Otherwise we or'ed camera id and component id to see if component can be rendered with this camera
            if ((mpRenderContext->GetRenderId() == 0) ||
                (pComponent->GetRenderId()) == 0 ||
                (mpRenderContext->GetRenderId() & pComponent->GetRenderId()))
            {
                pComponent->Render(mpRenderContext);
            }

            return true;
        }

        eg::IRenderContext *mpRenderContext;
    };
    */
} // namespace


namespace eg {

EG_REGISTER_COMPONENT(ComponentCamera);


void DejaDescriptor(const ComponentCamera& D)
{
    DEJA_TYPE(D, ComponentCamera);     // Must provide the type.
    DEJA_FIELD(D.mRenderId);
    DEJA_FIELD(D.mCameraValues);
    DEJA_FIELD(D.mClearSurface);
    DEJA_FIELD(D.mClearSurfaceFlags);
    DEJA_FIELD(D.mClearSurfaceColor);
    DEJA_FIELD(D.mShowStats);
    DEJA_FIELD(D.mSortValue);
}


ComponentCamera::ComponentCamera() :
    mCameraValues(60.0f, 0.1f, 10000.0f),
    mRenderId(1),
    mViewMatrix(m44::Identity()),
    mSortValue(0.0f),
    mClearSurfaceColor(0.9f, 0.45f, 0.25f, 1.0f),
    mShowStats(false),
    mbRender(true),
    mClearSurface(true),
    mClearSurfaceFlags(csfAllBuffer)
{
    DEJA_OBJECT_INIT(*this);
    mProperty.SetGroupName(_T("Camera"));
    mProperty.AddProperty<Flags_Tag, uint32_t>(_T("Render Id"), &mRenderId, false);
    mProperty.AddProperty<bool, bool>(_T("Render Camera"), &mbRender, false);
    mProperty.AddProperty<CustomLabels_Tag, Vector3>(_T("Eye Values"), &mCameraValues, false);
    mProperty.AddProperty<bool, bool>(_T("Clear Surface"), &mClearSurface, false);
    mProperty.AddProperty<Flags_Tag, uint32_t>(_T("Clear Flags"), &mClearSurfaceFlags, false);//, boost::bind(&ComponentCamera::OnRenderModeChanged, this, _1));
    mProperty.AddProperty<Color4_Tag, Vector4>(_T("Clear Color"), &mClearSurfaceColor, false);
    //mProperty.AddProperty<bool, bool>(_T("Enable Post Process"), &mPostProcess.mActive, false);
    mProperty.AddProperty<bool, bool>(_T("Show Stats"), &mShowStats, false);
    mProperty.AddProperty<float, float>(_T("Sort Value"), &mSortValue, false);
    //mProperty.AddProperty<RenderTarget_Tag, std::string>(_T("Render Target"), &mRenderTargetName, false, boost::bind(&ComponentCamera::OnRenderTargetChanged, this, _1));
    //mProperty.AddProperty<std::vector<std::string>, std::vector<std::string> >(_T("Shaders"), &mPostProcess.mpShaderNames, true, boost::bind(&ComponentCamera::PostProcess::OnShadersChanged, &mPostProcess, _1));
}


ComponentCamera::~ComponentCamera()
{
    DEJA_OBJECT_KILL(*this);
}


void ComponentCamera::DeleteThis()
{
    delete this;
}


/*
bool ComponentCamera::Init(ObjectId oId, ParameterNode& paramNode)
{
    Shutdown();

    if (wxXmlNode *pNode = FindSection(&paramNode, _T("backbuffer")))
    {
        mClearSurface = GetPropertyBool(pNode, _T("clearsurface"), true);
        mClearSurfaceColor = GetPropertyV4(pNode, _T("clearcolor"), Vector4(0.9f, 0.45f, 0.25f, 1.0f));
        mClearSurfaceFlags = GetPropertyInt(pNode, _T("clearflags"));
        mRenderTargetName = GetProperty(pNode, _T("target"));
        mPostProcess.mActive = GetPropertyBool(pNode, _T("postprocess"), false);

    }
    if (wxXmlNode *pNode = FindSection(&paramNode, _T("pass")))
    {
        mRenderId = GetPropertyInt(pNode, _T("id"), mRenderId);
        mSortValue = GetPropertyFloat(pNode, _T("sort"), mSortValue);
        mbRender = GetPropertyBool(pNode, _T("render"), mbRender);

        if (wxXmlNode *pShadersNode = FindSection(&paramNode, _T("shaders")))
        {
            xml::vNodeList_t shaderList;
            if (FindAllSections(pShadersNode, _T("shader"), shaderList, false))
            {
                for (xml::cit_N it = shaderList.begin(); it != shaderList.end(); ++it)
                {
                    std::string shaderName = GetProperty(*it, _T("name"));
                    mPostProcess.mpShaderNames.push_back(shaderName);
                }
            }
        }
    }

    return true;
}
*/


void ComponentCamera::SetObjectId(ObjectId oId)
{
    DEJA_OBJECT_LABEL(*this, "%s", GetObjectManager().GetObjectName(oId).c_str());
    DEJA_POST(*this);
    IComponentCamera::SetObjectId(oId);
    mProperty.SetInstanceId(GetInstanceId(this));
}


void ComponentCamera::Shutdown()
{
    //mCurrentRenderTarget = "";
    //mMainRenderTarget = 0;

    //mPostProcess.Shutdown();
    mCameraValues.set(60.0f, 0.1f, 10000.0f);
    mRenderId = 1;
    mViewMatrix = m44::Identity();
    mSortValue = 0.0f;
    mClearSurfaceColor.set(0.9f, 0.45f, 0.25f, 1.0f);
    mClearSurface = true;
    mClearSurfaceFlags = csfAllBuffer;
}


/*
bool ComponentCamera::Load()
{
    wxASSERT(Renderer::mD3DDevice);

    // this is called also from setting new render target from editor
    // so make sure that if you add any other loading code here, there will be
    // no side effects of re-initializing this multiply times

    // we have custom render target here
    mCurrentRenderTarget = mRenderTargetName;

    if (mCurrentRenderTarget)
    {
        // we also need to grab main back buffer so we can set it
        mMainRenderTarget = 0;
        HRESULT hr = Renderer::mD3DDevice->GetRenderTarget(0, &mMainRenderTarget);
    }
    else
    {
        mMainRenderTarget = 0;
    }

    return true;
}
*/


Dispatcher::Result_t ComponentCamera::HandleMessage(Message& msg)
{
    switch (msg.mType)
    {
        case mtype::kObjectCreated:
            if (!Renderer::mD3DDevice)
            {
                // this message might be send before we initialized our renderer
                // so we just break here,
                // otherwise let it fall trough it
                break;
            }

        case mtype::kRenderValid:
            //Load();
            break;

        case mtype::kGetChoiceLabels:
        {
            typedef std::vector<std::string> StringVector_t;
            typedef std::pair<std::string, StringVector_t> Labels_t;

            Labels_t labels = msg.GetValue<Labels_t>();
            if (labels.first == "Eye Values")
            {
                StringVector_t labelNames;
                labelNames.push_back(_T("View Angle"));
                labelNames.push_back(_T("Near Plane"));
                labelNames.push_back(_T("Far Plane"));
                labels.second = labelNames;
                msg.mAnyData = labels;
            }
            else if (labels.first == "Clear Flags")
            {
                StringVector_t labelNames;
                labelNames.push_back(_T("Color"));
                labelNames.push_back(_T("Depth"));
                labelNames.push_back(_T("Stencil"));
                labels.second = labelNames;
                msg.mAnyData = labels;
            }

            break;
        }
    }
}


Hash ComponentCamera::GetComponentTypeId() const
{
    return ComponentType::kCamera;
}

uint32_t ComponentCamera::GetRenderId() const
{
    return mRenderId;
}


float ComponentCamera::GetSortValue() const
{
    return mSortValue;
}


bool ComponentCamera::RegisterComponentType()
{
    IComponentCamera::RegisterInterface(ComponentType::kCamera);
    InitComponentType(ComponentType::kCamera, _T("Camera"));

    GetObjectManager().SubscribeToMessageType(ComponentType::kCamera, mtype::kObjectCreated);
    GetObjectManager().SubscribeToMessageType(ComponentType::kCamera, mtype::kRenderValid);
    GetObjectManager().SubscribeToMessageType(ComponentType::kCamera, mtype::kGetEditorProperties);
    GetObjectManager().SubscribeToMessageType(ComponentType::kCamera, mtype::kGetChoiceLabels);

    return true;
}


void ComponentCamera::SetViewMatrix(const Matrix44& matrix)
{
    mViewMatrix = matrix;
}


void ComponentCamera::Render(IRenderer *pRenderer, WXWidget widgetHandle, float deltaTime, const wxRect *pSize)
{
    if (mbRender && Renderer::mD3DDevice)
    {
        wxASSERT(pRenderer);
        D3DVIEWPORT9 viewportDX;
        float aspectRatio = 800.0f/600.0f;

        // always fill viewportDX structure so we can use it later to actually force
        // setting of a viewport
        wxASSERT(pSize);
        if (pSize)
        {
            viewportDX.X      = pSize->x;
            viewportDX.Y      = pSize->y;
            viewportDX.Width  = pSize->width;
            viewportDX.Height = pSize->height;
            viewportDX.MinZ   = 0.0f;
            viewportDX.MaxZ   = 1.0f;

            aspectRatio = static_cast<float>(viewportDX.Width) / static_cast<float>(viewportDX.Height);
        }
        else
        {
            /*
            const Texture::Info& backBufferInfo = pRenderer->GetBackBufferInfo();
            viewportDX.X      = 0;
            viewportDX.Y      = 0;
            viewportDX.Width  = backBufferInfo.mWidth;
            viewportDX.Height = backBufferInfo.mHeight;
            viewportDX.MinZ   = 0.0f;
            viewportDX.MaxZ   = 1.0f;

            aspectRatio = static_cast<float>(viewportDX.Width) / static_cast<float>(viewportDX.Height);
            */
        }

        /*
        if (mCurrentRenderTarget)
        {
            const Texture::Info& surfaceInfo = mCurrentRenderTarget->GetInfo();
            viewportDX.X      = 0;
            viewportDX.Y      = 0;
            viewportDX.Width  = surfaceInfo.mWidth;
            viewportDX.Height = surfaceInfo.mHeight;
            viewportDX.MinZ   = 0.0f;
            viewportDX.MaxZ   = 1.0f;

            aspectRatio = static_cast<float>(viewportDX.Width) / static_cast<float>(viewportDX.Height);

            if (mPostProcess.IsPostProcess(mCurrentRenderTarget))
            {
                bool result = mPostProcess.SetRenderTarget();
                wxASSERT(result);
            }
            else
            {
                if (IDirect3DSurface9 *pRenderTarget = static_cast<IDirect3DSurface9 *>(mCurrentRenderTarget->GetSurfaceHandle()))
                {
                    HRESULT hr = Renderer::mD3DDevice->SetRenderTarget(0, pRenderTarget);
                }
            }
        }
        */

        Matrix44 projectionMatrix = PerspectiveMatrixLH44(mCameraValues.x, aspectRatio, mCameraValues.y, mCameraValues.z);
        RenderContext renderContext;
        renderContext.SetRenderId(mRenderId);
        renderContext.SetViewMatrix(mViewMatrix);
        renderContext.SetProjectionMatrix(projectionMatrix);

        //ComponentListCallback<IComponentModel> modelList;
        //eg::GetObjectManager().EnumObjectsByInterface(InterfaceType::kModel, modelList);
        //modelList.Sort();

        if (pRenderer->BeginScene())
        {
            Renderer::mD3DDevice->SetViewport(&viewportDX);
            if (mClearSurface)
            {
                // we could try to clear only section of requested rect
                uint32_t clearFlags = 0;
                clearFlags |= (mClearSurfaceFlags & csfColorBuffer) ? D3DCLEAR_TARGET : 0;
                clearFlags |= (mClearSurfaceFlags & csfDepthBuffer) ? D3DCLEAR_ZBUFFER : 0;
                clearFlags |= (mClearSurfaceFlags & csfStencilBuffer) ? D3DCLEAR_STENCIL : 0;

                DWORD clearColor = D3DCOLOR_COLORVALUE(mClearSurfaceColor.x, mClearSurfaceColor.y, mClearSurfaceColor.z, mClearSurfaceColor.w);
                uint32_t numRect = pSize ? 1 : 0;
                D3DRECT rect;
                rect.x1 = viewportDX.X;
                rect.y1 = viewportDX.Y;
                rect.x2 = viewportDX.X + viewportDX.Width;
                rect.y2 = viewportDX.Y + viewportDX.Height;
                D3DRECT *pClearRect = numRect ? &rect : 0;

                Renderer::mD3DDevice->Clear(numRect, pClearRect, clearFlags, clearColor, 1.0f, 0);
            }

            /*
            if (modelList.IsFilled())
            {
                while (IComponentModel *pModel = modelList.Next())
                {
                    uint32_t cameraId = renderContext.GetRenderId();
                    uint32_t modelId = pModel->GetRenderId();

                    if (cameraId & modelId)
                    {
                        pModel->Render(&renderContext);
                    }
                }
            }
            */

            pRenderer->EndScene();
        }

        /*
        if (mCurrentRenderTarget)
        {
            if (mPostProcess.IsActive())
            {
                mPostProcess.UpdateSourceTarget(pRenderer, mCurrentRenderTarget, &renderContext);
            }

            wxASSERT(mMainRenderTarget);
            HRESULT hr = Renderer::mD3DDevice->SetRenderTarget(0, mMainRenderTarget);
        }
        */

        /*
        if (mShowStats)
        {
            const int32_t kOffsetLeft = 10.0f;
            const int32_t kOffsetTop = 10.0f;

            std::string fpsText;
            fpsText.Printf(_T("FPS: %.2f"), REGISTRATE(ClockManager).GetFrameRate());
        }
        */
    }

    DEJA_POST(*this);
}

/*
void ComponentCamera::OnRenderTargetChanged(const IProperty *pProperty)
{
    wxASSERT((*pProperty->GetName()) == "Render Target");
    Load();
}


ComponentCamera::PostProcess::PostProcess() :
    mActive(false)
{
}


bool ComponentCamera::PostProcess::IsPostProcess(const ResourceHandle<ITextureUserResource>& sourceTarget)
{
    if (mActive && !mpShaderNames.empty())
    {
        //! do we need to create our screen quad object and we only will create this one
        // regardless if we remove post process later or not
        if (!GetObjectManager().QueryInterface(ObjectId("ScreenQuad"), InterfaceType::kModel))
        {
            // we don't have our screen quad object created yet
            // and we also need to have special material for this object
            // with mRenderTarget as a texture to use
            // so we clone the white one
            // and replace the texture to use this render target
            ResourceHandle<IMaterialResource> sourceMaterial(_T("white"));
            mMaterial = sourceMaterial.Clone(_T("PostProcess"));

            ObjectTemplateNode objectTemplate(_T("ScreenQuad.xml"));
            if (wxXmlNode *pTemplateNode = objectTemplate.GetNode())
            {
                UpdateObjectTemplate(pTemplateNode);
                ObjectId oid = GetObjectManager().CreateObject(pTemplateNode);

                // make sure to disable rendering of this model until we need it
                Message msg(MType::kRenderModel, reinterpret_cast<void *>(0));
                GetObjectManager().PostMessage(oid, msg);
            }
        }

        const Texture::Info& info = sourceTarget->GetInfo();
        // le't make sure that we have render targets created and it's of the same type
        if (mRenderTargetA == 0 || (mRenderTargetA && mRenderTargetA->GetInfo() != info))
        {
            // do this here twice, so as to not get overhead of creating string, since
            // this block will get call every frame
            // make sure that we created unique name for our scratch texture
            std::string uniqueName;
            uniqueName.Printf(_T("%d_%d_%d"), info.mWidth, info.mHeight, info.mFormat);
            mRenderTargetA = sourceTarget.Clone(_T("ScratchA") + uniqueName);
        }

        if ((mRenderTargetB == 0 && mpShaderNames.size() > 1) ||
            (mRenderTargetB && mpShaderNames.size() > 1 && mRenderTargetB->GetInfo() != info))
        {
            // do this here twice, so as to not get overhead of creating string, since
            // this block will get call every frame
            // make sure that we created unique name for our scratch texture
            std::string uniqueName;
            uniqueName.Printf(_T("%d_%d_%d"), info.mWidth, info.mHeight, info.mFormat);
            mRenderTargetB = sourceTarget.Clone(_T("ScratchB") + uniqueName);
        }
        else if (mRenderTargetB && mpShaderNames.size() == 1)
        {
            mRenderTargetB = "";
        }
    }
    else
    {
        mRenderTargetA = "";
        mRenderTargetB = "";
    }

    return IsActive();
}

bool ComponentCamera::PostProcess::SetRenderTarget()
{
    wxASSERT(mActive);
    wxASSERT(mRenderTargetA);

    const Texture::Info& surfaceInfo = mRenderTargetA->GetInfo();
    float aspectRatio = surfaceInfo.mWidth / surfaceInfo.mHeight;

    if (IDirect3DSurface9 *pRenderTarget = static_cast<IDirect3DSurface9 *>(mRenderTargetA->GetSurfaceHandle()))
    {
        HRESULT hr = Renderer::mD3DDevice->SetRenderTarget(0, pRenderTarget);
        return SUCCEEDED(hr);
    }

    return false;
}


bool ComponentCamera::PostProcess::UpdateSourceTarget(IRenderer *pRenderer, ITextureUserResource *pSourceTarget, RenderContext *pRenderContext)
{
    wxASSERT(mActive);
    wxASSERT(mRenderTargetA);
    wxASSERT(pRenderer);
    wxASSERT(pSourceTarget);
    wxASSERT(pRenderContext);

    // this is our source texture which is bing to material on the first pass
    ITextureUserResource *pTextureA = mRenderTargetA;
    // this is destination texture (render to), which will be bind to right before we start rendering screen quad
    ITextureUserResource *pTextureB = mpShaderNames.empty() ? (ITextureUserResource *)0 : (ITextureUserResource *)mRenderTargetB;

    if (IComponentModel *pModel = static_cast<IComponentModel *>(GetObjectManager().QueryInterface(ObjectId("ScreenQuad"), InterfaceType::kModel)))
    {
        Message msgRenderOn(MType::kRenderModel, reinterpret_cast<void *>(1));
        pModel->HandleMessage(msgRenderOn);

        bool bNeedReferenceShaders = mPostProcesShaders.empty() ? true : false;
        for (std::vector<std::string>::iterator it = mpShaderNames.begin(); it != mpShaderNames.end(); ++it)
        {
            // set material to use specific texture
            mMaterial->SetTexture(ShaderBinds::kBaseTexture, pTextureA);


            // first, we need to set our model to use new shader
            if (bNeedReferenceShaders)
            {
                mPostProcesShaders.push_back(ResourceHandle<IShaderResource>(*it));
            }

            Message msg(MType::kSetShader, &(*it));
            pModel->HandleMessage(msg);

            if (it + 1 == mpShaderNames.end())
            {
                pTextureB = pSourceTarget;
            }

            // set next texture as render target
            IDirect3DSurface9 *pRenderTarget = static_cast<IDirect3DSurface9 *>(pTextureB->GetSurfaceHandle());
            HRESULT hr = Renderer::mD3DDevice->SetRenderTarget(0, pRenderTarget);

            // clear render target
            // \note, most likely we'll need to extract clear values from fx file using annotations
            DWORD clearColor = D3DCOLOR_COLORVALUE(0, 0, 0, 0);
            Renderer::mD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, clearColor, 1.0f, 0);

            if (SUCCEEDED(hr))
            {
                // and finally let's render our quad
                if (pRenderer->BeginScene())
                {
                    pModel->Render(pRenderContext);
                    pRenderer->EndScene();
                }
            }

            // now let's swap textures, so for the next pass
            // render target will become our material texture.
            Swap(pTextureA, pTextureB);
        }

        Message msgRenderOff(MType::kRenderModel, reinterpret_cast<void *>(0));
        pModel->HandleMessage(msgRenderOff);

        return true;
    }

    return false;
}


bool ComponentCamera::PostProcess::IsActive() const
{
    return mActive && mRenderTargetA && !mpShaderNames.empty();
}


EMessageResult ComponentCamera::PostProcess::HandleMessage(Message& msg)
{
    switch (msg.mType)
    {
        case kEditorUpdatePostProcessShader:
            if (std::vector<std::string> *pShaderNames = msg.GetValue<std::vector<std::string> *>())
            {
                mPostProcesShaders.clear();
                mpShaderNames = *pShaderNames;

                // if this is already active let's try to update our Model component with new shader
                if (IsActive())
                {
                    if (IComponent *pComponent = GetObjectManager().QueryInterface(ObjectId("ScreenQuad"), InterfaceType::kModel))
                    {
                        ObjectTemplateNode objectTemplate(_T("ScreenQuad.xml"));
                        if (wxXmlNode *pTemplateNode = objectTemplate.GetNode())
                        {
                            UpdateObjectTemplate(pTemplateNode);

                            if (wxXmlNode *pComponentNode = FindComponent(pTemplateNode, _T(""),  _T("Model")))
                            {
                                GetObjectManager().ReinitComponent(ObjectId("ScreenQuad"), pComponentNode);
                                int z = 0;
                            }
                        }
                    }
                }

                return MR_TRUE;
            }
            break;
    }

    return MR_IGNORED;
}


void ComponentCamera::PostProcess::OnShadersChanged(const Property *pProperty)
{
}


void ComponentCamera::PostProcess::UpdateObjectTemplate(wxXmlNode *pNode)
{
    // change material name to post process
    SetProperty(pNode, _T("material"), _T("name"), _T("PostProcess"));
    // and make sure that our render pass is unique
    //SetProperty(pNode, _T("pass"), _T("id"), _T("256"));
}


void ComponentCamera::PostProcess::Shutdown()
{
    mRenderTargetA = "";
    mRenderTargetB = "";
    mMaterial = "";
    mpShaderNames.clear();
    mPostProcesShaders.clear();
}
*/

} // namespace eg
