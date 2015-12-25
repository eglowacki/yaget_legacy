///////////////////////////////////////////////////////////////////////
// IComponentEntity.h
//
//  Copyright 5/25/2006 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Interface for Entity component
//
//
//  #include "IComponentEntity.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef I_COMPONENT_ENTITY_H
#define I_COMPONENT_ENTITY_H

#include "ComponentsBase.h"
#include "IComponent.h"
#include "ComponentEnumsComponents.h"
#include "Math\Matrix.h"

namespace eg
{
    namespace entity
    {
        static const char *kGroup = "Entity";
        static const char *kPropPosition = "Position";          ///< Vector3
        static const char *kPropScale = "Scale";                ///< Vector3
        static const char *kPropOrientation = "Orientation";    ///< Quaternion
    } // namespace entity

	/*!
	/code
	-- create entity table in DB
	CREATE TABLE component_entity
	(
	    object_id bigint NOT NULL,
	    position vector3,
        scale vector3,
	    orientation vector4,
	    CONSTRAINT pk_entity PRIMARY KEY (object_id)
	);
	/endcode
	*/
    class DLLIMPEXP_COMPONENTS IComponentEntity : public IComponent
    {
    public:
        //! Type of component
        const static uint32_t Type = 0x56e92758;//ComponentType::kEntity;

        //! Get full matrix of this Entity
        virtual const Matrix44& GetMatrix() const = 0;

    protected:
        virtual ~IComponentEntity() = 0 {}

        //static void RegisterInterface(Hash compId);
    };

} // namespace eg


#endif // I_COMPONENT_ENTITY_H


