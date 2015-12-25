///////////////////////////////////////////////////////////////////////
// CollisionBase.h
//
//  Copyright 09/13/2008 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Basic Collision types and defines for dll import and export macros
//
//
//  #include "CollisionBase.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef COLLISION_BASE_H
#define COLLISION_BASE_H

#include "Base.h"

// this needs to be define in $safeitemname$ project
#ifdef MAKINGDLL_COLLISION
#    define DLLIMPEXP_COLLISION __declspec(dllexport)
#    define DLLIMPEXP_DATA_COLLISION(type) __declspec(dllexport) type
#else // using DLL
#    define DLLIMPEXP_COLLISION __declspec(dllimport)
#    define DLLIMPEXP_DATA_COLLISION(type) __declspec(dllimport) type
#endif

//! This is our log
#define TR_COLLISION _T("Collision")

#endif // COLLISION_BASE_H

