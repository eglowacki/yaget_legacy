///////////////////////////////////////////////////////////////////////
// IComponentArea.h
//
//  Copyright 10/29/2005 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Interface for Area component which contains other components
//
//
//  #include "IComponentArea.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef I_COMPONENT_AREA_H
#define I_COMPONENT_AREA_H

#include "ComponentsBase.h"
#include "IComponent.h"
#include "ComponentEnumsComponents.h"

namespace eg
{
    namespace area
    {
        static const char *kGroup = "Area";
        static const char *kPropObjects = "Objects";    ///< std::vector<component::ObjectId>
    } // namespace area

    /*!
    This component provides container for other components.
    It does nothing except holds list of ObjectId's.
    It's up to user to decide on policy when loading this object
    as to create all contained objects, or do nothing.
    We provide templetize functions to help with object creation policy.
    In other words, it's primiry purpose is to offer collection mechanism.

    /code
    -- create area table in DB
    CREATE TABLE component_area
    (
    object_id bigint NOT NULL,
    objects bigint[],
    CONSTRAINT pk_area PRIMARY KEY (object_id)
    );
    /endcode
    */
    class DLLIMPEXP_COMPONENTS IComponentArea : public IComponent
    {
    public:
        //! Type of component
        const static uint32_t Type = 0x5e908246;//ComponentType::kArea;

    protected:
        virtual ~IComponentArea() = 0 {}
    };


} // namespace eg

#endif // I_COMPONENT_AREA_H

