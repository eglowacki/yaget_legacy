#include "Precompiled.h"
#include "ObjectManagerKonsole.h"
#include "StringHelper.h"
#include "IComponent.h"


namespace eg {
namespace config {


NewComponent_t ParseNewComponent(const std::string& componentType, const std::string& objectId, const std::string& objectName)
{
    uint64_t oId = from_string<uint64_t>(objectId);
    component::InstanceId iId = component::make(oId, component::ComponentId(componentType.c_str()));
    return NewComponent_t(iId, objectName);
}


component::ObjectId ResolveObjectId(const std::string& objectName)
{
    IObjectManager& om = REGISTRATE(IObjectManager);
    uint64_t objectId = from_string<uint64_t>(objectName);
    if (!objectId || !om.IsObjectExists(component::ObjectId(objectId)))
    {
        // there is no such object, let's treat parameter as an object name
        // and try to lookup, on the first come basis
        struct FindObject : public EnumComponentCallback
        {
            FindObject(const std::string& name) : Name(name), ObjectId(0) {}

            virtual bool OnComponent(IComponent *pComponent)
            {
                if (boost::iequals(pComponent->OI().Name, Name))
                {
                    ObjectId = pComponent->OI().Id.GetValue();
                    return false;
                }

                return true;
            }

            std::string Name;
            uint64_t ObjectId;
        };

        FindObject findObject(objectName);
        om.EnumObjectsByAll(findObject);
        objectId = findObject.ObjectId;
    }

    return component::ObjectId(objectId);
}


IComponent *FindComponent(const std::string& componentType, const std::string& objectId)
{
    component::ObjectId oId = ResolveObjectId(objectId);
    component::ComponentId cId(componentType.c_str());
    IObjectManager& om = REGISTRATE(IObjectManager);
    return om.FindComponentInstance(component::InstanceId(oId, cId));
}

} // namespace config
} // namespace eg
