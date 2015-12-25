/////////////////////////////////////////////////////////////////////
// Plane.h
//
//  Copyright 8/3/2008 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Provides simple plane widget
//      which can be rendered as a line grid
//      or filled triangles
//
//
//  #include "Widgets/Plane.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef WIDGETS_PLANE_H
#define WIDGETS_PLANE_H
#pragma once

#include "RenderBase.h"
#include "Widget/WidgetAsset.h"
#include "Math/Vector.h"
#include "../GeometryAsset.h"
#include "../ShaderAsset.h"
#include "../MaterialAsset.h"


namespace eg
{
    namespace widgets
    {
        class Plane : public WidgetAsset<Plane>
        {
        public:
            static const guid_t kType = 0x95a0d5b3;

            Plane(const std::string& name);
            virtual ~Plane();

            //! from IAsset
            virtual guid_t Type() const {return Plane::kType;}

        private:
            //! from WidgetAsset
            virtual void setOption(const std::string& key, const std::string& value);
            virtual void getOptions(std::string& options) const;

            //! Called by Shader as a callback method to actually execute render code
            void onRender(const IRenderContext& renderContext);

            //! Calbacks to render bounding box
            virtual void onEndRender(eg::Message& msg);
            //! process tick frame
            void onTick(eg::Message& msg);

            boost::signals::connection mTickConnection;

            bool mbRenderAsGrid;
            uint32_t mTesselate;
            float mSize;
            Color4_t mColor;

            ShaderAsset_t mShader;
            MaterialAsset_t mMaterial;

            //! This provides names for shader = first and material = second
            typedef std::pair<std::string, std::string> AssetNames_t;
            //! shader and material used for grid rendering
            AssetNames_t mGridNames;
            //! shader and material used for quad rendering
            AssetNames_t mFillNames;
        };
    } // namespace widgets
} // namespace eg

#endif // WIDGETS_PLANE_H


