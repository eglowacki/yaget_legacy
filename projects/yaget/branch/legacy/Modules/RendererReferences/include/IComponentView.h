///////////////////////////////////////////////////////////////////////
// IComponentView.h
//
//  Copyright 2/5/2006 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      This represents View component which renders
//      visual representation of this object.
//
//
//  #include "IComponentView.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef I_COMPONENT_VIEW_H
#define I_COMPONENT_VIEW_H

#include "RenderBase.h"
#include "Math/Ray.h"
#include "IComponent.h"
#include "ComponentEnumsRender.h"

namespace eg
{
    namespace view
    {
        static const char *kGroup = "View";
        static const char *kPropClearColor = "ClearColor";          ///< Vector4
        static const char *kPropClearFlags = "ClearFlags";          ///< uint32_t
        static const char *kPropRenderId = "RenderId";              ///< uint32_t (\note this might have to be bigger)
        static const char *kPropView = "View";                      ///< Vector3 (viewAngle, near, far)
        static const char *kPropEyePosition = "EyePosition";        ///< Vector3
        static const char *kPropTargetPosition = "TargetPosition";  ///< Vector3
        static const char *kPropUpVector = "UpVector";              ///< Vector3
    } // namespace view

    class Matrix44;
    class IRenderer;

    /*!
    /code
    -- create database table in DB
    CREATE TABLE component_view
    (
        object_id bigint NOT NULL,
        clear_color vector4
        view vector3
        eye_position vector3
        target_position vector3
        up_vector vector3
        CONSTRAINT pk_view PRIMARY KEY (object_id)
    );
    /endcode
    */


    /*
    This represents view to render (camera)
    It uses RenderId on itself and on each Model to
    see if it can be rendered.
    RenderId rules:
        1. If View is 0 or Model is 0 then skip it. Zero in essence means disabled.
        2. If the same View bit and Model bit is set then render it
    */
    class DLLIMPEXP_RENDER IComponentView : public IComponent
    {
    public:
        const static uint32_t Type = 0x3bad72e;//ComponentType::kView;

        static const uint32_t csfColorBuffer        = 0x0001; ///< clear color buffer only
        static const uint32_t csfDepthBuffer        = 0x0002; ///< clear depth buffer only
        static const uint32_t csfStencilBuffer      = 0x0004; ///< clear stencil buffer only
        static const uint32_t csfOnlyClear          = 0x0008; ///< only clear back buffer and skip any other rendering for this view
        static const uint32_t csfDepthStencilBuffer = csfDepthBuffer|csfStencilBuffer; ///< clear depth and stencil buffers together
        static const uint32_t csfAllBuffer          = csfColorBuffer|csfDepthStencilBuffer; ///< clear all buffers together

        //! This will return pass id of this camera, where Model can specify pass id
        virtual uint32_t GetRenderId() const = 0;

        /*!
        This will render all the Component Models which have valid Render ID
        \param pRenderer current render to use. Camera is responsible for Begin adn End scene calls
        \param widgetHandle in most implementation this represents window handle
        \param deltaTime time in milliseconds since last frame
        \param pSize size of the viewport. If NULL, then use default one
        */
        virtual void Render(IRenderer& renderer, WXWidget widgetHandle, float deltaTime, const wxRect& viewSize) = 0;

        //! This will return some arbitrary sort value, which is used to sort all the cameras before rendering each one.
        virtual float GetSortValue() const = 0;

        //! Return ray in world coordinates for current mouse/2d point position
        //!
        //! \param mousePos X mouse position in local window space
        //!
        //! \return Ray ray in world coordinates
        virtual Ray GetRayAtCursor(const Vector2& mousePos) const = 0;

    protected:
        virtual ~IComponentView() = 0 {}
    };

    //! Predicate used in std::sort algorithm
    inline bool RenderSort(const IComponentView *pValue1, const IComponentView *pValue2)
    {
        return pValue1->GetSortValue() < pValue2->GetSortValue();
    }

} // namespace eg

#endif // I_COMPONENT_VIEW_H

