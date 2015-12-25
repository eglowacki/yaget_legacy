/////////////////////////////////////////////////////////////////////////
// bind_editor.cpp
//
//  Copyright 4/8/2009 Edgar Glowacki.
//
// NOTES:
//      Provides bindings for editor from lua
//
//
//
/////////////////////////////////////////////////////////////////////////
//! \file
#include "Precompiled.h"
#include "Logger/Log.h"
#include "Script/ScriptAsset.h"
#include "Script/luacpp.h"
#include "../EditorPlugin.h"
#pragma warning(push)
#pragma warning (disable : 4100)  // '' : unreferenced formal parameter
#include <luabind/luabind.hpp>
#pragma warning(pop)
#include <luabind/operator.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>


namespace
{
    using namespace eg;
    using namespace luabind;

    template<typename T>
    luabind::object vector_to_table(lua_State *L, const std::vector<T>& v)
    {
        luabind::object result = luabind::newtable(L);

        int n = 1;
        for (std::vector<T>::const_iterator iter(v.begin()), end(v.end()); iter != end; ++iter)
        {
            result[n++] = *iter;
        }

        return result;
    }


    class LuaEditor
    {
    public:
        bool Load(const object& name_lua)
        {
            EditorPlugin& edp = script::get_ref<EditorPlugin>(name_lua.interpreter(), "edp");
            std::string name = luabind::object_cast<const std::string>(name_lua);
            return edp.Load(name);
        }

        bool Unload(const object& name_lua)
        {
            EditorPlugin& edp = script::get_ref<EditorPlugin>(name_lua.interpreter(), "edp");
            std::string name = luabind::object_cast<const std::string>(name_lua);
            return edp.Unload(name);
        }

        object List(const object& name_lua)
        {
            EditorPlugin& edp = script::get_ref<EditorPlugin>(name_lua.interpreter(), "edp");
            std::string name = luabind::object_cast<const std::string>(name_lua);
            std::vector<std::string> validNames = edp.List(name);

            object pluginList = vector_to_table(name_lua.interpreter(), validNames);
            return pluginList;
        }
    };
}


namespace eg { namespace script {

    void init_editor(lua_State *L, EditorPlugin *edp)
    {
        using namespace luabind;

        module(L, "yaget")
        [
            class_<EditorPlugin>("EditorPlugin"),

            class_<LuaEditor>("Editor")
            .def(constructor<>())
            .def("load", &LuaEditor::Load)
            .def("unload", &LuaEditor::Unload)
            .def("list", &LuaEditor::List)
        ];

        object g = globals(L);
        object lua_yaget = gettable(g, "yaget");
        lua_yaget["edp"] = edp;

        /*
        module(L, "editor")
            [
                class_<Vector3>("Vector3")
                .def(constructor<>())
                .def(constructor<float, float, float>())
                .def(constructor<const Vector3&>())
                .def(tostring(self))
                .def("set", (void(Vector3::*)(float, float, float))&Vector3::set)
                .def("set", (void(Vector3::*)(const Vector3&))&Vector3::set)
                .def_readwrite("x", &Vector3::x)
                .def_readwrite("y", &Vector3::y)
                .def_readwrite("z", &Vector3::z),

                class_<Vector4>("Vector4")
                .def(constructor<>())
                .def(constructor<float, float, float, float>())
                .def(constructor<const Vector4&>())
                .def(tostring(self))
                .def("set", (void(Vector4::*)(float, float, float, float))&Vector4::set)
                .def_readwrite("x", &Vector4::x)
                .def_readwrite("y", &Vector4::y)
                .def_readwrite("z", &Vector4::z)
                .def_readwrite("w", &Vector4::w)
            ];
            */
    }


}} // namespace script // namespace eg



