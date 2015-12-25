#include "Precompiled.h"
#include "Presenters.h"
#include "Message/Dispatcher.h"
#include "../Renderer.h"
#include "../ErrorHandler.h"
#include <boost/bind.hpp>


namespace eg {
namespace widgets {


LinePresenter::LinePresenter()
: mNextVertexData(0)
{
    Dispatcher& disp = REGISTRATE(Dispatcher);
    mVideoResetConnection = disp[mtype::kRendererVideoModeReset].connect(boost::bind(&LinePresenter::onVideoReset, this, _1));
}


LinePresenter::~LinePresenter()
{
    mVideoResetConnection.disconnect();
}


void LinePresenter::Render(const Verticies_t& lines)
{
    if (lines.empty())
    {
        // if there is no lines, just simple return
        return;
    }

    // (1Mb) * number_of_megabytes;
    const uint32_t kSize = (1024 * 1024) * 2;

    if (!mLineBuffer)
    {
        mNextVertexData = 0;
        HRESULT hr = Renderer::mD3DDevice->CreateVertexBuffer(kSize, D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY, D3DFVF_XYZ|D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &mLineBuffer, 0);
        if (!HR_IS_VALID(hr))
        {
            return;
        }
    }

    const uint32_t kVertexSize = sizeof(Vertex);
    DWORD lockFlags = D3DLOCK_NOOVERWRITE;
    const uint32_t dataSize = kVertexSize * lines.size();

    if (mNextVertexData > kSize - dataSize)
    {
        lockFlags = D3DLOCK_DISCARD;
        mNextVertexData = 0;
    }

    BYTE *pBytes;
    HRESULT hr = mLineBuffer->Lock(mNextVertexData, dataSize, (void **)&pBytes, lockFlags);
    if (!HR_IS_VALID(hr))
    {
        return;
    }

    memcpy(pBytes, &lines[0], dataSize);
    mLineBuffer->Unlock();

    hr = Renderer::mD3DDevice->SetStreamSource(0, mLineBuffer, mNextVertexData, kVertexSize);
    HR_IS_VALID(hr);

    hr = Renderer::mD3DDevice->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
    HR_IS_VALID(hr);

    //-------------------------------------------------------------------------------------
    hr = Renderer::mD3DDevice->DrawPrimitive(D3DPT_LINELIST, 0, static_cast<UINT>(lines.size()/2));
    HR_IS_VALID(hr);
    //-------------------------------------------------------------------------------------

    mNextVertexData += dataSize;
}


void LinePresenter::onVideoReset(Message& /*msg*/)
{
    mLineBuffer = 0;
}




TrianglePresenter::TrianglePresenter()
: mNextVertexData(0)
{
    Dispatcher& disp = REGISTRATE(Dispatcher);
    mVideoResetConnection = disp[mtype::kRendererVideoModeReset].connect(boost::bind(&TrianglePresenter::onVideoReset, this, _1));
}


TrianglePresenter::~TrianglePresenter()
{
    mVideoResetConnection.disconnect();
}


void TrianglePresenter::Render(const Triangles_t& triangles)
{
    if (triangles.empty())
    {
        // if there is no triangles, just simple return
        return;
    }

    // (1Mb) * number_of_megabytes;
    const uint32_t kSize = (1024 * 1024) * 4;

    if (!mTriangleBuffer)
    {
        mNextVertexData = 0;
        HRESULT hr = Renderer::mD3DDevice->CreateVertexBuffer(kSize, D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY, D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1, D3DPOOL_DEFAULT, &mTriangleBuffer, 0);
        if (!HR_IS_VALID(hr))
        {
            return;
        }
    }

    const uint32_t kVertexSize = sizeof(Vertex);
    DWORD lockFlags = D3DLOCK_NOOVERWRITE;
    const uint32_t dataSize = kVertexSize * (triangles.size() * 3);

    if (mNextVertexData > kSize - dataSize)
    {
        lockFlags = D3DLOCK_DISCARD;
        mNextVertexData = 0;
    }

    BYTE *pBytes;
    HRESULT hr = mTriangleBuffer->Lock(mNextVertexData, dataSize, (void **)&pBytes, lockFlags);
    if (!HR_IS_VALID(hr))
    {
        return;
    }

    memcpy(pBytes, &triangles[0], dataSize);
    mTriangleBuffer->Unlock();

    hr = Renderer::mD3DDevice->SetStreamSource(0, mTriangleBuffer, mNextVertexData, kVertexSize);
    HR_IS_VALID(hr);

    hr = Renderer::mD3DDevice->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1);
    HR_IS_VALID(hr);

    //-------------------------------------------------------------------------------------
    hr = Renderer::mD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, static_cast<UINT>(triangles.size()));
    HR_IS_VALID(hr);
    //-------------------------------------------------------------------------------------

    mNextVertexData += dataSize;
}


void TrianglePresenter::onVideoReset(Message& /*msg*/)
{
    mTriangleBuffer = 0;
}

} // namespace widgets
} // namespace eg
