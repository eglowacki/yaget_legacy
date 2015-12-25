#include "Precompiled.h"
#include "ObjectManagerDB.h"
#include "IComponent.h"
#include "ObjectMessage.h"


namespace eg {


void ObjectManagerDB::RegisterComponentType(const component::ComponentId& compID, ComponentCreationMethod pCreationFn, ComponentDestructionMethod pDestructionFn)
{
    std::map<component::ComponentId, ComponentTypeInfo>::const_iterator it = mComponentTypeFactory.find(compID);
    if (it != mComponentTypeFactory.end())
    {
        // this component already exist, but first let's check if this
        // component type is the same instance or completely new
        if ((*it).second.mCreationMethod == pCreationFn && (*it).second.mDestructionMethod == pDestructionFn)
        {
            wxLogError("Component %d is already registered, Ignoring.", 5);//compID.GetValue());
            return;
        }

        wxLogWarning("Component %x is already registered but new Instance for this ComponentType is beying registered.", compID.GetValue());
    }

    mComponentTypeFactory[compID].mCreationMethod = pCreationFn;
    mComponentTypeFactory[compID].mDestructionMethod = pDestructionFn;

    // and update our list of component types
    mComponentTypeList.insert(compID);
}


IComponent *ObjectManagerDB::CreateComponent(const component::InstanceId& iId)
{
    wxASSERT(mComponentTypeFactory.find(iId.second) != mComponentTypeFactory.end());
    wxASSERT(mComponentTypeFactory[iId.second].mCreationMethod);

    IComponent *pComp = mComponentTypeFactory[iId.second].mCreationMethod(iId.first);
    return pComp;
}


void ObjectManagerDB::DestroyComponent(IComponent *pComp)
{
    wxASSERT(pComp);
    wxASSERT(mComponentTypeFactory.find(pComp->GetComponentTypeId()) != mComponentTypeFactory.end());

    // before we delete this component we need to remove it from our instance list
    std::map<component::ComponentId, ComponentMap_t>::iterator it = mComponentInstanceList.find(pComp->GetComponentTypeId());
    wxASSERT(it != mComponentInstanceList.end());
    if (it != mComponentInstanceList.end())
    {
        ComponentMap_t& conponentInstanceList = (*it).second;
        conponentInstanceList.erase(pComp->OI().Id);
    }

    mComponentTypeFactory[pComp->GetComponentTypeId()].mDestructionMethod(pComp);
}


bool ObjectManagerDB::IsObjectExists(const component::ObjectId& oId) const
{
    typedef std::map<Hash, ComponentMap_t>::const_iterator cit_CM;
    for (cit_CM it = mComponentInstanceList.begin(); it != mComponentInstanceList.end(); ++it)
    {
        const ComponentMap_t &compMap = (*it).second;
        ComponentMap_t::const_iterator compMapIter = compMap.find(oId);
        if (compMapIter != compMap.end())
        {
            // We found a component belonging to this object, so the object exists.
            return true;
        }
    }

    return false;
}


bool ObjectManagerDB::GetAllObjectIDs(std::set<component::ObjectId>& objectIdList) const
{
    typedef std::map<Hash, ComponentMap_t>::const_iterator cit_CM;
    for (cit_CM it = mComponentInstanceList.begin(); it != mComponentInstanceList.end(); ++it)
    {
        const ComponentMap_t &compMap = (*it).second;
        for (ComponentMap_t::const_iterator ob_it = compMap.begin(); ob_it !=compMap.end(); ++ob_it)
        {
            objectIdList.insert((*ob_it).first);
        }
    }

    return !objectIdList.empty();
}


IComponent *ObjectManagerDB::FindComponentInstance(const component::InstanceId& iId) const
{
    std::map<component::ComponentId, ComponentMap_t>::const_iterator it = mComponentInstanceList.find(iId.second);
    if (it != mComponentInstanceList.end())
    {
        const ComponentMap_t& componentMap = (*it).second;
        ComponentMap_t::const_iterator iti = componentMap.find(iId.first);
        if (iti != componentMap.end())
        {
            return (*iti).second;
        }
    }

    return 0;
}


bool ObjectManagerDB::AddComponent(const component::ObjectId& oId, IComponent *pComp)
{
    return (mComponentInstanceList[pComp->GetComponentTypeId()].insert(ComponentMap_t::value_type(oId, pComp))).second;
}


void ObjectManagerDB::SubscribeToMessageType(const component::ComponentId& ctid, guid_t mtid)
{
    mMessageTypeToComponentTypes[mtid].insert(ctid);
}


void ObjectManagerDB::PostEvent(const component::ObjectId& oId, Message &msg)
{
    typedef std::map<guid_t, std::set<component::ComponentId> > TypeToComponentTypes_t;
    TypeToComponentTypes_t::iterator it_comps = mMessageTypeToComponentTypes.find(msg.mType);
    if (it_comps != mMessageTypeToComponentTypes.end())
    {
        std::set<Hash>& compSet = (*it_comps).second;

        // Find the component types that subscribe to this message
        for (std::set<Hash>::iterator it = compSet.begin(); it != compSet.end(); ++it)
        {
            // get the component ID (Hash, like Entity, Model, etc)
            Hash cid = (*it);

            // Go through the components of this component type and send message
            for (ComponentByTypeIterator ci(cid); !ci.IsDone(); ++ci)
            {
                IComponent *pComp = ci.GetComponent();
                if (pComp->OI().Id == oId)
                {
                    pComp->HandleMessage(msg);
                }
            }
        }
    }
}


void ObjectManagerDB::BroadcastEvent(Message& msg)
{
    typedef std::map<guid_t, std::set<component::ComponentId> > TypeToComponentTypes_t;
    TypeToComponentTypes_t::iterator it_comps = mMessageTypeToComponentTypes.find(msg.mType);
    if (it_comps != mMessageTypeToComponentTypes.end())
    {
        std::set<Hash>& compSet = (*it_comps).second;

        // Find the component types that subscribe to this message
        for (std::set<Hash>::iterator it = compSet.begin(); it != compSet.end(); ++it)
        {
            // Found one
            Hash ctid = (*it);

            // Go through the components of this component type and send message
            for (ComponentByTypeIterator ci(ctid); !ci.IsDone(); ++ci)
            {
                ci.GetComponent()->HandleMessage(msg);
            }
        }
    }
}


const std::set<component::ComponentId>& ObjectManagerDB::GetComponentTypes() const
{
    return mComponentTypeList;
}


const ComponentMap_t& ObjectManagerDB::GetComponentInstanceList(const component::ComponentId& cId)
{
    static ComponentMap_t emptyComponentMap;
    std::map<Hash, ComponentMap_t>::const_iterator it = mComponentInstanceList.find(cId);
    if (it != mComponentInstanceList.end())
    {
        return (*it).second;
    }

    return emptyComponentMap;
}


bool ObjectManagerDB::IsComponentTypeValid(const component::ComponentId& cid) const
{
    return mComponentTypeFactory.find(cid) != mComponentTypeFactory.end();
}

} // namespace eg

