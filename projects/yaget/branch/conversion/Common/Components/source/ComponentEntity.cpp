#include "Precompiled.h"
#include "ComponentEntity.h"
#include "Config/ConfigHelper.h"
#include "Plugin/ModuleHelper.h"


namespace eg {

EG_MODULE_REGISTER_COMPONENT(ComponentEntity);


ComponentEntity::ComponentEntity(component::ComponentId id)
: ComponentHelper(id, entity::kGroup, ComponentHelper::ccAttached)
, mPosition(get_propV<Vector3>(mProperty, entity::kPropPosition, v3::ZERO(), ""))
, mScale(get_propV<Vector3>(mProperty, entity::kPropScale, v3::ONE(), ""))
, mOrientation(get_propV<Quaternion>(mProperty, entity::kPropOrientation, v4::WONE(), ""))
, mMatrix(m44::Identity())
, mbNeedUpdateMatrix(false)
{
}


ComponentEntity::~ComponentEntity()
{
}


void ComponentEntity::DeleteThis()
{
    delete this;
}


void ComponentEntity::onPropertyChanged(const IProperty * /*pProperty*/)
{
    mbNeedUpdateMatrix = true;
}


const Matrix44& ComponentEntity::GetMatrix() const
{
    if (mbNeedUpdateMatrix)
    {
        mbNeedUpdateMatrix = false;
        Matrix44 matrixScale(ScaleMatrix44(mScale()));
        Matrix44 matrixRot(mOrientation());
        Matrix44 matrixPos(TranslateMatrix44(mPosition()));

        mMatrix = matrixScale * matrixRot * matrixPos;
    }

    return mMatrix;
}


void ComponentEntity::Shutdown()
{
}


Dispatcher::Result_t ComponentEntity::HandleMessage(Message& /*msg*/)
{
}


Hash ComponentEntity::GetComponentTypeId() const
{
    return ComponentType::kEntity;
}


bool ComponentEntity::RegisterComponentType()
{
    InitComponentType(ComponentType::kEntity, _T("Entity"));
    //REGISTRATE(IObjectManager).SubscribeToMessageType(ComponentType::kEntity, mtype::kComponentNew);

    return true;
}


} // namespace eg

