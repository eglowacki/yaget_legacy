#include "Precompiled.h"
#include "Axis.h"
#include "Presenters.h"
#include "Message/Dispatcher.h"
#include "Interface/IRendererTypes.h"
#include "Interface/IRenderContext.h"
#include "IComponentEntity.h"
#include "IComponentModel.h"
#include "IComponentView.h"
#include "ComponentEnumsRender.h"
#include "Presenters.h"
#include "../Renderer.h"
#include "../ErrorHandler.h"
#include "../RenderContext.h"

namespace
{
    using namespace eg;
    using namespace eg::widgets;
    AutoRegisterAssetFactory<Axis> myFactory;

    widgets::AutoRegister<Axis> registerWidget("Axis");

    Vector3 transform(const Matrix44& matrix, const Vector3& pos)
    {
        Vector4 newPos = matrix * Vector4(pos);
        Vector3 trPos(newPos);
        trPos /= newPos.w;
        return trPos;
    }

    Vector4 transform(const Matrix44& matrix, const Vector4& pos)
    {
        Vector4 newPos = matrix * pos;
        Vector4 trPos(newPos);
        trPos.x /= newPos.w;
        trPos.y /= newPos.w;
        trPos.z /= newPos.w;
        return trPos;
    }

    //! Calculate scale to apply to 1 unit to have
    //! rendered exact pixel numbers
    float ScaleByPixels(const IRenderContext& renderContext, float pixelLenght)
    {
        Matrix44 wvpMatrix = renderContext.GetWVPMatrix();

        const Vector2& renderSize = renderContext.GetRenderTargetSize();
        float aspectRatio = renderSize.x / renderSize.y;

        // length of each axis in pixels
        float xSize = pixelLenght / renderSize.x;
        float ySize = xSize * aspectRatio;

        // let's see where camera is and calculate length of axis based on distance from camera to an object
        component::InstanceId camId = component::make(renderContext.GetCameraId(), Hash("View"));
        property::hProp prop(camId);
        Vector3 camPos = prop[view::kPropEyePosition];
        Vector3 camValues = prop[view::kPropView];

        Vector3 centerPos = renderContext.GetWorldMatrix().GetTranslate();

        float d = (centerPos - camPos).length();
        float axisScale = (TanD(camValues.x/2) * d) * ySize;

        return axisScale;
    }

    void createArrow(TrianglePresenter::Triangles_t& triangles, const Color4_t& color, const Matrix44& translatem)
    {
        Vector3 arrowTip = Vector3(0.3f, 0.0f, 0.0f);
        arrowTip = translatem * arrowTip;
        Vector3 arrowBase = Vector3(0.0f, 0.0f, 0.0f);
        arrowBase = translatem * arrowBase;
        float radius = 0.05f;

        const int kNumRadialSubdivisions = 20;
        for (int i = 0; i < kNumRadialSubdivisions; i++)
        {
            float angle = 360.0F * ((float)i / (float)kNumRadialSubdivisions);
            float y = SinD(angle) * radius;
            float z = CosD(angle) * radius;
            Vector3 point0(0, y, z);
            point0 = translatem * point0;

            angle = 360.0F * ((float)(i+1) / (float)kNumRadialSubdivisions);
            y = SinD(angle) * radius;
            z = CosD(angle) * radius;
            Vector3 point1(0, y, z);
            point1 = translatem * point1;

            // sides of cone going around circle base ans tip
            TrianglePresenter::Vertex p0(point0, color, v2::ZERO());  // upper-left
            TrianglePresenter::Vertex p1(point1, color, v2::ZERO());  // upper-right
            TrianglePresenter::Vertex p2(arrowTip, color, v2::ZERO());     // lower-right

            TrianglePresenter::Triangle triangle(p1, p0, p2);
            triangles.push_back(triangle);

            // base of an arrow, reusing vertices from side and center of base
            triangle.P0 = TrianglePresenter::Vertex(point0, color*0.6f, v2::ZERO());
            triangle.P1 = TrianglePresenter::Vertex(point1, color*0.6f, v2::ZERO());
            triangle.P2 = TrianglePresenter::Vertex(arrowBase, color*0.6f, v2::ZERO());
            triangles.push_back(triangle);
        }
    }


