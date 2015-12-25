///////////////////////////////////////////////////////////////////////
// IComponentModel.h
//
//  Copyright 10/29/2005 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Interface for render component
//
//
//  #include "IComponentModel.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef I_COMPONENT_MODEL_H
#define I_COMPONENT_MODEL_H

#include "RenderBase.h"
#include "IComponent.h"
#include "ComponentEnumsRender.h"


namespace eg
{
    // forward declarations
    class IRenderContext;

    namespace model
    {
        static const char *kGroup = "Model";
        static const char *kPropGeometryName = "GeometryName";  ///< std::string
        static const char *kPropShaderName = "ShaderName";      ///< std::string
        static const char *kPropMaterialName = "MaterialName";  ///< std::string
		static const char *kPropRenderId = "RenderId";			///< uint32_t
    } // namespace model
    /*!
    /code
    -- create entity table in DB
    CREATE TABLE component_model
    (
    object_id bigint NOT NULL,
    geometry_name varchar,
    shader_name varchar,
    material_name varchar,
    CONSTRAINT pk_model PRIMARY KEY (object_id)
    );
    /endcode
    */

    class DLLIMPEXP_RENDER IComponentModel : public IComponent
    {
    public:
        const static uint32_t Type = 0xd4eb2d94;//ComponentType::kModel;

        virtual void Render(const IRenderContext& renderContext) = 0;
        //! Return render id, which is bitfield specifying all the valid cameras we can be rendered with
        virtual uint32_t GetRenderId() const = 0;

        //! This will return some arbitrary sort value, which is used to sort all the models before rendering each one.
        virtual float GetSortValue() const = 0;

    protected:
        virtual ~IComponentModel() = 0 {}
    };

} // namespace eg

#endif // COMPONENTS_I_COMPONENT_MODEL_H

