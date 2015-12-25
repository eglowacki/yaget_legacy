/////////////////////////////////////////////////////////////////////
// BindPolicy.h
//
//  Copyright 6/7/2008 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES: Template helpers for getting parameters from lua stack
//
//
//  #include "BindPolicy.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef BINDPOLICY_H
#define BINDPOLICY_H
#pragma once

#include "MessageInterface.h"
#include "BindHelpers.h"
#include "Math/Vector.h"
#include <vector>
#include "../luacpp.h"
#include <luabind/error.hpp>
#include <luabind/operator.hpp>


namespace eg
{
    namespace script
    {

        //! This provides extraction of correct types of individual element from lua stack
        struct ParameterPolicy
        {
            static bool extract(lua_State *L, int index, int32_t& value, std::string& errorMsg)
            {
                if (lua_type(L, index) == LUA_TNUMBER)
                {
                    value = static_cast<int32_t>(lua_tonumber(L, index));
                    return true;
                }

                errorMsg = "wrong parameter type. Expected int32_t but got " + getStackType(L, index);
                return false;
            }
            static bool extract(lua_State *L, int index, uint32_t& value, std::string& errorMsg)
            {
                if (lua_type(L, index) == LUA_TNUMBER)
                {
                    value = static_cast<uint32_t>(lua_tonumber(L, index));
                    return true;
                }
                errorMsg = "wrong parameter type. Expected uint32_t but got " + getStackType(L, index);
                return false;
            }
            static bool extract(lua_State *L, int index, int64_t& value, std::string& errorMsg)
            {
                if (lua_type(L, index) == LUA_TNUMBER)
                {
                    value = static_cast<int64_t>(lua_tonumber(L, index));
                    return true;
                }
                errorMsg = "wrong parameter type. Expected int64_t but got " + getStackType(L, index);
                return false;
            }
            static bool extract(lua_State *L, int index, uint64_t& value, std::string& errorMsg)
            {
                if (lua_type(L, index) == LUA_TNUMBER)
                {
                    value = static_cast<uint32_t>(lua_tonumber(L, index));
                    return true;
                }
                errorMsg = "wrong parameter type. Expected uint64_t but got " + getStackType(L, index);
                return false;
            }
            static bool extract(lua_State *L, int index, float& value, std::string& errorMsg)
            {
                if (lua_type(L, index) == LUA_TNUMBER)
                {
                    value = static_cast<float>(lua_tonumber(L, index));
                    return true;
                }
                errorMsg = "wrong parameter type. Expected float but got " + getStackType(L, index);
                return false;
            }
            static bool extract(lua_State *L, int index, double& value, std::string& errorMsg)
            {
                if (lua_type(L, index) == LUA_TNUMBER)
                {
                    value = lua_tonumber(L, index);
                    return true;
                }
                errorMsg = "wrong parameter type. Expected double but got " + getStackType(L, index);
                return false;
            }
            static bool extract(lua_State *L, int index, std::string& value, std::string& errorMsg)
            {
                if (lua_type(L, index) == LUA_TSTRING)
                {
                    value = lua_tostring(L, index);
                    return true;
                }
                errorMsg = "wrong parameter type. Expected string but got " + getStackType(L, index);
                return false;
            }
            static bool extract(lua_State *L, int index, bool& value, std::string& errorMsg)
            {
                if (lua_type(L, index) == LUA_TBOOLEAN)
                {
                    value = lua_toboolean(L, index) != 0;
                    return true;
                }
                errorMsg = "wrong parameter type. Expected bool but got " + getStackType(L, index);
                return false;
            }
            static bool extract(lua_State *L, int index, Vector2& value, std::string& errorMsg)
            {
                try
                {
                    luabind::object o(luabind::from_stack(L, index));
                    Vector2 luaValue = luabind::object_cast<Vector2>(o);
                    value = luaValue;
                    return true;
                }
                catch (const luabind::cast_failed& /*ex*/)
                {
                    errorMsg = "wrong parameter type. Expected Vector2 but got " + getStackType(L, index);
                }

                return false;
            }
            static bool extract(lua_State *L, int index, Vector3& value, std::string& errorMsg)
            {
                try
                {
                    luabind::object o(luabind::from_stack(L, index));
                    Vector3 luaValue = luabind::object_cast<Vector3>(o);
                    value = luaValue;
                    return true;
                }
                catch (const luabind::cast_failed& /*ex*/)
                {
                    errorMsg = "wrong parameter type. Expected Vector3 but got " + getStackType(L, index);
                }
                                
                return false;
            }
            static bool extract(lua_State *L, int index, Vector4& value, std::string& errorMsg)
            {
                try
                {
                    luabind::object o(luabind::from_stack(L, index));
                    Vector4 luaValue = luabind::object_cast<Vector4>(o);
                    value = luaValue;
                    return true;
                }
                catch (const luabind::cast_failed& /*ex*/)
                {
                    errorMsg = "wrong parameter type. Expected Vector4 but got " + getStackType(L, index);
                }

                return false;
            }
            static bool extract(lua_State *L, int index, Quaternion& value, std::string& errorMsg)
            {
                try
                {
                    luabind::object o(luabind::from_stack(L, index));
                    Quaternion luaValue = luabind::object_cast<Quaternion>(o);
                    value = luaValue;
                    return true;
                }
                catch (const luabind::cast_failed& /*ex*/)
                {
                    errorMsg = "wrong parameter type. Expected Quaternion but got " + getStackType(L, index);
                }

                return false;
            }
            static bool extract(lua_State *L, int index, std::vector<std::string>& value, std::string& errorMsg)
            {
                if (lua_type(L, index) == LUA_TTABLE)
                {
                    lua_pushnil(L);
                    while (lua_next(L, index) != 0)
                    {
                        if (lua_type(L, -1) == LUA_TSTRING)
                        {
                            if (const char *element = lua_tostring(L, -1))
                            {
                                value.push_back(element);
                            }
                        }
                        lua_pop(L, 1);
                    }

                    //stackDump(L);
                    return true;
                }

                errorMsg = "wrong parameter type. Expected Vector4 but got " + getStackType(L, index);
                return false;
            }
        };

