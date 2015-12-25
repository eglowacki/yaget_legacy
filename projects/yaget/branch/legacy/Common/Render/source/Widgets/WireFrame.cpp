#include "Precompiled.h"
#include "WireFrame.h"
#include "IComponentModel.h"
#include "Presenters.h"
#include "Message/Dispatcher.h"
#include "Interface/IRendererTypes.h"
#include "Interface/IRenderContext.h"
#include "IComponentEntity.h"
#include "ComponentEnumsRender.h"
#include "Presenters.h"
#include "../Renderer.h"
#include "../ErrorHandler.h"
#include "../RenderContext.h"

#pragma warning (disable:4312)  // 'variable' : conversion from 'type' to 'type' of greater size
    #include <atlcomcli.h>
    #include <D3D9.h>
#pragma warning (default : 4312)



namespace
{
    using namespace eg;
    using namespace eg::widgets;
    AutoRegisterAssetFactory<WireFrame> myFactory;

    widgets::AutoRegister<WireFrame> registerWidget("WireFrame");

// maps floating point channels (0.f to 1.f range) to D3DCOLOR
//#define COLOR_TO_D3D(r,g,b,a) \
//    D3DCOLOR_ARGB((DWORD)((a)*255.f),(DWORD)((r)*255.f),(DWORD)((g)*255.f),(DWORD)((b)*255.f))

} // namespace

namespace eg {
namespace widgets {

WireFrame::WireFrame(const std::string& name)
: WidgetAsset<WireFrame>(name)
, mLineColor(v4::ONE())
, mShader("WireFrame.fx")
, mMaterial("BoundingBox.mat")
{
    Dispatcher& disp = REGISTRATE(Dispatcher);
    mTickConnection = disp[mtype::kFrameTick].connect(boost::bind(&WireFrame::onTick, this, _1));
}


WireFrame::~WireFrame()
{
    mTickConnection.disconnect();
}


void WireFrame::onTick(eg::Message& /*msg*/)
{
    //float deltaTime = msg.GetValue<float>();
    // here we might fade out or in based on state, like select on, select of, etc
}


void WireFrame::onEndRender(Message& msg)
{
    // and reset current model to it's old cameraId
    if (showWidget())
    {
        if (msg.Is<const IRenderContext *>())
        {
            const IRenderContext& renderContext = *msg.GetValue<const IRenderContext *>();
			if (!(renderContext.GetRenderId() & renderId()))
			{
				return;
			}

            RenderContext rContext(renderContext);
            if (IComponentEntity *pEntity = component::get<IComponentEntity>(oId()))
            {
                rContext.SetWorldMatrix(pEntity->GetMatrix());
            }

            // we also need geometry assets
            component::InstanceId modelId(oId(), ComponentType::kModel);
            property::hProp prop(modelId);
            const IProperty& propValue = prop[model::kPropGeometryName];
            std::string geomName = (std::string)propValue;
            GeometryAsset_t geometry(geomName);

            asset::TryLocker<MaterialAsset> matLocker(mMaterial());
            asset::TryLocker<ShaderAsset> shaderLocker(mShader());
            asset::TryLocker<GeometryAsset> geometryLocker(geometry());
            if (matLocker && shaderLocker && geometryLocker)
            {
                material::AutoReset matReset(*matLocker, "LineColor", mLineColor);
                shaderLocker->Render(rContext, *matLocker, *geometryLocker, 0);
            }
        }
    }
}

void WireFrame::setOption(const std::string& key, const std::string& value)
{
    if (boost::algorithm::iequals(key, std::string("color")))
    {
        mLineColor = ConvertProperty<Color4_t>(value);
    }
}


void WireFrame::getOptions(std::string& options) const
{
    WidgetAsset<WireFrame>::getOptions(options);
    if (!options.empty())
    {
        options += "; ";
    }
    options += "color = <Vector4>";
}

} // namespace widgets
} // namespace eg
