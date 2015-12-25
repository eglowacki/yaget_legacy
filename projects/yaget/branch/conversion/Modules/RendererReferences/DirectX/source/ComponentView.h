///////////////////////////////////////////////////////////////////////
// ComponentView.h
//
//  Copyright 2/5/2006 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Visual Component implementation
//
//
//  #include "ComponentView.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef COMPONENT_VIEW_H
#define COMPONENT_VIEW_H

#include "IComponentView.h"
#include "Math/Matrix.h"
#include "Property/PropertyData.h"
#include "Message/Dispatcher.h"

namespace eg
{
    class RenderContext;

    /*!
    Visual implementation private to Render module
    */
    class ComponentView : public IComponentView, public ComponentHelper<ComponentView, PropertySet>
    {
    public:
        ComponentView(component::ObjectId id);
        virtual ~ComponentView();

        // from IComponentCamera
        virtual uint32_t GetRenderId() const;
        virtual void Render(IRenderer& renderer, WXWidget widgetHandle, float deltaTime, const wxRect& viewSize);
        virtual float GetSortValue() const;
        virtual Ray GetRayAtCursor(const Vector2& mousePos) const;

        // from IComponent
        virtual void DeleteThis();
        virtual Hash GetComponentTypeId() const;
        virtual Dispatcher::Result_t HandleMessage(Message& msg);
        virtual IPropertySet& GetPropertyData() {return CH_t::GetPropertyData();}
        virtual const ObjectInstance& OI() const {return CH_t::OI();}

        // Static methods
        static bool RegisterComponentType();

    private:
        //virtual bool Init(ObjectId oId, ParameterNode& paramNode);
        virtual void Shutdown();

        virtual void onPropertyChanged(const IProperty *pProperty);

        //! Called when RenderTarget is changed
        //void OnRenderTargetChanged(const IProperty *pProperty);

        //! Render id for this camera
        uint32_t mRenderId;

        //! combination of csf<...> flags
        Property<uint32_t>& mClearFlags;

        //! This is only here to provide edit capability
        //! but we maintain mRenderId member variable for speed of access.
        Property<uint32_t>& mRenderIdProperty;

        //! Color to clear render target to before rendering to it
        Property<Vector4>& mClearSurfaceColor;

        //! x - view angle in degrees
        //! y - near plane
        //! z - far plane
        Property<Vector3>& mCameraValues;

        //{@
        //! This is used in creating view matrix, if there is not Entity component
        //! for this object
        Property<Vector3>& mEyePosition;
        Property<Vector3>& mTargetPosition;
        Property<Vector3>& mUpVector;
        //}@

        //! This is filled every frame by the caller
        //Matrix44 mViewMatrix;
        float mSortValue;
        //! Each camera can be rendered to some render target. If this is not set then it uses main back buffer
        //std::string mRenderTargetName;
        //! this represents user set render target
        //ResourceHandle<ITextureUserResource> mCurrentRenderTarget;
        //! This will be handle to back buffer if mCurrentRenderTarget is set
        //CComPtr<IDirect3DSurface9> mMainRenderTarget;

        //! This allows to view performance stats
        bool mShowStats;

        //! This allows us to render this camera and all of it's models if it's TRUE
        //!  or not if it's FALSE
        bool mbRender;
        //! If true, then clear surface before doing any rendering, otherwise skip
        bool mClearSurface;

        //! This is used in conjunction with mClearSurface and can contain flags
        //! used to determine behaviour of clear
        //uint32_t mClearSurfaceFlags;

        /*
        //! This is used to manage post process for render targets
        struct PostProcess : public NotificationTarget
        {
            PostProcess();

            //! This will reset to clean state
            void Shutdown();

            //! This will return TRUE if we have post process
            //! it will also create new render target based on sourceTarget
            bool IsPostProcess(const ResourceHandle<ITextureUserResource>& sourceTarget);
            //! if mActive is TRUE, then call this method to use post process render target
            //! pSourceTarget our original render target
            bool SetRenderTarget();
            //! Call this to update pSourceTarget with data from mRenderTarget
            bool UpdateSourceTarget(IRenderer *pRenderer, ITextureUserResource *pSourceTarget, RenderContext *pRenderContext);
            //! Retunr true if this post process is active
            bool IsActive() const;
            //! This will update object tamplate with correct values
            void UpdateObjectTemplate(wxXmlNode *pNode);

            // from NotificationTarget
            virtual EMessageResult HandleMessage(Message& msg);

            void OnShadersChanged(const Property *pProperty);

            //! If true, then this post process is active
            bool mActive;
            //! Render target to use
            ResourceHandle<ITextureUserResource> mRenderTargetA;
            ResourceHandle<ITextureUserResource> mRenderTargetB;
            //! This material uses mRenderTarget as base texture and ist's bind to ScreenQuad object
            ResourceHandle<IMaterialResource> mMaterial;
            //! User specified shader to use in post process
            std::string mShaderName;

            // this is used in multi shader post process
            std::vector<std::string> mpShaderNames;
            std::vector<ResourceHandle<IShaderResource> > mPostProcesShaders;
        };

        PostProcess mPostProcess;
        */
    };

} // namespace eg

#endif // COMPONENT_VIEW_H

