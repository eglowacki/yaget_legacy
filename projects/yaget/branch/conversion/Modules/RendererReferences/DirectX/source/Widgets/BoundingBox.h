/////////////////////////////////////////////////////////////////////
// BoundingBox.h
//
//  Copyright 2/9/2008 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      This provides BoundingBox widget which renders bounding
//      box using lines to be rendered around object.
//
//
//  #include "Windgets/BoundingBox.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef WINDGETS_BOUNDING_BOX_H
#define WINDGETS_BOUNDING_BOX_H
#pragma once

#include "RenderBase.h"
#include "Widget/WidgetAsset.h"
#include "ObjectID.h"
#include "../GeometryAsset.h"
#include "../ShaderAsset.h"
#include "../MaterialAsset.h"

namespace eg
{
    namespace widgets
    {
        class BoundingBox : public WidgetAsset<BoundingBox>
        {
        public:
            //typedef asset::UserData U;
            static const guid_t kType = 0x54b8c7ff;

            BoundingBox(const std::string& name);
            virtual ~BoundingBox();

            //! from IAsset
            virtual guid_t Type() const {return BoundingBox::kType;}

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

            Color4_t mLineColor;
            GeometryAsset::BoundingBox_t mBoundingBox;

            ShaderAsset_t mShader;
            MaterialAsset_t mMaterial;

            enum State
            {
                sNone,      ///< keep current state
                sFading,    ///< keep fading line color using alpah channel
                sFlashing   ///< keep flashing 
            };

            State mCurrState;
            //! Used in fading in bounding box
            float mFade;
            float mFadeTimout;
        };


    } // namespace widgets
} // namespace eg

#endif // WINDGETS_BOUNDING_BOX_H

