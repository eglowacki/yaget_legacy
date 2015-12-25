///////////////////////////////////////////////////////////////////////
// ComponentsBase.h
//
//  Copyright 10/15/2005 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Basic objects types and defines for dll import and export macros
//
//
//  #include "ComponentsBase.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef COMPONENTS_BASE_H
#define COMPONENTS_BASE_H

#include "Base.h"

// this needs to be define in Components project
#ifdef MAKINGDLL_COMPONENTS
    #define DLLIMPEXP_COMPONENTS __declspec(dllexport)
#else // using DLL
    #define DLLIMPEXP_COMPONENTS __declspec(dllimport)
#endif // MAKINGDLLCOMPONENTS

//! This is our log
#define TR_COMPONENTS _T("Components")


#endif // COMPONENTS_BASE_H

