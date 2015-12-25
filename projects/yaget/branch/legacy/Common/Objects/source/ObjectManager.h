///////////////////////////////////////////////////////////////////////
// ObjectManager.h
//
//  Copyright 10/16/2005 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Private header file for ObjectManager
//      Based on some from GPG5
//      This is internal header file
//
//
//  #include "ObjectManager.h"
//
//////////////////////////////////////////////////////////////////////
//! \file


#ifndef OBJECT_MANAGER_H
#define OBJECT_MANAGER_H

#include "IObjectManager.h"
#include "IComponent.h"
#include "MessageEnums.h"
#include "ObjectID.h"
#include "Hash/ReverseHashLookup.h"
#include "Config/Console.h"
#include <wx/thread.h>
#include <vector>
#include <boost/smart_ptr.hpp>

namespace {struct Instance;}


namespace eg
{
    class Message;
    class ObjectManagerDB;
    class Dispatcher;
    namespace config { class Konsole; }


    /*!
    Our global object manager
    */
    class ObjectManager : public IObjectManager
    {
    public:
        ObjectManager(Dispatcher& dispatcher, config::Konsole& konsole);
        ~ObjectManager();

        // from IObjectManager
        // Enums
        virtual void EnumObjectsByType(const component::ComponentId& cid, EnumComponentCallback& componentCallback) const;
        virtual void EnumObjectsByObject(const component::ObjectId& oid, EnumComponentCallback& componentCallback) const;
        virtual void EnumObjectsByAll(EnumComponentCallback& componentCallback) const;
        virtual const std::vector<component::ObjectId>& EnumObjects() const;
        virtual IComponent *FindComponentInstance(const component::InstanceId& iId) const;

        // Create
        virtual IComponent *CreateComponent(const component::InstanceId& iId, const std::string& name);
        virtual bool CloneInstance(const component::InstanceId& sourceId, const component::ObjectId& targeId, const std::string& targetName);

        // Destroy
        virtual void DestroyComponent(IComponent *pComp);
        virtual void DestroyInstance(const component::ObjectId& targeId);

        // Utilities
        virtual const ObjectInstance& OI(const component::ObjectId& id) const;
        virtual std::string GetComponentName(const component::ComponentId& cId) const;
        virtual const std::set<component::ComponentId>& GetComponentTypes() const;
        virtual void RegisterComponentType(const component::ComponentId& cId, ComponentCreationMethod pCreationFn, ComponentDestructionMethod pDestructionFn, const wxChar *pReadableName);
        virtual bool IsObjectExists(const component::ObjectId& oId) const;

        // Save
        virtual void SaveObject(const component::ObjectId& oId, VirtualFileFactory::ostream_t ostream) const;
        virtual void SaveComponent(const component::InstanceId& iId, VirtualFileFactory::ostream_t ostream) const;

        // Load
        virtual bool LoadObject(const component::ObjectId& oId, VirtualFileFactory::istream_t istream);
        virtual IComponent *LoadComponent(const component::ObjectId& oId, VirtualFileFactory::istream_t istream, const std::string& name);

        // Messages
        virtual void SubscribeToMessageType(const component::ComponentId& tId, guid_t msgType);
        virtual void PostEvent(const component::ObjectId& oid, const Message &msg);
        virtual void PostEvent(const component::ObjectId& oid, Message &msg);
        virtual void BroadcastEvent(const Message &msg);
        virtual void BroadcastEvent(Message &msg);
        virtual component::ObjectId FindObjectByName(const std::string& objectName);

        //! This is internal functions to Objects library
        static ObjectManagerDB& GetDB();

    private:
        void OnTick(Message& msg);
        void OnShutdown(Message& msg);

        //! Does exactly what it says on the tin.
        void destroyAllComponents();

        //! This is called from Tick and it's actually destroys the component
        void DestroyComponent(const component::InstanceId& iId);

        //! Checks for the existence of an object (one or more component instances with the given object id)
        bool ObjectExists(const component::ObjectId& oId) const;

        //! Return reference to instance data, so OM can manipulate it as needed
        Instance& instance(const component::ObjectId& id) const;

        //! This allows to save object to ostream with different name.
        //! \param newName if empty() use existing object name, otherwise use this as new objectname embedded into stream
        //! The oId will be unchanged in either way
        //! \todo Allow arbitrary name change for any object.
        void saveObject(const component::ObjectId& oId, VirtualFileFactory::ostream_t ostream, const std::string& newName) const;

        //! Our internal database of created components
        boost::scoped_ptr<ObjectManagerDB> mpDB;

        ReverseHashLookup<component::ComponentId> mComponentTypeToName;

        //! Since we destroy object later (poor mans GC)
        //! we need to lock list before we remove or add items to it.
        wxCriticalSection mDestroyListLocker;

        //! In most cases we need to lock our access to DB when we manipulating it
        mutable wxCriticalSection mDBLocker;

        //! Used to keep which components will be destroyed on the next tick
        std::vector<component::InstanceId> mObjectsToDestroyList;
        //! Used when we are shutting down and do not want to send ComponentDelete message.
        bool mbShuttingDown;

        //! List of all current objects. This is rebuild only when EnumObjects() method is called
        //! and component was created or deleted since last time call to EnumObjects().
        mutable std::vector<component::ObjectId> mCurrentObjectCollection;

        typedef boost::shared_ptr<Instance> Instance_t;
        mutable std::map<component::ObjectId, Instance_t> mInstanceList;

        //! konsole command handlers
        std::string konsole_components(const std::vector<std::string>& parameters, config::KosoleResult& outResult);
        std::string konsole_objects(const std::vector<std::string>& parameters, config::KosoleResult& outResult);
        std::string konsole_property(const std::vector<std::string>& parameters, config::KosoleResult& outResult);

        Dispatcher& mDispatcher;
        config::Konsole& mKonsole;

    };


} // namespace eg


#endif // OBJECT_MANAGER_H