///////////////////////////////////////////////////////////////////////
// ComponentArea.h
//
//  Copyright 10/29/2005 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Private implementation of component Area
//
//
//  #include "ComponentArea.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef COMPONENT_AREA_H
#define COMPONENT_AREA_H

#include "IComponentArea.h"
#include "Math/Vector.h"
#include "Property/PropertyData.h"
#include "Message/Dispatcher.h"
#include <vector>


namespace eg
{
    class ComponentArea : public IComponentArea, public ComponentHelper<ComponentArea, PropertySet>
    {
    public:
        ComponentArea(component::ObjectId id);
        virtual ~ComponentArea();

        // from IComponent
        virtual void DeleteThis();
        virtual component::ComponentId GetComponentTypeId() const;
        virtual Dispatcher::Result_t HandleMessage(Message& msg);
        virtual IPropertySet& GetPropertyData() {return CH_t::GetPropertyData();}
        virtual const ObjectInstance& OI() const {return CH_t::OI();}

        // Static methods
        static bool RegisterComponentType();

    private:
        // from IComponentArea
        virtual void onPropertyChanged(const IProperty *pProperty);
        // from IComponent
        virtual void Shutdown();

        Property<std::vector<component::ObjectId> >& mObjects;
   };

} // namespace eg

#endif // COMPONENT_AREA_H

