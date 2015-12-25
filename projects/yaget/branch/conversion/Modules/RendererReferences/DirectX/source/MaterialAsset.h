//////////////////////////////////////////////////////////////////////
// MaterialAsset.h
//
//  Copyright 2/9/2008 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Provides Material data loaded from some resource
//
//
//  #include "MaterialAsset.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef MATERIAL_ASSET_H
#define MATERIAL_ASSET_H
#pragma once

#include "GeometryTextureAsset.h"
#include "Property/PropertyDataConversion.h"
#include "Math/Vector.h"
#include "Math/Matrix.h"
#pragma warning (disable:4312)  // 'variable' : conversion from 'type' to 'type' of greater size
    #include <atlcomcli.h>
    #include <d3dx9mesh.h>
#pragma warning (default : 4312)



namespace eg
{
    namespace missingasset
    {
        static const char *Material = "Missing.mat";
    }  // namespace missingasset


    class MaterialAsset : public eg::MediaAsset<MaterialAsset>
    {
    public:
        static const guid_t kType = 0xb4ba3d0d;

        /*!
           This represents one input from material to shader. Values for this are extracted
           from material file (xml) which contains name of input, type and it's value.
           So far it's very simple format and only supports types listed below in \see Type enum.
           Shader will get list of all inputs provided by material and will set it's shader inputs
           accordingly to this list. For any input not specified in this material, shader will use
           it's own default value (exposed from shader using semantics).
         */
        struct Input
        {
            //! \enum types of input we support.
            enum Type {tNone, tBool, tInt, tFloat, tVector2, tVector3, tVector4, tMatrix44, tTexture};

            Input()
            : Handle(0)
            , InputType(tNone)
            {
            }

            void setDefaultValue()
            {
                switch (InputType)
                {
                    case tBool:
                        DefaultValue = false;
                        break;

                    case tInt:
                        DefaultValue = (int)0;
                        break;

                    case tFloat:
                        DefaultValue = (float)0.0f;
                        break;

                    case tVector2:
                        DefaultValue = v2::ZERO();
                        break;

                    case tVector3:
                        DefaultValue = v3::ZERO();
                        break;

                    case tVector4:
                        DefaultValue = v4::WONE();;
                        break;

                    case tMatrix44:
                        DefaultValue = m44::Identity();
                        break;

                    case tTexture:
                        DefaultValue = GeometryTextureAsset::Asset_t();
                        break;
                }
            }

            void ConvertToValue(const std::string& valueString)
            {
                ConvertToValue(valueString, Value);
            }

            void ConvertToDefaultValue(const std::string& valueString)
            {
                ConvertToValue(valueString, DefaultValue);
            }

            //! Convert string representation to Type
            static Type Convert(const std::string& name)
            {
                if (boost::algorithm::iequals(name, std::string("None")))
                {
                    return tNone;
                }
                else if (boost::algorithm::iequals(name, std::string("Bool")))
                {
                    return tBool;
                }
                else if (boost::algorithm::iequals(name, std::string("Int")))
                {
                    return tInt;
                }
                else if (boost::algorithm::iequals(name, std::string("Float")))
                {
                    return tFloat;
                }
                else if (boost::algorithm::iequals(name, std::string("Vector2")))
                {
                    return tVector2;
                }
                else if (boost::algorithm::iequals(name, std::string("Vector3")))
                {
                    return tVector3;
                }
                else if (boost::algorithm::iequals(name, std::string("Vector4")))
                {
                    return tVector4;
                }
                else if (boost::algorithm::iequals(name, std::string("Matrix44")))
                {
                    return tMatrix44;
                }
                else if (boost::algorithm::iequals(name, std::string("Texture")))
                {
                    return tTexture;
                }
                else
                {
                    wxASSERT(0);
                }

                return tNone;
            }

            std::string Name;
            D3DXHANDLE Handle;
            Type InputType;
            boost::any Value;
            boost::any DefaultValue;

