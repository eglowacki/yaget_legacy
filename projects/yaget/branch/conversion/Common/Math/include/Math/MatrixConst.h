///////////////////////////////////////////////////////////////////////
// MatrixConst.h
//
//  Copyright 11/12/2005 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Const Matrix types
//
//  #include <Math/MatrixConst.h>
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef MATH_MATRIX_CONST_H
#define MATH_MATRIX_CONST_H

#ifndef MATH_MATRIX_INCLUDE_IMPLEMENTATION
    #error "Do not include this file explicitly."
#endif // MATH_MATRIX_INCLUDE_IMPLEMENTATION


namespace eg
{
    namespace m33
    {
        Matrix33 Identity();

    } // namespace M33

    namespace m44
    {
        Matrix44 Identity();

    } // namespace M44


    // inline implementation
        //----------------------------------------------------------------------
    // Matrix33
    //----------------------------------------------------------------------
    inline Matrix33 m33::Identity()
    {
        static Matrix33 value(v3::XONE(), v3::YONE(), v3::ZONE());
        return value;
    }

    //----------------------------------------------------------------------
    // Matrix44
    //----------------------------------------------------------------------
    inline Matrix44 m44::Identity()
    {
        static Matrix44 value(v4::XONE(), v4::YONE(), v4::ZONE(), v4::WONE());
        return value;
    }

} // namespace eg

#endif // MATH_MATRIX_CONST_H

