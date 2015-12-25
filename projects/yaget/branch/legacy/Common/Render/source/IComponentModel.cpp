#include "Precompiled.h"
#include "IComponentModel.h"
#include "IObjectManager.h"


namespace eg {


void IComponentModel::RegisterInterface(Hash compId)
{
    eg::GetObjectManager().RegisterInterfaceWithComponent(InterfaceType::kModel, compId);
}

} // namespace eg
