#include "Precompiled.h"
#include "ComponentArea.h"
#include "Config/ConfigHelper.h"
#include "Plugin/ModuleHelper.h"


namespace eg {

EG_MODULE_REGISTER_COMPONENT(ComponentArea);


ComponentArea::ComponentArea(component::ObjectId id)
: ComponentHelper(id, area::kGroup, ComponentHelper::ccAttached)
, mObjects(get_propV<std::vector<component::ObjectId> >(mProperty, area::kPropObjects, std::vector<component::ObjectId>(), ""))
{
}


ComponentArea::~ComponentArea()
{
}


void ComponentArea::DeleteThis()
{
    delete this;
}


void ComponentArea::Shutdown()
{
}


Dispatcher::Result_t ComponentArea::HandleMessage(Message& /*msg*/)
{
}


Hash ComponentArea::GetComponentTypeId() const
{
    return ComponentType::kArea;
}


bool ComponentArea::RegisterComponentType()
{
    InitComponentType(ComponentType::kArea, "Area");
    return true;
}


void ComponentArea::onPropertyChanged(const IProperty *pProperty)
{
    if (pProperty->GetPropId() == mObjects.GetPropId())
    {
    }
}


} // namespace eg
