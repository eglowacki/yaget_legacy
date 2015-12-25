///////////////////////////////////////////////////////////////////////
// BaseRegistrate.h
//
//  Copyright 11/26/2006 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Basic objects types and defines for dll import and export macros
//
//
//  #include "BaseRegistrate.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef BASE_REGISTRATE_H
#define BASE_REGISTRATE_H
#pragma once;

#include "Base.h"

// this needs to be define in BaseRegistrate project when building it
#ifdef REGISTRATE_EXPORTS
    #define DLLIMPEXP_BASEREGISTRATE __declspec(dllexport)
    #define DLLIMPEXP_BASEREGISTRATE_TEMPLATE
#else // using DLL
    #define DLLIMPEXP_BASEREGISTRATE __declspec(dllimport)
    //#define DLLIMPEXP_BASEREGISTRATE_TEMPLATE extern
    #define DLLIMPEXP_BASEREGISTRATE_TEMPLATE
#endif // REGISTRATE_EXPORTS

#define TR_REGISTRATE wxT("Registrate")


#endif // BASE_REGISTRATE_H

