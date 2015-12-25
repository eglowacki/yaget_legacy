/////////////////////////////////////////////////////////////////////
// Ray.h
//
//  Copyright 2/22/2008 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Simple Ray object with origin and direction
//
//
//  #include "Math/Ray.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef MATH_RAY_H
#define MATH_RAY_H
#pragma once

#include "Vector.h"

namespace eg
{
    /*!
    This simple class is used to represent ray, which has Origin and direction.
    Direction is normalized.
    */
    class Ray
    {
    public:
        Ray() : Origin(v3::ZERO()), Dir(v3::ZONE())
        {}

        Ray(const Vector3& o, const Vector3& d) : Origin(o), Dir(d)
        {}

        Vector3 Origin;
        Vector3 Dir;
    };

} // namespace eg

#endif // MATH_RAY_H

