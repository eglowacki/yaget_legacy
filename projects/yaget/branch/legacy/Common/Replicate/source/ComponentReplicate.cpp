#include "Precompiled.h"
#include "ComponentReplicate.h"
#include "Config/ConfigHelper.h"
#include "Timer/ClockUtilities.h"
#include "Plugin/ModuleHelper.h"
#include <wx/confbase.h>
#include <wx/statline.h>
#include <wx/thread.h>
#include <boost/bind.hpp>


namespace eg {

EG_MODULE_REGISTER_COMPONENT(ComponentReplicate);


ComponentReplicate::ComponentReplicate(component::ObjectId oId)
: ComponentHelper(oId, "Replicate", ComponentHelper::ccAttached)
, mReplicateRate(get_propV<float>(mProperty, "ReplicateRate", 10.0f, ""))
{
}


ComponentReplicate::~ComponentReplicate()
{
}


void ComponentReplicate::DeleteThis()
{
    delete this;
}


void ComponentReplicate::Shutdown()
{
    mReplicationTimer.reset();
}


void ComponentReplicate::onSendReplication(const Message& msg)
{
    msg;
    wxASSERT(msg.mType == mtype::kFrameTick);

    ChangedProperties_t pendingChanges;
    {
        wxCriticalSectionLocker locker(mCritSection);
        pendingChanges = mPendingProperties;
        mPendingProperties.clear();
    }

    // and here we'll start replicating all pending changes
    if (Transport && !pendingChanges.empty())
    {
        for (ChangedProperties_t::const_iterator it = pendingChanges.begin(); it != pendingChanges.end(); ++it)
        {
            const component::InstanceId& iId = (*it).first.first;
            const std::string& propName = (*it).second;

            Transport(iId, propName);
        }
    }
}


void ComponentReplicate::onPropertyChanged(const IProperty *pProperty)
{
    // we only save changed property id if this is not one of our property
    // and we have observers hooked to 'Transport'.
    if (Transport && pProperty->GetInstanceId() != mReplicateRate.GetInstanceId())
    {
        //! \todo add support for some kind of filtering
        //! which changes we are interested in
        component::InstanceId iId = pProperty->GetInstanceId();
        std::string propName = pProperty->GetName();

        wxCriticalSectionLocker locker(mCritSection);
        mPendingProperties[PropChangeKey_t(iId, Hash(propName.c_str()))] = propName;

        wxLogTrace(TR_REPLICATE, "Property '%s' changed.", propName.c_str());
    }
    else if (pProperty->GetInstanceId() == mReplicateRate.GetInstanceId())
    {
        // this is one of properties --------------------------------
        // user wants to change how often to send replication updates
        if (mReplicationTimer)
        {
            float freq = mReplicateRate;
            mReplicationTimer->SetFrequency(freq);
            //mReplicationTimer.reset(new IntervalTimer(boost::bind(&ComponentReplicate::onSendReplication, this, _1), freq));
        }
    }
}


Dispatcher::Result_t ComponentReplicate::HandleMessage(Message& msg)
{
    switch (msg.mType)
    {
        case mtype::kComponentNew:
            if (msg.Is<component::InstanceId>())
            {
                component::InstanceId sourceId = msg.GetValue<component::InstanceId>();

                // this message is generated by this object creation
                // so treat this as a initial setup (runs once)
                if (sourceId == component::getId(this))
                {
                    float freq = mReplicateRate;
                    mReplicationTimer.reset(new IntervalTimer(boost::bind(&ComponentReplicate::onSendReplication, this, _1), freq));

                    // this is us initiating message
                    // so let's grab all existing components for our object id
                    // and add us to listen for property changes
                    // \todo We need some kind of filter for which components are allowed to be replicated
                    //       Do we need to go as low as allowing to specify which properties?
                    //       Maybe, it's better to specify per component type (Entity, Area, etc), which properties are
                    //       replicable and which are not. This will assure that same components are replicated
                    //       the same way without any extra work.
                    ObjectListCallback components(OI().Id);
                    for (ObjectListCallback::List::const_iterator it = components.ComponentList.begin(); it != components.ComponentList.end(); ++it)
                    {
                        if (allowReplication((*it)->GetComponentTypeId()))
                        {
                            IPropertySet& propSet = (*it)->GetPropertyData();
                            propSet.AddNotifySlot(boost::bind(&ComponentReplicate::onPropertyChanged, this, _1), "Replicate");
                        }
                    }
                }
                else if (sourceId.first == OI().Id)
                {
                    // we only process components in the same instance us (same object id)
                    // check component id against our filter
                    if (allowReplication(sourceId.second))
                    {
                        IPropertySet& propSet = property::cast(sourceId);
                        propSet.AddNotifySlot(boost::bind(&ComponentReplicate::onPropertyChanged, this, _1), "Replicate");
                    }
                }

            }
            break;

        case mtype::kComponentDelete:
            if (msg.Is<component::InstanceId>())
            {
                component::InstanceId sourceId = msg.GetValue<component::InstanceId>();

                if (sourceId == component::getId(this))
                {
                    // this is us closing message (runs once)
                    // so let's remove ourself from all remaining components
                    ObjectListCallback components(OI().Id);
                    for (ObjectListCallback::List::const_iterator it = components.ComponentList.begin(); it != components.ComponentList.begin(); ++it)
                    {
                        if (allowReplication((*it)->GetComponentTypeId()))
                        {
                            IPropertySet& propSet = (*it)->GetPropertyData();
                            propSet.RemoveNotifySlot("Replicate");
                        }
                    }
                }
                else if (sourceId.first == OI().Id)
                {
                    // One of the components is getting deleted, for now we do not
                    // do anything yet.
                }
            }
            break;

    }
}


bool ComponentReplicate::allowReplication(const component::ComponentId& /*cid*/) const
{
    return true;
}


Hash ComponentReplicate::GetComponentTypeId() const
{
    return ComponentType::kReplicate;
}


bool ComponentReplicate::RegisterComponentType()
{
    InitComponentType(ComponentType::kReplicate, wxT("Replicate"));

    REGISTRATE(IObjectManager).SubscribeToMessageType(ComponentType::kReplicate, mtype::kComponentNew);
    REGISTRATE(IObjectManager).SubscribeToMessageType(ComponentType::kReplicate, mtype::kComponentDelete);

    return true;
}


} // namespace eg