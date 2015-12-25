// ComponentBinds.cpp
#include "Precompiled.h"
#pragma warning(push)
#pragma warning (disable : 4100)  // '' : unreferenced formal parameter
#pragma warning (disable : 4512)  // '' : assignment operator could not be generated
#include "ComponentBinds.h"
#pragma warning(pop)
#include "BindHelpers.h"
#include "IRenderer.h"
#include "ObjectID.h"
#include "Math/Vector.h"
#include "Registrate.h"
#include "IObjectManager.h"
#include "Interface/ICollision.h"
#include "../ComponentScriptLua.h"
#include "../Script.h"
#include "BindPolicy.h"
#include "AutoBinders.h"
#include <boost/format.hpp>
#pragma warning(push)
#pragma warning (disable : 4189)  // '' : local variable is initialized but not referenced
#include <luabind/error.hpp>
#include <luabind/operator.hpp>
#include <luabind/iterator_policy.hpp>
#pragma warning(pop)
#include <eh.h>



namespace
{
    //! Components based binds

    //! There are two functions for new component
    //! component.newc(string kComponentType)
    //! component.newc(number id, string kComponentType, string "user_name")
    int component_newc(lua_State *L)
    {
        using namespace eg;
        using namespace eg::script;

        std::string errorMsg;
        if (eg::ComponentScriptLua *pComponent = eg::script::GetScriptObject(L))
        {
            uint64_t oId;
            std::string compType;
            std::string name;
            std::vector<std::string> compList;
            if (Parameters::extract(L, compType, errorMsg))
            {
                component::ComponentId cId(compType.c_str());
                if (pComponent->script_newc(cId))
                {
                    return 0 ;
                }
            }
            else if (Parameters::extract(L, oId, compType, name, errorMsg))
            {
                component::ComponentId cId(compType.c_str());
                component::InstanceId iId = component::make(oId, cId);
                if (pComponent->script_newc(iId, name))
                {
                    return 0;
                }
            }
            else if (Parameters::extract(L, oId, compList, name, errorMsg))
            {
                for (std::vector<std::string>::const_iterator it = compList.begin(); it != compList.end(); ++it)
                {
                    component::ComponentId cId((*it).c_str());
                    component::InstanceId iId = component::make(oId, cId);
                    pComponent->script_newc(iId, name);
                }

                // \note if we could not create one or more components from list
                // what should we do here? There will be messages already logged.
                return 0;
            }
        }
        else
        {
            // no valid LuaComponent
            errorMsg = "no valid component (this)";
        }

        stackDump(L);
        return luaL_error(L, errorMsg.c_str());
    }

    //! delete specific component
    int component_freec(lua_State *L)
    {
        using namespace eg;
        using namespace eg::script;

        std::string errorMsg;
        if (eg::ComponentScriptLua *pComponent = eg::script::GetScriptObject(L))
        {
            uint64_t oId;
            std::string compType;
            //std::string name;
            if (Parameters::extract(L, compType, errorMsg))
            {
                component::ComponentId cId(compType.c_str());
                pComponent->script_freec(cId);
                return 0 ;
            }
            else if (Parameters::extract(L, oId, compType, errorMsg))
            {
                component::ComponentId cId(compType.c_str());
                component::InstanceId iId = component::make(oId, cId);
                pComponent->script_freec(iId);
                return 0;
            }
        }
        else
        {
            // no valid LuaComponent
            errorMsg = "no valid component (this)";
        }

        stackDump(L);
        return luaL_error(L, errorMsg.c_str());
    }

    //! return id of this object
    int component_this(lua_State *L)
    {
        using namespace eg;
        using namespace eg::script;

        std::string errorMsg;
        if (eg::ComponentScriptLua *pComponent = eg::script::GetScriptObject(L))
        {
            uint64_t oId = pComponent->OI().Id;
            lua_pushinteger(L, oId);
            return 1;
        }
        else
        {
            // no valid LuaComponent
            errorMsg = "no valid component (this)";
        }

        stackDump(L);
        return luaL_error(L, errorMsg.c_str());
    }

