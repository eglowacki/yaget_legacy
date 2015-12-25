///////////////////////////////////////////////////////////////////////
// ScriptBase.h
//
//  Copyright 05/30/2008 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Basic Script types and defines for dll import and export macros
//
//
//  #include "ScriptBase.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef SCRIPT_BASE_H
#define SCRIPT_BASE_H

#include "Base.h"

// this needs to be define in $safeitemname$ project
#ifdef MAKINGDLL_SCRIPT
#    define DLLIMPEXP_SCRIPT __declspec(dllexport)
#    define DLLIMPEXP_DATA_SCRIPT(type) __declspec(dllexport) type
#else // using DLL
#    define DLLIMPEXP_SCRIPT __declspec(dllimport)
#    define DLLIMPEXP_DATA_SCRIPT(type) __declspec(dllimport) type
#endif

//! This is our log
#define TR_SCRIPT _T("Script")

#endif // SCRIPT_BASE_H

