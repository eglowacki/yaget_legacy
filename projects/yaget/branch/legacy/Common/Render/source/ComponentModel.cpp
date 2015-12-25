#include "Precompiled.h"
#include "ComponentModel.h"
#include "Renderer.h"
#include "Config/ConfigHelper.h"
#include "Interface/IRenderContext.h"
#include "Math/Matrix.h"
#include <wx/confbase.h>
#include <wx/statline.h>
#include <d3dx9math.h>

namespace
{
    using namespace eg;
    AutoRegisterAssetFactory<GeometryAsset> myGeometryFactory;
    AutoRegisterAssetFactory<GeometryTextureAsset> myGeometryTextureFactory;

    AutoRegisterAssetFactory<ShaderAsset> myShaderFactory;
    AutoRegisterAssetFactory<MaterialAsset> myMaterialFactory;

} // namespace

namespace eg {

EG_REGISTER_COMPONENT(ComponentModel);


ComponentModel::ComponentModel(component::ObjectId oId)
: ComponentHelper(oId, model::kGroup, ComponentHelper::ccAttached)
, mGeometryName(get_prop<File_Tag, std::string>(mProperty, model::kPropGeometryName, "", "{ext = *.x; type = x; multi = false}"))
, mShaderName(get_prop<File_Tag, std::string>(mProperty, model::kPropShaderName, "", "{ext = *.fx; type = fx; multi = false}"))
, mMaterialName(get_prop<File_Tag, std::string>(mProperty, model::kPropMaterialName, "", "{ext = *.mat; type = mat; multi = false}"))
, mRenderIdProperty(get_prop<Flags_Tag, uint32_t>(mProperty, model::kPropRenderId, 1, ""))
, mRenderId(1)
, mSortValue(0.0f)
{
}


ComponentModel::~ComponentModel()
{
    mGeometry.Sync();
    mShader.Sync();
    mMaterial.Sync();
}


void ComponentModel::DeleteThis()
{
    delete this;
}


void ComponentModel::Shutdown()
{
}


float ComponentModel::GetSortValue() const
{
    return mSortValue;
}


Dispatcher::Result_t ComponentModel::HandleMessage(Message& /*msg*/)
{
}


void ComponentModel::onPropertyChanged(const IProperty *pProperty)
{
    if (pProperty->GetPropId() == mGeometryName.GetPropId())
    {
        mGeometry.Sync();
        mGeometry = static_cast<std::string>(mGeometryName);
    }
    else if (pProperty->GetPropId() == mShaderName.GetPropId())
    {
        mShader.Sync();
        mShader = static_cast<std::string>(mShaderName);
    }
    else if (pProperty->GetPropId() == mMaterialName.GetPropId())
    {
        mMaterial.Sync();
        mMaterial = static_cast<std::string>(mMaterialName);
    }
	else if (pProperty->GetPropId() == mRenderIdProperty.GetPropId())
	{
		mRenderId = mRenderIdProperty;
	}
}


void ComponentModel::Render(const IRenderContext& renderContext)
{
    asset::TryLocker<MaterialAsset> matLocker(mMaterial());
    asset::TryLocker<GeometryAsset> geomLocker(mGeometry());
    asset::TryLocker<ShaderAsset> shaderLocker(mShader());
    if (matLocker && geomLocker && shaderLocker)
    {
        shaderLocker->Render(renderContext, *matLocker, *geomLocker, 0);
    }
}


uint32_t ComponentModel::GetRenderId() const
{
    return mRenderId;
}


Hash ComponentModel::GetComponentTypeId() const
{
    return ComponentType::kModel;
}


bool ComponentModel::RegisterComponentType()
{
    InitComponentType(ComponentType::kModel, "Model");
    return true;
}


} // namespace eg
