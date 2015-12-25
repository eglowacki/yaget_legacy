/////////////////////////////////////////////////////////////////////////
// ShaderAsset.h
//
//  Copyright 6/1/2009 Edgar Glowacki.
//
// NOTES:
//
//
// #include "ShaderAsset.h"
//
/////////////////////////////////////////////////////////////////////////
//! \file

#ifndef SHADER_ASSET_H
#define SHADER_ASSET_H
#pragma once

#include "Asset/IAsset.h"
#include "IRendererPlugin.h"
#include "Material.h"
#include "Math/Matrix.h"

interface IDirect3DDevice9;
interface ID3DXEffect;

namespace eg
{
    class AssetLoadSystem;
    namespace {class ShaderAssetFactory;}

    class ShaderAsset : public IAsset
    {
    public:
        static const guid_t kType = 0xdb023e63;

        ShaderAsset(uint64_t id, const std::string& name, CComPtr<IDirect3DDevice9> device);
        //! Return unique instance of this shader. Material can be used to override any exposed inputs
        // \param name is only used to try to find load stream for it in one exists
        shaders::Material_t GetMaterial(const std::string& name) const;

        // from IAsset
        virtual uint64_t GetId() const;
        virtual const std::string& Name() const;
        virtual const std::string& Fqn() const;
        virtual Locker_t Lock();
        virtual guid_t Type() const;
        virtual void HandleMessage(const Message &msg);

        static Token Register(AssetLoadSystem &als, CComPtr<IDirect3DDevice9> device);

    private:
        void resolveInputs();

        enum State {sNone, sError, sLoaded};
        State mState;
        uint64_t mId;
        std::string mName;
        std::string mFqn;
        std::string mSource;
        bool mDebug;
        CComPtr<IDirect3DDevice9> mDevice;
        CComPtr<ID3DXEffect> mShaderObject;
        shaders::Material::Inputs_t mShaderInputs;

        typedef std::map<uint64_t, shaders::Material::Inputs_t> MaterialInputs_t;
        mutable MaterialInputs_t mMaterialInputs;

        friend ShaderAssetFactory;
        void load(std::istream& stream);
        void save(std::ostream& stream) const;

        static const uint32_t version = 1;

        //! \todo add notifications for live updates
    };

    typedef boost::shared_ptr<ShaderAsset> ShaderAsset_t;

} // namespace eg

#endif // SHADER_ASSET_H

