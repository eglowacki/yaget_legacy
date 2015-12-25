//////////////////////////////////////////////////////////////////////
// GeometryAsset.h
//
//  Copyright 1/6/2008 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Provides geometry data (mesh) loaded from some resource
//
//
//  #include "GeometryAsset.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef GEOMETRY_ASSET_H
#define GEOMETRY_ASSET_H
#pragma once

#include "Asset/MediaAsset.h"
#include "Asset/AssetHandle.h"
#include "Math/Vector.h"
#pragma warning (disable:4312)  // 'variable' : conversion from 'type' to 'type' of greater size
    #include <atlcomcli.h>
    #include <d3dx9mesh.h>
#pragma warning (default : 4312)



namespace eg
{
	namespace missingasset
	{
		static const char *Geometry = "Missing.x";
	}  // namespace missingasset

    class IRenderContext;

    class GeometryAsset : public eg::MediaAsset<GeometryAsset>
    {
    public:
        static const guid_t kType = 0x74b09745;
        //! first - lower/left/near corner
        //! second - upper/right/far corner
        typedef std::pair<Vector3, Vector3> BoundingBox_t;

        GeometryAsset(const std::string& name);
        ~GeometryAsset();

        //! Trigger rendering of this gemetry
        //! \note How do we aplly material here and what about sub pieces?
        void Render(const IRenderContext& renderContext);
        const BoundingBox_t& BoundingBox() const;

        // from IAsset
        virtual guid_t Type() const {return GeometryAsset::kType;}

    private:
        // From eg::MediaAsset
        virtual void SaveBlob(uint8_t *&pData, size_t& size) const;
        virtual void LoadBlob(const uint8_t *pData, size_t size, const std::string& streamName);

        //! Actual mesh object
        CComPtr<ID3DXMesh> mMeshObject;
        //CComPtr<IDirect3DVertexDeclaration9> mVertexDeclaration;
        //! number of materials/subsets
        uint32_t mNumAttributes;
        BoundingBox_t mBoundingBox;

		void onMissingAssetChanged(GeometryAsset::Asset_t asset, const std::string& name);
		void onFinishedLoading(GeometryAsset::Asset_t asset, const std::string& name);
        //! this is only used for resetting video device
        void onVideoReset(eg::Message& msg);
        void onVideoChanged(eg::Message& msg);
        boost::signals::connection mResetConnection;
        boost::signals::connection mChangedConnection;
		boost::signals::connection mMissingAssetConnection;
    };


    typedef asset::AssetHandle<GeometryAsset, asset::AutoLock<GeometryAsset>, asset::LoadTypeDynamic> GeometryAsset_t;

} // namespace eg


#endif // GEOMETRY_ASSET_H
