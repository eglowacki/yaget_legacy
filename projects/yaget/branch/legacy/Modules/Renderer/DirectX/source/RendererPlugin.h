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

#include "IRendererPlugin.h"
#include "Asset/IAsset.h"
#include "VertexBuffer.h"
#include <vector>
#include <atlbase.h>
#include <D3dx9core.h>

#include "GFxDrawText.h"
#include "GFxFontProviderWin32.h"

namespace eg
{
    class AssetLoadSystem;

    class RendererPlugin : public IRendererPlugin
    {
    public:
        RendererPlugin(IAsset::Token script, AssetLoadSystem& als);
        virtual ~RendererPlugin();

        uint32_t Generate(VertexBuffer::Generator_t generator, size_t vertexSize, D3DPRIMITIVETYPE type, D3DVERTEXELEMENT9 *layout);
        void Generate(uint32_t token);
        void Render(uint32_t token);

        // from IRendererPlugin
        virtual view_t NewView(app::handle_t handle);
        virtual void DestroyView(view_t viewToken);
        virtual math::Rect_t ResizeView(view_t viewToken);
        virtual math::Rect_t GetViewSize(view_t viewToken) const;
        virtual void RenderView(view_t viewToken, uint32_t clearFlags, const Color4_t& clearColor, boost::function<void()> callback);

        // from IPluginObject
        virtual const char *GetName() const {return "RendererDX9";}
        virtual const Version GetVersion() const;

    private:
        bool createDevice(handle_t handle);
        bool isValidView(view_t viewToken) const {return viewToken < mSwapChains.size();}
        bool activateView(view_t viewToken);

        CComPtr<IDirect3D9> mD3D;
        struct Device
        {
            Device() {ZeroMemory(&presentParams, sizeof(presentParams));}

            CComPtr<IDirect3DDevice9> device;
            CComPtr<IDirect3DSurface9> depthSurface;
            CComPtr<IDirect3DSurface9> renderSurface;
            D3DPRESENT_PARAMETERS presentParams;
        };
        Device mDevice;

        struct SwapChain
        {
            SwapChain() : errorTries(5), handle(0) {}

            math::Rect_t getViewSize() const;
            bool create(CComPtr<IDirect3DDevice9> device);
            bool activate(CComPtr<IDirect3DDevice9> device);

            CComPtr<IDirect3DSwapChain9> buffer;
            CComPtr<IDirect3DSurface9> dsBuffer;
            app::handle_t handle;
            int errorTries;
        };

        typedef std::vector<SwapChain> SwapChains_t;
        SwapChains_t mSwapChains;
        view_t mCurrentView;

        int mErrorTries;
        IAsset::Token mUI;
        AssetLoadSystem& mAls;

        VertexBuffer_t mVertexBuffer;
        IAsset::Token mShaderFactory;
    };



} // namespace eg


#endif // RENDERER_PLUGIN_H

