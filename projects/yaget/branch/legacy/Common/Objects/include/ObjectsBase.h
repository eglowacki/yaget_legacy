///////////////////////////////////////////////////////////////////////
// ObjectsBase.h
//
//  Copyright 10/15/2005 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Basic objects types and defines for dll import and export macros
//
//
//  #include "ObjectsBase.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef OBJECTS_BASE_H
#define OBJECTS_BASE_H

#include "Base.h"

// this needs to be define in Objects project
#ifdef MAKINGDLL_OBJECTS
    #define DLLIMPEXP_OBJECTS __declspec(dllexport)
    //#define DLLIMPEXP_DATA_OBJECTS(type) __declspec(dllexport) type
    #define DLLIMPEXP_TEMPLATE
#else // using DLL
    #define DLLIMPEXP_OBJECTS __declspec(dllimport)
    //#define DLLIMPEXP_DATA_OBJECTS(type) __declspec(dllimport) type
    #define DLLIMPEXP_TEMPLATE extern
#endif // MAKINGDLL_OBJECTS

//! This is our log
#define TR_OBJECTS _T("Objects")


#endif // OBJECTS_BASE_H

