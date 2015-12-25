#include "Precompiled.h"
#include "ObjectManager.h"
#include "ObjectManagerDB.h"
#include "ObjectManagerKonsole.h"
#include "ComponentIterators.h"
#include "Config/ConfigHelper.h"
#include "IComponent.h"
#include "Message/Dispatcher.h"
#include "Registrate.h"
#include "ObjectMessage.h"
#include "File/VirtualFileSystem.h"
#include "File/ArchiveHelpers.h"
#include "Config/Console.h"
#include "StringHelper.h"
#include <wx/confbase.h>
#include <wx/debug.h>
#include <wx/dynload.h>
#include <wx/thread.h>
#include <vector>
#include <boost/bind.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/format.hpp>


namespace {

    const char *kKonsoleComponent = "component";
    const char *kKonsoleObject = "object";
    const char *kKonsoleProperty = "property";

    //! Used to group all registered components,
    const char *kTemplateObjectName = "ComponentTypes";
    const eg::component::ObjectId kTemplateObjectId(kTemplateObjectName);

    //! This holds object info,
    //! like unique id and name
    //! This structure is read only to clients.
    struct Instance : public eg::ObjectInstance
    {
        Instance(const std::string& name, const eg::component::ObjectId& id)
        : eg::ObjectInstance(localName, localId)
        {
            localName = name;
            localId = id;
        }

        std::string localName;
        eg::component::ObjectId localId;
    };

    const uint32_t kArchiveObjectVersion = 1;
    const uint32_t kArchiveComponentVersion = 1;


    //! Return unique cloned name based on concentation of current oId name and incremented id number
    std::string getClonedName(const eg::component::ObjectId& oId, const eg::ObjectManager& /*om*/)
    {
        using namespace eg;
        //! This provides options for naming cloned objects, if user does not
        //! provide new name. Cloned name will be oId + next_int_id,
        // where next_int_id is incremented for every use for this oId.
        static std::map<component::ObjectId, uint32_t> clonnedNameCounters;

        if (clonnedNameCounters.find(oId) == clonnedNameCounters.end())
        {
            // there not e record of this object id yet as to regard to cloning yet.
            clonnedNameCounters.insert(std::make_pair(oId, 0));
        }

        std::string cloneName = component::objectName(oId) + "_" + boost::lexical_cast<std::string>(clonnedNameCounters[oId]++);
        return cloneName;
    }

} // namespace


