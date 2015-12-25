///////////////////////////////////////////////////////////////////////
// ComponentModel.h
//
//  Copyright 10/29/2005 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Private implementation of component model
//
//
//  #include "ComponentModel.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef COMPONENT_MODEL_H
#define COMPONENT_MODEL_H

#include "IComponentModel.h"
#include "Math/Vector.h"
#include "Property/PropertyData.h"
#include "Message/Dispatcher.h"
#include "GeometryAsset.h"
#include "ShaderAsset.h"
#include "GeometryTextureAsset.h"



namespace eg
{
    class ComponentModel : public IComponentModel, public ComponentHelper<ComponentModel, PropertySet>
    {
    public:
        ComponentModel(component::ObjectId oId);
        virtual ~ComponentModel();

        // from IComponentModel
        virtual void Render(const IRenderContext& renderContext);
        virtual uint32_t GetRenderId() const;
        virtual float GetSortValue() const;

        // from IComponent
        virtual void DeleteThis();
        virtual Hash GetComponentTypeId() const;
        virtual Dispatcher::Result_t HandleMessage(Message& msg);
        virtual IPropertySet& GetPropertyData() {return CH_t::GetPropertyData();}
        virtual const ObjectInstance& OI() const {return CH_t::OI();}

        // Static methods
        static bool RegisterComponentType();

    private:
        // from IComponent
        virtual void Shutdown();

        virtual void onPropertyChanged(const IProperty *pProperty);
        //! 'Geometry Name' geometery asset name
        Property<std::string>& mGeometryName;
        GeometryAsset_t mGeometry;

        //! 'Shader Name' shader asset name
        Property<std::string>& mShaderName;
        ShaderAsset_t mShader;

        //! 'Material Name' material asset name
        Property<std::string>& mMaterialName;
        MaterialAsset_t mMaterial;

		//! This is only here to provide edit capability 
		//! but we maintain mRenderId member variable for speed of access.
		Property<uint32_t>& mRenderIdProperty;

        //! Which camera(s) we want to be rendered with
        //! This represents bits for cameras we allow to be rendered with
        //! \note by using 32 bit we contain number of cameras to 32
        uint32_t mRenderId;

        // used by GetSortValue() method
        float mSortValue;
    };

} // namespace eg

#endif // COMPONENT_MODEL_H

