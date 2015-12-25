// ShaderAsset.cpp
#include "Precompiled.h"
#include "ShaderAsset.h"
#include "GeometryTextureAsset.h"
#include "Message/Dispatcher.h"
#include "IdGameCache.h"
#include "Interface/IRenderContext.h"
#include "StringHelper.h"
#include "Renderer.h"
#include "ErrorHandler.h"
#include <boost/bind.hpp>


namespace
{
    using namespace eg;

    MaterialAsset::Input::Type getTypeScalar(const D3DXPARAMETER_DESC& paramDesc)
    {
        switch (paramDesc.Type)
        {
            case D3DXPT_BOOL:
                return MaterialAsset::Input::tBool;
                break;

            case D3DXPT_INT:
                return MaterialAsset::Input::tInt;
                break;

            case D3DXPT_FLOAT:
                return MaterialAsset::Input::tFloat;
                break;
        }

        return MaterialAsset::Input::tNone;
    }

    MaterialAsset::Input::Type getTypeVector(const D3DXPARAMETER_DESC& paramDesc)
    {
        switch (paramDesc.Type)
        {
            case D3DXPT_FLOAT:
                if (paramDesc.Columns == 2)
                {
                    return MaterialAsset::Input::tVector2;
                }
                else if (paramDesc.Columns == 3)
                {
                    return MaterialAsset::Input::tVector3;
                }
                else if (paramDesc.Columns == 4)
                {
                    return MaterialAsset::Input::tVector4;
                }
                break;
        }

        return MaterialAsset::Input::tNone;
    }

    MaterialAsset::Input::Type getTypeMatrix(const D3DXPARAMETER_DESC& paramDesc)
    {
        switch (paramDesc.Type)
        {
            case D3DXPT_FLOAT:
                if (paramDesc.Rows == 4 && paramDesc.Columns == 4)
                {
                    return MaterialAsset::Input::tMatrix44;
                }
                break;
        }

        return MaterialAsset::Input::tNone;
    }

    MaterialAsset::Input::Type getTypeObject(const D3DXPARAMETER_DESC& paramDesc)
    {
        switch (paramDesc.Type)
        {
        case D3DXPT_TEXTURE:
            return MaterialAsset::Input::tTexture;
            break;
        }

        return MaterialAsset::Input::tNone;
    }

    MaterialAsset::Input::Type getType(const D3DXPARAMETER_DESC& paramDesc)
    {
        if (paramDesc.Class == D3DXPC_SCALAR)
        {
            return getTypeScalar(paramDesc);
        }
        else if (paramDesc.Class == D3DXPC_VECTOR)
        {
            return getTypeVector(paramDesc);
        }
        else if (paramDesc.Class == D3DXPC_MATRIX_ROWS || paramDesc.Class == D3DXPC_MATRIX_COLUMNS)
        {
            return getTypeMatrix(paramDesc);
        }
        else if (paramDesc.Class == D3DXPC_OBJECT)
        {
            return getTypeObject(paramDesc);
        }
        else if (paramDesc.Class == D3DXPC_STRUCT)
        {
            //.. don't know this one
            wxASSERT_MSG(0, "Do not know what is D3DXPC_STRUCT for type conversion.");
        }

        return MaterialAsset::Input::tNone;
    }

} // namespace


