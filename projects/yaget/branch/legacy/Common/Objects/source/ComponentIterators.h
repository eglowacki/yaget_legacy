///////////////////////////////////////////////////////////////////////
// ComponentIterators.h
//
//  Copyright 10/18/2005 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//
//
//  #include "ComponentIterators.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef COMPONENT_ITERATORS_H
#define COMPONENT_ITERATORS_H

#include "ComponentEnumsObjects.h"
#include "ObjectId.h"
#include <set>
//#include <boost\smart_ptr.hpp>


namespace eg
{
    class IComponent;
    //! map of component types
    typedef std::map<component::ObjectId, IComponent *> ComponentMap_t;

    // This class iterates over all the components of a given type in the object manager db.
    // It would be used like this:
    // for (ComponentByTypeIterator iter(Hash("Entity")) ; !iter.IsDone() ; ++iter)
    //      Do whatever it is you need to do with iter.GetComponent();
    class ComponentByTypeIterator
    {
    public:
        ComponentByTypeIterator(Hash type);

        // Prefix increment operator
        ComponentByTypeIterator &operator++();
        // Postfix increment operator
        ComponentByTypeIterator operator++(int);
        // Have we iterated beyond the end?
        bool IsDone() const;
        IComponent *GetComponent() const;

        void Reset(Hash type);

    private:
        ComponentMap_t::const_iterator mIter;
        component::ComponentId mType;
    };


    /*
    class ComponentByInterfaceIterator
    {
    public:
        ComponentByInterfaceIterator(guid_t interfaceType);

        ComponentByInterfaceIterator &operator++();
        ComponentByInterfaceIterator operator++(int);

        bool IsDone() const;
        IComponent *GetComponent() const;

    private:
        void SetToFirstValidComponent();

        std::set<Hash>::const_iterator mCompTypeIter;
        ComponentByTypeIterator mCompIter;
        guid_t mInterfaceType;
    };
    */


    class ComponentByObjectIterator
    {
    public:
        ComponentByObjectIterator(component::ObjectId oId);

        ComponentByObjectIterator &operator++();
        ComponentByObjectIterator operator++(int);

        bool IsDone() const;
        IComponent *GetComponent() const;

    private:
        void SetToFirstValidComponent();

        component::ObjectId mObjectId;
        std::set<component::ComponentId>::const_iterator mCompTypeIter;
    };

} // namespace eg




#endif //COMPONENT_ITERATORS_H