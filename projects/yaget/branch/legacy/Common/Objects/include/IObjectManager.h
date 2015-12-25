///////////////////////////////////////////////////////////////////////
// IObjectManager.h
//
//  Copyright 10/16/2005 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      ObjectManager interface
//
//
//  #include "IObjectManager.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef I_OBJECT_MANANGER_H
#define I_OBJECT_MANANGER_H
#pragma once

#include "ObjectID.h"
#include "ComponentEnumsObjects.h"
#include "File/VirtualFileFactory.h"
#include "MessageEnums.h"
#include "Registrate.h"
//#include "Plugin/IPluginObject.h"
//#include <wx/dynlib.h>
//#include <wx/confbase.h>
#include <set>
#include <vector>

namespace eg
{
    class Message;
    class IComponent;
    class ComponentByTypeIterator;
    class ObjectInstance;

    /*!
    This is used for EnumObjectsBy<...> methods
    */
    struct EnumComponentCallback
    {
        //! This is called for every enumerated component
        //! if this returns FALSE, stop enumerating, otherwise keep going
        virtual bool OnComponent(IComponent *pComponent) = 0;

        //! Called before first component is enumerated.
        //! If return FALSE, then do not enumerate any components.
        //! OnEnd() is always called.
        virtual bool OnBegin() {return true;}
        //! Called after all component enumeration is done
        virtual void OnEnd() {}
    };

    /*!
    Simple helper struct to be able to cast it to correct type
    when enumerating components.
    */
    template <typename T>
    struct EnumComponentSafeCallback : public EnumComponentCallback
    {
        typedef T CompType_t;

        virtual bool OnComponent(IComponent *pComponent)
        {
            return ProcessComponent(static_cast<T *>(pComponent));
        }

        virtual bool ProcessComponent(CompType_t *pComponent) = 0;
    };


    //! Helper typedefs for creation and destruction of components
    //! each component will recive it's object id on creation and
    //! will retain that id during it's lefetime.
    typedef IComponent* (*ComponentCreationMethod)(component::ObjectId);
    typedef bool (*ComponentDestructionMethod)(IComponent *);


    /*!
    Interface to our ObjectManager
    */
    class IObjectManager
    {
    public:
        //-------------------------------------------------------------------------------------
        // Enumarations
        //! Iterate trough all the components based on type
        //!
        //! \param cid Component Id we want to get from all current objects
        //! \param componentCallback [OUT] This will contain list of all current components.
        //! \note Make no assumptions concerning the order over which the objects are operated.
        virtual void EnumObjectsByType(const component::ComponentId& cid, EnumComponentCallback& componentCallback) const = 0;

        //! Iterate trough all the components based on object ID
        //!
        //! \param oid Object id we want to get all components for.
        //! \param componentCallback [OUT] This will contain list of all components associated with this Object Id.
        //! \note Make no assumptions concerning the order over which the objects are operated.
        virtual void EnumObjectsByObject(const component::ObjectId& oid, EnumComponentCallback& componentCallback) const = 0;

        //! This will enumerate all the components sorted by object Id
        //!
        //! \param componentCallback [OUT] This will contain list of all components for all current objects
        //!                                which are grouped by Object Id.
        virtual void EnumObjectsByAll(EnumComponentCallback& componentCallback) const = 0;

        //! Return all current object id's collection
        //!
        //! \return const std::vector<component::ObjectId>& Collection of all current Object Id's
        virtual const std::vector<component::ObjectId>& EnumObjects() const = 0;

        //-------------------------------------------------------------------------------------
        //@{
        //! Create
        //! Create component using specific iId, set's object name and load data from stream if one exist.
        //!
        //! \param iId unique id representing this instance of component
        //! \param name User supplied name for this object
        //! \param istream if not NULL, then load data from it, otherwise do nothing
        //!
        //! \return IComponent* if was sesesefully created, return pointer to it, otherwise NULL
        virtual IComponent *CreateComponent(const component::InstanceId& iId, const std::string& name) = 0;

