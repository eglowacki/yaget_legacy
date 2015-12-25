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
namespace yaget {}

/*!
\mainpage YAGET Engine
This is full MMO capable 3D engine (Yet Another Game Engine Technology)

- \subpage overview "Overview"
- \subpage guide "Guide"
*/


#include <stddef.h>
#include <boost/cstdint.hpp>

//! Bring built in typedefs into a global namespace
using boost::int8_t;
using boost::uint8_t;
using boost::int16_t;
using boost::uint16_t;
using boost::int32_t;
using boost::uint32_t;
using boost::int64_t;
using boost::uint64_t;

//! Define few of our own
typedef char        char8_t;
typedef uint32_t    guid_t;
typedef long        long_t;


#endif // INCLUDE_BASE_H









