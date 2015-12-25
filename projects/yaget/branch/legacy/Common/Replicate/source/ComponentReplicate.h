///////////////////////////////////////////////////////////////////////
// ComponentReplicate.h
//
//  Copyright 12/01/2005 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Private implementation of component model
//
//
//  #include "ComponentReplicate.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef COMPONENT_REPLICATE_H
#define COMPONENT_REPLICATE_H

#include "IComponentReplicate.h"
#include "Property/PropertyData.h"
#include "Message/Dispatcher.h"



namespace eg
{
    // forward declarations
    class IntervalTimer;

    class ComponentReplicate : public IComponentReplicate, public ComponentHelper<ComponentReplicate, PropertySet>
    {
    public:
        ComponentReplicate(component::ObjectId oId);
        virtual ~ComponentReplicate();

        // from IComponent
        virtual void DeleteThis();
        virtual Hash GetComponentTypeId() const;
        virtual Dispatcher::Result_t HandleMessage(Message& msg);
        virtual IPropertySet& GetPropertyData() {return CH_t::GetPropertyData();}
        virtual const ObjectInstance& OI() const {return CH_t::OI();}

        // Static methods
        static bool RegisterComponentType();

    private:
        // from ComponentHelper
        virtual void onPropertyChanged(const IProperty *pProperty);
        // from IComponent
        virtual void Shutdown();

        //! called for every update based on some time interval.
        //! This is used by all components, but can be called at different times
        void onSendReplication(const Message& msg);

        //! is this component is to be replicated?
        bool allowReplication(const component::ComponentId& cid) const;

        //! Buffered all properties changes this frame
        //! which will be replicated when we receive frame tick
        //! or some user specified update rate
        //!
        //! Key for each change
        //! first - instance id for this property
        //! second Hash of property name. Only used in adding new property change event
        typedef std::pair<component::InstanceId, Hash> PropChangeKey_t;

        //! first - PropChangeKey_t, between frame updates there might be
        //! several updates for different property and we need to keep track of it.
        //! second - name of property
        typedef std::map<PropChangeKey_t, std::string> ChangedProperties_t;
        ChangedProperties_t mPendingProperties;
        wxCriticalSection mCritSection;

        //! Specifies how often it needs to issue replicate event
        //! Value of 0 indicates every frame message.
        Property<float>& mReplicateRate;

        //! Interval timer used in triggering replication events based on mReplicateRate/sec value.
        boost::shared_ptr<IntervalTimer> mReplicationTimer;
    };

} // namespace eg

#endif // COMPONENT_REPLICATE_H

