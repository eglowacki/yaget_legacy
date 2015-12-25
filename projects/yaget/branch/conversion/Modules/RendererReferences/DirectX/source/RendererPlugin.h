/////////////////////////////////////////////////////////////////////////
// RendererPlugin.h
//
//  Copyright 4/26/2009 Edgar Glowacki.
//
// NOTES:
//      Concrete implementation of Renderer
//
//
// #include "RendererPlugin.h"
//
/////////////////////////////////////////////////////////////////////////
//! \file

#ifndef RENDERER_PLUGIN_H
#define RENDERER_PLUGIN_H
#pragma once

#include "Interface/IRendererPlugin.h"
#include <atlbase.h>
#include <D3dx9core.h>

namespace eg
{
    class RendererPlugin : public IRendererPlugin
    {
    public:
        RendererPlugin(const Environment *environment);
        virtual ~RendererPlugin();

        // from IRendererPlugin
        virtual view_t NewView(handle_t handle);
        virtual void DestroyView(view_t handle);
        // from IPluginObject
        virtual const char *GetName() const {return "RendererDX9";}
        virtual const Version GetVersion() const;

    private:
        bool createDevice(handle_t handle);

        CComPtr<IDirect3D9> mD3D;
        CComPtr<IDirect3DDevice9> mD3DDevice;
        const Environment *mEnvironment;
    };



} // namespace eg


#endif // RENDERER_PLUGIN_H

