#include "Material.h"
#include "Asset/IAsset.h"
#include "File/AssetLoadSystem.h"
#include "File/VirtualFileSystem.h"
#include "DXErrorHandler.h"
#include "IRendererPlugin.h"
#include "File/XmlHelpers.h"
//#include <tinyxml.h>
#include "Material2.hxx"
#include <D3DX9Effect.h>
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/variant/apply_visitor.hpp>



namespace
{
    using namespace eg;

    class MaterialAssetFactory;

    class MaterialAsset : public IAsset
    {
    public:
        static const guid_t kType = 0xc89c6500;

        MaterialAsset(uint64_t id, const std::string& name);

        // from IAsset
        virtual uint64_t GetId() const {return mId;}
        virtual const std::string& Name() const {return mName;}
        virtual const std::string& Fqn() const {return mFqn;}
        virtual Locker_t Lock() {return Locker_t();}
        virtual guid_t Type() const {return MaterialAsset::kType;}
        virtual void HandleMessage(const Message &/*msg*/) {}

    private:
        friend MaterialAssetFactory;
        uint64_t mId;
        std::string mName;
        std::string mFqn;
    };

    MaterialAsset::MaterialAsset(uint64_t id, const std::string& name)
    : mId(id)
    , mName(name)
    {}


    //-------------------------------------------------------------------------------------
    class MaterialAssetFactory
    {
        // no copy semantics
        MaterialAssetFactory(const MaterialAssetFactory&);
        MaterialAssetFactory& operator=(const MaterialAssetFactory&);

    public:
        MaterialAssetFactory(AssetLoadSystem& als)
        : mals(als)
        {
            mals.RegisterFactory(MaterialAsset::kType, boost::bind(&MaterialAssetFactory::New, this, _1, _2),
                                                       boost::bind(&MaterialAssetFactory::Load, this, _1, _2, _3),
                                                       boost::bind(&MaterialAssetFactory::Save, this, _1, _2, _3),
                                                       boost::bind(&MaterialAssetFactory::Query, this, _1, _2));
        }

        ~MaterialAssetFactory()
        {
            mals.UnregisterFactory(MaterialAsset::kType);
        }

        void AddMaterial(shaders::Material *material)
        {
            mMaterials.push_back(material);
            mals.GetAsset<MaterialAsset>(material->Name(), AssetLoadSystem::eLoadBlocking);
        }

        void RemoveMaterial(shaders::Material *material)
        {
            std::remove(mMaterials.begin(), mMaterials.end(), material);
        }

        IAsset *New(const std::string& name, uint64_t asssetId)
        {
            IAsset *pAsset = new MaterialAsset(asssetId, name);
            return pAsset;
        }

        void Load(AssetLoadSystem::IAsset_t asset, VirtualFileFactory::istream_t stream, const std::string& name)
        {
            boost::shared_ptr<MaterialAsset> materialAsset = boost::dynamic_pointer_cast<MaterialAsset>(asset);
            if (materialAsset && stream && eg::stream::size(*stream))
            {
                BOOST_FOREACH(shaders::Material *material, mMaterials)
                {
                    if (asset->Name() == material->Name())
                    {
                        std::string fqn = mals.vfs().GetFqn(name);
                        materialAsset->mFqn = fqn;
                        material->load(*stream);
                    }
                }
            }
        }

