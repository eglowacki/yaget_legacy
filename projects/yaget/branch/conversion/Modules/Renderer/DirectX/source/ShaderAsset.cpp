//#include "Precompiled.h"
#include "ShaderAsset.h"
#include "MessageInterface.h"
#include "File/AssetLoadSystem.h"
#include "File/VirtualFileSystem.h"
#include "File/DiskFileFactory.h"
#include "DXErrorHandler.h"
#include <D3dx9core.h>
#include "StringHelper.h"
#include <boost/bind.hpp>
#include <boost/foreach.hpp>

namespace eg {

namespace
{
    const char *kShaderInputTag = "SHADER_INPUT";

    class ShaderAssetFactory : public IAsset::Factory
    {
        ShaderAssetFactory(const ShaderAssetFactory&);
        ShaderAssetFactory& operator=(const ShaderAssetFactory&);

    public:
        ShaderAssetFactory(AssetLoadSystem& als, CComPtr<IDirect3DDevice9> device)
        : mAls(als)
        , mLoadType(AssetLoadSystem::eLoadBlocking)
        , mDevice(device)
        {
            mAls.RegisterFactory(ShaderAsset::kType, boost::bind(&ShaderAssetFactory::New, this, _1, _2),
                                                     boost::bind(&ShaderAssetFactory::Load, this, _1, _2, _3),
                                                     boost::bind(&ShaderAssetFactory::Save, this, _1, _2, _3),
                                                     boost::bind(&ShaderAssetFactory::Query, this, _1, _2));

            log_trace(tr_render) << "ShaderAssetFactory object created.";
        }

        ~ShaderAssetFactory()
        {
            mAls.UnregisterFactory(ShaderAsset::kType);
            log_trace(tr_render) << "ShaderAssetFactory object deleted.";
        }

    private:
        IAsset *New(const std::string& name, uint64_t asssetId)
        {
            IAsset *pAsset = new ShaderAsset(asssetId, name, mDevice);
            return pAsset;
        }

        void Load(AssetLoadSystem::IAsset_t asset, VirtualFileFactory::istream_t stream, const std::string& name)
        {
            boost::shared_ptr<ShaderAsset> shaderAsset = boost::dynamic_pointer_cast<ShaderAsset>(asset);
            if (shaderAsset)
            {
                if (stream && eg::stream::size(*stream))
                {
                    if (WildCompareISafe("*.fx.options", name))
                    {
                        // we have options let's process it
                        stream::stream_addapter<std::istream> astream(*stream);
                        astream >> shaderAsset->mDebug;
                    }
                    else if (WildCompareISafe("*.fx", name))
                    {
                        std::string fqn = mAls.vfs().GetFqn(name);
                        shaderAsset->mFqn = fqn;
                        shaderAsset->load(*stream);
                    }
                }
            }
        }

        void Save(AssetLoadSystem::IAsset_t asset, VirtualFileFactory::ostream_t stream, const std::string& name)
        {
            name;
            // we can catch ex::serialize exception here if needed.
            // AssetLoadSystem does this already.
            boost::shared_ptr<ShaderAsset> shaderAsset = boost::dynamic_pointer_cast<ShaderAsset>(asset);
            if (shaderAsset && stream)
            {
                if (WildCompareISafe("*.fx.options", name))
                {
                    stream::stream_addapter<std::ostream> astream(*stream);
                    astream << shaderAsset->mDebug;
                }
                else if (WildCompareISafe("*.fx", name))
                {
                    shaderAsset->save(*stream);
                }
            }
        }

        std::vector<std::string> Query(AssetLoadSystem::IAsset_t asset, const VirtualFileSystem& vfs)
        {
            vfs;
            const std::string& name = asset->Name();
            // here we control the order of iteration over streams for usage in Load(...) and Save(...) methods
            std::vector<std::string> files;
            std::string file = name;
            file += ".options.fx";
            files.push_back(file);
            file = name;
            file += ".fx";
            files.push_back(file);
            return files;
        }

