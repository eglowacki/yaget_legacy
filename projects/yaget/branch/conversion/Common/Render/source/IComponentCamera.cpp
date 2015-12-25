#include "Precompiled.h"
#include "IComponentCamera.h"
#include "IObjectManager.h"


namespace eg {

void IComponentCamera::RegisterInterface(Hash compId)
{
    GetObjectManager().RegisterInterfaceWithComponent(InterfaceType::kCamera, compId);
}

} // namespace eg