    //! view.print(math.Vector2 pos, string text)
    //! \todo add color parameter (Vector4)
    int view_print(lua_State *L)
    {
        using namespace eg::script;
        std::string errorMsg;
        eg::Vector2 pos;
        std::string stringToPrint;
        if (Parameters::extract(L, pos, stringToPrint, errorMsg))
        {
            eg::Print(static_cast<int32_t>(pos.x), static_cast<int32_t>(pos.y), stringToPrint);
            return 0;
        }

        stackDump(L);
        return luaL_error(L, errorMsg.c_str());
    }


    const luaL_reg componentLib[] =
    {
        {"newc",            component_newc},
        {"freec",           component_freec},
        {"this",            component_this},
        {NULL,              NULL}
    };

    const luaL_reg viewLib[] =
    {
        {"print",           view_print},
        {NULL,              NULL}
    };


    // return burnable id
    int idspace_burnable(lua_State *L)
    {
        uint64_t id = eg::idspace::get_burnable();
        lua_pushnumber(L, id);
        return 1;
    }

    // return _persistent id
    int idspace_persistent(lua_State *L)
    {
        uint64_t id = eg::idspace::get_persistent();
        lua_pushnumber(L, id);
        return 1;
    }

    // get burnable and persistent id's
    const luaL_reg idspaceLib[] =
    {
        {"burnable",        idspace_burnable},
        {"persistent",      idspace_persistent},
        {NULL,              NULL}
    };

    //! Provides save and load functions for entire object
    //! object.save(objectId)
    //! object.save(objectId, "myFoo")
    int object_save(lua_State *L)
    {
        using namespace eg;
        using namespace eg::script;

        std::string errorMsg;
        uint64_t objectId = 0;;
        component::ObjectId oId;
        std::string objectName;
        if (Parameters::extract(L, objectId, objectName, errorMsg) && IObjectManager::ValidateName(objectName))
        {
            oId.Set(objectId);
        }
        else if (Parameters::extract(L, objectId, errorMsg))
        {
            oId.Set(objectId);
            IObjectManager& om = REGISTRATE(IObjectManager);
            const ObjectInstance& oi = om.OI(oId);
            objectName = oi.Name;
        }

        if (oId.IsValid())
        {
            if (!WildCompareI("*.odf", objectName))
            {
                objectName += ".odf";
            }
            VirtualFileSystem& vfs = REGISTRATE(VirtualFileSystem);
            if (VirtualFileFactory::ostream_t ostream = vfs.AttachFileStream(objectName))
            {
				REGISTRATE(IObjectManager).SaveObject(oId, ostream);
            }
        }

        return 0;
    }

    int object_load(lua_State *L)
    {
        using namespace eg;
        using namespace eg::script;

        std::string errorMsg;
        uint64_t objectId = 0;;
        component::ObjectId oId;
        std::string objectName;
        bool bLoaded = false;
        if (Parameters::extract(L, objectId, objectName, errorMsg) && IObjectManager::ValidateName(objectName))
        {
            oId.Set(objectId);
        }
        else if (Parameters::extract(L, objectId, errorMsg))
        {
            oId.Set(objectId);
            objectName = boost::str(boost::format("0x%016X") % objectId);
        }

        if (oId.IsValid())
        {
            if (!WildCompareI("*.odf", objectName))
            {
                objectName += ".odf";
            }
            VirtualFileSystem& vfs = REGISTRATE(VirtualFileSystem);
            if (VirtualFileFactory::istream_t istream = vfs.GetFileStream(objectName))
            {
				bLoaded = REGISTRATE(IObjectManager).LoadObject(oId, istream);
            }
        }

        lua_pushboolean(L, bLoaded);
        return 1;
    }

    //save and load entire object
    const luaL_reg objectLib[] =
    {
        {"save",        object_save},
        {"load",        object_load},
        {NULL,          NULL}
    };

    // expose messages id's as enums
    class MessageIds
    {
    public:
    };

} // namespace



