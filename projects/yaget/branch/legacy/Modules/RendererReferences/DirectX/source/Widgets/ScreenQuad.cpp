#include "Precompiled.h"
#include "ScreenQuad.h"
#include "Presenters.h"
#include "Message/Dispatcher.h"
#include "Interface/IRendererTypes.h"
#include "Interface/IRenderContext.h"
#include "IComponentEntity.h"
#include "IComponentModel.h"
#include "ComponentEnumsRender.h"
#include "Presenters.h"
#include "../Renderer.h"
#include "../ErrorHandler.h"
#include "../RenderContext.h"

namespace
{
    using namespace eg;
    using namespace eg::widgets;
    AutoRegisterAssetFactory<ScreenQuad> myFactory;

    widgets::AutoRegister<ScreenQuad> registerWidget("ScreenQuad");

} // namespace


namespace eg {
namespace widgets {

ScreenQuad::ScreenQuad(const std::string& name)
: WidgetAsset<ScreenQuad>(name)
, mShader("ScreenQuad.fx")
, mMaterial("ScreenQuad.mat")
, mMatColor(v4::ONE())
, mPosition(-1, 1, 1, -1)
{
    Dispatcher& disp = REGISTRATE(Dispatcher);
    mTickConnection = disp[mtype::kFrameTick].connect(boost::bind(&ScreenQuad::onTick, this, _1));
}


ScreenQuad::~ScreenQuad()
{
    mTickConnection.disconnect();
}


void ScreenQuad::onTick(eg::Message& /*msg*/)
{
    //float deltaTime = msg.GetValue<float>();
    // here we might fade out or in based on state, like select on, select of, etc
}


void ScreenQuad::onRender(const IRenderContext& /*renderContext*/)
{
    if (TrianglePresenter *pPresenter = registrate::p_cast<TrianglePresenter>("Widgets.TrianglePresenter"))
    {
        TrianglePresenter::Triangles_t triangles;

        float x1 = mPosition.x;
        float y1 = mPosition.y;
        float x2 = mPosition.z;
        float y2 = mPosition.w;
        TrianglePresenter::Vertex p0(Vector3(x1, y1, 0), v4::ONE(), Vector2(0, 0));     // upper-left
        TrianglePresenter::Vertex p1(Vector3(x2, y1, 0), v4::ONE(), Vector2(1, 0));     // upper-right
        TrianglePresenter::Vertex p2(Vector3(x2, y2, 0), v4::ONE(), Vector2(1, 1));     // lower-right
        TrianglePresenter::Vertex p3(Vector3(x1, y2, 0), v4::ONE(), Vector2(0, 1));     // lower-left

        TrianglePresenter::Triangle triangle0(p0, p1, p2);
        TrianglePresenter::Triangle triangle1(p0, p2, p3);

        triangles.push_back(triangle0);
        triangles.push_back(triangle1);

        pPresenter->Render(triangles);
    }
}


void ScreenQuad::onEndRender(Message& msg)
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

            asset::TryLocker<MaterialAsset> matLocker(mMaterial());
            asset::TryLocker<ShaderAsset> shaderLocker(mShader());
            if (matLocker && shaderLocker)
            {
                material::AutoReset matReset(*matLocker, "QuadColor", mMatColor);
                shaderLocker->Render(renderContext, *matLocker, GeometryAsset::Asset_t(), boost::bind(&ScreenQuad::onRender, this, _1));
            }
        }
    }
}


void ScreenQuad::setOption(const std::string& key, const std::string& value)
{
    if (boost::algorithm::iequals(key, std::string("color")))
    {
        mMatColor = ConvertProperty<Vector4>(value);
    }
    else if (boost::algorithm::iequals(key, std::string("shader")))
    {
        typedef std::pair<std::string, std::string> AssetNames_t;
        AssetNames_t assetNames = ConvertProperty<AssetNames_t>(value);
        if (!assetNames.first.empty())
        {
            mShader = assetNames.first;
        }
        if (!assetNames.second.empty())
        {
            mMaterial = assetNames.second;
        }
    }
    else if (boost::algorithm::iequals(key, std::string("position")))
    {
        mPosition = ConvertProperty<Vector4>(value);
    }
}


void ScreenQuad::getOptions(std::string& options) const
{
    WidgetAsset<ScreenQuad>::getOptions(options);
    if (!options.empty())
    {
        options += "; ";
    }
    options += "color = <Vector4>; shader = <shaderName, materialName>; position = <left, top, right, bottom>";
}

} // namespace widgets
} // namespace eg

