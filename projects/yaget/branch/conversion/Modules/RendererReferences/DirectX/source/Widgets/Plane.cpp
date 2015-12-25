#include "Precompiled.h"
#include "Plane.h"
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
    AutoRegisterAssetFactory<Plane> myFactory;

    widgets::AutoRegister<Plane> registerWidget("Plane");

} // namespace


namespace eg {
namespace widgets {

Plane::Plane(const std::string& name)
: WidgetAsset<Plane>(name)
, mGridNames("GridPlane.fx", "GridPlane.mat")
, mFillNames("Garbage.fx", "Garbage.mat")
, mShader("GridPlane.fx")
, mMaterial("GridPlane.mat")
, mTesselate(10)
, mSize(0.1f)
, mColor(v4::ONE())
, mbRenderAsGrid(true)
{
    Dispatcher& disp = REGISTRATE(Dispatcher);
    mTickConnection = disp[mtype::kFrameTick].connect(boost::bind(&Plane::onTick, this, _1));
}


Plane::~Plane()
{
    mTickConnection.disconnect();
}


void Plane::onTick(eg::Message& /*msg*/)
{
    //float deltaTime = msg.GetValue<float>();
    // here we might fade out or in based on state, like select on, select of, etc
}


void Plane::onRender(const IRenderContext& renderContext)
{
	if (!(renderContext.GetRenderId() & renderId()))
	{
		return;
	}

    if (mbRenderAsGrid)
    {
        if (LinePresenter *pPresenter = registrate::p_cast<LinePresenter>("Widgets.LinePresenter"))
        {
            LinePresenter::Verticies_t lines;

            float length = mTesselate * mSize;
            float startX = -(length/2);
            float startZ = (length/2);
            float middleXAxis = 0;
            const Color4_t kDefaultLineColor(0.6f, 0.6f, 0.6f, 1.0f);
            // vertical lines
            for (uint32_t i = 0; i <= mTesselate; i++, startX += mSize)
            {
                Color4_t currColor = kDefaultLineColor;
                if (!(i % 5) || !(i % 10))
                {
                    if ((startX <= 0.0f && (startX + mSize) > 0.0f) || (startX >= 0.0f && (startX - mSize) < 0.0f))
                    {
                        // vertical line at the center will be blue
                        currColor = Vector4(0, 0, 1, 1);
                        middleXAxis = startX;
                    }
                    else
                    {
                        // for every 5th and 10th line we adjust colors
                        if (!(i % 10))
                        {
                            currColor = Vector4(0.0f, 0.0f, 0.0f, 1);
                        }
                        else if (!(i % 5))
                        {
                            currColor = Vector4(0.4f, 0.4f, 0.4f, 1);
                        }
                    }
                }

                LinePresenter::Vertex p0(Vector3(startX, 0, -startZ), currColor);
                LinePresenter::Vertex p1(Vector3(startX, 0, startZ), currColor);
                lines.push_back(p0);
                lines.push_back(p1);
            }

            const int kAxisCount = 2;
            // duplicate middle axis to be slightly stronger in intensity
            for (int i = 0; i < kAxisCount; i++)
            {
                // duplicate middle axis to be slightly stronger in intensity
                LinePresenter::Vertex pv0(Vector3(middleXAxis, 0, -startZ), Vector4(0, 0, 1, 1));
                LinePresenter::Vertex pv1(Vector3(middleXAxis, 0, startZ), Vector4(0, 0, 1, 1));
                lines.push_back(pv0);
                lines.push_back(pv1);
            }


            startX = (length/2);
            startZ = -(length/2);
            float middleZAxis = 0;
            // horizontal lines
            for (uint32_t i = 0; i <= mTesselate; i++, startZ += mSize)
            {
                Color4_t currColor = kDefaultLineColor;
                if (!(i % 5) || !(i % 10))
                {
                    if ((startZ <= 0.0f && (startZ + mSize) > 0.0f) || (startZ >= 0.0f && (startZ - mSize) < 0.0f))
                    {
                        // horizontal line at the center will be red
                        currColor = Vector4(1, 0, 0, 1);
                        middleZAxis = startZ;
                    }
                    else
                    {
                        // for every 5th and 10th line we adjust colors
                        if (!(i % 10))
                        {
                            currColor = Vector4(0.0f, 0.0f, 0.0f, 1);
                        }
                        else if (!(i % 5))
                        {
                            currColor = Vector4(0.4f, 0.4f, 0.4f, 1);
                        }
                    }
                }

                LinePresenter::Vertex p0(Vector3(-startX, 0, startZ), currColor);
                LinePresenter::Vertex p1(Vector3(startX, 0, startZ), currColor);
                lines.push_back(p0);
                lines.push_back(p1);
            }

            // duplicate middle axis to be slightly stronger in intensity
            for (int i = 0; i < kAxisCount; i++)
            {
                // duplicate middle axis to be slightly stronger in intensity
                LinePresenter::Vertex ph0(Vector3(-startX, 0, middleZAxis), Vector4(1, 0, 0, 1));
                LinePresenter::Vertex ph1(Vector3(startX, 0, middleZAxis), Vector4(1, 0, 0, 1));
                lines.push_back(ph0);
                lines.push_back(ph1);
            }

            pPresenter->Render(lines);
        }
    }
    else
    {
        if (TrianglePresenter *pPresenter = registrate::p_cast<TrianglePresenter>("Widgets.TrianglePresenter"))
        {
            TrianglePresenter::Triangles_t triangles;

            float length = mTesselate * mSize;
            float startX = -(length/2);
            float startZ = -(length/2);

            TrianglePresenter::Vertex p0(Vector3(startX, 0, startZ), v4::ONE(), Vector2(0, 1));
            TrianglePresenter::Vertex p1(Vector3(startX, 0, startZ+length), v4::ONE(), Vector2(0, 0));
            TrianglePresenter::Vertex p2(Vector3(startX+length, 0, startZ+length), v4::ONE(), Vector2(1, 0));
            TrianglePresenter::Vertex p3(Vector3(startX+length, 0, startZ), v4::ONE(), Vector2(1, 1));

            TrianglePresenter::Triangle triangle0(p0, p1, p2);
            TrianglePresenter::Triangle triangle1(p0, p2, p3);

            triangles.push_back(triangle0);
            triangles.push_back(triangle1);

            pPresenter->Render(triangles);
        }
    }
}


void Plane::onEndRender(Message& msg)
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