        void Save(AssetLoadSystem::IAsset_t asset, VirtualFileFactory::ostream_t stream, const std::string& name)
        {
            name;
            if (stream)
            {
                BOOST_FOREACH(shaders::Material *material, mMaterials)
                {
                    if (asset->Name() == material->Name())
                    {
                        material->save(*stream);
                        return;
                    }
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
            file += ".mat";
            files.push_back(file);
            return files;
        }

        AssetLoadSystem& mals;
        std::list<shaders::Material *> mMaterials;
    };



    struct input_apply_visitor : public boost::static_visitor<>
    {
        void operator()(bool v, const CComPtr<ID3DXEffect>& shader, D3DXHANDLE hParam) const
        {
            HRESULT hr = shader->SetBool(hParam, v);
            HR_REPORT_INVALID(hr);
        }
        void operator()(int v, const CComPtr<ID3DXEffect>& shader, D3DXHANDLE hParam) const
        {
            HRESULT hr = shader->SetInt(hParam, v);
            HR_REPORT_INVALID(hr);
        }
        void operator()(float v, const CComPtr<ID3DXEffect>& shader, D3DXHANDLE hParam) const
        {
            HRESULT hr = shader->SetFloat(hParam, v);
            HR_REPORT_INVALID(hr);
        }
        void operator()(const Vector2& v, const CComPtr<ID3DXEffect>& shader, D3DXHANDLE hParam) const
        {
            HRESULT hr = shader->SetFloatArray(hParam, v.getF(), 2);
            HR_REPORT_INVALID(hr);
        }
        void operator()(const Vector3& v, const CComPtr<ID3DXEffect>& shader, D3DXHANDLE hParam) const
        {
            HRESULT hr = shader->SetFloatArray(hParam, v.getF(), 3);
            HR_REPORT_INVALID(hr);
        }
        void operator()(const Vector4& v, const CComPtr<ID3DXEffect>& shader, D3DXHANDLE hParam) const
        {
            HRESULT hr = shader->SetFloatArray(hParam, v.getF(), 4);
            HR_REPORT_INVALID(hr);
        }
        void operator()(const Matrix33& v, const CComPtr<ID3DXEffect>& shader, D3DXHANDLE hParam) const
        {
            Matrix44 matrix(v);
            HRESULT hr = shader->SetFloatArray(hParam, matrix.GetF(), 12);
            HR_REPORT_INVALID(hr);
        }
        void operator()(const Matrix44& v, const CComPtr<ID3DXEffect>& shader, D3DXHANDLE hParam) const
        {
            HRESULT hr = shader->SetFloatArray(hParam, v.GetF(), 16);
            HR_REPORT_INVALID(hr);
        }
    };

    boost::shared_ptr<MaterialAssetFactory> materialAssetFactory;

} // namespace

namespace eg {
namespace shaders {


void Material::Register(AssetLoadSystem &als)
{
    materialAssetFactory.reset(new MaterialAssetFactory(als));
}


Material::Material(const std::string& name, const CComPtr<ID3DXEffect>& shaderObject, const Inputs_t& shaderInputs)
: mName(name)
, mShaderObject(shaderObject)
, mErrorTries(5)
, mStateFlags(0)
, mShaderInputs(shaderInputs)
{
    if (!materialAssetFactory)
    {
        using namespace boost;
        throw ex::bad_init(str(format("MaterialAssetFactory is not valid for '%1%'. Call Material::Register(AssetLoadSystem &) first.") % name).c_str());
    }

    materialAssetFactory->AddMaterial(this);
}


Material::~Material()
{
    materialAssetFactory->RemoveMaterial(this);
}


const std::string& Material::Name() const
{
    return mName;
}


void Material::applyInputs() const
{
    // make a map key by input name
    typedef std::map<std::string, const Input *> CurrentInputs_t;
    CurrentInputs_t inputs;
    BOOST_FOREACH(const Input& input, mShaderInputs)
    {
        inputs[input.name()] = &input;
    }

    // apply value to shader input using material inputs first
    BOOST_FOREACH(const Input& input, mMaterialInputs)
    {
        std::map<std::string, const Input *>::iterator it = inputs.find(input.name());
        if (it != inputs.end() && (*it).second)
        {
            (*it).second->apply(mShaderObject, &input);
            (*it).second = 0;
        }
        else
        {
            log_warning << "Input: '" << input.name() << "' for Material: '" << Name() << "' does not exist in Shader or was called more then once.";
        }
    }

    //! then whatever inputs are left over, we'll set it to default value
    BOOST_FOREACH(CurrentInputs_t::value_type input, inputs)
    {
        if (input.second)
        {
            input.second->apply(mShaderObject, 0);
        }
    }
}


void Material::Render(Callback_t callback) const
{
    if (!callback || !mErrorTries || !mShaderObject)
    {
        return;
    }

    applyInputs();

    UINT passes = 0;
    HRESULT hr = mShaderObject->Begin(&passes, mStateFlags);
    if (FAILED(hr))
    {
        HR_PRINT(hr);
        mErrorTries--;
        return;
    }

    for (UINT p = 0; p < passes; p++)
    {
        hr = mShaderObject->BeginPass(p);
        if (FAILED(hr))
        {
            HR_PRINT(hr);
            mErrorTries--;
            return;
        }

        callback();

        hr = mShaderObject->EndPass();
        if (FAILED(hr))
        {
            HR_PRINT(hr);
            mErrorTries--;
            return;
        }
    }

    hr = mShaderObject->End();
    if (FAILED(hr))
    {
        HR_PRINT(hr);
        mErrorTries--;
        return;
    }

    mErrorTries = 5;
}


void Material::save(std::ostream& /*stream*/) const
{
    log_trace(tr_render) << "Saving '" << Name() << "' <MaterialAsset> asset. version = " << Material::version;

    //stream << std::noskipws;
    //stream << mSource;
}




namespace
{

