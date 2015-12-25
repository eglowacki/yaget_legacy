/////////////////////////////////////////////////////////////////////
// WireFrame.h
//
//  Copyright 9/10/2008 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Provides rendering of wire frame mode of any geometry
//
//  #include "Widgets/WireFrame.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef WIDGETS_WIRE_FRAME_H
#define WIDGETS_WIRE_FRAME_H
#pragma once

#include "RenderBase.h"
#include "Widget/WidgetAsset.h"
#include "ObjectID.h"
#include "Math/Vector.h"
#include "../GeometryAsset.h"
#include "../ShaderAsset.h"
#include "../MaterialAsset.h"


namespace eg
{
    namespace widgets
    {
        class WireFrame : public WidgetAsset<WireFrame>
        {
        public:
            static const guid_t kType = 0xf5d3f398;

            WireFrame(const std::string& name);
            virtual ~WireFrame();

            //! from IAsset
            virtual guid_t Type() const {return WireFrame::kType;}

        private:
            //! from WidgetAsset
            virtual void setOption(const std::string& key, const std::string& value);
            virtual void getOptions(std::string& options) const;

            //! Calbacks to render bounding box
            virtual void onEndRender(Message& msg);
            //! process tick frame
            void onTick(eg::Message& msg);

            boost::signals::connection mTickConnection;

            //! shader and material to render ray line
            ShaderAsset_t mShader;
            MaterialAsset_t mMaterial;
            //! Line color for this wire frame mode. This is applied uniformly
            Color4_t mLineColor;
        };
    } // namespace widgets
} // namespace eg

#endif // WIDGETS_WIRE_FRAME_H

