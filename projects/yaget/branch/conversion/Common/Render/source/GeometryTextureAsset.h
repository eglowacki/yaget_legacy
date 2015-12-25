//////////////////////////////////////////////////////////////////////
// GeometryTextureAsset.h
//
//  Copyright 1/6/2008 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Provides texture data loaded from some resource
//
//
//  #include "GeometryTextureAsset.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef GEOMETRY_TEXTURE_ASSET_H
#define GEOMETRY_TEXTURE_ASSET_H
#pragma once

#include "Asset/MediaAsset.h"
#include "Asset/AssetHandle.h"
#pragma warning (disable:4312)  // 'variable' : conversion from 'type' to 'type' of greater size
    #include <atlcomcli.h>
    #include <d3dx9mesh.h>
#pragma warning (default : 4312)



namespace eg
{
	namespace missingasset
	{
		static const char *GeometryTexture = "Missing.tga";
	}  // namespace missingasset

    class IRenderContext;

    class GeometryTextureAsset : public eg::MediaAsset<GeometryTextureAsset>
    {
    public:
        static const guid_t kType = 0x74b0defd;

        GeometryTextureAsset(const std::string& name);
        ~GeometryTextureAsset();

        CComPtr<IDirect3DTexture9> GetNativeObject() const;

        // from IAsset
        virtual guid_t Type() const {return GeometryTextureAsset::kType;}

    private:
        // From eg::MediaAsset
        virtual void SaveBlob(uint8_t *&pData, size_t& size) const;
        virtual void LoadBlob(const uint8_t *pData, size_t size, const std::string& streamName);

        CComPtr<IDirect3DTexture9> mTextureObject;

		void onMissingAssetChanged(GeometryTextureAsset::Asset_t asset, const std::string& name);
        void onFinishedLoading(GeometryTextureAsset::Asset_t asset, const std::string& name);
        //! this is only used for resetting video device
        void onVideoReset(eg::Message& msg);
        void onVideoChanged(eg::Message& msg);
        boost::signals::connection mResetConnection;
        boost::signals::connection mChangedConnection;

		boost::signals::connection mMissingAssetConnection;

    };

    typedef asset::AssetHandle<GeometryTextureAsset, asset::AutoLock<GeometryTextureAsset>, asset::LoadTypeDynamic> GeometryTextureAsset_t;

} // namespace eg


#endif // GEOMETRY_TEXTURE_ASSET_H
