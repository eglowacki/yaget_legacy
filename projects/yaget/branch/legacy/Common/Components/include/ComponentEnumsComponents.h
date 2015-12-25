///////////////////////////////////////////////////////////////////////
// ComponentEnumsComponents.h
//
//  Copyright 10/16/2005 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//
//
//  #include "ComponentEnumsComponents.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef COMPONENT_ENUMS_COMPONENTS_H
#define COMPONENT_ENUMS_COMPONENTS_H

#include "ComponentsBase.h"
#include "ComponentEnumsObjects.h"

namespace eg
{
    /*!
    Components provided by this module and available to the game.
    */
    namespace ComponentType
    {
        //! This is included in Area module because
        //! it the most common component and needs be visible everywhere
        //! We skipped Entity component enumeration here
        //! and put in Obkect modules it the most common component
        //! and helps to be visible to everone.

        //! Provides container for other objects
        static const Hash kArea("Area");
        //! Provides visual rendering helpers, which are managed as a colection
        //! by this Component
        static const Hash kWidget("Widget");

    } // namespace ComponentType


} // namespace eg

#endif //COMPONENT_ENUMS_COMPONENTS_H