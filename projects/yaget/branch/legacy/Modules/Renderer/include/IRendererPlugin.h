/////////////////////////////////////////////////////////////////////////
// IRendererPlugin.h
//
//  Copyright 4/26/2009 Edgar Glowacki.
//
// NOTES:
//
//
// #include "IRendererPlugin.h"
//
/////////////////////////////////////////////////////////////////////////
//! \file

#ifndef NTERFACE_IRENDERER_PLUGIN_H
#define NTERFACE_IRENDERER_PLUGIN_H
#pragma once

#include "Plugin/IPluginObject.h"
#include "App/AppUtilities.h"
#include "Math/Vector.h"
#include <boost/function.hpp>

namespace eg
{
    class IRendererPlugin : public IPluginObject
    {
    public:
        typedef uint32_t view_t;
        enum Clear {Color = 1, ZBuffer = 2, Stencil = 4};

        virtual view_t NewView(app::handle_t handle) = 0;
        virtual void DestroyView(view_t viewToken) = 0;
        virtual math::Rect_t ResizeView(view_t viewToken) = 0;
        virtual math::Rect_t GetViewSize(view_t viewToken) const = 0;
        virtual void RenderView(view_t viewToken, uint32_t clearFlags, const Color4_t& clearColor, boost::function<void()> callback) = 0;
    };


    namespace view
    {
        inline bool isValid(IRendererPlugin::view_t v) {return v != (IRendererPlugin::view_t)-1;}
    } // namespace view


} // namespace eg


#define tr_render "render"

#endif // NTERFACE_IRENDERER_PLUGIN_H

