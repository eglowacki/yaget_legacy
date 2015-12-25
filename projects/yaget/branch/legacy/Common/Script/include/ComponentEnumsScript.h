///////////////////////////////////////////////////////////////////////
// ComponentEnums$safeitemname$.h
//
//  Copyright 05/30/2008 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//
//
//  #include "ComponentEnumsScript.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef COMPONENT_ENUMS_SCRIPT_H
#define COMPONENT_ENUMS_SCRIPT_H

#include "ScriptBase.h"
#include "Hash\Hash.h"

namespace eg
{
    /*!
    Components provided by this module and available to the game.
    */
    namespace ComponentType
    {
        static const Hash kScript("Script");
    } // namespace ComponentType


} // namespace eg

#endif //COMPONENT_ENUMS_SCRIPT_H
