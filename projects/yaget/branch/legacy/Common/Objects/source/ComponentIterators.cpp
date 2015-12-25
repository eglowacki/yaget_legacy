#include "Precompiled.h"
#include "ComponentIterators.h"
#include "ObjectManager.h"
#include "ObjectManagerDB.h"


///////////////////////////////////////////////////////
// ComponentByTypeIterator methods
///////////////////////////////////////////////////////
namespace eg {

ComponentByTypeIterator::ComponentByTypeIterator(component::ComponentId type)
{
    Reset(type);
}


// Prefix increment operator
ComponentByTypeIterator& ComponentByTypeIterator::operator++()
{
    if (!IsDone())
    {
        // Let's not iterate past end()
        ++mIter;
    }
    return *this;
}


// Postfix increment operator
ComponentByTypeIterator ComponentByTypeIterator::operator++(int)
{
    ComponentByTypeIterator retVal = *this;
    // Call the prefix increment operator
    ++(*this);

    return retVal;
}


bool ComponentByTypeIterator::IsDone() const
{
    if (!ObjectManager::GetDB().IsComponentTypeValid(mType))
    {
        return true;
    }

    return (mIter == ObjectManager::GetDB().GetComponentInstanceList(mType).end());
}


IComponent  *ComponentByTypeIterator::GetComponent() const
{
    if (IsDone())
    {
        return 0;
    }

    IComponent* pComponent = (*mIter).second;
    return pComponent;
}


void ComponentByTypeIterator::Reset(Hash type)
{
    mType = type;
    if (ObjectManager::GetDB().IsComponentTypeValid(mType))
    {
        mIter = ObjectManager::GetDB().GetComponentInstanceList(type).begin();
    }
}


///////////////////////////////////////////////////////
// ComponentByObjectIterator methods
///////////////////////////////////////////////////////
ComponentByObjectIterator::ComponentByObjectIterator(component::ObjectId oId) :
    mObjectId(oId),
    mCompTypeIter(ObjectManager::GetDB().GetComponentTypes().begin())
{
    SetToFirstValidComponent();
}


void ComponentByObjectIterator::SetToFirstValidComponent()
{
    if (mCompTypeIter != ObjectManager::GetDB().GetComponentTypes().end())
    {
        if (!ObjectManager::GetDB().FindComponentInstance(component::InstanceId(mObjectId, *mCompTypeIter)))
        {
            // Couldn't find a component belonging to oId in the first list, now a simple increment will do
            ++(*this);
        }
    }
}


ComponentByObjectIterator &ComponentByObjectIterator::operator++()
{
    while (++mCompTypeIter != ObjectManager::GetDB().GetComponentTypes().end())
    {
        if (ObjectManager::GetDB().FindComponentInstance(component::InstanceId(mObjectId, *mCompTypeIter)))
        {
            // We found a component pointer, so we'll stop here.
            break;
        }
    }

    return *this;
}


ComponentByObjectIterator ComponentByObjectIterator::operator++(int)
{
    ComponentByObjectIterator retVal = *this;
    ++(*this);
    return retVal;
}


bool ComponentByObjectIterator::IsDone() const
{
    return mCompTypeIter == ObjectManager::GetDB().GetComponentTypes().end();
}


IComponent  *ComponentByObjectIterator::GetComponent() const
{
    if (IsDone())
    {
        return 0;
    }

    IComponent *pComponent = ObjectManager::GetDB().FindComponentInstance(component::InstanceId(mObjectId, *mCompTypeIter));
    return pComponent;
}


} // namespace eg

