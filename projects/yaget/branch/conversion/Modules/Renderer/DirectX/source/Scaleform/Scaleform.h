//////////////////////////////////////////////////////////////////////////
// Scaleform.h
//
//  Copyright 5/3/2009 Edgar Glowacki.
//
// NOTES:
//      Wrapper for scaleform library
//
//
// #include "Scaleform/Scaleform.h"
//
/////////////////////////////////////////////////////////////////////////
//! \file

#ifndef SCALEFORM_SCALEFORM_H
#define SCALEFORM_SCALEFORM_H
#pragma once

#include "Math/Vector.h"
#include "GFxLoader.h"
#include "GRendererD3D9.h"

class GFxMovieDef;
class GFxMovieView;
class GRendererD3D9;
class GFxRenderConfig;


namespace eg
{
    namespace ui
    {

        class Scaleform
        {
        public:
            Scaleform();
            ~Scaleform();

            void Attach(IDirect3DDevice9 *device, D3DPRESENT_PARAMETERS *presentParams, const math::Rect_t& rect);
            void Resize(const math::Rect_t& rect);
            void Render(float deltaTime);

        private:
            GFxSystem mSystem;
            GFxLoader mLoader;
            GPtr<GFxMovieDef> mMovieDef;
            GPtr<GFxMovieView> mMovie;
            GPtr<GRendererD3D9> mRenderer;
            GPtr<GFxRenderConfig> mRenderConfig;
        };


    } // namespace ui
} // namespace eg

#endif // SCALEFORM_SCALEFORM_H

