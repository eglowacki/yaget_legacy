/////////////////////////////////////////////////////////////////////////
// bind_renderer.cpp
//
//  Copyright 4/27/2009 Edgar Glowacki.
//
// NOTES:
//      Provides bindings for log output from lua
//
//
//
/////////////////////////////////////////////////////////////////////////
//! \file
#include "Logger/Log.h"
extern "C"
{
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}
#pragma warning(push)
#pragma warning (disable : 4100)  // '' : unreferenced formal parameter
#include <luabind/luabind.hpp>
#pragma warning(pop)



namespace
{
    void renderer_newview()
    {
        log_trace(tr_script) << "[lua:] " << "newview called";
    }

    /*
    void lb_log_debug(const char *msg)
    {
        if (msg && *msg)
        {
            log_debug << msg;
        }
    }

    void lb_log_warning(const char *msg)
    {
        if (msg && *msg)
        {
            log_warning << msg;
        }
    }

    void lb_log_error(const char *msg)
    {
        if (msg && *msg)
        {
            log_error << msg;
        }
    }
    */
}


namespace eg { namespace script {

void init_renderer(lua_State *L)
{
    using namespace luabind;

    module(L, "renderer")
    [
        def("newview", &renderer_newview)
    ];
}

}} // namespace script // namespace eg