        //! Object used to extract all parameters from lua stack based
        //! on passed parameters
        struct Parameters
        {
            template <typename T1>
            static bool extract(lua_State *L, T1& value1, std::string& errorMsg)
            {
                errorMsg.clear();
                int argc = lua_gettop(L);
                if (argc == 1)
                {
                    /*
                    try
                    {
                        luabind::object o(luabind::from_stack(L, 1));
                        value1 = luabind::object_cast<T1>(o);
                        return true;
                    }
                    catch (const luabind::cast_failed& ex)
                    {
                        errorMsg = std::string("Error casting: ") + ex.what() + ex.info()->name();
                        wxLogError(errorMsg.c_str());
                    }
                    */

                    return ParameterPolicy::extract(L, 1, value1, errorMsg);
                }
                else
                {
                    // error, wrong count of arguments
                    errorMsg = "wrong number of parameters.";
                }

                return false;
            }
            template <typename T1>
            static bool extract_check(lua_State *L, T1& value1)
            {
                std::string errorMsg;
                return extract(L, value1, errorMsg);
            }

            template <typename T1, typename T2>
            static bool extract(lua_State *L, T1& value1, T2& value2, std::string& errorMsg)
            {
                errorMsg.clear();
                int argc = lua_gettop(L);
                if (argc == 2)
                {
                    /*
                    try
                    {
                        luabind::object o1(luabind::from_stack(L, 1));
                        value1 = luabind::object_cast<T1>(o1);
                        luabind::object o2(luabind::from_stack(L, 2));
                        value2 = luabind::object_cast<T2>(o2);
                        return true;
                    }
                    catch (const luabind::cast_failed& ex)
                    {
                        errorMsg = std::string("Error casting: ") + ex.what() + ex.info()->name();
                        wxLogError(errorMsg.c_str());
                    }
                    */

                    if (ParameterPolicy::extract(L, 1, value1, errorMsg))
                    {
                        return ParameterPolicy::extract(L, 2, value2, errorMsg);
                    }
                }
                else
                {
                    // error, wrong count of arguments
                    errorMsg = "wrong number of parameters.";
                }

                return false;
            }
            template <typename T1, typename T2>
            static bool extract_check(lua_State *L, T1& value1, T2& value2)
            {
                std::string errorMsg;
                return extract(L, value1, value2, errorMsg);
            }

