#include "Precompiled.h"
#include "ObjectID.h"
#pragma warning(push)
#pragma warning (disable : 4100)  // '' : unreferenced formal parameter
#pragma warning (disable : 4702)  // '' : unreachable code
#include "..\ComponentScriptLua.h"
#pragma warning(pop)
#include "..\luacpp.h"
#include <luabind/error.hpp>
#include <luabind/operator.hpp>
#include <luabind/object.hpp>
#include "BindPolicy.h"
#include <boost/format.hpp>

namespace
{
    //! Name of our metatable for Component PropertySet
    const char *kPropertySetTable = "PropertySet";

    struct LuaPropertySet
    {
        eg::component::InstanceId iId;
    };

} // namespace

namespace eg {
namespace script {



//! Return property set for specific component instance
//! PropertySet get(string componentType)
//! PropertySet get(uint64_t objectId, string componentType)
int propertySet_get(lua_State *L)
{
    using namespace eg;
    using namespace eg::script;
    std::string errorMsg;
    uint64_t oId;
    std::string compType;
    if (Parameters::extract(L, oId, compType, errorMsg))
    {
        component::ComponentId cId(compType.c_str());
        // return property set to instance id
        component::InstanceId iId = component::make(oId, cId);
		if (component::get(iId))
		{
			if (LuaPropertySet *pSelf = (LuaPropertySet *)lua_newuserdata(L, sizeof(LuaPropertySet)))
			{
				pSelf->iId = iId;
				luaL_getmetatable(L, kPropertySetTable);
				lua_setmetatable(L, -2);
				return 1;
			}
			else
			{
				errorMsg = "Could not extract create Lua PropertySet.";
			}
		}
		else
		{
			// \note do we need to have a way to trap this?
			// just log trace and return nill if does not exist
			// or throw error if using more strict run time flags
			//if (strict_checking)
			// if this particular property does not exist we do nothing
			wxLogTrace(TR_SCRIPT, "Component '%s' does not exist on Object '%s'.", compType, component::objectName(iId));
			return 0;
		}
    }
    else if (Parameters::extract(L, compType, errorMsg))
    {
        component::ComponentId cId(compType.c_str());
        if (eg::ComponentScriptLua *pComponent = eg::script::GetScriptObject(L))
        {
            component::InstanceId iId = component::make(pComponent->OI().Id.GetValue(), cId);
            if (LuaPropertySet *pSelf = (LuaPropertySet *)lua_newuserdata(L, sizeof(LuaPropertySet)))
            {
                pSelf->iId = iId;
                luaL_getmetatable(L, kPropertySetTable);
                lua_setmetatable(L, -2);
                return 1;
            }
            else
            {
                errorMsg = "Could not extract create Lua PropertySet.";
            }
        }
        else
        {
            // no valid LuaComponent
            errorMsg = "no valid component (this)";
        }
    }

    stackDump(L);
    return luaL_error(L, errorMsg.c_str());
}

//! get property from Set
//! value = propertySet["Position"];
int propertySet_index(lua_State *L)
{
    using namespace eg::script;

    std::string errorMsg;
    const char *key = luaL_checkstring(L, 2);

    lua_getmetatable(L, 1);
    lua_getfield(L, -1, key);

    // Either key is name of a method in the metatable
    if(!lua_isnil(L, -1))
        return 1;

    // ... or its a field access, so recall as self.get(self, value).
    lua_settop(L, 2);

    if (LuaPropertySet *pSelf = (LuaPropertySet *)luaL_checkudata(L, 1, kPropertySetTable))
    {
        std::string propName;
        if (eg::script::ParameterPolicy::extract(L, 2, propName, errorMsg))
        {
            // get current value from propertySet[key];
            if (eg::IProperty *pProp = eg::property::cast(pSelf->iId, propName))
            {
                if (pProp->IsValue<bool>())
                {
                    lua_pushboolean(L, (bool)*pProp);
                    return 1;
                }
                else if (pProp->IsValue<float>())
                {
                    lua_pushnumber(L, (float)*pProp);
                    return 1;
                }
                else if (pProp->IsValue<int32_t>())
                {
                    lua_pushnumber(L, (int32_t)*pProp);
                    return 1;
                }
                else if (pProp->IsValue<uint32_t>())
                {
                    lua_pushnumber(L, (uint32_t)*pProp);
                    return 1;
                }
                else if (pProp->IsValue<std::string>())
                {
                    std::string propValue = *pProp;
                    lua_pushstring(L, propValue.c_str());
                    return 1;
                }
                else if (pProp->IsValue<eg::Vector2>())
                {
                    eg::Vector2 value = *pProp;
                    luabind::object o(L, value);
                    o.push(L);
                    return 1;
                }
                else if (pProp->IsValue<eg::Vector3>())
                {
                    eg::Vector3 value = *pProp;
                    luabind::object o(L, value);
                    o.push(L);
                    return 1;
                }
                else if (pProp->IsValue<eg::Vector4>())
                {
                    eg::Vector4 value = *pProp;
                    luabind::object o(L, value);
                    o.push(L);
                    return 1;
                }
                else if (pProp->IsValue<eg::Quaternion>())
                {
                    eg::Quaternion value = *pProp;
                    luabind::object o(L, value);
                    o.push(L);
                    return 1;
                }
                else
                {
                    errorMsg = "Extraction from Property is not supported.";
                }
            }

            errorMsg = boost::str(boost::format("Instance %1% does not have %2% property.") % eg::component::instanceName(pSelf->iId) % propName);
        }
    }
    else
    {
        errorMsg = "Could not extract Self from PropertySet.";
    }

    // error here
    eg::script::stackDump(L);
    return luaL_error(L, errorMsg.c_str());
}

struct PropertyConversion
{
    template <typename T>
    static bool Set(lua_State *L, int index, eg::IProperty *pProp, std::string& errorMsg)
    {
        T value;
        if (eg::script::ParameterPolicy::extract(L, index, value, errorMsg))
        {
             (*pProp) = value;
             return true;
        }

        return false;
    }

