#include "Precompiled.h"
#include "RenderContext.h"


namespace eg {


RenderContext::RenderContext()
: mWorldMatrix(m44::Identity())
, mViewMatrix(m44::Identity())
, mProjectionMatrix(m44::Identity())
, mWVPMatrix(m44::Identity())
, mRenderTargetSize(1, 1)
, mWorldScale(v3::ONE())
, mId(1)
, mCameraId(0)
, mbWVMDirty(false)
{
}


RenderContext::RenderContext(const IRenderContext& source)
: mWorldMatrix(source.GetWorldMatrix())
, mViewMatrix(source.GetViewMatrix())
, mProjectionMatrix(source.GetProjectionMatrix())
, mWVPMatrix(source.GetWVPMatrix())
, mRenderTargetSize(source.GetRenderTargetSize())
, mWorldScale(source.GetWorldScale())
, mId(source.GetRenderId())
, mCameraId(source.GetCameraId())
, mbWVMDirty(false)
{
}


RenderContext::~RenderContext()
{
}


void RenderContext::SetProjectionMatrix(const Matrix44& matrix)
{
    mProjectionMatrix = matrix;
    mbWVMDirty = true;
}


void RenderContext::SetViewMatrix(const Matrix44& matrix)
{
    mViewMatrix = matrix;
    mbWVMDirty = true;
}


void RenderContext::SetWorldMatrix(const Matrix44& matrix)
{
    mWorldMatrix = matrix;
    mbWVMDirty = true;
}


void RenderContext::SetRenderTargetSize(const Vector2& size)
{
    mRenderTargetSize = size;
}


void RenderContext::SetRenderId(uint32_t id)
{
    mId = id;
}


void RenderContext::SetWorldScale(const Vector3& scale)
{
    mWorldScale = scale;
}


void RenderContext::SetCameraId(uint64_t cameraId)
{
    mCameraId = cameraId;
}


const Matrix44& RenderContext::GetProjectionMatrix() const
{
    return mProjectionMatrix;
}


const Matrix44& RenderContext::GetWorldMatrix() const
{
    return mWorldMatrix;
}


const Matrix44& RenderContext::GetViewMatrix() const
{
    return mViewMatrix;
}


uint32_t RenderContext::GetRenderId() const
{
    return mId;
}


const Vector2& RenderContext::GetRenderTargetSize() const
{
    return mRenderTargetSize;
}


const Matrix44& RenderContext::GetWVPMatrix() const
{
    if (mbWVMDirty)
    {
        mbWVMDirty = false;
        mWVPMatrix = mWorldMatrix * mViewMatrix * mProjectionMatrix;
    }
    return mWVPMatrix;
}

const Vector3& RenderContext::GetWorldScale() const
{
    return mWorldScale;
}


uint64_t RenderContext::GetCameraId() const
{
    return mCameraId;
}


} // namespace eg