            template <typename T1, typename T2, typename T3>
            static bool extract(lua_State *L, T1& value1, T2& value2, T3& value3, std::string& errorMsg)
            {
                errorMsg.clear();
                int argc = lua_gettop(L);
                if (argc == 3)
                {
                    if (ParameterPolicy::extract(L, 1, value1, errorMsg))
                    {
                        if (ParameterPolicy::extract(L, 2, value2, errorMsg))
                        {
                            return ParameterPolicy::extract(L, 3, value3, errorMsg);
                        }
                    }
                }
                else
                {
                    // error, wrong count of arguments
                    errorMsg = "wrong number of parameters.";
                }

                return false;
            }
            template <typename T1, typename T2, typename T3>
            static bool extract_check(lua_State *L, T1& value1, T2& value2, T3& value3)
            {
                std::string errorMsg;
                return extract(L, value1, value2, value3, errorMsg);
            }

            template <typename T1, typename T2, typename T3, typename T4>
            static bool extract(lua_State *L, T1& value1, T2& value2, T3& value3, T4& value4, std::string& errorMsg)
            {
                errorMsg.clear();
                int argc = lua_gettop(L);
                if (argc == 4)
                {
                    if (ParameterPolicy::extract(L, 1, value1, errorMsg))
                    {
                        if (ParameterPolicy::extract(L, 2, value2, errorMsg))
                        {
                            if (ParameterPolicy::extract(L, 3, value3, errorMsg))
                            {
                                return ParameterPolicy::extract(L, 4, value4, errorMsg);
                            }
                        }
                    }
                }
                else
                {
                    // error, wrong count of arguments
                    errorMsg = "wrong number of parameters.";
                }

                return false;
            }
            template <typename T1, typename T2, typename T3, typename T4>
            static bool extract_check(lua_State *L, T1& value1, T2& value2, T3& value3, T4& value4)
            {
                std::string errorMsg;
                return extract(L, value1, value2, value3, value4, errorMsg);
            }
        };

        //! Convert Message object into lua paramters
        inline int marshal(lua_State *L, const Message& msg)
        {
            lua_pushnumber(L, msg.mType);
            if (msg.Is<bool>())
            {
                lua_pushboolean(L, msg.GetValue<bool>());
            }
            else if (msg.Is<int32_t>())
            {
                lua_pushnumber(L, msg.GetValue<int32_t>());
            }
            else if (msg.Is<uint32_t>())
            {
                lua_pushnumber(L, msg.GetValue<uint32_t>());
            }
            else if (msg.Is<int64_t>())
            {
                lua_pushnumber(L, static_cast<lua_Number>(msg.GetValue<int64_t>()));
            }
            else if (msg.Is<uint64_t>())
            {
                lua_pushnumber(L, static_cast<lua_Number>(msg.GetValue<uint64_t>()));
            }
            else if (msg.Is<float>())
            {
                lua_pushnumber(L, msg.GetValue<float>());
            }
            else if (msg.Is<double>())
            {
                lua_pushnumber(L, msg.GetValue<double>());
            }
            else if (msg.Is<std::string>())
            {
                lua_pushstring(L, msg.GetValue<std::string>().c_str());
            }
            else
            {
                wxLogError("marshal can not convert Message mAnyData.");
                lua_pushnil(L);
            }

            return 2;
        }

    } // namespace script
} // namespace eg

#endif // BINDPOLICY_H