    const int kAxisX = 1;
    const int kAxisY = 2;
    const int kAxisZ = 3;
    Matrix44 axisMatrix(const Vector3& pos, float axisScale, int axisType)
    {
        Matrix44 posm = TranslateMatrix44(pos);
        Matrix44 scalem = ScaleMatrix44(axisScale, axisScale, axisScale);
        Matrix44 translatem = m44::Identity();

        if (axisType == kAxisX)
        {
            // do x axis first (to the right)
            translatem = posm * scalem;
        }
        else if (axisType == kAxisZ)
        {
            // translate to be z (into a screen)
            Matrix44 rotatem = RotateDegMatrix44('y', 90);
            rotatem = TransposeMatrix44(rotatem);
            translatem = (posm * scalem) * rotatem;
        }
        else if (axisType == kAxisY)
        {
            // translate to be y (up)
            Matrix44 rotatem = RotateDegMatrix44('z', -90);
            rotatem = TransposeMatrix44(rotatem);
            translatem = (posm * scalem) * rotatem;
        }

        return translatem;
    }



// maps floating point channels (0.f to 1.f range) to D3DCOLOR
//#define COLOR_TO_D3D(r,g,b,a) \
//    D3DCOLOR_ARGB((DWORD)((a)*255.f),(DWORD)((r)*255.f),(DWORD)((g)*255.f),(DWORD)((b)*255.f))

} // namespace


