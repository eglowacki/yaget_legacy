#include "Precompiled.h"
#include "BoundingBox.h"
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
    AutoRegisterAssetFactory<BoundingBox> myFactory;

    widgets::AutoRegister<BoundingBox> registerWidget("BoundingBox");

} // namespace

namespace eg {
namespace widgets {


BoundingBox::BoundingBox(const std::string& name)
: WidgetAsset<BoundingBox>(name)
, mLineColor(v4::ONE())
, mBoundingBox(-v3::ONE(), v3::ONE())
, mShader("BoundingBox.fx")
, mMaterial("BoundingBox.mat")
, mCurrState(sNone)
, mFade(0)
, mFadeTimout(0)
{
    Dispatcher& disp = REGISTRATE(Dispatcher);
    mTickConnection = disp[mtype::kFrameTick].connect(boost::bind(&BoundingBox::onTick, this, _1));
}


BoundingBox::~BoundingBox()
{
    mTickConnection.disconnect();
}


void BoundingBox::onTick(eg::Message& msg)
{
    float deltaTime = msg.GetValue<float>();
    // here we might fade in or out based on state, like select on, select off, etc.
    if (mCurrState == sFading)
    {
        mFade -= deltaTime;
        mFade = std::max(0.0f, mFade);
        float currAlpha = 1 - (mFade / mFadeTimout);
        currAlpha = std::min(1.0f, currAlpha);
        mLineColor.w = currAlpha;

        if (mFade == 0)
        {
            mCurrState = sNone;
        }
    }
    else if (mCurrState == sFlashing)
    {
        mFade -= deltaTime;
        if (mFade < 0)
        {
            if (mLineColor.w == 1)
            {
                mLineColor.w = 0.2;
            }
            else
            {
                mLineColor.w = 1;
            }

            mFadeTimout--;
            mFade = 0.1f;
        }

        if (mFadeTimout <= 0)
        {
            mCurrState = sNone;
            mLineColor.w = 1;
        }
    }
}


void BoundingBox::onRender(const IRenderContext& renderContext)
{
	if (!(renderContext.GetRenderId() & renderId()))
	{
		return;
	}

    if (LinePresenter *pLP = registrate::p_cast<LinePresenter>("Widgets.LinePresenter"))
    {
        LinePresenter::Verticies_t lines;

        // create all 8 corners first from bounding box
        // starting lower.left.near clokwise and then move to
        // lower.left.far and also clockwise
        // and then connecting near to far starting lower.left.near
        // and clockwise
        const Vector3& l = mBoundingBox.first;
        const Vector3& u = mBoundingBox.second;
        // near
        Vector3 c0(l.x, l.y, l.z);
        Vector3 c1(l.x, u.y, l.z);
        Vector3 c2(u.x, u.y, l.z);
        Vector3 c3(u.x, l.y, l.z);
        // far
        Vector3 c4(l.x, l.y, u.z);
        Vector3 c5(l.x, u.y, u.z);
        Vector3 c6(u.x, u.y, u.z);
        Vector3 c7(u.x, l.y, u.z);

        // near
        lines.push_back(LinePresenter::Vertex(c0));
        lines.push_back(LinePresenter::Vertex(c1));
        lines.push_back(LinePresenter::Vertex(c1));
        lines.push_back(LinePresenter::Vertex(c2));
        lines.push_back(LinePresenter::Vertex(c2));
        lines.push_back(LinePresenter::Vertex(c3));
        lines.push_back(LinePresenter::Vertex(c3));
        lines.push_back(LinePresenter::Vertex(c0));
        // far
        lines.push_back(LinePresenter::Vertex(c4));
        lines.push_back(LinePresenter::Vertex(c5));
        lines.push_back(LinePresenter::Vertex(c5));
        lines.push_back(LinePresenter::Vertex(c6));
        lines.push_back(LinePresenter::Vertex(c6));
        lines.push_back(LinePresenter::Vertex(c7));
        lines.push_back(LinePresenter::Vertex(c7));
        lines.push_back(LinePresenter::Vertex(c4));
        // connectors (near -> far)
        lines.push_back(LinePresenter::Vertex(c0));
        lines.push_back(LinePresenter::Vertex(c4));
        lines.push_back(LinePresenter::Vertex(c1));
        lines.push_back(LinePresenter::Vertex(c5));
        lines.push_back(LinePresenter::Vertex(c2));
        lines.push_back(LinePresenter::Vertex(c6));
        lines.push_back(LinePresenter::Vertex(c3));
        lines.push_back(LinePresenter::Vertex(c7));

        pLP->Render(lines);
        //LinePresenter.Render(lines);
    }
}


void BoundingBox::onEndRender(eg::Message& msg)
{
    // and reset current model to it's old cameraId
    if (showWidget())
    {
        if (msg.Is<const IRenderContext *>())
        {
            const IRenderContext& renderContext = *msg.GetValue<const IRenderContext *>();

            RenderContext rContext(renderContext);
            if (IComponentEntity *pEntity = component::get<IComponentEntity>(oId()))
            {
                rContext.SetWorldMatrix(pEntity->GetMatrix());
            }

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
                mBoundingBox = geometryLocker->BoundingBox();
                material::AutoReset matReset(*matLocker, "LineColor", mLineColor);
                shaderLocker->Render(rContext, *matLocker, GeometryAsset::Asset_t(), boost::bind(&BoundingBox::onRender, this, _1));
            }
        }
    }
}

void BoundingBox::setOption(const std::string& key, const std::string& value)
{
    if (boost::algorithm::iequals(key, std::string("color")))
    {
        Color4_t newColor = ConvertProperty<Color4_t>(value);
        if (mCurrState != sNone)
        {
            newColor.w = mLineColor.w;
        }
        mLineColor = newColor;
    }
    else if (boost::algorithm::iequals(key, std::string("fade")))
    {
        mCurrState = sFading;
        mLineColor.w = 0;
        mFadeTimout = mFade = ConvertProperty<float>(value);
    }
    else if (boost::algorithm::iequals(key, std::string("flash")))
    {
        mCurrState = sFlashing;
        //! this is switched between on and off
        mLineColor.w = 1;
        mFade = 0.1f;
        //! use as re decrement counter, number of flashes
        mFadeTimout = std::max(0.0f, (ConvertProperty<float>(value) * 2));
    }
}


void BoundingBox::getOptions(std::string& options) const
{
    WidgetAsset<BoundingBox>::getOptions(options);
    if (!options.empty())
    {
        options += "; ";
    }
    options += "color = <Vector4>; fade = <seconds>; flash = <times>";
}


} // namespace widgets
} // namespace eg
