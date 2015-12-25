#include "Precompiled.h"
#include "Ray.h"
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
    AutoRegisterAssetFactory<eg::widgets::Ray> myFactory;

    widgets::AutoRegister<eg::widgets::Ray> registerWidget("Ray");

// maps floating point channels (0.f to 1.f range) to D3DCOLOR
//#define COLOR_TO_D3D(r,g,b,a) \
//    D3DCOLOR_ARGB((DWORD)((a)*255.f),(DWORD)((r)*255.f),(DWORD)((g)*255.f),(DWORD)((b)*255.f))

} // namespace

namespace eg {
namespace widgets {

Ray::Ray(const std::string& name)
: WidgetAsset<Ray>(name)
, mLineColor(v4::ONE())
, mShader("BoundingBox.fx")
, mMaterial("BoundingBox.mat")
, mDirection(v3::ZONE())
, mLength(1)
{
    Dispatcher& disp = REGISTRATE(Dispatcher);
    mTickConnection = disp[mtype::kFrameTick].connect(boost::bind(&Ray::onTick, this, _1));
}


Ray::~Ray()
{
    mTickConnection.disconnect();
}


void Ray::onTick(eg::Message& /*msg*/)
{
    //float deltaTime = msg.GetValue<float>();
    // here we might fade out or in based on state, like select on, select of, etc

}


void Ray::onRender(const IRenderContext& /*renderContext*/)
{
    if (LinePresenter *pLP = registrate::p_cast<LinePresenter>("Widgets.LinePresenter"))
    {
        LinePresenter::Verticies_t lines;
        lines.push_back(LinePresenter::Vertex(v3::ZERO()));
        lines.push_back(LinePresenter::Vertex(mDirection * mLength));
        pLP->Render(lines);
    }
}


void Ray::onEndRender(Message& msg)
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

            asset::TryLocker<MaterialAsset> matLocker(mMaterial());
            asset::TryLocker<ShaderAsset> shaderLocker(mShader());
            if (matLocker && shaderLocker)
            {
                shaderLocker->Render(rContext, *matLocker, GeometryAsset::Asset_t(), boost::bind(&Ray::onRender, this, _1));
            }
        }
    }
}

void Ray::setOption(const std::string& key, const std::string& value)
{
    if (boost::algorithm::iequals(key, std::string("color")))
    {
        mLineColor = ConvertProperty<Color4_t>(value);
    }
    else if (boost::algorithm::iequals(key, std::string("direction")))
    {
        mDirection = ConvertProperty<Vector3>(value);
        mDirection.normalize();
    }
    else if (boost::algorithm::iequals(key, std::string("length")))
    {
        mLength = ConvertProperty<float>(value);
    }
}


void Ray::getOptions(std::string& options) const
{
    WidgetAsset<Ray>::getOptions(options);
    if (!options.empty())
    {
        options += "; ";
    }
    options += "color = <Vector4>; direction = <Vector3>; length = <float>";
}

} // namespace widgets
} // namespace eg