        private:
            void ConvertToValue(const std::string& valueString, boost::any& anyValue)
            {
                switch (InputType)
                {
                    case MaterialAsset::Input::tNone:
                        anyValue = boost::any();
                        break;

                    case MaterialAsset::Input::tBool:
                        anyValue = ConvertProperty<bool>(valueString);
                        break;

                    case MaterialAsset::Input::tInt:
                        anyValue = ConvertProperty<int>(valueString);
                        break;

                    case MaterialAsset::Input::tFloat:
                        anyValue = ConvertProperty<float>(valueString);
                        break;

                    case MaterialAsset::Input::tVector2:
                        anyValue = ConvertProperty<Vector2>(valueString);
                        break;

                    case MaterialAsset::Input::tVector3:
                        anyValue = ConvertProperty<Vector3>(valueString);
                        break;

                    case MaterialAsset::Input::tVector4:
                        anyValue = ConvertProperty<Vector4>(valueString);
                        break;

                    case MaterialAsset::Input::tMatrix44:
                        anyValue = ConvertProperty<Matrix44>(valueString);
                        break;

                    case MaterialAsset::Input::tTexture:
                        {
                            AssetLoadSystem& als = registrate::ref_cast<AssetLoadSystem>("AssetLoadSystem");
                            std::string textureName = valueString;
                            if (GeometryTextureAsset::Asset_t inputValue = als.GetAsset<GeometryTextureAsset>(textureName))
                            {
                                anyValue = inputValue;
                            }
                            else
                            {
                                // this should get some "Missing Texture"
                                anyValue = GeometryTextureAsset::Asset_t();
                            }
                        }
                        break;
                }
            }
        };

        typedef std::vector<Input> InputsPerPass_t;
        typedef std::map<uint32_t, InputsPerPass_t> Inputs_t;

        MaterialAsset(const std::string& name);
        ~MaterialAsset();

        //! Return collection of inputs for that particular pass
        //!
        //! \param passIndex pass index to get active inputs
        //!
        //! \return const std::vector<Input>& collection of inputs for this pass
        //!               \note once input was activated, it will stay in effect for the remaining
        //!                     inputs passes
        const InputsPerPass_t& GetInputs(uint32_t passIndex) const;

        //! Set input value to new and return old one. if there is not input of that name
        //! return value will be empty
        boost::any SetInput(const std::string& name, boost::any newValue, uint32_t passIndex = 0);
        //! Return current input value
        boost::any GetInput(const std::string& name, uint32_t passIndex = 0);

        // from IAsset
        virtual guid_t Type() const {return MaterialAsset::kType;}

        template <typename T>
        T GetInputValue(const std::string& name, const T& defaultValue = T(), uint32_t passIndex = 0)
        {
            boost::any inputValue = GetInput(name, passIndex);
            if (!inputValue.empty() && inputValue.type() == typeid(T))
            {
                try
                {
                    return boost::any_cast<T>(inputValue);
                }
                catch (const boost::bad_any_cast& er)
                {
                    wxLogError("MaterialAsset::GetInputValue could not cast input value: %s", er.what());
                }
            }
            return defaultValue;
        }

    private:
        void onMissingAssetChanged(MaterialAsset::Asset_t asset, const std::string& name);
        void onFinishedLoading(MaterialAsset::Asset_t asset, const std::string& name);
        // From eg::MediaAsset
        virtual void SaveBlob(uint8_t *&pData, size_t& size) const;
        virtual void LoadBlob(const uint8_t *pData, size_t size, const std::string& streamName);

        //! Actual shader object
        Inputs_t mInputs;
        boost::signals::connection mMissingAssetConnection;
    };


    /*
    template <typename T>
    inline T MaterialAsset<T>::GetInputValue(const std::string& name, uint32_t passIndex)
    {
        return T();
    }
    */

    typedef asset::AssetHandle<MaterialAsset, asset::AutoLock<MaterialAsset>, asset::LoadTypeDynamic> MaterialAsset_t;

    namespace material
    {
        //! helper class to allow setting input value and resetting back to it's old value in dtor
        class AutoReset
        {
        public:
            AutoReset(MaterialAsset::Asset_t matAsset, const std::string& name, boost::any newValue)
            : mMatAsset(matAsset)
            , mName(name)
            , mOldValue(matAsset->SetInput(name, newValue))
            {
            }

            ~AutoReset()
            {
                mMatAsset->SetInput(mName, mOldValue);
            }

        private:
            MaterialAsset::Asset_t mMatAsset;
            std::string mName;
            boost::any mOldValue;
        };

    }

} // namespace eg


BOOST_CLASS_VERSION(eg::MaterialAsset, 1);

#endif // MATERIAL_ASSET_H
