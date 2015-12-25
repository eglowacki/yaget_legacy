#include "Precompiled.h"
#include "BindHelpers.h"
#include "..\ComponentScriptLua.h"
#include "..\luacpp.h"
#include "Property/PropertyDataConversion.h"


namespace eg {
namespace script {

std::string getStackType(lua_State *L, int index)
{
    std::string textMsg;
    int t = lua_type(L, index);
    switch (t)
    {
    case LUA_TSTRING:  // strings
        printf("`%s'", lua_tostring(L, index));
        textMsg += lua_tostring(L, index);
        break;

    case LUA_TBOOLEAN:  // booleans
        printf(lua_toboolean(L, index) ? "true" : "false");
        textMsg += lua_toboolean(L, index) ? "true" : "false";
        break;

    case LUA_TNUMBER:  // numbers
        printf("%g", lua_tonumber(L, index));
        textMsg += eg::ConvertProperty(lua_tonumber(L, index));
        break;

    default:  // other values
        printf("%s", lua_typename(L, t));
        textMsg += lua_typename(L, t);
        break;

    }
    printf("  ");  // put a separator
    textMsg += " ";
    return textMsg;
}

void stackDump(lua_State *L)
{
    std::string textMsg;
    int top = lua_gettop(L);
    for (int i = 1; i <= top; i++)
    {
        textMsg += getStackType(L, i);
    }
    printf("\n");  // end the listing
    wxLogTrace("lua", textMsg.c_str());
}


ComponentScriptLua *GetScriptObject(lua_State *L)
{
    lua_pushlightuserdata(L, L);
    lua_gettable(L, LUA_GLOBALSINDEX);
    ComponentScriptLua *pLuaComponent = (ComponentScriptLua*)lua_touserdata(L, -1);
    //   remove it from stack
    lua_pop(L, 1);
    return pLuaComponent;
}



} // namespace script
} // namespace eg

