///////////////////////////////////////////////////////////////////////
// MathBase.h
//
//  Copyright 10/1/2005 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Basic math dfines and functions
//
//  #include <Math\MathBase.h>
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef MATH_MATH_BASE_H
#define MATH_MATH_BASE_H
#pragma once



#include <Base.h>
#include <math.h>

namespace eg
{
/** \addtogroup Math */
//@{

    //! Small number used on comparing the floating point numbers
    const float kEpsilon(0.00001f);

    //! PI
    const float kPI(3.14159265359f);

    //! Very large number;
    const float kBigNumber(1e37f);

    //! RADIAN defined as float.
    typedef float RADIAN;

    //! Convert from Degrees to Radians.
    float DegToRad(float a);

    //! Convert from Radians to Degrees.
    float RadToDeg(float a);

    //! Sin function using radians.
    float Sin(RADIAN a);

    //! Sin function using degrees.
    float SinD(float a);

    //! Cos function using radians.
    float Cos(RADIAN a);

    //! Cos function using degrees.

    float CosD(float a);
    //! Tan function using radians.
    float Tan(RADIAN a);

    //! Tan function using degrees.
    float TanD(float a);

    //! Power function
    float Pow(float a, float b);

    //! Log function
    float Log(float a);

    //! Acos function
    float Acos(RADIAN a);

    float AcosD(float a);

    //! Return cos and sin of a value
    void SinCos(float a, float &s_out, float &c_out);

    //! Is this 2 float are almost equal (within the Error value)
    bool IsAEq(float a, float b, float error);

    //! Is this 2 float are almost equal
    bool IsAEq(float a, float b);

    //! square root function
    float Sqrt(float a);

    //! return integer larger or equal to float a
    int Ceil(float a);

    //! Return absolute value of the float
    float Abs(float a);

    //! This will convert unsigned char to float. It assumes that a_value of 128 is 0
    float Uint8Tofloat(unsigned char value);

    //! This will convert float to unsigned char. It assumes that a_value of 128 is 0
    unsigned char floatToUint8(float value);

    //! This will return random number from 0 to 1
    float GetRandom();

    //! This will retunr random number between low and hi value
    float GetRandomRange(float lowValue, float hiValue);

    //! Return random value from 0 to NumberOfSides. Simulate dice roll
    int GetDice(int numberSides);


    // inline implementation
    inline float DegToRad(float a)
    {
        return a * 0.01745329252f;
    }

    inline float RadToDeg(float a)
    {
        return a * 57.29577951f;
    }

    inline float Sin(RADIAN a)
    {
        return sinf(a);
    }

    inline float SinD(float a)
    {
        return Sin(DegToRad(a));
    }

    inline float Cos(RADIAN a)
    {
        return cosf(a);
    }

    inline float CosD(float a)
    {
        return Cos(DegToRad(a));
    }

    inline float Tan(RADIAN a)
    {
        return tanf(a);
    }

    inline float TanD(float a)
    {
        return Tan(DegToRad(a));
    }

    inline float Pow(float a, float b)
    {
        return powf(a, b);
    }

    inline float Log(float a)
    {
        return logf(a);
    }

    inline float Acos(RADIAN a)
    {
        return acosf(a);
    }

    inline float AcosD(float a)
    {
        return Acos(DegToRad(a));
    }

    inline void SinCos(float a, float &s_out, float &c_out)
    {
        s_out = Sin(a);
        c_out = Cos(a);
    }


    inline bool IsAEq(float a, float b, float error)
    {
        return(fabsf(a - b) < error);
    }

    inline bool IsAEq(float a, float b)
    {
        return IsAEq(a, b, 0.0001f);
    }

    inline float Sqrt(float a)
    {
        return(float)sqrt(a);
    }

    inline int Ceil(float a)
    {
        return(int)ceil(a);
    }

    inline float Abs(float a)
    {
        return fabsf(a);
    }

    inline float Uint8Tofloat(unsigned char value)
    {
        return (float)((char)(value - 128));
    }

    inline unsigned char floatToUint8(float value)
    {
        return (unsigned char)((char)value + 128);
    }


//@}

} // namespace eg

#endif // MATH_MATH_BASE_H