    bool isInputSame(const Material::Input& input, const std::string& name)
    {
        return boost::algorithm::iequals(input.name(), name);
    }
} // namespace


const Material::Input *Material::findInput(const std::string& name) const
{
    //using namespace boost;
    Inputs_t::const_iterator it = std::find_if(mShaderInputs.begin(), mShaderInputs.end(), boost::bind(&isInputSame, _1, name));

    //bind(&Material::Input::name, _1)
    //Inputs_t::const_iterator it = std::find_if(mShaderInputs.begin(), mShaderInputs.end(), boost::bind(boost::algorithm::iequals, boost::bind(&Material::Input::name, _1), name));
    if (it != mShaderInputs.end())
    {
        return &(*it);
    }

    return 0;
}


Material::Input& Material::GetInput(const std::string& name)
{
    Inputs_t::iterator it = std::find_if(mMaterialInputs.begin(), mMaterialInputs.end(), boost::bind(&isInputSame, _1, name));
    if (it != mMaterialInputs.end())
    {
        return *it;
    }

    static Input dummyInput;
    return dummyInput;
}


void Material::load(std::istream& stream)
{
    /*
    try
    {
        std::auto_ptr<Material2> xsdMaterial = Material2_(stream);
    }
    catch (const xml_schema::exception& e)
    {
        const char * errorText = e.what();
        int z = 0;
    }
    */

    using namespace boost;

    std::string xmlData;
    stream >> std::noskipws;
    std::copy(std::istream_iterator<char>(stream), std::istream_iterator<char>(), std::back_inserter(xmlData));

    mMaterialInputs.clear();
    TiXmlDocument doc;
    if (doc.Parse(xmlData.c_str()))
    {
        TiXmlHandle doc_handle(&doc);
        TiXmlElement *rootNode = doc_handle.FirstChild("Material").FirstChild("Inputs").ToElement();

        TiXmlNode *child = 0;
        while ((child = rootNode->IterateChildren(child)) != 0)
        {
            try
            {
                std::string name = xml::get_prop<std::string>(child, "name");
                if (const Input *shaderInput = findInput(name))
                {
                    std::string inputType = xml::get_value<std::string>(child, "Type");
                    /*int inputPass =*/ xml::get_value<int>(child, "Pass");
                    std::string inputValue = xml::get_value<std::string>(child, "Value");
                    Input matInput(shaderInput->name(), 0);
                    if (algorithm::iequals(inputType, "bool"))
                    {
                        matInput.set<bool>(inputValue);
                    }
                    else if (algorithm::iequals(inputType, "int"))
                    {
                        matInput.set<int>(inputValue);
                    }
                    else if (algorithm::iequals(inputType, "float"))
                    {
                        matInput.set<float>(inputValue);
                    }
                    else if (algorithm::iequals(inputType, "Vector2"))
                    {
                        matInput.set<Vector2>(inputValue);
                    }
                    else if (algorithm::iequals(inputType, "Vector3"))
                    {
                        matInput.set<Vector3>(inputValue);
                    }
                    else if (algorithm::iequals(inputType, "Vector4"))
                    {
                        matInput.set<Vector4>(inputValue);
                    }
                    else if (algorithm::iequals(inputType, "Matrix33"))
                    {
                        matInput.set<Matrix33>(inputValue);
                    }
                    else if (algorithm::iequals(inputType, "Matrix44"))
                    {
                        matInput.set<Matrix44>(inputValue);
                    }

                    mMaterialInputs.push_back(matInput);
                }
                else
                {
                    log_error << "Could not find input '" << name << "' on material '" << Name() << "'.";
                }
            }
            catch (const ex::bad_param& e)
            {
                log_error << "Could not process input data from material: '" << Name() << "'. Exception: " << e.what();
            }
        }
    }


    if (const Input *shaderInput = findInput("worldViewProj"))
    {
        Inputs_t::iterator it = std::find_if(mMaterialInputs.begin(), mMaterialInputs.end(), boost::bind(&isInputSame, _1, "worldViewProj"));
        if (it == mMaterialInputs.end())
        {
            // let's add this one from shader as to always expose some default inputs
            // \note how do we drive which inputs are auto update/expose
            Input matInput(shaderInput->name(), 0);
            matInput.set<Matrix44>(shaderInput->get<Matrix44>());
            mMaterialInputs.push_back(matInput);
        }
    }
}


void Material::Input::apply(const CComPtr<ID3DXEffect>& shader, const Input *input) const
{
    boost::apply_visitor(boost::bind(input_apply_visitor(), _1, shader, static_cast<D3DXHANDLE>(mParamh)), input ? input->mValue : mValue);
}


} // namespace shaders
} // namespace eg
