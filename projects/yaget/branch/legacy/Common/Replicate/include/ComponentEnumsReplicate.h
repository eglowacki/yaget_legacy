///////////////////////////////////////////////////////////////////////
// ComponentEnumsReplicate.h
//
//  Copyright 12/01/2007 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//
//
//  #include "ComponentEnumsReplicate.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef COMPONENT_ENUMS_REPLICATE_H
#define COMPONENT_ENUMS_REPLICATE_H

#include "ReplicateBase.h"
#include "Hash\Hash.h"

namespace eg
{
    /*!
    Components provided by this module and available to the game.
    */
    namespace ComponentType
    {
        static const Hash kReplicate("Replicate");
    } // namespace ComponentType


    namespace InterfaceType
    {
        static const guid_t kReplicate = 0x545c9bc9;
    } // namespace InterfaceType

} // namespace eg

#endif //COMPONENT_ENUMS_REPLICATE_H