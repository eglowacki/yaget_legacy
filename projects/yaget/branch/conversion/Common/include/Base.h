///////////////////////////////////////////////////////////////////////
// Base.h
//
//  Copyright 10/1/2005 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Basic #define and typedef, which is inlcuded in all the Muck source files
//
//
//  #include "Base.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef INCLUDE_BASE_H
#define INCLUDE_BASE_H
#pragma once

//! Entire YAGET engine is encapsulated in this namespace
namespace eg {}

/*!
\mainpage YAGET Engine
This is 3D engine (Yet Another Game Engine Technology)

- \subpage overview "Overview"
- \subpage guide "Guide"
*/


#include <stddef.h>
#include <cstdint>

//! Define few of our own
typedef char        char8_t;
typedef uint32_t    guid_t;
typedef long        long_t;


//! Used to provide ability to export functions from DLL
//! Provide header file declaration of functions to be exported
//! \code
//! foo_file.h
//! YAGET_BASE_SYMBOL boost::any Get(const char *pName);
//! \endcode
//!
//! Add implementation of those functions to DLL project
//! which will be link implicitly with other projects.
//! For example, Utility project provides header files
//! and Registry project has implementation which all
//! other yaget projects had implicit dependency on it.
//!
//! \code
//! foo_file.cpp
//! __declspec(dllexport) boost::any Get(const char *pName)
//! {
//!   // ... code snipped
//! }
//! \endcode
//!
//! And not define YAGET_BASE_EXPORTS project wide
//! Project Properties is recommended.

/*
#ifdef YAGET_BASE_EXPORTS
#define YAGET_BASE_SYMBOL __declspec(dllexport)
#else // using DLL
#define YAGET_BASE_SYMBOL __declspec(dllimport)
#endif // YAGET_BASE_EXPORTS


#ifdef UTIL_CLASS_EXPORTS
#define DLLIMPEXP_UTIL_CLASS __declspec(dllexport)
#else // using DLL
#define DLLIMPEXP_UTIL_CLASS
#endif // UTIL_EXPORTS

//! There are some functions which should be provided
//! by main executable app, and it needs to define this
//! symbol project wide
#ifdef YAGET_EXPORT_GLOBAL_FUNCTIONS
#define YAGET_GLOBAL_FUNCTION __declspec(dllexport)
#else // used by *.exe
#define YAGET_GLOBAL_FUNCTION __declspec(dllimport)
#endif // YAGET_EXPORT_GLOBAL_FUNCTIONS
*/

#define DLLIMPEXP_UTIL_CLASS

namespace eg { namespace file {

    static const char *sep = "/";

}} // namespace file // namespace eg


#define tr_util "Utility"
#define tr_script "Script"

#endif // INCLUDE_BASE_H