            asset::TryLocker<MaterialAsset> matLocker(mMaterial());
            asset::TryLocker<ShaderAsset> shaderLocker(mShader());
            if (matLocker && shaderLocker)
            {
                Color4_t currColor = matLocker->GetInputValue<Color4_t>("LineColor", v4::ONE());
                material::AutoReset matReset(*matLocker, "LineColor", mColor * currColor);
                shaderLocker->Render(rContext, *matLocker, GeometryAsset::Asset_t(), boost::bind(&Plane::onRender, this, _1));
            }
        }
    }
}

void Plane::setOption(const std::string& key, const std::string& value)
{
    if (boost::algorithm::iequals(key, std::string("tesselate")))
    {
        mTesselate = ConvertProperty<uint32_t>(value);
    }
    else if (boost::algorithm::iequals(key, std::string("size")))
    {
        mSize = ConvertProperty<float>(value);
    }
    else if (boost::algorithm::iequals(key, std::string("color")))
    {
        mColor = ConvertProperty<Color4_t>(value);
    }
    else if (boost::algorithm::iequals(key, std::string("grid")))
    {
        mbRenderAsGrid = ConvertProperty<bool>(value);
        if (mbRenderAsGrid)
        {
            mShader = mGridNames.first;
            mMaterial = mGridNames.second;
        }
        else
        {
            mShader = mFillNames.first;
            mMaterial = mFillNames.second;
        }
    }
    else if (boost::algorithm::iequals(key, std::string("gridshader")))
    {
        mGridNames = ConvertProperty(value, mGridNames);
        mShader = mGridNames.first;
        mMaterial = mGridNames.second;
    }
    else if (boost::algorithm::iequals(key, std::string("quadshader")))
    {
        mFillNames = ConvertProperty(value, mFillNames);
        mShader = mFillNames.first;
        mMaterial = mFillNames.second;
    }
}


void Plane::getOptions(std::string& options) const
{
    WidgetAsset<Plane>::getOptions(options);
    if (!options.empty())
    {
        options += "; ";
    }
    options += "tesselate = <uint32_t>; size = <float>; color = <Vector4>; grid = <true|false>; gridshader = <shaderName, materialName>; quadshader = <shaderName, materialName>";
}

} // namespace widgets
} // namespace eg

