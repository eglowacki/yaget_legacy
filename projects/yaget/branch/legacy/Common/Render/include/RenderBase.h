///////////////////////////////////////////////////////////////////////
// RenderBase.h
//
//  Copyright 10/8/2005 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Basic render types and defines for dll import and export macros
//
//
//  #include "RenderBase.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef RENDER_BASE_H
#define RENDER_BASE_H

#include "Base.h"

// this needs to be define in Render project
#ifdef MAKINGDLL_RENDER
#    define DLLIMPEXP_RENDER __declspec(dllexport)
#    define DLLIMPEXP_DATA_RENDER(type) __declspec(dllexport) type
#else // using DLL
#    define DLLIMPEXP_RENDER __declspec(dllimport)
#    define DLLIMPEXP_DATA_RENDER(type) __declspec(dllimport) type
#endif

//! This is our log
#define TR_RENDER _T("Renderer")
#define TR_RENDER_RESOURCE _T("RendererResource")

#endif // RENDER_BASE_H