        AssetLoadSystem& mAls;
        AssetLoadSystem::Load mLoadType;
        CComPtr<IDirect3DDevice9> mDevice;
    };

} // namespace


IAsset::Token ShaderAsset::Register(AssetLoadSystem &als, CComPtr<IDirect3DDevice9> device)
{
    Token factory(new ShaderAssetFactory(als, device));
    shaders::Material::Register(als);
    return factory;
}


ShaderAsset::ShaderAsset(uint64_t id, const std::string& name, CComPtr<IDirect3DDevice9> device)
: mState(sNone)
, mId(id)
, mName(name)
, mFqn(name)
, mDebug(true)
, mDevice(device)
{
}


uint64_t ShaderAsset::GetId() const
{
    return mId;
}


const std::string& ShaderAsset::Name() const
{
    return mName;
}


const std::string& ShaderAsset::Fqn() const
{
    return mFqn;
}

IAsset::Locker_t ShaderAsset::Lock()
{
    return Locker_t();
}


guid_t ShaderAsset::Type() const
{
    return ShaderAsset::kType;
}


void ShaderAsset::HandleMessage(const Message &msg)
{
    msg;
}


void ShaderAsset::save(std::ostream& stream) const
{
    log_trace(tr_render) << "Saving '" << Name() << "' <ShaderAsset> asset. version = " << ShaderAsset::version;

    stream << std::noskipws;
    stream << mSource;
}


void ShaderAsset::load(std::istream& stream)
{
    // this call could have originate from different thread
    // so we do not want to mess with lua and only copy to memory
    mSource.clear();
    stream >> std::noskipws;
    std::copy(std::istream_iterator<char>(stream), std::istream_iterator<char>(), std::back_inserter(mSource));

    DWORD flags = mDebug ? (D3DXSHADER_DEBUG|D3DXSHADER_SKIPOPTIMIZATION|D3DXSHADER_NO_PRESHADER) : 0;
    CComPtr<ID3DXEffect> shaderObject;
    CComPtr<ID3DXBuffer> errorBuffer;
    HRESULT hr = D3DXCreateEffect(mDevice,
                                  mSource.c_str(),
                                  static_cast<UINT>(mSource.size()),
                                  0,
                                  0,
                                  flags,
                                  0,
                                  &shaderObject,
                                  &errorBuffer);

    if (FAILED(hr))
    {
        mState = sError;
        mShaderObject = 0;
        HR_PRINT(hr);

        const char *pCompilerErrors = errorBuffer ? (const char *)errorBuffer->GetBufferPointer() : "No errors available";
        log_error << "Failed to create '" << Name() << ".fx' Shader Asset. Error: '" << pCompilerErrors << "'.";
        return;
    }

    mShaderObject.Attach(shaderObject.Detach());
    mState = sLoaded;
    resolveInputs();
}


shaders::Material_t ShaderAsset::GetMaterial(const std::string& name) const
{
    std::string matName = name.empty() ? Name() : name;
    return shaders::Material_t(new shaders::Material(matName, mShaderObject, mShaderInputs));
}

namespace
{
    bool isInputSame(const eg::shaders::Material::Input& input, const std::string& name)
    {
        return boost::algorithm::iequals(input.name(), name);
    }
} // namespace

void ShaderAsset::resolveInputs()
{
    D3DXEFFECT_DESC desc;
    HRESULT hr = mShaderObject->GetDesc(&desc);
    HR_REPORT_INVALID(hr);
    log_trace(tr_render) << "Resolving ShaderAsset: '" << Name() << "' with number of raw inputs: " << desc.Parameters << "...";

    mShaderInputs.clear();
    for (UINT p = 0; p < desc.Parameters; p++)
    {
        D3DXHANDLE hParam = mShaderObject->GetParameter(0, p);
        if (hParam)
        {
            D3DXPARAMETER_DESC paramDesc;
            hr = mShaderObject->GetParameterDesc(hParam, &paramDesc);
            HR_REPORT_INVALID(hr);

            if (!paramDesc.Semantic)
            {
                continue;
            }

            std::string semantic(paramDesc.Semantic);
            if (boost::algorithm::iequals(semantic, kShaderInputTag))
            {
                if (paramDesc.Class == D3DXPC_VECTOR && paramDesc.Type == D3DXPT_FLOAT)
                {
                    float values[4] = {0, 0, 0, 1};
                    hr = mShaderObject->GetFloatArray(hParam, values, paramDesc.Columns);
                    HR_REPORT_INVALID(hr);
                    shaders::Material::Input input(paramDesc.Name, (void *)hParam);
                    switch (paramDesc.Columns)
                    {
                    case 1:
                        input.set<float>(values[0]);
                        break;
                    case 2:
                        input.set<Vector2>(Vector2(values));
                        break;
                    case 3:
                        input.set<Vector3>(Vector3(values));
                        break;
                    case 4:
                        input.set<Vector4>(Vector4(values));
                        break;
                    }
                    mShaderInputs.push_back(input);
                    log_trace(tr_render) << "   Found Shader Input: '" << paramDesc.Name << "', of type Vector" << paramDesc.Columns << ".";
                }
                else if (paramDesc.Class == D3DXPC_MATRIX_ROWS && paramDesc.Type == D3DXPT_FLOAT)
                {
                    float values[16] = {1, 0, 0, 0,
                                        0, 1, 0, 0,
                                        0, 0, 1, 0,
                                        0, 0, 0, 1};
                    hr = mShaderObject->GetFloatArray(hParam, values, paramDesc.Columns*paramDesc.Rows);
                    HR_REPORT_INVALID(hr);
                    if (paramDesc.Columns*paramDesc.Rows == 16)
                    {
                        shaders::Material::Input input(paramDesc.Name, (void *)hParam);
                        input.set<Matrix44>(Matrix44(values));
                        mShaderInputs.push_back(input);
                        log_trace(tr_render) << "   Found Shader Input: '" << paramDesc.Name << "', of type Matrix" << paramDesc.Columns << paramDesc.Rows << ".";
                    }
                }
                else
                {
                    log_trace(tr_render) <<    "Shader Input: '" << (paramDesc.Name ? paramDesc.Name : "") << "' not handled." <<
                            "\n   Semantic: '" << (paramDesc.Semantic ? paramDesc.Semantic : "") << "'" <<
                            "\n   Class: " << paramDesc.Class <<
                            "\n   Type: " << paramDesc.Type <<
                            "\n   Col/Row: (" << paramDesc.Columns << "/" << paramDesc.Rows << ")" <<
                            "\n   Elements: " << paramDesc.Elements <<
                            "\n   Flags: " << paramDesc.Flags;
                }

                for (UINT a = 0; a < paramDesc.Annotations; a++)
                {
                    D3DXHANDLE hAnnot = mShaderObject->GetAnnotation(hParam, a);
                    if (hAnnot)
                    {
                        D3DXPARAMETER_DESC annotDesc;
                        hr = mShaderObject->GetParameterDesc(hAnnot, &annotDesc);
                        HR_REPORT_INVALID(hr);
                        log_trace(tr_render) << "      Shader Input: '" << paramDesc.Name << "' has Annotation: '" << annotDesc.Name << "', Class: " << annotDesc.Class << ", Type: " << annotDesc.Type << ", Col/Row: (" << annotDesc.Columns << "/" << annotDesc.Rows << ").";
                    }
                }
            }
        }
    }

    // \note do we need a way to signal observers that our shader potentially changed?

    log_trace(tr_render) << "Finished ShaderAsset Shader: '" << Name() << "'.";
}



} // namespace eg

