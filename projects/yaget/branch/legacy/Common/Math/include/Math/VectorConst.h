///////////////////////////////////////////////////////////////////////
// VectorConst.h
//
//  Copyright 11/6/2005 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Const Vector types
//
//
//  #include <Math/VectorConst.h>
//
//////////////////////////////////////////////////////////////////////
//! \file


#ifndef MATH_VECTOR_CONST_H
#define MATH_VECTOR_CONST_H

#ifndef MATH_VECTOR_INCLUDE_IMPLEMENTATION
    #error "Do not include this file explicitly."
#endif // MATH_VECTOR_INCLUDE_IMPLEMENTATION


namespace eg
{
    //typedef Vector4 Quaternion;

    namespace v2
    {
        Vector2 ZERO();
        Vector2 ONE();
        Vector2 XONE();
        Vector2 YONE();

    } // namespace V2

    namespace v3
    {
        Vector3 ZERO();
        Vector3 ONE();
        Vector3 XONE();
        Vector3 YONE();
        Vector3 ZONE();

    } // namespace V3

    namespace v4
    {
        Vector4 ZERO();
        Vector4 ONE();
        Vector4 XONE();
        Vector4 YONE();
        Vector4 ZONE();
        Vector4 WONE();
    } // namespace V4

    // inline implementation
    //----------------------------------------------------------------------
    // Vector2
    //----------------------------------------------------------------------
    inline Vector2 v2::ZERO()
    {
        static Vector2 value(0, 0);
        return value;
    }

    inline Vector2 v2::ONE()
    {
        static Vector2 value(1, 1);
        return value;
    }

    inline Vector2 v2::XONE()
    {
        static Vector2 value(1, 0);
        return value;
    }

    inline Vector2 v2::YONE()
    {
        static Vector2 value(0, 1);
        return value;
    }
    //----------------------------------------------------------------------
    // Vector2 End
    //----------------------------------------------------------------------

    //----------------------------------------------------------------------
    // Vector3
    //----------------------------------------------------------------------
    inline Vector3 v3::ZERO()
    {
        static Vector3 value(0, 0, 0);
        return value;
    }

    inline Vector3 v3::ONE()
    {
        static Vector3 value(1, 1, 1);
        return value;
    }

    inline Vector3 v3::XONE()
    {
        static Vector3 value(1, 0, 0);
        return value;
    }

    inline Vector3 v3::YONE()
    {
        static Vector3 value(0, 1, 0);
        return value;
    }

    inline Vector3 v3::ZONE()
    {
        static Vector3 value(0, 0, 1);
        return value;
    }
    //----------------------------------------------------------------------
    // Vector3 End
    //----------------------------------------------------------------------

    //----------------------------------------------------------------------
    // Vector4
    //----------------------------------------------------------------------
    inline Vector4 v4::ZERO()
    {
        static Vector4 value(0, 0, 0, 0);
        return value;
    }

    inline Vector4 v4::ONE()
    {
        static Vector4 value(1, 1, 1, 1);
        return value;
    }

    inline Vector4 v4::XONE()
    {
        static Vector4 value(1, 0, 0, 0);
        return value;
    }

    inline Vector4 v4::YONE()
    {
        static Vector4 value(0, 1, 0, 0);
        return value;
    }

    inline Vector4 v4::ZONE()
    {
        static Vector4 value(0, 0, 1, 0);
        return value;
    }

    inline Vector4 v4::WONE()
    {
        static Vector4 value(0, 0, 0, 1);
        return value;
    }
    //----------------------------------------------------------------------
    // Vector4 End
    //----------------------------------------------------------------------

} // namespace eg

#endif // MATH_VECTOR_CONST_H

