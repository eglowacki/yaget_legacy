// GeometryTextureAsset.cpp
#include "Precompiled.h"
#include "GeometryTextureAsset.h"
#include "Message/Dispatcher.h"
#include "IdGameCache.h"
#include "Interface/IRenderContext.h"
#include "StringHelper.h"
#include "Renderer.h"
#include "ErrorHandler.h"
#include <boost/bind.hpp>


namespace eg
{

GeometryTextureAsset::GeometryTextureAsset(const std::string& name)
: eg::MediaAsset<GeometryTextureAsset>(name)
{
    Dispatcher& disp = REGISTRATE(Dispatcher);
    mResetConnection = disp[mtype::kRendererVideoModeReset].connect(boost::bind(&GeometryTextureAsset::onVideoReset, this, _1));
    mChangedConnection = disp[mtype::kRendererVideoModeChanged].connect(boost::bind(&GeometryTextureAsset::onVideoChanged, this, _1));

    // when asset is finished loading, it will trigger signal, which will allow us to use missing texture
    // if there is not loaded one.
    sigLoadFinish.connect(0, boost::bind(&GeometryTextureAsset::onFinishedLoading, this, _1, _2));
}

GeometryTextureAsset::~GeometryTextureAsset()
{
    mResetConnection.disconnect();
    mChangedConnection.disconnect();
    mMissingAssetConnection.disconnect();
}


void GeometryTextureAsset::onFinishedLoading(GeometryTextureAsset::Asset_t asset, const std::string& name)
{
    DataLocker lock(*this);
    if (!mTextureObject)
    {
        wxLogWarning("GeometryTexture Asset '%s' was not loaded successfully, using '%s' as a placeholder.", name.c_str(), missingasset::GeometryTexture);

        asset::AssetLocker<GeometryTextureAsset, asset::NoSyncAssetPolicy> assetLocker(missingasset::GeometryTexture);
        if (GeometryTextureAsset *source = assetLocker)
        {
            mMissingAssetConnection = source->sigLoadFinish.connect(0, boost::bind(&GeometryTextureAsset::onMissingAssetChanged, this, _1, _2));
            mTextureObject = source->mTextureObject;
        }
    }
}


void GeometryTextureAsset::onMissingAssetChanged(GeometryTextureAsset::Asset_t asset, const std::string& /*name*/)
{
    DataLocker lock(*this);
    mTextureObject = asset->mTextureObject;
}


void GeometryTextureAsset::onVideoReset(eg::Message& /*msg*/)
{
}


void GeometryTextureAsset::onVideoChanged(eg::Message& /*msg*/)
{
}


void GeometryTextureAsset::SaveBlob(uint8_t *&/*pData*/, size_t& /*size*/) const
{
    /*
    DataLocker lock(*this);

    size = mScriptData.size();
    pData = new uint8_t[size];
    memcpy(pData, mScriptData.c_str(), size);
    */
}


void GeometryTextureAsset::LoadBlob(const uint8_t *pData, size_t size, const std::string& streamName)
{
    mMissingAssetConnection.disconnect();
    CComPtr<IDirect3DTexture9> textureObject;
    HRESULT hr = D3DXCreateTextureFromFileInMemory(Renderer::mD3DDevice,
                                                   pData,
                                                   static_cast<UINT>(size),
                                                   &textureObject);
    if (HR_IS_VALID(hr))
    {
        // now, let's lock to update mesh pointer
        DataLocker lock(*this);
        mTextureObject = textureObject;

        wxLogTrace(TR_RENDER, "GeometryTexture Asset '%s' loaded successfully.", streamName.c_str());
    }
}


CComPtr<IDirect3DTexture9> GeometryTextureAsset::GetNativeObject() const
{
    wxASSERT(isLocked());

    return mTextureObject;
}


} // namespace eg

