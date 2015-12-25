///////////////////////////////////////////////////////////////////////
// RenderContext.h
//
//  Copyright 11/2/2005 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Implementation of RenderContex class
//
//
//  #include "RenderContext.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef RENDER_CONTEXT_H
#define RENDER_CONTEXT_H

#include "Interface/IRenderContext.h"
#include "Math/Matrix.h"


namespace eg
{
    /*!
    This structure is passed to each
    */
    class RenderContext : public IRenderContext
    {
    public:
        RenderContext();
        RenderContext(const IRenderContext& source);
        virtual ~RenderContext();

        void SetProjectionMatrix(const Matrix44& matrix);
        void SetViewMatrix(const Matrix44& matrix);
        void SetWorldMatrix(const Matrix44& matrix);
        void SetWorldScale(const Vector3& scale);
        void SetRenderTargetSize(const Vector2& size);
        void SetRenderId(uint32_t id);
        void SetCameraId(uint64_t cameraId);

        // from IRenderContext
        virtual const Matrix44& GetProjectionMatrix() const;
        virtual const Matrix44& GetWorldMatrix() const;
        virtual const Matrix44& GetViewMatrix() const;
        virtual uint32_t GetRenderId() const;
        virtual const Vector2& GetRenderTargetSize() const;
        virtual const Matrix44& GetWVPMatrix() const;
        virtual const Vector3& GetWorldScale() const;
        virtual uint64_t GetCameraId() const;

    private:
        Matrix44 mWorldMatrix;
        Matrix44 mViewMatrix;
        Matrix44 mProjectionMatrix;
        mutable Matrix44 mWVPMatrix;
        Vector2 mRenderTargetSize;
        Vector3 mWorldScale;
        uint32_t mId;
        uint64_t mCameraId;
        mutable bool mbWVMDirty;
    };

} // namespace eg

#endif // RENDER_CONTEXT_H

