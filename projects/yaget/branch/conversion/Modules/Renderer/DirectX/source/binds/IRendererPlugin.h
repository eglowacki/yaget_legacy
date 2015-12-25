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

namespace eg
{
    class IRendererPlugin : public IPluginObject
    {
    public:
        typedef uint32_t view_t;

        virtual view_t NewView(app::handle_t handle) = 0;
        virtual void DestroyView(view_t handle) = 0;
    };


    namespace view
    {
        inline bool isValid(IRendererPlugin::view_t v) {return v != (IRendererPlugin::view_t)-1;}
    } // namespace view


} // namespace eg


#define tr_render "render"

#endif // NTERFACE_IRENDERER_PLUGIN_H

