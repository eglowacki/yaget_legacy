/////////////////////////////////////////////////////////////////////
// SplashCamera.h
//
//  Copyright 12/21/2008 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Provides camera when there is no other one
//      It is alos used by Pong project and as such
//      it relise on headers included in main.
//
//  #include "SplashCamera.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef SPLASH_CAMERA_H
#define SPLASH_CAMERA_H
#pragma once

#include "Base.h"

// forward declarations
namespace eg { class Dispatcher; }

namespace pong
{
    //! This will keep all needed components to display splash screen
    //! and/or clearing main window
    //! It responds to this two events
    //! mtype::kComponentNew - remove splash camera
    //! mtype::kComponentDelete - will add splash camera
    class SplashCamera
    {
    public:
        SplashCamera(eg::Dispatcher& dispatcher)
        : mCameraId(eg::component::make(eg::idspace::get_burnable(), eg::ComponentType::kView))
        , mClearColor(0, 0, 0.2f, 1)
        , mTimeUnit(0.35f)
        , mDispatcher(dispatcher)
        {
            using namespace eg;

            component::newc("SplashCamera", mCameraId);
            wxLogTrace("pong", "Splash Camera created.");

            property::hProp prop(mCameraId);
            prop[view::kPropClearColor] = mClearColor;
            prop[view::kPropClearFlags] = IComponentView::csfAllBuffer|IComponentView::csfOnlyClear;

            mDispatcher[mtype::kComponentNew].connect(boost::ref(*this));
            mDispatcher[mtype::kFrameTick].connect(boost::ref(*this));
        }

        ~SplashCamera()
        {
        }

        void operator()(const eg::Message& msg)
        {
            using namespace eg;

            if (msg.mType == mtype::kComponentNew && msg.GetValue<component::InstanceId>().second == ComponentType::kView)
            {
                // we need to remove our camera
                // and then connect to kComponentDelete, so when there is no more camera left
                // we'll put up our splash camera
                mDispatcher[mtype::kComponentNew].disconnect(boost::ref(*this));
                mDispatcher[mtype::kFrameTick].disconnect(boost::ref(*this));

                component::freec(mCameraId);
                wxLogTrace("pong", "Splash Camera destroyed.");

                mDispatcher[mtype::kComponentDelete].connect(boost::ref(*this));
            }
            else if (msg.mType == mtype::kComponentDelete && msg.GetValue<component::InstanceId>().second == ComponentType::kView)
            {
                // let's seeif there are any more view components left
                ComponentListCallback<IComponentView> viewComponents;
                if (viewComponents.Size() == 1)
                {
                    // there is no more view components left
                    mDispatcher[mtype::kComponentDelete].disconnect(boost::ref(*this));

                    component::newc("SplashCamera", mCameraId);
                    wxLogTrace("pong", "Splash Camera created.");

                    property::hProp prop(mCameraId);
                    prop[view::kPropClearColor] = mClearColor;
                    prop[view::kPropClearFlags] = IComponentView::csfAllBuffer|IComponentView::csfOnlyClear;

                    // make sure that we connect to new message after we already created
                    // view component
                    mDispatcher[mtype::kComponentNew].connect(boost::ref(*this));
                    mDispatcher[mtype::kFrameTick].connect(boost::ref(*this));
                }
            }
            else if (msg.mType == mtype::kFrameTick)
            {
                float deltaTime = msg.GetValue<float>();
                float modifier = mTimeUnit.Tick(deltaTime);
                math::SmoothStepType lerpType(false, true, 0.5f);
                modifier = lerpType.GetValue(0.2f, 0.5f, modifier);

                mClearColor.z = modifier;
                property::hProp prop(mCameraId);
                prop[view::kPropClearColor] = mClearColor;
            }
        }

    private:
        eg::component::InstanceId mCameraId;
        eg::Color4_t mClearColor;
        eg::math::TimeUnit mTimeUnit;
        eg::Dispatcher& mDispatcher;
    };

} // namespace pong

#endif // SPLASH_CAMERA_H

