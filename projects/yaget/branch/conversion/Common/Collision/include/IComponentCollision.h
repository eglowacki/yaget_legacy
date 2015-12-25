///////////////////////////////////////////////////////////////////////
// IComponentCollision.h
//
//  Copyright 09/13/2008 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Interface for Collision component
//
//
//  #include "IComponentCollision.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef I_COMPONENT_COLLISION_H
#define I_COMPONENT_COLLISION_H

#include "CollisionBase.h"
#include "IComponent.h"
#include "ComponentEnumsCollision.h"



namespace eg
{

    namespace collision
    {
        static const char *kGroup = "Collision";
        static const char *kPropShapeName = "ShapeName";  ///< std::string
    } // namespace collision

    /*!
     /code
     -- create collision table in DB
     CREATE TABLE component_collisiont
     (
         object_id bigint NOT NULL,
         CONSTRAINT pk_scollision PRIMARY KEY (object_id)
     );
     /endcode
    */
    class DLLIMPEXP_COLLISION IComponentCollision : public IComponent
    {
    public:
        //! Type of component
        const static uint32_t Type = 0x95d8036d;//ComponentType::kCollision

    private:
    };

} // namespace eg

#endif // I_COMPONENT_COLLISION_H

