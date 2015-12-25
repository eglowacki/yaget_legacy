// this will trigger all registered binds for orchestrating vacuous C++ object which should be exposed to Lua script
#include "Precompiled.h"
#include "AutoBinders.h"
#include <vector>
#include "../Script.h"
#include "../luacpp.h"

namespace
{
    bool bStillDefereed = true;
    std::vector<eg::script::LuaRegistrate_t> Registratrations;
}  // namespace


namespace eg {
namespace script {

typedef boost::function<void (lua_State * /*L*/)> LuaRegistrate_t;

bool addRegScriptRegistrate(LuaRegistrate_t registrates)
{
    if (bStillDefereed)
    {
        Registratrations.push_back(registrates);
    }
    else
    {
        registrates(eg::Script::L);
    }

    return true;
}

void RegisterAllDeferedModules()
{
    bStillDefereed = false;
    for (std::vector<eg::script::LuaRegistrate_t>::const_iterator it = Registratrations.begin(); it != Registratrations.end(); ++it)
    {
        (*it)(eg::Script::L);
    }
    Registratrations.clear();
}


} // namespace script`
} // namespace eg
