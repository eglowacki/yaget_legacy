/////////////////////////////////////////////////////////////////////
// AutoBinders.h
//
//  Copyright 9/12/2008 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//    Entry point for registration of c++ methods exposed to script
//    Functions which will be used for registration takes lua_State
//    as a parameter, son exclusive knowlege of lua is required at point of registration
//
//  #include "AutoBinders.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef AUTO_BINDERS_H
#define AUTO_BINDERS_H
#pragma once

#include "ScriptBase.h"
#include <boost/function.hpp>

struct lua_State;

namespace eg {
namespace script {

    // from any point in the the program call this method to registered
    // to expose your 'C' functions/methods to script
    typedef boost::function<void (lua_State * /*L*/)> LuaRegistrate_t;

    //! Call this function for any module you need to registered C++ functions with lua script
    bool addRegScriptRegistrate(LuaRegistrate_t registrates);

    //! Called by Script Module during initialization to trigger binds
    void RegisterAllDeferedModules();

} // namespace script
} // namespace eg




#endif // AUTO_BINDERS_H

