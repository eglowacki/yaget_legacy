/////////////////////////////////////////////////////////////////////
// BindHelpers.h
//
//  Copyright 6/3/2008 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES: Helpers for bind functions including debug
//
//
//  #include "BindHelpers.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef BIND_HELPERS_H
#define BIND_HELPERS_H
#pragma once

//! forward declarations
struct lua_State;

namespace eg
{
	class ComponentScriptLua;

    namespace script
    {
        //! Dump entire stack
        //! \todo dump it to logger
        void stackDump(lua_State *L);
		//! Return type of value at current index on the stack
		std::string getStackType(lua_State *L, int index);

        //! Return current script object associated with this lua state
        ComponentScriptLua *GetScriptObject(lua_State *L);

    } // namespace script
} // namespace eg

#endif // BIND_HELPERS_H

