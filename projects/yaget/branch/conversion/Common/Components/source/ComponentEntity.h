///////////////////////////////////////////////////////////////////////
// ComponentEntity.h
//
//  Copyright 5/25/2006 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Private implementation of component Gui
//
//
//  #include "ComponentEntity.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef COMPONENT_ENTITY_H
#define COMPONENT_ENTITY_H

#include "IComponentEntity.h"
#include "Property/PropertyData.h"
#include "Message/Dispatcher.h"


namespace eg
{

    class ComponentEntity : public IComponentEntity, public ComponentHelper<ComponentEntity, PropertySet>
    {
    public:
        ComponentEntity(component::ComponentId id);
        virtual ~ComponentEntity();

        // from IComponentEntity
        virtual const Matrix44& GetMatrix() const;

        // from IComponent
        virtual void DeleteThis();
        virtual component::ComponentId GetComponentTypeId() const;
        virtual Dispatcher::Result_t HandleMessage(Message& msg);
        virtual IPropertySet& GetPropertyData() {return CH_t::GetPropertyData();}
        virtual const ObjectInstance& OI() const {return CH_t::OI();}

        // Static methods
        static bool RegisterComponentType();

    private:
        // from IComponent
        virtual void Shutdown();
        // from ComponentHelper
        virtual void onPropertyChanged(const IProperty *pProperty);

        Property<Vector3>& mPosition;
        Property<Vector3>& mScale;
        Property<Quaternion>& mOrientation;

        mutable Matrix44 mMatrix;
        mutable bool mbNeedUpdateMatrix;
    };

} // namespace eg

#endif // COMPONENT_ENTITY_H

