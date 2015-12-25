/////////////////////////////////////////////////////////////////////
// Axis.h
//
//  Copyright 8/2/2008 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Provides axis widget, where each exis
//      is rendered rgb = xyz
//
//
//  #include "Widgets/Axis.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef WIDGETS_AXIS_H
#define WIDGETS_AXIS_H
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
        class Axis : public WidgetAsset<Axis>
        {
        public:
            static const guid_t kType = 0xb5a0622e;

            Axis(const std::string& name);
            virtual ~Axis();

            //! from IAsset
            virtual guid_t Type() const {return Axis::kType;}

        private:
            //! from WidgetAsset
            virtual void setOption(const std::string& key, const std::string& value);
            virtual void getOptions(std::string& options) const;

            //! Called by Shader as a callback method to actually execute render code
            void onRender(const IRenderContext& renderContext);

            //! Callbacks to render bounding box
            virtual void onEndRender(eg::Message& msg);
            //! process tick frame
            void onTick(eg::Message& msg);

            //! This widget provides several different axis rendering
            enum AxisType {atArrows, atMove, atRotate, atScale, atPoint};
            AxisType mType;
            boost::signals::connection mTickConnection;

            ShaderAsset_t mShader;
            MaterialAsset_t mMaterial;
            GeometryAsset_t mGeometry;
        };

    } // namespace widgets

} // namespace eg

#endif // WIDGETS_AXIS_H

