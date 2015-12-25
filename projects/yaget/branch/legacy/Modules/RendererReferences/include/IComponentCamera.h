///////////////////////////////////////////////////////////////////////
// IComponentCamera.h
//
//  Copyright 2/5/2006 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      This represents camera component
//
//
//  #include "IComponentCamera.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef I_COMPONENT_CAMERA_H
#define I_COMPONENT_CAMERA_H

#include "RenderBase.h"
#include "IComponent.h"
#include "ComponentEnumsRender.h"


namespace eg
{
    class Matrix44;
    class IRenderer;

    class DLLIMPEXP_RENDER IComponentCamera : public IComponent
    {
    public:

        //! This will return pass id of this camera, where Model can specify pass id
        virtual uint32_t GetRenderId() const = 0;

        /*!
        This will render all the Component Models which have valid Render ID
        \param pRenderer current render to use. Camera is responsible for Begin adn End scene calls
        \param widgetHandle in most implementation this represents window handle
        \param deltaTime time in milliseconds since last frame
        \param pSize size of the viewport. If NULL, then use default one
        */
        virtual void Render(IRenderer *pRenderer, WXWidget widgetHandle, float deltaTime, const wxRect *pSize) = 0;

        //! Set camera matrix
        virtual void SetViewMatrix(const Matrix44& matrix) = 0;

        //! This will return some arbitrary sort value, which is used to sort all the cameras before rendering each one.
        virtual float GetSortValue() const = 0;

        // from IComponent
        virtual ObjectId GetObjectId() const {return mObjectId;}

    protected:
        virtual ~IComponentCamera() = 0 {}

        static void RegisterInterface(Hash compId);

        // from IComponent
        virtual void SetObjectId(ObjectId oId) {mObjectId = oId;}

    private:
#pragma warning (disable : 4251) // 'eg::IComponentCamera::mObjectId' : class 'eg::Hash' needs to have dll-interface to be used by clients of class 'eg::IComponentCamera'
        ObjectId mObjectId;
#pragma warning (default : 4251)
    };

    //! Predicate used in std::sort algorithm
    inline bool CameraSort(const IComponentCamera *pCameraA, const IComponentCamera *pCameraB)
    {
        return pCameraA->GetSortValue() < pCameraB->GetSortValue();
    }

} // namespace eg

#endif // I_COMPONENT_CAMERA_H