namespace eg {
namespace script {

void straight_to_debugger(unsigned int, _EXCEPTION_POINTERS*)
{
    throw;
}


int add_file_and_line(lua_State* L)
{
    lua_Debug d;
    lua_getstack(L, 1, &d);
    lua_getinfo(L, "Slnu", &d);
    std::string err = lua_tostring(L, -1);
    lua_pop(L, 1);
    std::stringstream msg;
    msg << d.source << "(" << d.currentline << ") : error";
    //msg << d.short_src << ":" << d.currentline;

    if (d.name != 0)
    {
        msg << "(" << d.namewhat << " " << d.name << ")";
    }
    msg << " " << err;
    lua_pushstring(L, msg.str().c_str());
    return 1;
}


extern void Register_PropertySet(lua_State *L);


void ComponentBinds(lua_State *L)
{
    using namespace luabind;

    set_pcall_callback(add_file_and_line);

    luaL_register(L, "component", componentLib);
    luaL_register(L, "view", viewLib);
    luaL_register(L, "idspace", idspaceLib);
    luaL_register(L, "object", objectLib);

    Register_PropertySet(L);

    module(L)
    [
        class_<MesssageData>("Message")
        .def("Type", &MesssageData::Type)
        .def("asFloat", &MesssageData::asFloat)
        .def("asInt", &MesssageData::asInt)
        .def("asId", &MesssageData::asId)
		.def("asVector2", &MesssageData::asVector2)
		.def("asVector3", &MesssageData::asVector4)
		.def("asVector4", &MesssageData::asVector4)
    ];

    module(L)
    [
        class_<Component, ComponentWraper>("Component")
        .def(constructor<int, int>())
        .def("addMessage", &Component::addMessage)
        .def("removeMessage", &Component::removeMessage)
        .def("onMessage", &Component::onMessage, &ComponentWraper::default_onMessage)
        .def("addAction", &Component::addAction)
        .def("addActionSwitch", &Component::addActionSwitch)
        .def("removeAction", &Component::removeAction)
        .def("onAction", &Component::onAction, &ComponentWraper::default_onAction)
        .def("isAction", &Component::isAction)
        .def("isActionSwitch", &Component::isActionSwitch)
        .def("getMouseDelta", &Component::getMouseDelta)
        .def("Id", &Component::Id)
    ];

    module(L)
    [
        class_<MessageIds>("mtype")
        .enum_("constants")
        [
            value("tick", 0x918b5491),
            value("objectNew", 0xd466fa9f),
            value("objectConstructed", 0xf583e07a),
            value("objectDelete", 0x1466fad4),
            value("componentNew", 0xd27f4e59),
            value("componentDelete", 0xd27f4e5c),
			value("viewSelectedObject", 0x9619beb8)
			
        ]
    ];

    module(L)
    [
        class_<LuaBase, LuaBaseWraper>("Base")
        .def(constructor<>())
    ];

    module(L, "input")
    [
        class_<InputSwitch>("Action")
        .def(constructor<const char *, const char *>())
        .def("isOn", &InputSwitch::isOn)
        .def("getMouseDelta", &InputSwitch::getMouseDelta)
        .def("getMousePos", &InputSwitch::getMousePos)
    ];

    module(L, "component")
    [
        class_<eg::component::ObjectId>("ObjectId")
        .def(constructor<uint32_t>())
        .def("getValue", &eg::component::ObjectId::GetValue)
        .def(tostring(self))
    ];

    module(L, "collision")
    [
        class_<eg::collision::Request>("Request")
        .def(constructor<>())
        .def_readwrite("Ray", &eg::collision::Request::ray)
        .def_readwrite("Lenght", &eg::collision::Request::lenght)
        .def_readwrite("AllHits", &eg::collision::Request::allHits)
    ];

    module(L, "collision")
    [
        class_<eg::collision::Response::Record>("Record")
        .def(constructor<>())
        .def_readonly("oId", &eg::collision::Response::Record::oId)
        .def_readonly("hitPoint", &eg::collision::Response::Record::hitPoint)
        .def_readonly("hitNormal", &eg::collision::Response::Record::hitNormal)
    ];

    module(L, "collision")
    [
        class_<eg::collision::Response>("Response")
        .def_readonly("Hits", &eg::collision::Response::Hits, return_stl_iterator)
    ];

    module(L, "view")
    [
        def("getHitObjects", (eg::collision::Response(*)(uint64_t, const Vector2&, bool))&render::GetHitObjects)
        ,def("getHitObjects", (eg::collision::Response(*)(const std::string&, const Vector2&, bool))&render::GetHitObjects)
        ,def("setFont", (void(*)(const std::string& fontName, size_t size, bool bBold))&render::SetFont)
    ];

    RegisterAllDeferedModules();
    //::_set_se_translator(straight_to_debugger);
}


LuaBase::LuaBase()
{
}


LuaBase::~LuaBase()
{
}


LuaBaseWraper::LuaBaseWraper()
: LuaBase()
{
}


LuaBaseWraper::~LuaBaseWraper()
{
}


Component::Component(int oId, int ownerId)
: mObjectId(oId)
, mOwnerId(ownerId)
{
    ComponentScriptLua *pLuaComponent = 0;
    if ((pLuaComponent = component::get<ComponentScriptLua>(mObjectId)) == 0)
    {
        pLuaComponent = component::get<ComponentScriptLua>(mOwnerId);
    }

    if (pLuaComponent)
    {
        pLuaComponent->script_AddComponent(this);
    }
    else
    {
        // if we are not owned by ComponentScriptLua, we need to wire itself to shutdown message
        Dispatcher& disp = REGISTRATE(Dispatcher);
        disp[mtype::kShutdownBegin].connect(boost::bind(&Component::onShutdown, this, _1));
    }
}


Component::~Component()
{
    ComponentScriptLua *pLuaComponent = 0;
    if ((pLuaComponent = component::get<ComponentScriptLua>(mObjectId)) == 0)
    {
        pLuaComponent = component::get<ComponentScriptLua>(mOwnerId);
    }

    if (pLuaComponent)
    {
        pLuaComponent->script_RemoveComponent(this);
    }
    //Finalize();
}


void Component::Finalize()
{
    while (!mRegisteredMessages.empty())
    {
        removeMessage(*mRegisteredMessages.begin());
    }
    while (!mActionMap.empty())
    {
        removeAction((*mActionMap.begin()).first);
    }

    mActionMapSwitch.clear();
}


void Component::onShutdown(Message& msg)
{
    Finalize();
    Dispatcher& disp = REGISTRATE(Dispatcher);
    disp[mtype::kShutdownBegin].disconnect(boost::bind(&Component::onShutdown, this, _1));
}


void Component::addMessage(guid_t msgType)
{
    std::pair<std::set<guid_t>::iterator, bool> ret = mRegisteredMessages.insert(msgType);
    if (ret.second)
    {
        Dispatcher& disp = REGISTRATE(Dispatcher);
        disp[msgType].connect(boost::bind(&Component::onRegisteredMessage, this, _1));
    }
}


void Component::removeMessage(guid_t msgType)
{
    if (mRegisteredMessages.erase(msgType))
    {
        Dispatcher& disp = REGISTRATE(Dispatcher);
        disp[msgType].disconnect(boost::bind(&Component::onRegisteredMessage, this, _1));
    }
}


void Component::addAction(const std::string& actionName)
{
    ActionMap_t::const_iterator it = mActionMap.find(actionName);
    if (it == mActionMap.end())
    {
        InputManager& im = REGISTRATE(InputManager);
        InputManager::ActionListener_t connection = im.ListenForAction(actionName, boost::bind(&Component::onRegisteredAction, this, _1, _2, _3, _4));
        mActionMap.insert(std::make_pair(actionName, connection));
    }
    else
    {
        wxLogError("Trying to register '%s' input action already registered, ignoring.", actionName.c_str());
    }
}


void Component::removeAction(const std::string& actionName)
{
    ActionMap_t::iterator it = mActionMap.find(actionName);
    if (it != mActionMap.end())
    {
        mActionMap.erase(it);
    }
    else
    {
        wxLogError("Trying to unregister '%s' input action which is not registered, ignoring.", actionName.c_str());
    }
}

void Component::addActionSwitch(const std::string& actionNameOn, const std::string& actionNameOff)
{
    ActionMapSwitch_t::iterator it = mActionMapSwitch.find(actionNameOn);
    if (it == mActionMapSwitch.end())
    {
        mActionMapSwitch.insert(std::make_pair(actionNameOn, new InputSwitch(actionNameOn.c_str(), actionNameOff.c_str())));
    }
    else
    {
        wxLogError("Trying to register '%s' input action switch which is already registered, ignoring.", actionNameOn.c_str());
    }
}


bool Component::isAction(const std::string& actionName) const
{
    ActionMap_t::const_iterator it = mActionMap.find(actionName);
    if (it != mActionMap.end())
    {
        InputManager& im = REGISTRATE(InputManager);
        return im.IsAction(actionName.c_str());
    }

    wxLogError("Trying to check Action '%s' which was not registered.", actionName.c_str());
    return false;
}


bool Component::isActionSwitch(const std::string& actionNameOn) const
{
    ActionMapSwitch_t::const_iterator it = mActionMapSwitch.find(actionNameOn);
    if (it != mActionMapSwitch.end())
    {
        return (*it).second->isOn();
    }

    wxLogError("Trying to check ActionSwitch '%s' which was not registered.", actionNameOn.c_str());
    return false;
}


Vector2 Component::getMouseDelta(const std::string& actionNameOn) const
{
    ActionMapSwitch_t::const_iterator it = mActionMapSwitch.find(actionNameOn);
    if (it != mActionMapSwitch.end())
    {
        return (*it).second->getMouseDelta();
    }

    wxLogError("Trying to check ActionSwitch '%s' which was not registered.", actionNameOn.c_str());
    return v2::ZERO();
}

void Component::onRegisteredMessage(Message& msg)
{
    std::set<guid_t>::const_iterator it = mRegisteredMessages.find(msg.mType);
    if (it != mRegisteredMessages.end())
    {
        try
        {
            MesssageData newMsg(msg);
            onMessage(newMsg);
        }
        catch (const luabind::error& ex)
        {
            const char* errorMsg = lua_tostring(ex.state(), -1);
            wxLogError("Calling onMessage 0x%X for ScriptComponent 0x%X failed\n%s", msg.mType,  mObjectId.GetValue(), (errorMsg ? errorMsg : "luabind::error"));
            lua_pop(ex.state(), 1);
        }
    }
    else
    {
        wxLogError("Calling onMessage 0x%X for ScriptComponent 0x%X but it's not registered.", msg.mType,  mObjectId.GetValue());
    }
}


void Component::onRegisteredAction(const std::string& actionName, uint32_t timeStamp, int32_t mouseX, int32_t mouseY)
{
    ActionMap_t::iterator it = mActionMap.find(actionName);
    if (it != mActionMap.end())
    {
        try
        {
            onAction(actionName, timeStamp, mouseX, mouseY);
        }
        catch (const luabind::error& ex)
        {
            const char* errorMsg = lua_tostring(ex.state(), -1);
            wxLogError("Calling onAction '%s' for ScriptComponent %s failed\n%s", actionName.c_str(), component::objectName(mObjectId).c_str(), (errorMsg ? errorMsg : "luabind::error"));
            lua_pop(ex.state(), 1);
        }
    }
    else
    {
        wxLogError("Calling onAction '%s' for ScriptComponent %s  but it's not registered.", actionName.c_str(), component::objectName(mObjectId).c_str());
    }
}



/*
//! Return TRUE if there is any kind of data attached to this message and it's of type T
//! This does not trigger any errors
template <typename T>
bool Is(const boost::any& value) const
{
    return !value.empty() && value.type() == typeid(T);
}


//! Return data attached to this message. This will trigger error, if there is no data
//! attached or is not of type T
template <typename T>
T GetValue(const boost::any& value) const
{
    try
    {
        return boost::any_cast<T>(value);
    }
    catch (const boost::bad_any_cast& er)
    {
        er;
        wxLogError("Message::mAnyData: %s", er.what());
        return T();
    }
}
*/


float MesssageData::asFloat() const
{
    if (Is<float>())
    {
        return GetValue<float>();
    }

    return 0;
}


int MesssageData::asInt() const
{
    if (Is<int>())
    {
        return GetValue<int>();
    }
    else if (Is<long>())
    {
        return (int)GetValue<long>();
    }
    else if (Is<int32_t>())
    {
        return (int)GetValue<int32_t>();
    }

    return 0;
}


int MesssageData::asId() const
{
    if (Is<uint64_t>())
    {
        return GetValue<uint64_t>();
    }
	else if (Is<component::ObjectId>())
	{
		return GetValue<component::ObjectId>().GetValue();
	}

    return 0;
}

Vector2 MesssageData::asVector2() const
{
	if (Is<Vector2>())
	{
		return GetValue<Vector2>();
	}

	return v2::ZERO();
}


Vector3 MesssageData::asVector3() const
{
	if (Is<Vector3>())
	{
		return GetValue<Vector3>();
	}

	return v3::ZERO();
}


Vector4 MesssageData::asVector4() const
{
	if (Is<Vector4>())
	{
		return GetValue<Vector4>();
	}

	return v4::ZERO();
}

int MesssageData::Type() const
{
    return static_cast<int>(mType);
}

} // namespace script
} // namespace eg

