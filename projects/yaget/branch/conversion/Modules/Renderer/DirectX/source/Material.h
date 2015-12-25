/////////////////////////////////////////////////////////////////////////
// Material.h
//
//  Copyright 6/1/2009 Edgar Glowacki.
//
// NOTES:
//      This is returned by ShaderScript
//      and represent unqiue set of inputs into shader
//
// #include "Material.h"
//
/////////////////////////////////////////////////////////////////////////
//! \file

#ifndef MATERIAL_H
#define MATERIAL_H
#pragma once

#include "Base.h"
#include "Math/Matrix.h"
#include "Logger/Log.h"
#include "Property/PropertyDataConversion.h"
#include <atlbase.h>
//#include <vector>
#include <list>
#include <ostream>
#include <istream>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#pragma warning(push)
#pragma warning (disable : 4100)  // '' : unreferenced formal parameter
    #include <boost/variant.hpp>
#pragma warning(pop)


interface ID3DXEffect;
namespace {class MaterialAssetFactory;}

namespace boost
{
    template <>
    inline eg::Vector2 lexical_cast(const std::string& value)
    {
        return eg::ConvertProperty<eg::Vector2>(value);
    }

    template <>
    inline eg::Vector3 lexical_cast(const std::string& value)
    {
        return eg::ConvertProperty<eg::Vector3>(value);
    }

    template <>
    inline eg::Vector4 lexical_cast(const std::string& value)
    {
        return eg::ConvertProperty<eg::Vector4>(value);
    }

    template <>
    inline eg::Matrix33 lexical_cast(const std::string& value)
    {
        return eg::ConvertProperty<eg::Matrix33>(value);
    }

    template <>
    inline eg::Matrix44 lexical_cast(const std::string& value)
    {
        return eg::ConvertProperty<eg::Matrix44>(value);
    }
}

namespace eg
{
    class AssetLoadSystem;

    namespace shaders
    {
        class Material
        {
            // no copy semantics
            Material(const Material&);
            Material& operator=(const Material&);

        public:
            //! This represents one exposed shader input
            //! Shader usually maintains default input value
            //! by using value from it's parent if one exists
            struct Input
            {
                Input() : mParamh(0)
                {}
                Input(const std::string& name, void *param) : mName(name), mParamh(param)
                {}

                template <typename T>
                void set(const T& value);

                template <typename T>
                void set(const std::string& value);

                template <typename T>
                T get() const;

                const std::string& name() const {return mName;}
                void apply(const CComPtr<ID3DXEffect>& shader, const Input *input) const;

            private:
                std::string mName;
                void *mParamh;
                boost::variant<bool, int, float, Vector2, Vector3, Vector4, Matrix33, Matrix44> mValue;
            };

            typedef std::list<Input> Inputs_t;
            typedef boost::function<void ()> Callback_t;

            Material(const std::string& name, const CComPtr<ID3DXEffect>& shaderObject, const Inputs_t& shaderInputs);
            ~Material();

            const std::string& Name() const;
            void Render(Callback_t callback) const;

            //! This allows to get input and manipulate it.
            //! If input does not exist, it will return no-op object
            Input& GetInput(const std::string& name);

            static void Register(AssetLoadSystem &als);

        private:
            const Input *findInput(const std::string& name) const;
            void applyInputs() const;

            std::string mName;
            const CComPtr<ID3DXEffect>& mShaderObject;
            mutable int mErrorTries;
            uint32_t mStateFlags;
            const Inputs_t& mShaderInputs;
            Inputs_t mMaterialInputs;

            friend MaterialAssetFactory;
            void load(std::istream& stream);
            void save(std::ostream& stream) const;

            static const uint32_t version = 1;
        };

        typedef boost::shared_ptr<Material> Material_t;

        template <typename T>
        inline void Material::Input::set(const T& value)
        {
            mValue = value;
        }

        template <typename T>
        T Material::Input::get() const
        {
            return boost::get<T>(mValue);
        }

        template <typename T>
        inline void Material::Input::set(const std::string& value)
        {
            try
            {
                if (typeid(T) == typeid(bool))
                {
                    T v = boost::lexical_cast<T>(value);
                    mValue = v;
                }
                else if (typeid(T) == typeid(int))
                {
                    T v = boost::lexical_cast<T>(value);
                    mValue = v;
                }
                else if (typeid(T) == typeid(float))
                {
                    T v = boost::lexical_cast<T>(value);
                    mValue = v;
                }
                else if (typeid(T) == typeid(Vector2))
                {
                    T v = boost::lexical_cast<T>(value);
                    mValue = v;
                }
                else if (typeid(T) == typeid(Vector3))
                {
                    T v = boost::lexical_cast<T>(value);
                    mValue = v;
                }
                else if (typeid(T) == typeid(Vector4))
                {
                    T v = boost::lexical_cast<T>(value);
                    mValue = v;
                }
                else if (typeid(T) == typeid(Matrix33))
                {
                    T v = boost::lexical_cast<T>(value);
                    mValue = v;
                }
                else if (typeid(T) == typeid(Matrix44))
                {
                    T v = boost::lexical_cast<T>(value);
                    mValue = v;
                }
                else
                {
                    log_error << "Unhadled cast for string value: '" << value << "', input: '" << name() << "'";
                }
            }
            catch (boost::bad_lexical_cast& ex)
            {
                log_error << "Could not cast from string value: '" << value << "', input: '" << name() << "', Exception: " << ex.what();
            }
        }


    } // namespace shaders

} // namespace eg

#endif // MATERIAL_H

