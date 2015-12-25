///////////////////////////////////////////////////////////////////////
// ReplicateBase.h
//
//  Copyright 12/01/2007 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Basic Replicate types and defines for dll import and export macros
//
//
//  #include "ReplicateBase.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef REPLICATE_BASE_H
#define REPLICATE_BASE_H

#include "Base.h"

// this needs to be define in Replicate project
#ifdef MAKINGDLL_REPLICATE
#    define DLLIMPEXP_REPLICATE __declspec(dllexport)
#    define DLLIMPEXP_DATA_REPLICATE(type) __declspec(dllexport) type
#else // using DLL
#    define DLLIMPEXP_REPLICATE __declspec(dllimport)
#    define DLLIMPEXP_DATA_REPLICATE(type) __declspec(dllimport) type
#endif

//! This is our log
#define TR_REPLICATE _T("Replicate")

#endif // REPLICATE_BASE_H

