///////////////////////////////////////////////////////////////////////
// ObjectManagerDB.h
//
//  Copyright 10/16/2005 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      ObjectManager database storage class.
//      Internal header file
//
//
//  #include "ObjectManagerDB.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef OBJECT_MANAGER_DB_H
#define OBJECT_MANAGER_DB_H

#include "IObjectManager.h"
#include "ComponentIterators.h"
#include "MessageEnums.h"
#include <map>

namespace eg
{
    // forward declarations
    class IComponent;
    class Message;

    struct ComponentTypeInfo
    {
        ComponentTypeInfo() :
            mCreationMethod(0),
            mDestructionMethod(0)
        {
        }

        ComponentCreationMethod mCreationMethod;
        ComponentDestructionMethod mDestructionMethod;
    };


    class ObjectManagerDB
    {
    public:
        //! @name Component Management
        //@{
        //! This will register component type. You can look at that as a factory for creation and deletion of component types
        void RegisterComponentType(const component::ComponentId& compID, ComponentCreationMethod pCreationFn, ComponentDestructionMethod pDestructionFn);
        //! Create component of type
        IComponent *CreateComponent(const component::InstanceId& iId);
        //! Add this component DB and associate with specific object ID
        bool AddComponent(const component::ObjectId& oId, IComponent *pComp);
        //! Delete the component. This will also remove from DB.
        void DestroyComponent(IComponent *pComp);
        //@}

        //! Check to see if this object exist
        bool IsObjectExists(const component::ObjectId& oId) const;
        //! This will find instnace of the component which has component type and belongs to Object ID
        IComponent *FindComponentInstance(const component::InstanceId& iId) const;

        void SubscribeToMessageType(const component::ComponentId& ctid, guid_t mtid);
        void PostEvent(const component::ObjectId& oid, Message &msg);
        void BroadcastEvent(Message& msg);

        //! @name Accessors Management
        //! Accessors for internal data structure
        //@{
        //! Return const reference to set of all registered component types
        const std::set<component::ComponentId>& GetComponentTypes() const;
        //! Return const reference to map of component::ObjectId to components instances
        const ComponentMap_t& GetComponentInstanceList(const component::ComponentId& cId);
        //! This will return true if this component type is registered
        bool IsComponentTypeValid(const component::ComponentId& cId) const;
        //! Return in out paramter list of all unique object id's present in this database
        bool GetAllObjectIDs(std::set<component::ObjectId>& objectIdList) const;
        //@}

    private:
        /*!
        Factory of components with creation and destruction functions
        Each unique component type (component::ComponentId, like Entity, Model) has data associated
        with it which allows us to create and delete instances of that type.

        \param component::ComponentId unique component type (Entity, Model, etc)
        \param ComponentTypeInfo struct which holds create and destroy function pointers
        */
        std::map<component::ComponentId, ComponentTypeInfo> mComponentTypeFactory;

        /*!
        This contains list of all registered component types,
        since we need to be able expose the list of all component types.
        It's 1:1 mapping to mComponentTypeFactory component::ComponentId value.

        \param component::ComponentId unique component type (Entity, Model, etc)
        */
        std::set<component::ComponentId> mComponentTypeList;

        /*!
        Maps which message type is associated with which component type,
        like Message::Created (guid_t) is associated with Entity (component::ComponentId)

        \param guid_t unique message ID
        \param std::set<component::ComponentId> list of all unique types (Entity, Model, etc) which will receive this message.
        */
        std::map<guid_t, std::set<component::ComponentId> > mMessageTypeToComponentTypes;

        /*!
        Dynamic component data, IOW all the instances of created components will be stored in this container


        \param component::ComponentId unique component type (Entity, Model, etc)
        \param ComponentMap_t mapping from unique object ID (we use object name) to e specific instance of a component
                              std::map<component::ObjectId, IComponent *>
        */
        std::map<component::ComponentId, ComponentMap_t> mComponentInstanceList;
    };


} // namespace eg

#endif //OBJECT_MANAGER_DB_H