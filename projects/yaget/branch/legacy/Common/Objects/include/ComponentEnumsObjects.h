///////////////////////////////////////////////////////////////////////
// ComponentEnumsObjects.h
//
//  Copyright 10/16/2005 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//
//
//  #include "ComponentEnumsObjects.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef COMPONENT_ENUMS_OBJECTS_H
#define COMPONENT_ENUMS_OBJECTS_H

#include "ObjectsBase.h"
#include "Hash\Hash.h"

namespace eg
{
    /*!
    Components provided by this module and available to the game.
    */
    namespace ComponentType
    {
        static const Hash kInvalid;
        //! This is included in Objects module because
        //! it the most common component and needs be visible everywhere
        static const Hash kEntity("Entity");
    } // namespace ComponentType


    /*
    namespace InterfaceType
    {
        //! This is included in Objects module because
        //! it the most common component and needs be visible everywhere
        static const guid_t kEntity = 0x5181ed28;
    } // namespace InterfaceType
    */

} // namespace eg

#endif //COMPONENT_ENUMS_OBJECTS_H