namespace eg {
namespace widgets {

Axis::Axis(const std::string& name)
: WidgetAsset<Axis>(name)
, mShader("Gizmos.fx")
, mMaterial("Gizmos.mat")
, mGeometry("Axis.x")
, mType(atArrows)
{
    Dispatcher& disp = REGISTRATE(Dispatcher);
    mTickConnection = disp[mtype::kFrameTick].connect(boost::bind(&Axis::onTick, this, _1));
}


Axis::~Axis()
{
    mTickConnection.disconnect();
}


void Axis::onTick(eg::Message& /*msg*/)
{
    //float deltaTime = msg.GetValue<float>();
    // here we might fade in or out based on state, like select on, select off, etc.

}


void Axis::onRender(const IRenderContext& renderContext)
{
	if (!(renderContext.GetRenderId() & renderId()))
	{
		return;
	}

    //tArrows, tMove, tRotate, tScale
    if (mType == atArrows)
    {
        if (LinePresenter *pLP = registrate::p_cast<LinePresenter>("Widgets.LinePresenter"))
        {
            LinePresenter::Verticies_t lines; lines.reserve(18);

            float axisScale = ScaleByPixels(renderContext, 180);
            float arrowScale = axisScale/8;

            Vector3 scale = renderContext.GetWorldScale();
            Matrix44 wvpMatrix = renderContext.GetWVPMatrix();
            Vector3 centerPos = renderContext.GetWorldMatrix().GetTranslate();

            // create x axis
            Vector3 xAxis = (v3::XONE() / scale) * axisScale;
            Vector3 xArrow1 = ((Normalized(Vector3(-1, 0, 1)) / scale) * arrowScale) + xAxis;
            Vector3 xArrow2 = ((Normalized(Vector3(-1, 0, -1)) / scale) * arrowScale) + xAxis;
            xArrow1 = transform(wvpMatrix, xArrow1);
            xArrow2 = transform(wvpMatrix, xArrow2);
            xAxis = transform(wvpMatrix, xAxis);

            // create y axis
            Vector3 yAxis = (v3::YONE() / scale) * axisScale;
            Vector3 yArrow1 = ((Normalized(Vector3(0, -1, 1)) / scale) * arrowScale) + yAxis;
            Vector3 yArrow2 = ((Normalized(Vector3(0, -1, -1)) / scale) * arrowScale) + yAxis;
            yArrow1 = transform(wvpMatrix, yArrow1);
            yArrow2 = transform(wvpMatrix, yArrow2);
            yAxis = transform(wvpMatrix, yAxis);

            Vector3 zAxis = (v3::ZONE() / scale) * axisScale;
            Vector3 zArrow1 = ((Normalized(Vector3(-1, 0, -1)) / scale) * arrowScale) + zAxis;
            Vector3 zArrow2 = ((Normalized(Vector3(1, 0, -1)) / scale) * arrowScale) + zAxis;
            zArrow1 = transform(wvpMatrix, zArrow1);
            zArrow2 = transform(wvpMatrix, zArrow2);
            zAxis = transform(wvpMatrix, zAxis);

            centerPos = transform(wvpMatrix, v3::ZERO());

            // x axis
            lines.push_back(LinePresenter::Vertex(centerPos, v4::XONE()));
            lines.push_back(LinePresenter::Vertex(xAxis, v4::XONE()));
            // arrow parts of x
            lines.push_back(LinePresenter::Vertex(xAxis, v4::XONE()));
            lines.push_back(LinePresenter::Vertex(xArrow1, v4::XONE()));
            lines.push_back(LinePresenter::Vertex(xAxis, v4::XONE()));
            lines.push_back(LinePresenter::Vertex(xArrow2, v4::XONE()));

            // y axis
            lines.push_back(LinePresenter::Vertex(centerPos, v4::YONE()));
            lines.push_back(LinePresenter::Vertex(yAxis, v4::YONE()));
            // arrow parts of y
            lines.push_back(LinePresenter::Vertex(yAxis, v4::YONE()));
            lines.push_back(LinePresenter::Vertex(yArrow1, v4::YONE()));
            lines.push_back(LinePresenter::Vertex(yAxis, v4::YONE()));
            lines.push_back(LinePresenter::Vertex(yArrow2, v4::YONE()));

            // z axis
            lines.push_back(LinePresenter::Vertex(centerPos, v4::ZONE()));
            lines.push_back(LinePresenter::Vertex(zAxis, v4::ZONE()));
            // arrow parts of z
            lines.push_back(LinePresenter::Vertex(zAxis, v4::ZONE()));
            lines.push_back(LinePresenter::Vertex(zArrow1, v4::ZONE()));
            lines.push_back(LinePresenter::Vertex(zAxis, v4::ZONE()));
            lines.push_back(LinePresenter::Vertex(zArrow2, v4::ZONE()));

            pLP->Render(lines);
        }
    }
    else if (mType == atMove)
    {
        if (LinePresenter *pLP = registrate::p_cast<LinePresenter>("Widgets.LinePresenter"))
        {
            LinePresenter::Verticies_t lines;// lines.reserve(18);

            float axisScale = ScaleByPixels(renderContext, 180);

            Vector3 scale = renderContext.GetWorldScale();
            Matrix44 wvpMatrix = renderContext.GetWVPMatrix();
            Vector3 centerPos = renderContext.GetWorldMatrix().GetTranslate();

            float startAxis = (0.2f / scale.x) * axisScale;
            float endAxis = (1.0f / scale.x) * axisScale;
            float bracketSize = (0.3f / scale.x) * axisScale;


            Vector3 xlineBeg = transform(wvpMatrix, Vector3(startAxis, 0, 0));
            Vector3 xlineEnd = transform(wvpMatrix, Vector3(endAxis, 0, 0));
            Vector3 xBracketBeg = transform(wvpMatrix, Vector3(bracketSize, 0, 0));
            Vector3 xBracketUp = transform(wvpMatrix, Vector3(bracketSize, bracketSize, 0));
            Vector3 xBracketSide = transform(wvpMatrix, Vector3(bracketSize, 0, bracketSize));

            // create y axis
            Vector3 ylineBeg = transform(wvpMatrix, Vector3(0, startAxis, 0));
            Vector3 ylineEnd = transform(wvpMatrix, Vector3(0, endAxis, 0));
            Vector3 yBracketBeg = transform(wvpMatrix, Vector3(0, bracketSize, 0));
            Vector3 yBracketUp = transform(wvpMatrix, Vector3(bracketSize, bracketSize, 0));
            Vector3 yBracketSide = transform(wvpMatrix, Vector3(0, bracketSize, bracketSize));


            // create y axis
            Vector3 zlineBeg = transform(wvpMatrix, Vector3(0, 0, startAxis));
            Vector3 zlineEnd = transform(wvpMatrix, Vector3(0, 0, endAxis));
            Vector3 zBracketBeg = transform(wvpMatrix, Vector3(0, 0, bracketSize));
            Vector3 zBracketUp = transform(wvpMatrix, Vector3(0, bracketSize, bracketSize));
            Vector3 zBracketSide = transform(wvpMatrix, Vector3(bracketSize, 0, bracketSize));

            lines.push_back(LinePresenter::Vertex(xlineBeg, v4::XONE()));
            lines.push_back(LinePresenter::Vertex(xlineEnd, v4::XONE()));
            lines.push_back(LinePresenter::Vertex(xBracketBeg, v4::XONE()));
            lines.push_back(LinePresenter::Vertex(xBracketUp, v4::XONE()));
            lines.push_back(LinePresenter::Vertex(xBracketBeg, v4::XONE()));
            lines.push_back(LinePresenter::Vertex(xBracketSide, v4::XONE()));

            lines.push_back(LinePresenter::Vertex(ylineBeg, v4::YONE()));
            lines.push_back(LinePresenter::Vertex(ylineEnd, v4::YONE()));
            lines.push_back(LinePresenter::Vertex(yBracketBeg, v4::YONE()));
            lines.push_back(LinePresenter::Vertex(yBracketUp, v4::YONE()));
            lines.push_back(LinePresenter::Vertex(yBracketBeg, v4::YONE()));
            lines.push_back(LinePresenter::Vertex(yBracketSide, v4::YONE()));

            lines.push_back(LinePresenter::Vertex(zlineBeg, v4::ZONE()));
            lines.push_back(LinePresenter::Vertex(zlineEnd, v4::ZONE()));
            lines.push_back(LinePresenter::Vertex(zBracketBeg, v4::ZONE()));
            lines.push_back(LinePresenter::Vertex(zBracketUp, v4::ZONE()));
            lines.push_back(LinePresenter::Vertex(zBracketBeg, v4::ZONE()));
            lines.push_back(LinePresenter::Vertex(zBracketSide, v4::ZONE()));

            pLP->Render(lines);

            if (TrianglePresenter *pPresenter = registrate::p_cast<TrianglePresenter>("Widgets.TrianglePresenter"))
            {
                TrianglePresenter::Triangles_t triangles;

                // do x axis first (to the right)
                Vector3 arrowOffset(0.7f, 0, 0);
                Matrix44 translatem = axisMatrix(arrowOffset, axisScale, kAxisX);
                createArrow(triangles, v4::XONE(), translatem);

                // translate to be z (into a screen)
                translatem = axisMatrix(arrowOffset, axisScale, kAxisZ);
                createArrow(triangles, v4::ZONE(), translatem);

                // translate to be y (up)
                translatem = axisMatrix(arrowOffset, axisScale, kAxisY);
                createArrow(triangles, v4::YONE(), translatem);

                for (TrianglePresenter::Triangles_t::iterator it = triangles.begin(); it != triangles.end(); ++it)
                {
                    Vector3& p0 = (*it).P0.Pos;
                    p0 = transform(wvpMatrix, p0);

                    Vector3& p1 = (*it).P1.Pos;
                    p1 = transform(wvpMatrix, p1);

                    Vector3& p2 = (*it).P2.Pos;
                    p2 = transform(wvpMatrix, p2);
                }

                pPresenter->Render(triangles);
            }
        }
    }
    else if (mType == atRotate)
    {
    }
    else if (mType == atPoint)
    {
        if (LinePresenter *pLP = registrate::p_cast<LinePresenter>("Widgets.LinePresenter"))
        {
            LinePresenter::Verticies_t lines;// lines.reserve(18);

            float axisScale = ScaleByPixels(renderContext, 90);

            Vector3 scale = renderContext.GetWorldScale();
            Matrix44 wvpMatrix = renderContext.GetWVPMatrix();
            Vector3 centerPos = renderContext.GetWorldMatrix().GetTranslate();

            float endAxis = (1.0f / scale.x) * axisScale;

            // all three axis
            Vector3 xlineBeg = transform(wvpMatrix, Vector3(-endAxis, 0, 0));
            Vector3 xlineEnd = transform(wvpMatrix, Vector3(endAxis, 0, 0));

            Vector3 ylineBeg = transform(wvpMatrix, Vector3(0, -endAxis, 0));
            Vector3 ylineEnd = transform(wvpMatrix, Vector3(0, endAxis, 0));

            Vector3 zlineBeg = transform(wvpMatrix, Vector3(0, 0, -endAxis));
            Vector3 zlineEnd = transform(wvpMatrix, Vector3(0, 0, endAxis));

            lines.push_back(LinePresenter::Vertex(xlineBeg, v4::XONE()));
            lines.push_back(LinePresenter::Vertex(xlineEnd, v4::XONE()));

            lines.push_back(LinePresenter::Vertex(ylineBeg, v4::YONE()));
            lines.push_back(LinePresenter::Vertex(ylineEnd, v4::YONE()));

            lines.push_back(LinePresenter::Vertex(zlineBeg, v4::ZONE()));
            lines.push_back(LinePresenter::Vertex(zlineEnd, v4::ZONE()));

            pLP->Render(lines);
        }
    }
}


void Axis::onEndRender(eg::Message& msg)
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
                const IPropertySet& propSet = pEntity->GetPropertyData();
                Vector3 scale = propSet["Scale"];
                rContext.SetWorldScale(scale);
            }

