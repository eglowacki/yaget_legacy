#include "Precompiled.h"
#include "ComponentCollisionBullet.h"
#include "Collision.h"
#include "Config/ConfigHelper.h"
#include "Plugin/ModuleHelper.h"
#include <wx/confbase.h>
#include <wx/statline.h>
#include <wx/thread.h>
#include <wx/filename.h>
#include <wx/regex.h>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>


namespace eg {

EG_MODULE_REGISTER_COMPONENT(ComponentCollisionBullet);


ComponentCollisionBullet::ComponentCollisionBullet(component::ObjectId oId)
: ComponentHelper(oId, collision::kGroup, ComponentHelper::ccAttached)
{
}


ComponentCollisionBullet::~ComponentCollisionBullet()
{
}


void ComponentCollisionBullet::DeleteThis()
{
    delete this;
}


void ComponentCollisionBullet::Shutdown()
{
}


Dispatcher::Result_t ComponentCollisionBullet::HandleMessage(Message& /*msg*/)
{
}


void ComponentCollisionBullet::onPropertyChanged(const IProperty * /*pProperty*/)
{
}


Hash ComponentCollisionBullet::GetComponentTypeId() const
{
    return ComponentType::kCollision;
}


bool ComponentCollisionBullet::RegisterComponentType()
{
    InitComponentType(ComponentType::kCollision, wxT("Collision"));
    return true;
}


} // namespace eg
