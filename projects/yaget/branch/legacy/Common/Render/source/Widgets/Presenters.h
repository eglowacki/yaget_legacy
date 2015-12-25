/////////////////////////////////////////////////////////////////////
// Presenters.h
//
//  Copyright 2/23/2008 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Provides variety of objects which are used to
//      render different types of widgets asset which are objects used
//      in debugging and edit/visualization rendering.
//      Each widget asset is responsible for invoking correct
//      presenters with appropriate parameters
//
//  #include "Widgets/Presenters.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef WIDGETS_PRESENTERS_H
#define WIDGETS_PRESENTERS_H
#pragma once

#include "RenderBase.h"
#include "Math/Vector.h"
#include "MessageInterface.h"
#include <vector>
#include <boost/signal.hpp>

namespace eg
{
    namespace widgets
    {

        //! This Presenter provides line rendering facility
        //! where for each call to Render it will actually render those lines
        //! All render state including any shaders and materials must be set before
        //! It also it's assumed that any matrices are already set to appropriate results.
        //!
        //! Each individual BoundingBox object should call Render method
        //! in kEndRenderFrame event to have it render over entire scene if that desirable.
        class LinePresenter
        {
        public:
            //! Each line point is represented by this structure
            struct Vertex
            {
                Vertex() 
                : Pos(v3::ZERO()) 
                , Color(0xFFFFFFFF) 
                {}

                Vertex(const Vector3& pos) 
                : Pos(pos) 
                , Color(0xFFFFFFFF) 
                {}

                Vertex(const Vector3& pos, const Color4_t& color) 
                : Pos(pos) 
                , Color(D3DCOLOR_COLORVALUE(color.x, color.y, color.z, color.w)) 
                {}

                Vector3 Pos;
                DWORD Color;
            };

            typedef std::vector<Vertex> Verticies_t;

            LinePresenter();
            ~LinePresenter();

            void Render(const Verticies_t& lines);

        private:
            void onVideoReset(Message& msg);
            boost::signals::connection mVideoResetConnection;

            CComPtr<IDirect3DVertexBuffer9> mLineBuffer;
            uint32_t mNextVertexData;
        };


        //! Renders triangle list.
        class TrianglePresenter
        {
        public:
            //! Used for triangle points
            struct Vertex
            {
                Vertex() 
                : Pos(v3::ZERO())
                , Color(0xFFFFFFFF)
                , UV(v2::ZERO())
                {}

                Vertex(const Vector3& pos, const Color4_t& color, const Vector2& uv)
                : Pos(pos) 
                , Color(D3DCOLOR_COLORVALUE(color.x, color.y, color.z, color.w))
                , UV(uv)
                {}

                Vector3 Pos;
                DWORD Color;
                Vector2 UV;
            };

            // represents one triangle
            struct Triangle
            {
                Triangle(const Vertex& p0, const Vertex& p1, const Vertex& p2)
                : P0(p0)
                , P1(p1)
                , P2(p2)
                {}

                Vertex P0, P1, P2;
            };


            typedef std::vector<Triangle> Triangles_t;

            TrianglePresenter();
            ~TrianglePresenter();

            void Render(const Triangles_t& triangles);

        private:
            void onVideoReset(Message& msg);
            boost::signals::connection mVideoResetConnection;

            CComPtr<IDirect3DVertexBuffer9> mTriangleBuffer;
            uint32_t mNextVertexData;
        };


    } // namespace widgets
} // namespace eg

#endif // WIDGETS_PRESENTERS_H

