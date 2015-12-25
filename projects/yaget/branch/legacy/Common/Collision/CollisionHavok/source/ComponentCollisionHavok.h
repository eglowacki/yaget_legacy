///////////////////////////////////////////////////////////////////////
// ComponentCollisionHavok.h
//
//  Copyright 09/13/2008 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Private implementation of ComponentCollision model
//
//
//  #include "ComponentCollisionHavok.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef COMPONENT_COLLISION_HAVOK_H
#define COMPONENT_COLLISION_HAVOK_H

#include "IComponentCollision.h"
#include "Property/PropertyData.h"
#include "Message/Dispatcher.h"
#include "Input/InputManager.h"
#include <set>

// forward declerations
class hkResource;


namespace eg
{
    //! Component for wrapping collision library for Bullet middleware
    class ComponentCollisionHavok : public IComponentCollision, public ComponentHelper<ComponentCollisionHavok, PropertySet>
    {
    public:
        ComponentCollisionHavok(component::ObjectId oId);
        virtual ~ComponentCollisionHavok();

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
        virtual void onEntityChanged(const IProperty *pProperty);
        virtual void onModelChanged(const IProperty *pProperty);

        // from IComponent
        virtual void Shutdown();

        //! this can represent shape file to load which will represent
        //! object i collision system
        Property<std::vector<std::string> >& mShapeNames;

        struct CollisionAsset
        {
            CollisionAsset(uint64_t oId, const std::vector<std::string>& pathNames, const Matrix44& transform);
            ~CollisionAsset();

            void SetTransform(const Matrix44& transform);

            typedef std::vector<hkResource *> Shapes_t;
            Shapes_t mShapes;
            Matrix44 mTransform;
            uint64_t moId;
        };

        boost::shared_ptr<CollisionAsset> mCollisionAsset;
    };

} // namespace eg

#endif // COMPONENT_COLLISION_HAVOK_H