namespace eg {


ObjectManager::ObjectManager(Dispatcher& dispatcher, config::Konsole& konsole)
: mbShuttingDown(false)
, mpDB(new ObjectManagerDB)
, mDispatcher(dispatcher)
, mKonsole(konsole)
{
    wxLogTrace(TR_OBJECTS, "ObjectManager Created.");

    mDispatcher[mtype::kFrameTick].connect(boost::bind(&ObjectManager::OnTick, this, _1));
    mDispatcher[mtype::kShutdownBegin].connect(boost::bind(&ObjectManager::OnShutdown, this, _1));

    mKonsole.AddItem(kKonsoleComponent, boost::bind(&ObjectManager::konsole_components, this, _1, _2));
    mKonsole.AddItem(kKonsoleObject, boost::bind(&ObjectManager::konsole_objects, this, _1, _2));
    mKonsole.AddItem(kKonsoleProperty, boost::bind(&ObjectManager::konsole_property, this, _1, _2));
}


ObjectManager::~ObjectManager()
{
	mpDB.reset();
    wxLogTrace(TR_OBJECTS, "ObjectManager Deleted.");
}


void ObjectManager::OnShutdown(Message& /*msg*/)
{
    wxLogTrace(TR_OBJECTS, "ObjectManager kShutdownBegin.");

	mDispatcher[mtype::kFrameTick].disconnect(boost::bind(&ObjectManager::OnTick, this, _1));
	mDispatcher[mtype::kShutdownBegin].disconnect(boost::bind(&ObjectManager::OnShutdown, this, _1));

	mKonsole.DeleteItem(kKonsoleComponent);
	mKonsole.DeleteItem(kKonsoleObject);
	mKonsole.DeleteItem(kKonsoleProperty);

	if (mpDB && !mbShuttingDown)
	{
        mbShuttingDown = true;
        destroyAllComponents();
        // we need to call Tick last time to trigger our GC
        Message msg(mtype::kFrameTick, 0.0f);
        OnTick(msg);
    }
}


void ObjectManager::OnTick(Message& /*msg*/)
{
    //float deltaTime = msg.GetValue<float>();
    std::vector<component::InstanceId> objectsToDestroyList;
    {
        // we only will lock to grab the objects and clear the list
        wxCriticalSectionLocker locker(mDestroyListLocker);
        objectsToDestroyList = mObjectsToDestroyList;
        mObjectsToDestroyList.clear();
    }

    // \todo We should add timing for each deletion
    // and when exceed some max time, stop and put remaining
    // objects into destroy list.
    if (!objectsToDestroyList.empty())
    {
        typedef std::vector<component::InstanceId>::iterator gc_it;
        // we have some objects to destroy it, let's do it
        for (gc_it it = objectsToDestroyList.begin(); it != objectsToDestroyList.end(); ++it)
        {
            DestroyComponent(*it);
        }
    }
}


void ObjectManager::destroyAllComponents()
{
    wxCriticalSectionLocker locker(mDBLocker);
    const std::set<component::ComponentId>& componentTypes = mpDB->GetComponentTypes();
    for (std::set<component::ComponentId>::const_iterator it_ct = componentTypes.begin(); it_ct != componentTypes.end(); ++it_ct)
    {
        const ComponentMap_t& conponentInstanceList = mpDB->GetComponentInstanceList(*it_ct);
        typedef ComponentMap_t::const_iterator cit_cm;
        for (cit_cm it_i = conponentInstanceList.begin(); it_i != conponentInstanceList.end(); ++it_i)
        {
            IComponent *pComp = (*it_i).second;
            // do not print out component types template objects
            if (kTemplateObjectId != pComp->OI().Id)
            {
                wxLogWarning("Component {%s} of object %d was not destroyed.", component::instanceName(pComp).c_str(), pComp->OI().Id.GetValue());
            }
            DestroyComponent(pComp);
        }
    }
}


// -----------------------------------------------------------------------------------------------
// Enums methods
void ObjectManager::EnumObjectsByType(const component::ComponentId& cid, EnumComponentCallback& componentCallback) const
{
    if (componentCallback.OnBegin())
    {
        ComponentByTypeIterator iter(cid);
        while (!iter.IsDone())
        {
            if (!componentCallback.OnComponent(iter.GetComponent()))
            {
                break;
            }

            ++iter;
        }

        componentCallback.OnEnd();
    }
}


void ObjectManager::EnumObjectsByObject(const component::ObjectId& oid, EnumComponentCallback& componentCallback) const
{
    if (componentCallback.OnBegin())
    {
        ComponentByObjectIterator iter(oid);
        while (!iter.IsDone())
        {
            if (!componentCallback.OnComponent(iter.GetComponent()))
            {
                break;
            }

            ++iter;
        }

        componentCallback.OnEnd();
    }
}


void ObjectManager::EnumObjectsByAll(EnumComponentCallback& componentCallback) const
{
    if (componentCallback.OnBegin())
    {
        wxCriticalSectionLocker locker(mDBLocker);
        std::set<component::ObjectId> objectIdList;
        mpDB->GetAllObjectIDs(objectIdList);
        for (std::set<component::ObjectId>::const_iterator it = objectIdList.begin(); it != objectIdList.end(); ++it)
        {
            ComponentByObjectIterator iter(*it);
            while (!iter.IsDone())
            {
                if (!componentCallback.OnComponent(iter.GetComponent()))
                {
                    break;
                }

                ++iter;
            }
        }

        componentCallback.OnEnd();
    }
}


IComponent *ObjectManager::FindComponentInstance(const component::InstanceId& iId) const
{
    if (mpDB)
    {
        wxCriticalSectionLocker locker(mDBLocker);
        return mpDB->FindComponentInstance(iId);
    }

    return 0;
}


const std::vector<component::ObjectId>& ObjectManager::EnumObjects() const
{
    if (mpDB)
    {
        if (mCurrentObjectCollection.empty())
        {
            std::set<component::ObjectId> objectIdList;
            mpDB->GetAllObjectIDs(objectIdList);

            mCurrentObjectCollection.resize(objectIdList.size());
            std::copy(objectIdList.begin(), objectIdList.end(), mCurrentObjectCollection.begin());
        }
    }

    return mCurrentObjectCollection;
}

// Enums methods
// -----------------------------------------------------------------------------------------------


// -----------------------------------------------------------------------------------------------
// Create methods
IComponent *ObjectManager::CreateComponent(const component::InstanceId& iId, const std::string& name)
{
    if (!mpDB->IsComponentTypeValid(iId.second))
    {
        wxLogWarning("Component '%d' is not valid for object '%s'.", iId.second, name.c_str());
        // Don't know this one
        return 0;
    }

    {
        wxCriticalSectionLocker locker(mDestroyListLocker);
        if (std::find(mObjectsToDestroyList.begin(), mObjectsToDestroyList.end(), iId) != mObjectsToDestroyList.end())
        {
            wxLogWarning("Creating component %s of object %d which already is in DestroyList for this Tick.", component::instanceName(iId).c_str(), iId.first.GetValue());
            // remove it from delete list, since this could have pappend in one tick here
            mObjectsToDestroyList.erase(std::remove(mObjectsToDestroyList.begin(), mObjectsToDestroyList.end(), iId), mObjectsToDestroyList.end());
            DestroyComponent(iId);
        }
    }


    wxCriticalSectionLocker locker(mDBLocker);
    IComponent *pComp = 0;
    if ((pComp = mpDB->FindComponentInstance(iId)) == false)
    {
        bool bNewObject = !IsObjectExists(iId.first);
        if (name.empty() && bNewObject)
        {
            // if passed name is blank, let's use object iId name and convert it into
            // hex string (64bit) but only if this is enterly new object
            uint64_t objectId = iId.first.GetValue();
            std::string hexIdNumber = boost::str(boost::format("0x%016X") % objectId);
            instance(iId.first).localName = hexIdNumber;
            wxLogWarning("Generated new name '%s'.", hexIdNumber.c_str());
        }
        else if (!name.empty())
        {
            // This allows us to change object name at any time
            instance(iId.first).localName = name;
        }

        // let's send new object if there is not even one component created yet.
        if (bNewObject)
        {
            Message(mtype::kObjectNew, iId.first).Send();
            //! if we create entire new object, clear current collection
            //! so it could be rebuild on call to EnumObjects() method.
            mCurrentObjectCollection.clear();
        }

        if ((pComp = mpDB->CreateComponent(iId)) != 0)
        {
            // this gets executed only if we don't have component already created
            // so we need to add it to our database.
            mpDB->AddComponent(iId.first, pComp);
            // Send event to all components belonging to this objectId including
            // this new created component
            PostEvent(pComp->OI().Id, Message(mtype::kComponentNew, component::getId(pComp)));
            // and we also want to sent global event on kComponentNew channel advertising
            // creation. PostEvent is quick operation, where this global event might be more expensive
            // if there are quite bit listeners.
            Message(mtype::kComponentNew, component::getId(pComp)).Send();

            wxLogTrace(TR_OBJECTS, "Creating Component '%s' for Object '%s'.", component::componentName(iId).c_str(), component::objectName(iId).c_str());
        }
    }

    return pComp;
}


bool ObjectManager::CloneInstance(const component::InstanceId& sourceId, const component::ObjectId& targetId, const std::string& targetName)
{
    VirtualFileSystem& vfs = REGISTRATE(VirtualFileSystem);
    if (sourceId.second.IsValid())
    {
        // we just clone this particular component
        if (VirtualFileFactory::ostream_t ostream = vfs.AttachFileStream("cloned.scratch"))
        {
            // save component data
            SaveComponent(sourceId, ostream);
        }

        if (VirtualFileFactory::istream_t istream = vfs.GetFileStream("cloned.scratch"))
        {
            // load it back up into new component
            return LoadComponent(targetId, istream, targetName) != 0;
        }
    }
    else
    {
        if (VirtualFileFactory::ostream_t ostream = vfs.AttachFileStream("cloned.scratch"))
        {
            // this will save entire object (all components compromising this object
            saveObject(sourceId.first, ostream, targetName);
        }
        if (VirtualFileFactory::istream_t istream = vfs.GetFileStream("cloned.scratch"))
        {
            instance(targetId).localName = targetName.empty() ? getClonedName(sourceId.first, *this) : targetName;
            // load it back up into new object
            return LoadObject(targetId, istream);
        }
    }

    return false;
}
// Create methods
// -----------------------------------------------------------------------------------------------


// -----------------------------------------------------------------------------------------------
// Destroy methods
void ObjectManager::DestroyComponent(const component::InstanceId& iId)
{
    wxCriticalSectionLocker locker(mDBLocker);
    IComponent *pComp = mpDB->FindComponentInstance(iId);
    wxASSERT(pComp);
    if (pComp)
    {
        wxLogTrace(TR_OBJECTS, "GC: Destroying Component '%s' from Object '%s'.", component::objectName(iId).c_str(), component::componentName(iId).c_str());
        if (!mbShuttingDown)
        {
            PostEvent(pComp->OI().Id, Message(mtype::kComponentDelete, component::getId(pComp)));
            Message(mtype::kComponentDelete, component::getId(pComp)).Send();
        }
        pComp->Shutdown();
        mpDB->DestroyComponent(pComp);

        // if there is no more any components left, send this message
        if (!mbShuttingDown && !IsObjectExists(iId.first))
        {
            Message(mtype::kObjectDelete, iId.first).Send();
            //! Entire object is going aways, clear current collection
            //! so it could be rebuild on call to EnumObjects() method.
            mCurrentObjectCollection.clear();
        }
    }
}


void ObjectManager::DestroyComponent(IComponent *pComp)
{
    wxASSERT(pComp);
    // we are going to add this component to destroy list
    // so we can delete it on the next tick
    wxCriticalSectionLocker locker(mDestroyListLocker);

    component::InstanceId iId(component::getId(pComp));

    // only add component if it's not already in delete list
    if (std::find(mObjectsToDestroyList.begin(), mObjectsToDestroyList.end(), iId) == mObjectsToDestroyList.end())
    {
        wxLogTrace(TR_OBJECTS, "GC: Adding Component {%s} to GC.", component::instanceName(pComp).c_str());
        mObjectsToDestroyList.push_back(iId);
    }
    else
    {
        wxLogWarning("GC: Component {%s} already is in GC list, ignoring.", component::instanceName(pComp).c_str());
    }
}

void ObjectManager::DestroyInstance(const component::ObjectId& oid)
{
    ObjectListCallback componentList(oid);
    for (ObjectListCallback::List::const_iterator it = componentList.ComponentList.begin(); it != componentList.ComponentList.end(); ++it)
    {
        DestroyComponent(*it);
    }
}

// Destroy methods
// -----------------------------------------------------------------------------------------------


// -----------------------------------------------------------------------------------------------
// Message methods
void ObjectManager::SubscribeToMessageType(const component::ComponentId& ctid, guid_t mtid)
{
    mpDB->SubscribeToMessageType(ctid, mtid);
}


void ObjectManager::PostEvent(const component::ObjectId& oid, const Message& msg)
{
    PostEvent(oid, *const_cast<Message *>(&msg));
}


// maybe all messages should be const?
void ObjectManager::PostEvent(const component::ObjectId& oid, Message &msg)
{
    mpDB->PostEvent(oid, msg);
}


void ObjectManager::BroadcastEvent(const Message& msg)
{
    BroadcastEvent(*const_cast<Message *>(&msg));
}


void ObjectManager::BroadcastEvent(Message& msg)
{
    mpDB->BroadcastEvent(msg);
}
// Message methods
// -----------------------------------------------------------------------------------------------


// -----------------------------------------------------------------------------------------------
// Utilities methods
void ObjectManager::RegisterComponentType(const component::ComponentId& cId, ComponentCreationMethod pCreationFn, ComponentDestructionMethod pDestructionFn, const wxChar *pReadableName)
{
    mComponentTypeToName.Add(pReadableName, cId);
    mpDB->RegisterComponentType(cId, pCreationFn, pDestructionFn);

    // now, wee need to create component to use as a template
    //CreateComponent(component::InstanceId(kTemplateObjectId, cId), kTemplateObjectName);
}


bool ObjectManager::IsObjectExists(const component::ObjectId& oId) const
{
    wxCriticalSectionLocker locker(mDBLocker);
    return mpDB->IsObjectExists(oId);
}


std::string ObjectManager::GetComponentName(const component::ComponentId& cId) const
{
    return mComponentTypeToName.GetName(cId);
}


const std::set<component::ComponentId>& ObjectManager::GetComponentTypes() const
{
    return mpDB->GetComponentTypes();
}


ObjectManagerDB& ObjectManager::GetDB()
{
    static IObjectManager& objectManager = REGISTRATE(IObjectManager);
    return *(static_cast<ObjectManager&>(objectManager).mpDB);
}


Instance& ObjectManager::instance(const component::ObjectId& id) const
{
    // this is the only place we create ObjectInstance data
    // so, let' smake sure that id key match to ObjectInstance member
    std::map<component::ObjectId, Instance_t>::iterator it = mInstanceList.find(id);
    if (it == mInstanceList.end())
    {
        // let's create new entry with id converted into string to use as a initial name
        Instance_t newInstance(new Instance(boost::lexical_cast<std::string>(id), id));
        it = mInstanceList.insert(std::make_pair(id, newInstance)).first;
    }

    return *(*it).second.get();
}


const ObjectInstance& ObjectManager::OI(const component::ObjectId& id) const
{
    return instance(id);
}
// Utilities methods
// -----------------------------------------------------------------------------------------------



// -----------------------------------------------------------------------------------------------
// Load/Save methods
bool ObjectManager::LoadObject(const component::ObjectId& oId, VirtualFileFactory::istream_t istream)
{
    if (istream)
    {
        stream::LoadMarker loadMarker(*istream);

        uint32_t versionNumber = 0;
        std::string name;
        size_t numComponents = 0;
        (*istream) >> versionNumber;

        if (versionNumber == 1)
        {
            (*istream) >> numComponents >> name;
        }
        else
        {
            // this version of archive stream is not supported yet
            wxASSERT(0);
            wxLogError("Version load number %d is not supported.", versionNumber);
        }

        for (size_t i = 0; i < numComponents; i++)
        {
            LoadComponent(oId, istream, name);
        }
    }

    return IsObjectExists(oId);
}


void ObjectManager::SaveObject(const component::ObjectId& oId, VirtualFileFactory::ostream_t ostream) const
{
    saveObject(oId, ostream, "");
}


void ObjectManager::saveObject(const component::ObjectId& oId, VirtualFileFactory::ostream_t ostream, const std::string& newName) const
{
    wxASSERT(ostream);
    if (ostream)
    {
        stream::SaveMarker saveMarker(*ostream);

        // \todo Add signature here so when we load it it can be somewhat verified that is correct stream data
        ObjectListCallback objectList(oId);
        std::string objectName = newName.empty() ? OI(oId).Name : newName;
        (*ostream) << " " << kArchiveObjectVersion << " " << objectList.ComponentList.size() << " " << objectName;

        for (std::vector<IComponent *>::const_iterator it = objectList.ComponentList.begin(); it != objectList.ComponentList.end(); ++it)
        {
            component::InstanceId iId(oId, (*it)->GetComponentTypeId());
            SaveComponent(iId, ostream);
        }
    }
}

IComponent *ObjectManager::LoadComponent(const component::ObjectId& oId, VirtualFileFactory::istream_t istream, const std::string& name)
{
    if (istream)
    {
        stream::LoadMarker loadMarker(*istream);

        uint32_t vesrionNumber = 0;
        component::ComponentId cId;
        (*istream) >> vesrionNumber;

        if (vesrionNumber == 1)
        {
            (*istream) >> cId;
        }
        else
        {
            // this version of archive stream is not supported yet
            wxASSERT(0);
        }

        component::InstanceId iId(oId, cId);

        if (IComponent *pComponent = CreateComponent(iId, name))
        {
            IPropertySet& propSet = pComponent->GetPropertyData();
            eg::LoadFromArchive<boost::archive::text_iarchive>(propSet, *istream);
            return pComponent;
        }
    }

    return 0;
}


void ObjectManager::SaveComponent(const component::InstanceId& iId, VirtualFileFactory::ostream_t ostream) const
{
    wxASSERT(ostream);
    if (ostream)
    {
        stream::SaveMarker saveMarker(*ostream);

        if (IComponent *pComponent = FindComponentInstance(iId))
        {
            (*ostream) << " " << kArchiveComponentVersion << " " << iId.second;
            const IPropertySet& propSet = pComponent->GetPropertyData();
            eg::SaveToArchive<boost::archive::text_oarchive>(propSet, *ostream);
        }
    }
}
// Load/Save methods
// -----------------------------------------------------------------------------------------------

component::ObjectId ObjectManager::FindObjectByName(const std::string& objectName)
{
    for (std::map<component::ObjectId, Instance_t>::const_iterator it =  mInstanceList.begin(); it != mInstanceList.end(); ++it)
    {
        if ((*it).second->Name == objectName)
        {
            return (*it).first;
        }
    }

    return Hash();
}


std::string ObjectManager::konsole_objects(const std::vector<std::string>& parameters, config::KosoleResult& outResult)
{
    wxASSERT(boost::icontains(parameters[0], kKonsoleObject));
    if (parameters.size() > 1)
    {
        if (boost::algorithm::iequals(parameters[1], std::string("help")))
        {
            std::string resultString = "Valid subcommands for '" + parameters[0] + "' are:";
            resultString += "\n\tlist [-h] = (List all current objects, show id's as hex value if -h)";
            resultString += "\n\tshow <object_id|object_name> = (Show all components for object_id or object_name)";

            outResult = config::krHelp;
            return resultString;
        }
        else if (boost::algorithm::iequals(parameters[1], std::string("list")))
        {
            bool bShowAsHex = false;
            if (parameters.size() > 2)
            {
                bShowAsHex = boost::algorithm::iequals(parameters[2], std::string("-h"));
            }

            typedef std::vector<component::ObjectId> ObjectCollection_t;
            const ObjectCollection_t& objectCollection = EnumObjects();

            std::string resultString = "Current Objects:";
            for (ObjectCollection_t::const_iterator it = objectCollection.begin(); it != objectCollection.end(); ++it)
            {
                uint64_t objectId = (*it).GetValue();
                std::string objectIdString = bShowAsHex ? (boost::str(boost::format("0x%016X") % objectId)) : (boost::str(boost::format("% 21d") % objectId));
                resultString += "\n\t" + objectIdString + " " + OI(*it).Name;
            }

            outResult = config::krOk;
            return resultString;
        }
        else if (boost::algorithm::iequals(parameters[1], std::string("show")) && parameters.size() > 2)
        {
            // first we'll treat second parameter as object_id (ether hex or decimal)
            // and then try to use it as a name
            component::ObjectId objectId = config::ResolveObjectId(parameters[2]);
            // there could no object at all for this id, but we'll catch the below

            ObjectListCallback objectComponents(objectId);
            if (objectComponents.ComponentList.empty())
            {
                outResult = config::krError;
                return "'" + parameters[2] + "' object does not exist.";
            }

            std::string resultString = "Current components for '" + parameters[2] + "':";
            for (ObjectListCallback::List::const_iterator it = objectComponents.ComponentList.begin(); it != objectComponents.ComponentList.end(); ++it)
            {
                resultString += "\n\t" + GetComponentName((*it)->GetComponentTypeId());
            }

            outResult = config::krOk;
            return resultString;
        }
    }

    outResult = config::krError;
    return "Command '" + parameters[0] + "' does not have valid syntax/parameters.";
}


std::string ObjectManager::konsole_components(const std::vector<std::string>& parameters, config::KosoleResult& outResult)
{
    wxASSERT(boost::icontains(parameters[0], kKonsoleComponent));
    if (parameters.size() > 1)
    {
        // there are several subcommands for component command
        if (boost::algorithm::iequals(parameters[1], std::string("help")))
        {
            std::string resultString = "Valid subcommands for '" + parameters[0] + "' are:";
            resultString += "\n\tnew <component_type> <object_id> [object_name] = (Create new component)";
            resultString += "\n\tdelete <component_type> <object_id|object_name> = (Delete existing component)";
            resultString += "\n\ttypes = (List registered component types)";
            resultString += "\n\tshow <component_type> <object_id|object_name> = (Show Component details)";

            outResult = config::krHelp;
            return resultString;
        }
        else if (boost::algorithm::iequals(parameters[1], std::string("new")) && parameters.size() > 3)
        {
            config::NewComponent_t params = config::ParseNewComponent(parameters[2], parameters[3], parameters.size() > 4 ? parameters[4] : "");

            std::string resultString = "Component '" + parameters[2] + "' for Object '" + boost::str(boost::format("0x%016X") % params.get<0>().first.GetValue()) + "'";
            if (IComponent *pComponent = CreateComponent(params.get<0>(), params.get<1>()))
            {
                outResult = config::krOk;
                resultString += " with '" + pComponent->OI().Name + "' name was created successfully.";
            }
            else
            {
                outResult = config::krError;
                resultString += " was not created successfully.";
            }

            return resultString;
        }
        else if (boost::algorithm::iequals(parameters[1], std::string("delete")) && parameters.size() > 3)
        {
            if (IComponent *pComponent = config::FindComponent(parameters[2], parameters[3]))
            {
                DestroyComponent(pComponent);
                outResult = config::krInfo;
                return "Component '" + parameters[2] + "' for Object '" + parameters[3] + "' was destroyed.";
            }

            outResult = config::krError;
            return "There is no Component '" + parameters[2] + "' for Object '" + parameters[3] + "'.";
        }
        else if (boost::algorithm::iequals(parameters[1], std::string("types")))
        {
            std::string resultString = "Valid types:";

            const std::set<component::ComponentId>& componentTypes = GetComponentTypes();
            for (std::set<component::ComponentId>::const_iterator it = componentTypes.begin(); it != componentTypes.end(); ++it)
            {
                resultString += "\n\t" + GetComponentName(*it);
            }

            outResult = config::krInfo;
            return resultString;
        }
        else if (boost::algorithm::iequals(parameters[1], std::string("show")) && parameters.size() > 3)
        {
            if (IComponent *pComponent = config::FindComponent(parameters[2], parameters[3]))
            {
                // we have component here, let's iterate over all properties and display current values
                std::string resultString = "Property values of Component '" + parameters[2] + "' for Object '" + pComponent->OI().Name + "':";

                IPropertySet& propSet = pComponent->GetPropertyData();
                resultString += "\n\tGroup '" + propSet.GetGroupName() + "'";

                IProperty *pProp = propSet.GetFirstProperty();
                while (pProp)
                {
                    resultString += "\n\t  Name: '" + pProp->GetName() + "'";
                    resultString += "\n\t    Value: '" + pProp->GetValue() + "'";

                    pProp = propSet.GetNextProperty();
                }

                outResult = config::krInfo;
                return resultString;
            }

            outResult = config::krError;
            return "There is no Component '" + parameters[2] + "' for Object '" + parameters[3] + "'.";
        }
    }

    outResult = config::krError;
    return "Command '" + parameters[0] + "' does not have valid syntax/parameters.";
}


std::string ObjectManager::konsole_property(const std::vector<std::string>& parameters, config::KosoleResult& outResult)
{
    wxASSERT(boost::icontains(parameters[0], kKonsoleProperty));
    if (parameters.size() > 1)
    {
        // there are several subcommands for component command
        if (boost::algorithm::iequals(parameters[1], std::string("help")))
        {
            std::string resultString = "Valid subcommands for '" + parameters[0] + "' are:";
            resultString += "\n\tget <component_type> <object_id|object_name> <property_name> = (Get current property value)";
            resultString += "\n\tset <component_type> <object_id|object_name> <property_name> \"new_value\" = (Set new property value)";

            outResult = config::krHelp;
            return resultString;
        }
        else if (boost::algorithm::iequals(parameters[1], std::string("get")) && parameters.size() > 4)
        {
            if (IComponent *pComponent = config::FindComponent(parameters[2], parameters[3]))
            {
                IPropertySet& propSet = pComponent->GetPropertyData();
                if (IProperty *pProperty = propSet.FindProperty(parameters[4]))
                {
                    std::string resultString = "Property '" + parameters[4] + "' value of Component '" + parameters[2] + "' for Object '" + pComponent->OI().Name + "':";
                    resultString += "\n\t'" + pProperty->GetValue() + "'.";

                    outResult = config::krInfo;
                    return resultString;
                }
            }

            outResult = config::krError;
            return "There is no Property '" + parameters[4] + "' for Instance '" + parameters[3] + ":" + parameters[2] + "''";
        }
        else if (boost::algorithm::iequals(parameters[1], std::string("set")) && parameters.size() > 5)
        {
            if (IComponent *pComponent = config::FindComponent(parameters[2], parameters[3]))
            {
                IPropertySet& propSet = pComponent->GetPropertyData();
                if (IProperty *pProperty = propSet.FindProperty(parameters[4]))
                {
                    pProperty->SetValue(parameters[5]);
                    std::string resultString = "New Property '" + parameters[4] + "' value of Component '" + parameters[2] + "' for Object '" + pComponent->OI().Name + "':";
                    resultString += "\n\t'" + pProperty->GetValue() + "'.";

                    outResult = config::krInfo;
                    return resultString;
                }
            }

            outResult = config::krError;
            return "There is no Property '" + parameters[4] + "' for Instance '" + parameters[3] + ":" + parameters[2] + "''";
        }
    }

    outResult = config::krError;
    return "Command '" + parameters[0] + "' does not have valid syntax/parameters.";
}

} // namespace eg