        //! \param sourceId instance unique object instance Id for this instance
        //! \param targeId new unique object Id for cloned instance
        //! \param targetName User supplied name for this object
        //! \return TRUE, cloned object in it's entirety, otherwise FALSE
        virtual bool CloneInstance(const component::InstanceId& sourceId, const component::ObjectId& targeId, const std::string& targetName) = 0;
        bool CloneInstance(const component::ObjectId& oId, const component::ObjectId& targeId, const std::string& targetName)
        {
            using namespace component;
            return CloneInstance(InstanceId(oId, ComponentId()), targeId, targetName);
        }

        //! This will return component with instnace id iId, or NULL if it does not exist
        virtual IComponent *FindComponentInstance(const component::InstanceId& iId) const = 0;
        //@}

        //-------------------------------------------------------------------------------------
        //@{
        //! Destroy
        //! Destroy and remove this component from ObjectManager
        virtual void DestroyComponent(IComponent *pComp) = 0;
        virtual void DestroyInstance(const component::ObjectId& targeId) = 0;
        //@}

        //-------------------------------------------------------------------------------------
        //@{
        //! Utilities
        //! This needs to be called once for each component type. This sets up the class factory that creates a component instance
        virtual void RegisterComponentType(const component::ComponentId& cId, ComponentCreationMethod pCreationFn, ComponentDestructionMethod pDestructionFn, const char *pReadableName) = 0;

        //! Query methods to check if this object already exist
        virtual bool IsObjectExists(const component::ObjectId& oId) const = 0;

        //! Return ObjectInstance structure, This is used primary
        //! by components to return ObjectInstance, rather then story it
        virtual const ObjectInstance& OI(const component::ObjectId& id) const = 0;

        //! Helper methods to reverse hash id and object id into text names
        virtual std::string GetComponentName(const component::ComponentId& cId) const = 0;

        //! return list of all registered types of components
        virtual const std::set<component::ComponentId>& GetComponentTypes() const = 0;
        //@}

        //-------------------------------------------------------------------------------------
        //@{
        //! Save/Load
        //! Save entire object to a ostream
        virtual void SaveObject(const component::ObjectId& oId, VirtualFileFactory::ostream_t ostream) const = 0;

        //! Load entire object, will create all components and set name loaded from stream
        virtual bool LoadObject(const component::ObjectId& oId, VirtualFileFactory::istream_t istream)= 0;

        //! Save component to a stream
        virtual void SaveComponent(const component::InstanceId& iId, VirtualFileFactory::ostream_t ostream) const = 0;

        virtual IComponent *LoadComponent(const component::ObjectId& oId, VirtualFileFactory::istream_t istream, const std::string& name) = 0;
        //@}

        //-------------------------------------------------------------------------------------
        //@{
        //! Messages
        //! Sets up a subscription to a particular message type for the given component type. Whenever the message gets sent, it will
        //! go out to any components of the types that have subscribed to the message type.
        virtual void SubscribeToMessageType(const component::ComponentId& cId, guid_t msgType) = 0;

        //@{
        //Sends a message to all subscribing components of one object (as defined by the first parameter).
        virtual void PostEvent(const component::ObjectId& oid, const Message &msg) = 0;
        virtual void PostEvent(const component::ObjectId& oid, Message &msg) = 0;
        //@}

        //@{
        //Sends a message to all subscribing components of all objects
        virtual void BroadcastEvent(const Message &msg) = 0;
        virtual void BroadcastEvent(Message &msg) = 0;
        //@}
        //@}

        //! Return object id if Object with that name exist, otherwise id will be NotValid.
        virtual component::ObjectId FindObjectByName(const std::string& objectName) = 0;

        //! Validated that name is valid
        //! \note for now all it does is make sure that is not empty
        static bool ValidateName(const std::string& name)
        {
            return !name.empty();
        }

        static IObjectManager& Get()
        {
            return eg::registrate::ref_cast<eg::IObjectManager>("IObjectManager");
        }
    };

} // namespace eg

#endif // I_OBJECT_MANANGER_H

