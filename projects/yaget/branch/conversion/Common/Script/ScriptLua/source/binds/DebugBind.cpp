#include "Precompiled.h"
#pragma warning(push)
#pragma warning (disable : 4100)  // '' : unreferenced formal parameter
#pragma warning (disable : 4512)  // '' : assignment operator could not be generated
#include "ComponentBinds.h"
#pragma warning(pop)
#include "AutoBinders.h"
#include "BindHelpers.h"
#include "IRenderer.h"
#include "ObjectID.h"
#include "Math/Vector.h"
#include "Registrate.h"
#include "IObjectManager.h"
#include "../ComponentScriptLua.h"
#include "../Script.h"
#include "BindPolicy.h"
#include <luabind/error.hpp>
#include <luabind/operator.hpp>

namespace
{

//! Method wired to wxLogDebug function
int log_debug(lua_State *L)
{
    using namespace eg::script;
    std::string errorMsg;
    std::string logMsg;
    if (Parameters::extract(L, logMsg, errorMsg))
    {
        wxLogDebug(logMsg.c_str());
        return 0;
    }

    stackDump(L);
    return luaL_error(L, errorMsg.c_str());
}

//! Method wired to wxLogTrace function
int log_trace(lua_State *L)
{
    using namespace eg::script;
    std::string errorMsg;
    std::string logMsg;
    if (Parameters::extract(L, logMsg, errorMsg))
    {
        wxLogTrace("lua", logMsg.c_str());
        return 0;
    }

    stackDump(L);
    return luaL_error(L, errorMsg.c_str());
}

//! Method wired to wxLogWarning function
int log_warning(lua_State *L)
{
    using namespace eg::script;
    std::string errorMsg;
    std::string logMsg;
    if (Parameters::extract(L, logMsg, errorMsg))
    {
        wxLogWarning(logMsg.c_str());
        return 0;
    }

    stackDump(L);
    return luaL_error(L, errorMsg.c_str());
}

//! Method wired to wxLogError function
int log_error(lua_State *L)
{
    using namespace eg::script;
    std::string errorMsg;
    std::string logMsg;
    if (Parameters::extract(L, logMsg, errorMsg))
    {
        wxLogError(logMsg.c_str());
        return 0;
    }

    stackDump(L);
    return luaL_error(L, errorMsg.c_str());
}

const luaL_reg logLib[] =
{
    {"trace",           log_trace},
    {"debug",           log_debug},
    {"warning",         log_warning},
    {"error",           log_error},
    {NULL,              NULL}
};

} // namespace

namespace eg {
namespace script {

void intitDebugModule(lua_State *L)
{
    using namespace luabind;
    luaL_register(L, "log", logLib);

}

} // namespace script
} // namespace eg


namespace
{
    bool result = eg::script::addRegScriptRegistrate(eg::script::intitDebugModule);
} // namespace