namespace eg
{

ShaderAsset::ShaderAsset(const std::string& name)
: eg::MediaAsset<ShaderAsset>(name)
{
    Dispatcher& disp = REGISTRATE(Dispatcher);
    mResetConnection = disp[mtype::kRendererVideoModeReset].connect(boost::bind(&ShaderAsset::onVideoReset, this, _1));
    mChangedConnection = disp[mtype::kRendererVideoModeChanged].connect(boost::bind(&ShaderAsset::onVideoChanged, this, _1));

    // when asset is finished loading, it will trigger signal, which will allow us to use missing material
    // if there is not loaded one.
    sigLoadFinish.connect(0, boost::bind(&ShaderAsset::onFinishedLoading, this, _1, _2));
}

ShaderAsset::~ShaderAsset()
{
    mResetConnection.disconnect();
    mChangedConnection.disconnect();
    mMissingAssetConnection.disconnect();
}


void ShaderAsset::onVideoReset(eg::Message& /*msg*/)
{
    if (mShaderObject)
    {
        mShaderObject->OnLostDevice();
    }
}


void ShaderAsset::onVideoChanged(eg::Message& /*msg*/)
{
    if (mShaderObject)
    {
        mShaderObject->OnResetDevice();
    }
}


void ShaderAsset::onFinishedLoading(ShaderAsset::Asset_t asset, const std::string& name)
{
    DataLocker lock(*this);
    if (!mShaderObject)
    {
        wxLogWarning("Shader Asset '%s' was not loaded successfully, using '%s' as a placeholder.", name.c_str(), missingasset::Shader);

        asset::AssetLocker<ShaderAsset, asset::NoSyncAssetPolicy> assetLocker(missingasset::Shader);
        if (ShaderAsset *source = assetLocker)
        {
            mMissingAssetConnection = source->sigLoadFinish.connect(0, boost::bind(&ShaderAsset::onMissingAssetChanged, this, _1, _2));
            mShaderObject = source->mShaderObject;
            mInputs = source->mInputs;
            mWVPMatrix = source->mWVPMatrix;
        }
    }
}


void ShaderAsset::onMissingAssetChanged(ShaderAsset::Asset_t asset, const std::string& /*name*/)
{
    DataLocker lock(*this);
    mShaderObject = asset->mShaderObject;
    mInputs = asset->mInputs;
    mWVPMatrix = asset->mWVPMatrix;
}


void ShaderAsset::SaveBlob(uint8_t *&/*pData*/, size_t& /*size*/) const
{
    /*
    DataLocker lock(*this);

    size = mScriptData.size();
    pData = new uint8_t[size];
    memcpy(pData, mScriptData.c_str(), size);
    */
}


void ShaderAsset::LoadBlob(const uint8_t *pData, size_t size, const std::string& streamName)
{
    mMissingAssetConnection.disconnect();
    // first, we'll just load all the needed data into local variables,
    // thus avoiding locking in Render(...) method
    CComPtr<ID3DXEffect> shaderObject;
    CComPtr<ID3DXBuffer> errorBuffer;
    HRESULT hr = D3DXCreateEffect(Renderer::mD3DDevice,
                                  pData,
                                  static_cast<UINT>(size),
                                  0,
                                  0,
                                  D3DXSHADER_DEBUG|D3DXSHADER_SKIPOPTIMIZATION|D3DXSHADER_NO_PRESHADER,
                                  0,
                                  &shaderObject,
                                  &errorBuffer);

    if (HR_IS_VALID(hr))
    {
        wxASSERT(isLocked() == false);
        DataLocker lock(*this);
        mInputs.clear();
        mShaderObject = shaderObject;
        resolve();
    }
    else
    {
        const char *pCompilerErrors = errorBuffer ? (const char *)errorBuffer->GetBufferPointer() : "No errors available";
        wxLogError("Failed to create '%s' Shader Asset. Error: '%s'.", streamName.c_str(), pCompilerErrors);
    }
}


void ShaderAsset::Render(const IRenderContext& renderContext, MaterialAsset::Asset_t materialAsset, GeometryAsset::Asset_t geometryAsset, geomRender_t geomRender)
{
    wxASSERT(isLocked());
    wxASSERT(materialAsset);
    wxASSERT(geometryAsset || geomRender);

    if (mShaderObject)
    {
        //! reset all inputs to default state
        //! \note we need to optimize this to only set when there is no material input
        setDefaultInputs();
        const std::vector<MaterialAsset::Input>& matInputs = materialAsset->GetInputs(0);

        // set all shader inputs based on material
        typedef std::vector<MaterialAsset::Input>::const_iterator cit_t;
        for (cit_t it = matInputs.begin(); it != matInputs.end(); ++it)
        {
            if (const MaterialAsset::Input *pInput = findShaderInput(*it))
            {
                // we got local input of the same name as material one
                // so let use local input (because of cached Handle)
                // and get value from material input
                // this will also use DefaultValue if (*it).Value
                // is not valid.
                setInput(*pInput, (*it).Value);
            }
        }

        if (mWVPMatrix.Handle)
        {
            // get all needed matrices and update it
            mWVPMatrix.Value = renderContext.GetWVPMatrix();
            setInput(mWVPMatrix);
        }

        //mShaderObject->GetTechniqueByName("some_fanagle_technique_name");
        //mShaderObject->SetTechnique();
        UINT passes = 0;
        HRESULT hr = mShaderObject->Begin(&passes, 0);
        HR_IS_VALID(hr);
        for (UINT p = 0; p < passes; p++)
        {
            hr = mShaderObject->BeginPass(p);
            HR_IS_VALID(hr);

            if (geometryAsset)
            {
                geometryAsset->Render(renderContext);
            }
            else
            {
                geomRender(renderContext);
            }

            hr = mShaderObject->EndPass();
            HR_IS_VALID(hr);
        }

        hr = mShaderObject->End();
        HR_IS_VALID(hr);
    }
}


const MaterialAsset::Input *ShaderAsset::findShaderInput(const MaterialAsset::Input& input) const
{
    typedef std::vector<MaterialAsset::Input>::const_iterator cit_t;

    for (cit_t it = mInputs.begin(); it != mInputs.end(); ++it)
    {
        if (boost::algorithm::iequals((*it).Name, input.Name))
        {
            return &(*it);
        }
    }

    return 0;
}


void ShaderAsset::resolve()
{
    wxASSERT(mShaderObject);

    D3DXEFFECT_DESC desc;
    HRESULT hr = mShaderObject->GetDesc(&desc);
    HR_IS_VALID(hr);

    for (UINT p = 0; p < desc.Parameters; p++)
    {
        D3DXHANDLE hParam = mShaderObject->GetParameter(0, p);
        if (hParam)
        {
            D3DXPARAMETER_DESC paramDesc;
            hr = mShaderObject->GetParameterDesc(hParam, &paramDesc);
            HR_IS_VALID(hr);

            // look for yaget specific annotations to
            // desribe this input
            if (paramDesc.Annotations)
            {
                D3DXHANDLE hAnnot = mShaderObject->GetAnnotationByName(hParam, "ShaderInput");
                if (hAnnot)
                {
                    BOOL bIsInput = false;
                    hr = mShaderObject->GetBool(hAnnot, &bIsInput);
                    if (HR_IS_VALID(hr) && bIsInput)
                    {
                        // this is valid shader input,
                        // let's create entry for it including
                        // default value if one exist
                        MaterialAsset::Input shaderInput;
                        shaderInput.Name = paramDesc.Semantic ? paramDesc.Semantic : "";
                        shaderInput.Handle = hParam;
                        shaderInput.InputType = getType(paramDesc);
                        wxASSERT(shaderInput.InputType != MaterialAsset::Input::tNone);
                        shaderInput.setDefaultValue();

                        if (boost::algorithm::iequals(shaderInput.Name, std::string("wvpmatrix")))
                        {
                            mWVPMatrix = shaderInput;
                        }

                        mInputs.push_back(shaderInput);

                        // we only process any other annotations if there was ShaderInput annotation
                        // defined for this paramter
                        hAnnot = mShaderObject->GetAnnotationByName(hParam, "DefaultValue");
                        if (hAnnot)
                        {
                            LPCSTR stringInput = 0;;
                            hr = mShaderObject->GetString(hAnnot, &stringInput);
                            if (HR_IS_VALID(hr) && stringInput)
                            {
                                MaterialAsset::Input& currInput = mInputs.back();
                                currInput.ConvertToDefaultValue(std::string(stringInput));
                            }
                        }
                    }
                }
            }
        }
    }

    wxLogDebug("ShaderAsset [%s] resolved.", Name().c_str());
}


void ShaderAsset::setDefaultInputs()
{
    wxASSERT(mShaderObject);
    typedef std::vector<MaterialAsset::Input>::const_iterator cit_t;

    for (cit_t it = mInputs.begin(); it != mInputs.end(); ++it)
    {
        setInput(*it, (*it).DefaultValue);
    }
}


void ShaderAsset::setInput(const MaterialAsset::Input& input)
{
    setInput(input, input.Value);
}


void ShaderAsset::setInput(const MaterialAsset::Input& input, const boost::any& value)
{
    wxASSERT(mShaderObject);

    try
    {
        HRESULT hr;
        switch (input.InputType)
        {
            case MaterialAsset::Input::tBool:
                hr = mShaderObject->SetBool(input.Handle, boost::any_cast<bool>(value));
                HR_IS_VALID(hr);
                break;

            case MaterialAsset::Input::tInt:
                hr = mShaderObject->SetInt(input.Handle, boost::any_cast<int>(value));
                HR_IS_VALID(hr);
                break;

            case MaterialAsset::Input::tFloat:
                hr = mShaderObject->SetFloat(input.Handle, boost::any_cast<float>(value));
                HR_IS_VALID(hr);
                break;

            case MaterialAsset::Input::tVector2:
                {
                    Vector2 inputValue = boost::any_cast<Vector2>(value);
                    hr = mShaderObject->SetFloatArray(input.Handle, inputValue.getF(), 2);
                    HR_IS_VALID(hr);
                }
                break;

            case MaterialAsset::Input::tVector3:
                {
                    Vector3 inputValue = boost::any_cast<Vector3>(value);
                    hr = mShaderObject->SetFloatArray(input.Handle, inputValue.getF(), 3);
                    HR_IS_VALID(hr);
                }
                break;

            case MaterialAsset::Input::tVector4:
                {
                    Vector4 inputValue = boost::any_cast<Vector4>(value);
                    hr = mShaderObject->SetFloatArray(input.Handle, inputValue.getF(), 4);
                    HR_IS_VALID(hr);
                }
                break;

            case MaterialAsset::Input::tTexture:
                {
                    if (GeometryTextureAsset::Asset_t inputValue = boost::any_cast<GeometryTextureAsset::Asset_t>(value))
                    {
                        if (GeometryTextureAsset::Locker_t locker = inputValue->Lock())
                        {
                            hr = mShaderObject->SetTexture(input.Handle, inputValue->GetNativeObject());
                        }
                    }
                }
                break;

            case MaterialAsset::Input::tMatrix44:
                {
                    Matrix44 inputValue = boost::any_cast<Matrix44>(value);
                    hr = mShaderObject->SetMatrix(input.Handle, (D3DXMATRIX *)(inputValue.GetF()));
                    HR_IS_VALID(hr);
                }
                break;

            default:
                wxLogDebug("There is no conversion for ShaderAsset::setInput '%s' with type '%d'.", input.Name.c_str(), input.InputType);
        }
    }
    catch (const boost::bad_any_cast& er)
    {
        wxLogDebug("Input Name: %s. ErrMsg: %s.", input.Name.c_str(), er.what());
    }
}






} // namespace eg

