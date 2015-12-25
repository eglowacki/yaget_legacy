///////////////////////////////////////////////////////////////////////
// ComponentCamera.h
//
//  Copyright 2/5/2006 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Component camera implementation
//
//
//  #include "ComponentCamera.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef COMPONENT_CAMERA_H
#define COMPONENT_CAMERA_H

#include "IComponentCamera.h"
#include "Math/Matrix.h"
#include "Property/PropertyData.h"
#include "Message/Dispatcher.h"
//#pragma warning (disable:4312)  // 'variable' : conversion from 'type' to 'type' of greater size
//#include <atlcomcli.h>
//#pragma warning (default : 4312)
//#include <d3d9.h>

namespace eg
{
    class RenderContext;

    /*!
    Camera implementation private to Render module
    */
    class ComponentCamera : public IComponentCamera, public ComponentHelper<ComponentCamera, PropertySet>
    {
    public:
        ComponentCamera();
        virtual ~ComponentCamera();

        // from IComponentCamera
        virtual uint32_t GetRenderId() const;
        virtual void Render(IRenderer *pRenderer, WXWidget widgetHandle, float deltaTime, const wxRect *pSize);
        virtual void SetViewMatrix(const Matrix44& matrix);
        virtual float GetSortValue() const;

        // from IComponent
        virtual void DeleteThis();
        virtual Hash GetComponentTypeId() const;
        virtual Dispatcher::Result_t HandleMessage(Message& msg);
        virtual IPropertySet& GetPropertyData()
        {
            return CH_t::GetPropertyData();
        }

        // Static methods
        static bool RegisterComponentType();

    private:
        // from IComponent
        virtual void SetObjectId(ObjectId oId);
        //virtual bool Init(ObjectId oId, ParameterNode& paramNode);
        virtual void Shutdown();
        //! Load any resources used by this camera
        //bool Load();

        //! Called when RenderTarget is changed
        //void OnRenderTargetChanged(const IProperty *pProperty);

        //! x - view angle in degrees
        //! y - near plane
        //! z - far plane
        Vector3 mCameraValues;
        //! Render id for this camera
        uint32_t mRenderId;
        //! Color to clear redner target to before rendering to it
        Vector4 mClearSurfaceColor;

        //! This is filled every frame by the caller
        Matrix44 mViewMatrix;
        float mSortValue;
        //! Each camera can be rendered to some rendder target. If this is not set then it uses main back buffer
        //std::string mRenderTargetName;
        //! this reprsents user set render target
        //ResourceHandle<ITextureUserResource> mCurrentRenderTarget;
        //! This will be handle to back buffer if mCurrentRenderTarget is set
        //CComPtr<IDirect3DSurface9> mMainRenderTarget;

        //! This allows to view performance stats
        bool mShowStats;

        //! This allows us to render this camera and all of it's models if it's TRUE
        //!  or not if it's FALSE
        bool mbRender;
        //! If true, then clear surface before doing any redering, otherise skip
        bool mClearSurface;

        static const uint32_t csfColorBuffer        = 0x0001; ///< clear color buffer only
        static const uint32_t csfDepthBuffer        = 0x0002; ///< clear depth buffer only
        static const uint32_t csfStencilBuffer      = 0x0004; ///< clear stencil buffer only
        static const uint32_t csfDepthStencilBuffer = csfDepthBuffer|csfStencilBuffer; ///< clear depth and stencil buffers together
        static const uint32_t csfAllBuffer          = csfColorBuffer|csfDepthStencilBuffer; ///< clear all buffers together

        //! This is used in conjunction with mClearSurface and can contain flags
        //! used to determine behaviour of clear
        uint32_t mClearSurfaceFlags;

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

		// Must grant friendship to access non-public fields.
		friend void DejaDescriptor(const ComponentCamera&);
    };

} // namespace eg

#endif // COMPONENT_CAMERA_H

