///////////////////////////////////////////////////////////////////////
// EditorBase.h
//
//  Copyright 10/15/2005 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Basic objects types and defines for dll import and export macros
//
//
//  #include "EditorBase.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef EDITOR_BASE_H
#define EDITOR_BASE_H

#include "Base.h"

// this needs to be define in Editor project
#ifdef MAKINGDLL_EDITORFRAME
    #define DLLIMPEXP_EDITORFRAME __declspec(dllexport)
#else // using DLL
    #define DLLIMPEXP_EDITORFRAME __declspec(dllimport)
#endif // MAKINGDLL_EDITORFRAME

//! This is our log
#define TR_EDITOR wxT("Editor")


#endif // EDITOR_BASE_H

