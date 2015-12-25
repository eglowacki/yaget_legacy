//////////////////////////////////////////////////////////////////////
// ShaderAsset.h
//
//  Copyright 2/9/2008 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Provides Shader data loaded from some resource
//
//
//  #include "ShaderAssmet.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef SHADER_ASSET_H
#define SHADER_ASSET_H
#pragma once

#include "MaterialAsset.h"
#include "GeometryAsset.h"


namespace eg
{
	namespace missingasset
	{
		static const char *Shader = "Missing.fx";
	}  // namespace missingasset

    /*!
       This asset provides shader object with which we render
       any geometry by passing geometry asset as a parameter or
       boost::function \see geomRender_t. User also must provide
       material asset, which inputs will be assigned to this shader
       before triggering rendering of geometry.
     */
    class ShaderAsset : public eg::MediaAsset<ShaderAsset>
    {
    public:
        static const guid_t kType = 0x54b9fcdb;

        //! Callback for geometry to be rendered. Entire shader state is already setup
        //! and should not be adjusted inside this function.
        typedef boost::function<void (const IRenderContext&  /*renderContext*/)> geomRender_t;

        ShaderAsset(const std::string& name);
        ~ShaderAsset();

        /*!
           Render particular geometry asset using material.

           \param pRenderContext All matricies and some data about camera.
           \param materialAsset Material asset which inputs are used into thi sshder before triggering rendering.
           \param geometryAsset Geometry asset to render or null and then geomRender must be set
           \param geomRender If geometryAsset is set to nuill, then this callack will be used.
         */
        void Render(const IRenderContext& renderContext, MaterialAsset::Asset_t materialAsset, GeometryAsset::Asset_t geometryAsset, geomRender_t geomRender);

        // from IAsset
        virtual guid_t Type() const {return ShaderAsset::kType;}

    private:
		void onMissingAssetChanged(ShaderAsset::Asset_t asset, const std::string& name);
		void onFinishedLoading(ShaderAsset::Asset_t asset, const std::string& name);
        // From eg::MediaAsset
        virtual void SaveBlob(uint8_t *&pData, size_t& size) const;
        virtual void LoadBlob(const uint8_t *pData, size_t size, const std::string& streamName);

        //! Helper method to resolve all valid input into this shader.
        void resolve();
        void setDefaultInputs();
        void setInput(const MaterialAsset::Input& input, const boost::any& value);
        void setInput(const MaterialAsset::Input& input);
        const MaterialAsset::Input *findShaderInput(const MaterialAsset::Input& input) const;

        //! Actual shader object
        CComPtr<ID3DXEffect> mShaderObject;
        std::vector<MaterialAsset::Input> mInputs;
        MaterialAsset::Input mWVPMatrix;

        //! this is only used for resetting video device
        void onVideoReset(eg::Message& msg);
        void onVideoChanged(eg::Message& msg);
        boost::signals::connection mResetConnection;
        boost::signals::connection mChangedConnection;
		boost::signals::connection mMissingAssetConnection;
    };


    typedef asset::AssetHandle<ShaderAsset, asset::AutoLock<ShaderAsset>, asset::LoadTypeDynamic> ShaderAsset_t;

} // namespace eg


BOOST_CLASS_VERSION(eg::ShaderAsset, 1);

#endif // SHADER_ASSET_H
