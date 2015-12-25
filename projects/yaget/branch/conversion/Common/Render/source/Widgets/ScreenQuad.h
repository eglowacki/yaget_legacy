/////////////////////////////////////////////////////////////////////
// ScreenQuad.h
//
//  Copyright 8/24/2008 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Provides simple ScreenQuad widget
//
//
//  #include "Widgets/ScreenQuad.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef WIDGETS_SCREEN_QUAD_H
#define WIDGETS_SCREEN_QUAD_H
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
        class ScreenQuad : public WidgetAsset<ScreenQuad>
        {
        public:
            static const guid_t kType = 0x75bc6e32;

            ScreenQuad(const std::string& name);
            virtual ~ScreenQuad();

            //! from IAsset
            virtual guid_t Type() const {return ScreenQuad::kType;}

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

            ShaderAsset_t mShader;
            MaterialAsset_t mMaterial;
            //! input color for material
            Vector4 mMatColor;
            //! position to make four corners (left-top, right-bottom
            Vector4 mPosition;
        };
    } // namespace widgets

} // namespace eg

#endif // WIDGETS_SCREEN_QUAD_H


