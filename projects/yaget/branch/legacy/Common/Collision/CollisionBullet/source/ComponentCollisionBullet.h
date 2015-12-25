///////////////////////////////////////////////////////////////////////
// ComponentCollisionBullet.h
//
//  Copyright 09/13/2008 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Private implementation of ComponentCollision model
//
//
//  #include "ComponentCollisionBullet.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef COMPONENT_COLLISION_H
#define COMPONENT_COLLISION_H

#include "IComponentCollision.h"
#include "Property/PropertyData.h"
#include "Message/Dispatcher.h"
#include "Input/InputManager.h"
#include <set>


namespace eg
{
    //! Component for wrapping collision library for Bullet middleware
    class ComponentCollisionBullet : public IComponentCollision, public ComponentHelper<ComponentCollisionBullet, PropertySet>
    {
    public:
        ComponentCollisionBullet(component::ObjectId oId);
        virtual ~ComponentCollisionBullet();

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
    };

} // namespace eg

#endif // COMPONENT_COLLISION_H

