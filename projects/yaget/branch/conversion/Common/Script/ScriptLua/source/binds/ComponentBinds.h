/////////////////////////////////////////////////////////////////////
// ComponentBinds.h
//
//  Copyright 8/4/2008 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Expose base component class wrapper
//      to allow derivation in lua
//
//  #include "binds/ComponentBinds.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef BINDS_COMPONENT_BINDS_H
#define BINDS_COMPONENT_BINDS_H
#pragma once

#include "ScriptBase.h"
#include "ObjectID.h"
#include "MessageInterface.h"
#include "Input/InputManager.h"
#include "Hash/Hash.h"
#include <string>
#include <set>
#include <boost/signal.hpp>
#include "../luacpp.h"
#pragma warning(push)
#pragma warning (disable : 4702)  // unreachable code
#include <luabind/luabind.hpp>
#include <luabind/operator.hpp>
#pragma warning(pop)

namespace eg
{
    namespace script
    {
        //! This is used in message data to wrap Message structure from c++
        struct MesssageData : public Message
        {
            MesssageData(const Message& msg) : Message(msg)
            {}

            int Type() const;
            float asFloat() const;
            int asInt() const;
            int asId() const;
			Vector2 asVector2() const;
			Vector3 asVector3() const;
			Vector4 asVector4() const;
        };

        //!! Instance representing id of a particular instanced based on std::pair<component::ObjectId, component::ComponentId>
        struct Instance
        {
            Instance()
            : objectId(eg::Hash().GetValue())
            , compomentId(eg::Hash().GetValue())
            {}

            Instance(uint64_t oId, uint64_t cId)
            : objectId(oId)
            , compomentId(cId)
            {}

            Instance(uint64_t oId, const std::string& compName)
            : objectId(oId)
            , compomentId(eg::Hash(compName.c_str()).GetValue())
            {}

            bool isValid() const
            {
                return isObjectValid() && isComponentValid();
            }

            bool isObjectValid() const
            {
                return eg::Hash(static_cast<uint32_t>(objectId)).IsValid();
            }

            bool isComponentValid() const
            {
                return eg::Hash(static_cast<uint32_t>(compomentId)).IsValid();
            }
            
            uint64_t objectId;
            uint64_t compomentId;
        };

        //! Any scrips can be derived from this class to provide
        //! simple message and input callbacks
        class Component : public boost::signals::trackable
        {
        public:
            //! Create this object. oId needs to be valid object which has ScriptComponent
            Component(int oId, int ownerId);
            virtual ~Component();

            int Id() const {return mObjectId.GetValue();}
            //! Called when script files is getting destroyed
            //! this is purely internal call and it's used by ScriptComponent
            void Finalize();

            //! Methods which can be overridden in script
            virtual void onMessage(const MesssageData& /*msg*/) {}
            virtual void onAction(const std::string& /*actionName*/, uint32_t /*timeStamp*/, int32_t /*mouseX*/, int32_t /*mouseY*/) {}

            //{@
            //! Called from script to add or remove messages
            void addMessage(guid_t msgType);
            void removeMessage(guid_t msgType);
            //}@

            //{@
            //! Called from script to add or remove Action messages (input)
            void addAction(const std::string& actionName);
            void removeAction(const std::string& actionName);
            void addActionSwitch(const std::string& actionNameOn, const std::string& actionNameOff);
            //}@
            //! Return true if this particular action is active, false otherwise
            bool isAction(const std::string& actionName) const;
            //! Return true if particular action behave like switch
            bool isActionSwitch(const std::string& actionNameOn) const;
            //! Return mouse delta (move in pixel) since the last call to this method
            Vector2 getMouseDelta(const std::string& actionNameOn) const;

        private:
            void onShutdown(Message& msg);
            //! Called by dispatcher for any connected messages
            void onRegisteredMessage(Message& msg);
            //! Called by Input when registered action is triggered
            void onRegisteredAction(const std::string& actionName, uint32_t timeStamp, int32_t mouseX, int32_t mouseY);

            //! Object id which this component belongs to (is part of it)
            component::ObjectId mObjectId;
            component::ObjectId mOwnerId;

            //! Which messages are registered from script
            std::set<guid_t> mRegisteredMessages;

            //! This is used to keep input action registration
            typedef std::map<std::string, InputManager::ActionListener_t> ActionMap_t;
            ActionMap_t mActionMap;

            typedef boost::shared_ptr<InputSwitch> InputSwitch_t;
            typedef std::map<std::string, InputSwitch_t> ActionMapSwitch_t;
            ActionMapSwitch_t mActionMapSwitch;
        };


        //! Works with Component class but exposes to script
        class ComponentWraper : public Component, public luabind::wrap_base
        {
        public:
            ComponentWraper(int oId, int ownerId)
            : Component(oId, ownerId)
            {
            }

            virtual void onMessage(const MesssageData& msg)
            {
                call<void>("onMessage", msg);
            }
            static void default_onMessage(Component *ptr, const MesssageData& msg)
            {
                return ptr->Component::onMessage(msg);
            }

            virtual void onAction(const std::string& actionName, uint32_t timeStamp, int32_t mouseX, int32_t mouseY)
            {
                call<void>("onAction", actionName, timeStamp, mouseX, mouseY);
            }
            static void default_onAction(Component *ptr, const std::string& actionName, uint32_t timeStamp, int32_t mouseX, int32_t mouseY)
            {
                return ptr->Component::onAction(actionName, timeStamp, mouseX, mouseY);
            }
        };

        //! This is just empty class object to allow us to expose it
        //! to lua script
        class LuaBase
        {
        public:
            LuaBase();
            virtual ~LuaBase();
        };

        class LuaBaseWraper : public LuaBase, public luabind::wrap_base
        {
        public:
            LuaBaseWraper();
            virtual ~LuaBaseWraper();
        };

    } // namespace script

} // namespace eg

#endif // BINDS_COMPONENT_BINDS_H