            asset::TryLocker<MaterialAsset> matLocker(mMaterial());
            asset::TryLocker<ShaderAsset> shaderLocker(mShader());
            //asset::TryLocker<GeometryAsset> geomLocker(mGeometry());

            if (matLocker && shaderLocker)// && geomLocker)
            {
                // based on type, select which technique to use from shader
                shaderLocker->Render(rContext, *matLocker, GeometryAsset::Asset_t(), boost::bind(&Axis::onRender, this, _1));
            }
        }
    }
}

void Axis::setOption(const std::string& key, const std::string& value)
{
    if (boost::algorithm::iequals(key, std::string("type")))
    {
        if (boost::algorithm::iequals(value, std::string("arrows")))
        {
            mType = atArrows;
        }
        else if (boost::algorithm::iequals(value, std::string("move")))
        {
            mType = atMove;
        }
        else if (boost::algorithm::iequals(value, std::string("rotate")))
        {
            mType = atRotate;
        }
        else if (boost::algorithm::iequals(value, std::string("scale")))
        {
            mType = atScale;
        }
        else if (boost::algorithm::iequals(value, std::string("point")))
        {
            mType = atPoint;
        }
    }
}


void Axis::getOptions(std::string& options) const
{
    WidgetAsset<Axis>::getOptions(options);
    if (!options.empty())
    {
        options += "; ";
    }
    options += "type = <arrows|move|rotate|scale|point>";
}

} // namespace widgets
} // namespace eg
