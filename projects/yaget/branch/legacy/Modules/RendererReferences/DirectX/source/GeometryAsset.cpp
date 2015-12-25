// ScriptAsset.cpp
#include "Precompiled.h"
#include "GeometryAsset.h"
#include "IdGameCache.h"
#include "Message/Dispatcher.h"
#include "Interface/IRenderContext.h"
#include "StringHelper.h"
#include "Renderer.h"
#include "ErrorHandler.h"
#include <boost/bind.hpp>


namespace eg
{

GeometryAsset::GeometryAsset(const std::string& name)
: eg::MediaAsset<GeometryAsset>(name)
, mNumAttributes(0)
, mBoundingBox(v3::ZERO(), v3::ZERO())
{
    Dispatcher& disp = REGISTRATE(Dispatcher);
    mResetConnection = disp[mtype::kRendererVideoModeReset].connect(boost::bind(&GeometryAsset::onVideoReset, this, _1));
    mChangedConnection = disp[mtype::kRendererVideoModeChanged].connect(boost::bind(&GeometryAsset::onVideoChanged, this, _1));

    // when asset is finished loading, it will trigger signal, which will allow us to use missing geometry
    // if there is not loaded one.
    sigLoadFinish.connect(0, boost::bind(&GeometryAsset::onFinishedLoading, this, _1, _2));
}

GeometryAsset::~GeometryAsset()
{
    mResetConnection.disconnect();
    mChangedConnection.disconnect();
    mMissingAssetConnection.disconnect();
}


void GeometryAsset::onVideoReset(eg::Message& /*msg*/)
{
}


void GeometryAsset::onVideoChanged(eg::Message& /*msg*/)
{
}


void GeometryAsset::onFinishedLoading(GeometryAsset::Asset_t asset, const std::string& name)
{
    DataLocker lock(*this);
    if (!mMeshObject)
    {
        wxLogWarning("Geometry Asset '%s' was not loaded successfully, using '%s' as a placeholder.", name.c_str(), missingasset::Geometry);

        asset::AssetLocker<GeometryAsset, asset::NoSyncAssetPolicy> assetLocker(missingasset::Geometry);
        if (GeometryAsset *source = assetLocker)
        {
            mMissingAssetConnection = source->sigLoadFinish.connect(0, boost::bind(&GeometryAsset::onMissingAssetChanged, this, _1, _2));
            mBoundingBox = source->mBoundingBox;
            mNumAttributes = source->mNumAttributes;
            mMeshObject = source->mMeshObject;
        }
    }
}


void GeometryAsset::onMissingAssetChanged(GeometryAsset::Asset_t asset, const std::string& /*name*/)
{
    DataLocker lock(*this);
    mBoundingBox = asset->mBoundingBox;
    mNumAttributes = asset->mNumAttributes;
    mMeshObject = asset->mMeshObject;
}


void GeometryAsset::SaveBlob(uint8_t *&/*pData*/, size_t& /*size*/) const
{
    /*
    DataLocker lock(*this);

    size = mScriptData.size();
    pData = new uint8_t[size];
    memcpy(pData, mScriptData.c_str(), size);
    */
}


void GeometryAsset::LoadBlob(const uint8_t *pData, size_t size, const std::string& /*streamName*/)
{
    mMissingAssetConnection.disconnect();
    // first, we'll just load all the needed data into local variables,
    // thus avoiding locking in Render(...) method
    CComPtr<ID3DXMesh> meshObject;
    CComPtr<ID3DXBuffer> materialBuffer;
    DWORD numMaterials;
    HRESULT hr = D3DXLoadMeshFromXInMemory(pData,
                                           static_cast<DWORD>(size),
                                           D3DXMESH_MANAGED,
                                           Renderer::mD3DDevice,
                                           0,
                                           &materialBuffer,
                                           0,
                                           &numMaterials,
                                           &meshObject);
    if (HR_IS_VALID(hr))
    {
        uint32_t numVerts = meshObject->GetNumVertices();
        uint32_t stride = meshObject->GetNumBytesPerVertex();
        void *pVertexData = NULL;
        hr = meshObject->LockVertexBuffer(D3DLOCK_READONLY, (void **)&pVertexData);
        HR_IS_VALID(hr);

        D3DXVECTOR3 vMin, vMax;
        // now, let's lock to update mesh pointer
        // so now, let's calculate bounding box for this mesh
        hr = D3DXComputeBoundingBox((CONST D3DXVECTOR3 *)pVertexData,
                                    numVerts,
                                    stride,
                                    &vMin,
                                    &vMax);
        HR_IS_VALID(hr);

        hr = meshObject->UnlockVertexBuffer();
        HR_IS_VALID(hr);

        /*
        D3DVERTEXELEMENT9 decl[MAX_FVF_DECL_SIZE];
        hr = meshObject->GetDeclaration(decl);
        HR_IS_VALID(hr);

        CComPtr<IDirect3DVertexDeclaration9> vertexDeclaration;
        hr = Renderer::mD3DDevice->CreateVertexDeclaration(decl, &vertexDeclaration);
        HR_IS_VALID(hr);
        */

        DataLocker lock(*this);
        mBoundingBox.first = Vector3(vMin.x, vMin.y, vMin.z);
        mBoundingBox.second = Vector3(vMax.x, vMax.y, vMax.z);

        mNumAttributes = numMaterials;
        mMeshObject = meshObject;
        //mVertexDeclaration = vertexDeclaration;
    }
}


const GeometryAsset::BoundingBox_t& GeometryAsset::BoundingBox() const
{
    return mBoundingBox;
}


void GeometryAsset::Render(const IRenderContext& /*renderContext*/)
{
    wxASSERT(isLocked());
    if (mMeshObject)
    {
        for (uint32_t i = 0; i < mNumAttributes; i++)
        {
            HRESULT hr = mMeshObject->DrawSubset(i);
            HR_IS_VALID(hr);
        }
    }
}

} // namespace eg

