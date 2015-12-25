/////////////////////////////////////////////////////////////////////
// Ray.h
//
//  Copyright 2/23/2008 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Ray widget, using origin from Entity component
//      and internal direction vector all in world coordinates
//
//
//  #include "Widgets/Ray.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef WIDGETS_RAY_H
#define WIDGETS_RAY_H
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
        class Ray : public WidgetAsset<Ray>
        {
        public:
            static const guid_t kType = 0x14cc621d;

            Ray(const std::string& name);
            virtual ~Ray();

            //! from IAsset
            virtual guid_t Type() const {return Ray::kType;}

        private:
            //! from WidgetAsset
            virtual void setOption(const std::string& key, const std::string& value);
            virtual void getOptions(std::string& options) const;

            //! Called by Shader as a callback method to actually execute render code
            void onRender(const IRenderContext& renderContext);

            //! Calbacks to render bounding box
            virtual void onEndRender(Message& msg);
            //! process tick frame
            void onTick(eg::Message& msg);

            boost::signals::connection mTickConnection;

            //! shader and material to render ray line
            ShaderAsset_t mShader;
            MaterialAsset_t mMaterial;
            //! Line color for this ray. This is applied uniformly
            //! \note how do we allow setting color per line point?
            Color4_t mLineColor;
            //! Direction of this rya. Origin is extracted from Entity component
            //! This allways will be normalized
            Vector3 mDirection;
            //! length of this direction. Only used in visualization
            float mLength;
        };
    } // namespace widgets
} // namespace eg

#endif // WIDGETS_RAY_H

