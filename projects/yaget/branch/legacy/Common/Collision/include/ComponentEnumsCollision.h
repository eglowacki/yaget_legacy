///////////////////////////////////////////////////////////////////////
// ComponentEnumsCollision.h
//
//  Copyright 09/13/2008 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//
//
//  #include "ComponentEnumsCollision.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef COMPONENT_ENUMS_COLLISION_H
#define COMPONENT_ENUMS_COLLISION_H

#include "CollisionBase.h"
#include "Hash\Hash.h"

namespace eg
{
    /*!
    Components provided by this module and available to the game.
    */
    namespace ComponentType
    {
        static const Hash kCollision("Collision");
    } // namespace ComponentType


} // namespace eg

#endif //COMPONENT_ENUMS_COLLISION_H