    static bool Set(lua_State *L, int index, const eg::component::InstanceId& iId, const std::string& propName, std::string& errorMsg)
    {
        using namespace eg;

        if (IProperty *pProp = property::cast(iId, propName))
        {
            if (pProp->IsValue<bool>())
            {
                return Set<bool>(L, index, pProp, errorMsg);
            }
            else if (pProp->IsValue<float>())
            {
                return Set<float>(L, index, pProp, errorMsg);
            }
            else if (pProp->IsValue<int32_t>())
            {
                return Set<int32_t>(L, index, pProp, errorMsg);
            }
            else if (pProp->IsValue<uint32_t>())
            {
                return Set<uint32_t>(L, index, pProp, errorMsg);
            }
            else if (pProp->IsValue<std::string>())
            {
                return Set<std::string>(L, index, pProp, errorMsg);
            }
            else if (pProp->IsValue<eg::Vector2>())
            {
                return Set<eg::Vector2>(L, index, pProp, errorMsg);
            }
            else if (pProp->IsValue<eg::Vector3>())
            {
                return Set<eg::Vector3>(L, index, pProp, errorMsg);
            }
            else if (pProp->IsValue<eg::Vector4>())
            {
                return Set<eg::Vector4>(L, index, pProp, errorMsg);
            }
            else if (pProp->IsValue<eg::Quaternion>())
            {
                return Set<eg::Quaternion>(L, index, pProp, errorMsg);
            }
            else if (pProp->IsValue<std::vector<std::string> >())
            {
                return Set<std::vector<std::string> >(L, index, pProp, errorMsg);
            }
            else
            {
                errorMsg = "Conversion to Property is not supported.";
            }
        }
        else
        {
            errorMsg = boost::str(boost::format("Instance %1% does not have %2% property.") % component::instanceName(iId) % propName);
        }

        return false;
    }
};


//! set property on Set
//! propertySet["Position"] = value;
int propertySet_newindex(lua_State *L)
{
    std::string errorMsg;

    if (LuaPropertySet *pSelf = (LuaPropertySet *)luaL_checkudata(L, 1, kPropertySetTable))
    {
        std::string propName;
        if (eg::script::ParameterPolicy::extract(L, 2, propName, errorMsg))
        {
            if (PropertyConversion::Set(L, 3, pSelf->iId, propName, errorMsg))
            {
                return 0;
            }
        }
    }
    else
    {
        errorMsg = "Could not extract Self from PropertySet.";
    }

    // error here
    eg::script::stackDump(L);
    return luaL_error(L, errorMsg.c_str());
}


//-------------------------------------------------------------------------------------
// tables for Component PropertySet
const luaL_reg propertySet_class_methods[] =
{
    {"get",     propertySet_get},
    {NULL,      NULL}
};

const luaL_reg propertySet_instance_methods[] =
{
    {"__index",         propertySet_index},
    {"__newindex",      propertySet_newindex},
    {NULL,              NULL}
};
//-------------------------------------------------------------------------------------


void Register_PropertySet(lua_State *L)
{
    luaL_newmetatable(L, kPropertySetTable);
    luaL_register(L, NULL, propertySet_instance_methods);
    luaL_register(L, "property", propertySet_class_methods);
}

} // namespace script
} // namespace eg

