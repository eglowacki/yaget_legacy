#include "Precompiled.h"
#include "ComponentView.h"
#include "RenderContext.h"
#include "Renderer.h"
//#include "Timer/ClockManager.h"
#include "IEditorMessages.h"
#include "ErrorHandler.h"
#include "IComponentModel.h"
#include "IComponentEntity.h"

//#include <AssetResource/AssetResourceFactory.h>
//#include <BoolControlValidator.h>
//#include <Components/IComponentModel.h>
//#include <Components/IComponentEntity.h>
//#include <IGameGui.h>
//#include <IMaterialResource.h>
//#include <IShaderResource.h>
//#include <ITextureUserResource.h>
//#include <ObjectData/ObjectDataParser.h>
//#include <ObjectMessage.h>

namespace
{
    //! mpData will point to Vector3
    //const guid_t kEditorUpdatePostProcessShader = 0x11297145;

    /*
    struct RenderCallback : public eg::EnumComponentSafeCallback<eg::IComponentModel>
    {
        RenderCallback(const eg::IRenderContext& renderContext) :
            mRenderContext(renderContext)
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
                pComponent->Render(mRenderContext);
            }

            return true;
        }

        const eg::IRenderContext& mRenderContext;
    };
    */
} // namespace


namespace eg {

EG_REGISTER_COMPONENT(ComponentView);

static const uint32_t csfColorBuffer        = 0x0001; ///< clear color buffer only
static const uint32_t csfDepthBuffer        = 0x0002; ///< clear depth buffer only
static const uint32_t csfStencilBuffer      = 0x0004; ///< clear stencil buffer only
static const uint32_t csfOnlyClear          = 0x0008; ///< only clear back buffer and skip any other rendering for this view
static const uint32_t csfDepthStencilBuffer = csfDepthBuffer|csfStencilBuffer; ///< clear depth and stencil buffers together

ComponentView::ComponentView(component::ObjectId id)
: ComponentHelper(id, view::kGroup, ComponentHelper::ccAttached)
, mRenderId(1)
, mSortValue(0.0f)
, mShowStats(false)
, mbRender(true)
, mClearSurface(true)
//, mClearSurfaceFlags(csfAllBuffer)
, mClearSurfaceColor(get_prop<Color4_Tag, Vector4>(mProperty, view::kPropClearColor, Vector4(0.9f, 0.45f, 0.25f, 1.0f), ""))
, mClearFlags(get_prop<Flags_Tag, uint32_t>(mProperty, view::kPropClearFlags, csfAllBuffer, "{bitfield = Color, Depth, Stencil, OnlyClear}"))
, mRenderIdProperty(get_prop<Flags_Tag, uint32_t>(mProperty, view::kPropRenderId, 1, ""))
, mCameraValues(get_prop<CustomLabels_Tag, Vector3>(mProperty, view::kPropView, Vector3(60.0f, 0.1f, 10000.0f), "{labels = ViewAngle, Near, Far}"))
, mEyePosition(get_propV<Vector3>(mProperty, view::kPropEyePosition, v3::ZERO(), ""))
, mTargetPosition(get_propV<Vector3>(mProperty, view::kPropTargetPosition, v3::ZONE(), ""))
, mUpVector(get_propV<Vector3>(mProperty, view::kPropUpVector, v3::YONE(), ""))
{
    /*
    mProperty.SetGroupName(_T("Camera"));
    mProperty.AddProperty<Flags_Tag, uint32_t>(_T("Render Id"), &mRenderId, false);
    mProperty.AddProperty<bool, bool>(_T("Render Camera"), &mbRender, false);
    mProperty.AddProperty<bool, bool>(_T("Clear Surface"), &mClearSurface, false);
    mProperty.AddProperty<Flags_Tag, uint32_t>(_T("Clear Flags"), &mClearSurfaceFlags, false);//, boost::bind(&ComponentRender::OnRenderModeChanged, this, _1));
    mProperty.AddProperty<Color4_Tag, Vector4>(_T("Clear Color"), &mClearSurfaceColor, false);
    //mProperty.AddProperty<bool, bool>(_T("Enable Post Process"), &mPostProcess.mActive, false);
    mProperty.AddProperty<bool, bool>(_T("Show Stats"), &mShowStats, false);
    mProperty.AddProperty<float, float>(_T("Sort Value"), &mSortValue, false);
    //mProperty.AddProperty<RenderTarget_Tag, std::string>(_T("Render Target"), &mRenderTargetName, false, boost::bind(&ComponentRender::OnRenderTargetChanged, this, _1));
    //mProperty.AddProperty<std::vector<std::string>, std::vector<std::string> >(_T("Shaders"), &mPostProcess.mpShaderNames, true, boost::bind(&ComponentRender::PostProcess::OnShadersChanged, &mPostProcess, _1));
    */
}


ComponentView::~ComponentView()
{
}


void ComponentView::DeleteThis()
{
    delete this;
}


void ComponentView::Shutdown()
{
    //mCurrentRenderTarget = "";
    //mMainRenderTarget = 0;

    //mPostProcess.Shutdown();
    mCameraValues = Vector3(60.0f, 0.1f, 10000.0f);
    mRenderId = 1;
    //mViewMatrix = m44::Identity();
    mSortValue = 0.0f;
    mClearSurface = true;
    mClearFlags = csfAllBuffer;
    //mClearSurfaceFlags = csfAllBuffer;
}



Dispatcher::Result_t ComponentView::HandleMessage(Message& msg)
{
    switch (msg.mType)
    {
        case mtype::kGetPickRay:
            if (msg.Is<Vector2>())
            {
                Vector2 mousePos = msg.GetValue<Vector2>();
                Ray ray = GetRayAtCursor(mousePos);
                msg.mAnyData = ray;
            }
            break;

        case mtype::kComponentNew:
            if (!Renderer::mD3DDevice)
            {
                // this message might be send before we initialized our renderer
                // so we just break here,
                // otherwise let it fall trough it
                break;
            }

        case mtype::kRenderValid:
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


Hash ComponentView::GetComponentTypeId() const
{
    return ComponentType::kView;
}

uint32_t ComponentView::GetRenderId() const
{
    return mRenderId;
}


float ComponentView::GetSortValue() const
{
    return mSortValue;
}


bool ComponentView::RegisterComponentType()
{
    InitComponentType(ComponentType::kView, _T("View"));

    IObjectManager& om = REGISTRATE(IObjectManager);
    om.SubscribeToMessageType(ComponentType::kView, mtype::kComponentNew);
    om.SubscribeToMessageType(ComponentType::kView, mtype::kRenderValid);
    om.SubscribeToMessageType(ComponentType::kView, mtype::kGetEditorProperties);
    om.SubscribeToMessageType(ComponentType::kView, mtype::kGetChoiceLabels);

    return true;
}


void ComponentView::Render(IRenderer& renderer, WXWidget /*widgetHandle*/, float /*deltaTime*/, const wxRect& viewSize)
{
    if (mbRender && Renderer::mD3DDevice)
    {
        D3DVIEWPORT9 viewportDX;
        viewportDX.X      = viewSize.x;
        viewportDX.Y      = viewSize.y;
        viewportDX.Width  = viewSize.width;
        viewportDX.Height = viewSize.height;
        viewportDX.MinZ   = 0.0f;
        viewportDX.MaxZ   = 1.0f;

        float aspectRatio = static_cast<float>(viewportDX.Width) / static_cast<float>(viewportDX.Height);

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

        Matrix44 viewMatrix(m44::Identity());
        if (IComponentEntity *pEntity = component::cast<IComponentEntity>(this))
        {
            // update view matrix which is based on entity position this view is part of
            viewMatrix = pEntity->GetMatrix();
            viewMatrix = InverseMatrix44(viewMatrix);
        }
        else
        {
            // if there is no Entity, then uses it's own eye position and target value
            Vector3 eye = mEyePosition;
            Vector3 target = mTargetPosition;
            Vector3 upVector = mUpVector;

            viewMatrix = LookAtMatrixLH44(eye, target, upVector);
        }

        Vector3 camValues = mCameraValues;
        Matrix44 projectionMatrix = PerspectiveMatrixLH44(camValues.x, aspectRatio, camValues.y, camValues.z);
        RenderContext renderContext;
        renderContext.SetCameraId(OI().Id.GetValue());
        renderContext.SetRenderId(mRenderId);
        renderContext.SetViewMatrix(viewMatrix);
        renderContext.SetProjectionMatrix(projectionMatrix);
        renderContext.SetRenderTargetSize(Vector2(viewSize.GetWidth(), viewSize.GetHeight()));

        if (renderer.BeginScene())
        {
            uint32_t currentClearFlags = mClearFlags;

            HRESULT hr = Renderer::mD3DDevice->SetViewport(&viewportDX);
            if (HR_IS_VALID(hr))
            {
                if (mClearSurface)
                {
                    // we could try to clear only section of requested rect
                    uint32_t clearFlags = 0;
                    clearFlags |= (currentClearFlags & csfColorBuffer) ? D3DCLEAR_TARGET : 0;
                    clearFlags |= (currentClearFlags & csfDepthBuffer) ? D3DCLEAR_ZBUFFER : 0;
                    clearFlags |= (currentClearFlags & csfStencilBuffer) ? D3DCLEAR_STENCIL : 0;

                    Vector4 csc = mClearSurfaceColor;
                    DWORD clearColor = D3DCOLOR_COLORVALUE(csc.x, csc.y, csc.z, csc.w);
                    uint32_t numRect = 1;
                    D3DRECT rect;
                    rect.x1 = viewportDX.X;
                    rect.y1 = viewportDX.Y;
                    rect.x2 = viewportDX.X + viewportDX.Width;
                    rect.y2 = viewportDX.Y + viewportDX.Height;
                    D3DRECT *pClearRect = numRect ? &rect : 0;

                    hr = Renderer::mD3DDevice->Clear(numRect, pClearRect, clearFlags, clearColor, 1.0f, 0);
                    HR_IS_VALID(hr);
                }

                Message(mtype::kBeginRenderFrame, static_cast<const IRenderContext *>(&renderContext)).Send();
                if (!(currentClearFlags & csfOnlyClear))
                {
                    ComponentListCallback<IComponentModel> modelList;

                    // here we would cull all objects which can not be seen in current view port
                    while (IComponentModel *pModel = modelList.Next())
                    {
                        uint32_t modelRenderId = pModel->GetRenderId();
                        // let's check for render id's that they match
                        if (modelRenderId & mRenderId)
                        {
                            // before we issue render command we need to update matrices based on Entity component
                            if (IComponentEntity *pEntity = component::cast<IComponentEntity>(pModel))
                            {
                                renderContext.SetWorldMatrix(pEntity->GetMatrix());
                                // \note this should be potential optimization
                                // by fixing our scale issue at asset export
                                const IPropertySet& propSet = pEntity->GetPropertyData();
                                Vector3 scale = propSet["Scale"];
                                renderContext.SetWorldScale(scale);
                            }
                            else
                            {
                                renderContext.SetWorldMatrix(m44::Identity());
                                renderContext.SetWorldScale(v3::ONE());
                            }

                            pModel->Render(renderContext);
                        }
                    }
                }

                renderContext.SetWorldMatrix(m44::Identity());
                Message(mtype::kEndRenderFrame, static_cast<const IRenderContext *>(&renderContext)).Send();
            }

            renderer.EndScene();
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
}


Ray ComponentView::GetRayAtCursor(const Vector2& mousePos) const
{
    if (Renderer::mD3DDevice)
    {
        D3DVIEWPORT9 viewportDX;
        Renderer::mD3DDevice->GetViewport(&viewportDX);
        float aspectRatio = static_cast<float>(viewportDX.Width) / static_cast<float>(viewportDX.Height);

        Matrix44 viewMatrix(m44::Identity());
        if (IComponentEntity *pEntity = component::cast<IComponentEntity>(this))
        {
            // update view matrix which is based on entity position this view is part of
            viewMatrix = pEntity->GetMatrix();
            viewMatrix = InverseMatrix44(viewMatrix);
        }
        else
        {
            // if there is no Entity, then uses it's own eye position and target value
            Vector3 eye = mEyePosition;
            Vector3 target = mTargetPosition;
            Vector3 upVector = mUpVector;

            viewMatrix = LookAtMatrixLH44(eye, target, upVector);
        }

        Vector3 camValues = mCameraValues;
        Matrix44 projectionMatrix = PerspectiveMatrixLH44(camValues.x, aspectRatio, camValues.y, camValues.z);

        D3DXVECTOR3 originVector(mousePos.x, mousePos.y, 0);
        D3DXVECTOR3 vPickOrigin;
        D3DXVec3Unproject(&vPickOrigin,
                          &originVector,
                          &viewportDX,
                          (D3DXMATRIX *)(projectionMatrix.GetF()),
                          (D3DXMATRIX *)(viewMatrix.GetF()),
                          (D3DXMATRIX *)(m44::Identity().GetF()));

        D3DXVECTOR3 dirVector(mousePos.x, mousePos.y, 1);
        D3DXVECTOR3 vPickDir;
        D3DXVec3Unproject(&vPickDir,
                          &dirVector,
                          &viewportDX,
                          (D3DXMATRIX *)(projectionMatrix.GetF()),
                          (D3DXMATRIX *)(viewMatrix.GetF()),
                          (D3DXMATRIX *)(m44::Identity().GetF()));

        D3DXVec3Subtract(&vPickDir, &vPickDir, &vPickOrigin);

        Ray hitRay(Vector3(vPickOrigin.x, vPickOrigin.y, vPickOrigin.z), Vector3(vPickDir.x, vPickDir.y, vPickDir.z));
        hitRay.Dir.normalize();
        return hitRay;


        /*
        // Mouse variables
        //POINT pt;

        // UnProjection variables
        D3DVIEWPORT9 vpScreen;
        D3DXMATRIX matProj;
        D3DXMATRIX matView;
        D3DXMATRIX matWorld;
        D3DXVECTOR3 vPickOrigin;
        D3DXVECTOR3 vPickDir;

        // D3DXIntersect variables
        BOOL bHit=0;
        DWORD uFaceIndex=0;
        DWORD uFaceCount=0;
        float fU=0;
        float fV=0;
        float fG=0;
        float fDist=0;

        // Get the screen cursor
        //GetCursorPos(&pt);
        //ScreenToClient(lhWnd, &pt);

        // Get all the 3D UnProjection variables
        Renderer::mD3DDevice->GetViewport(&vpScreen);
        Renderer::mD3DDevice->GetTransform(D3DTS_PROJECTION, &matProj);
        Renderer::mD3DDevice->GetTransform(D3DTS_VIEW, &matView);
        Renderer::mD3DDevice->GetTransform(D3DTS_WORLD, &matWorld);

        hr = mShaderObject->SetMatrix(input.Handle, &D3DXMATRIX(inputValue.GetF()));


        // Compute UnProjection
        D3DXVec3Unproject(&vPickOrigin, &D3DXVECTOR3(mousePos.x, mousePos.y, 0), &vpScreen, &matProj, &matView, &matWorld);
        D3DXVec3Unproject(&vPickDir, &D3DXVECTOR3(mousePos.x, mousePos.y, 1.0f), &vpScreen, &matProj, &matView, &matWorld);
        D3DXVec3Subtract(&vPickDir, &vPickDir, &vPickOrigin);
        */

        /*
        // Compute D3DXIntersect
        D3DXIntersect(lD3DMesh,
                      &vPickOrigin,
                      &vPickDir,
                      &bHit,
                      &uFaceIndex,
                      &fV,
                      &fG,
                      &fDist,
                      NULL,
                      &uFaceCount);

        return bHit;
        */
    }

    return Ray();
}


void ComponentView::onPropertyChanged(const IProperty *pProperty)
{
    if (pProperty->GetPropId() == mClearSurfaceColor.GetPropId())
    {

    }
    else if (pProperty->GetPropId() == mRenderIdProperty.GetPropId())
    {
        mRenderId = mRenderIdProperty;
    }
}

/*
void ComponentVisual::OnRenderTargetChanged(const IProperty *pProperty)
{
    wxASSERT((*pProperty->GetName()) == "Render Target");
    Load();
}


ComponentVisual::PostProcess::PostProcess() :
    mActive(false)
{
}


bool ComponentVisual::PostProcess::IsPostProcess(const ResourceHandle<ITextureUserResource>& sourceTarget)
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

bool ComponentVisual::PostProcess::SetRenderTarget()
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


bool ComponentVisual::PostProcess::UpdateSourceTarget(IRenderer *pRenderer, ITextureUserResource *pSourceTarget, RenderContext *pRenderContext)
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


bool ComponentVisual::PostProcess::IsActive() const
{
    return mActive && mRenderTargetA && !mpShaderNames.empty();
}


EMessageResult ComponentVisual::PostProcess::HandleMessage(Message& msg)
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


void ComponentVisual::PostProcess::OnShadersChanged(const Property *pProperty)
{
}


void ComponentVisual::PostProcess::UpdateObjectTemplate(wxXmlNode *pNode)
{
    // change material name to post process
    SetProperty(pNode, _T("material"), _T("name"), _T("PostProcess"));
    // and make sure that our render pass is unique
    //SetProperty(pNode, _T("pass"), _T("id"), _T("256"));
}


void ComponentVisual::PostProcess::Shutdown()
{
    mRenderTargetA = "";
    mRenderTargetB = "";
    mMaterial = "";
    mpShaderNames.clear();
    mPostProcesShaders.clear();
}
*/

} // namespace eg
