///////////////////////////////////////////////////////////////////////
// Vector.h
//
//  Copyright 10/1/2005 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:          Partially based on the "Game Programing Gems"
//                  ource by Dante Treglia II and Mark A. DeLoura.
//                  C++ Matrix Library.
//                  Some extra functionality added/changed by Edgar Glowacki
//
//  #include "Math/Vector.h"
//
//////////////////////////////////////////////////////////////////////
//! \file
//
#ifndef MATH_VECTOR_H
#define MATH_VECTOR_H
#pragma once

#include "MathBase.h"
#include <iostream>

// forward declarations
namespace eg
{
    class Vector2;
    class Vector3;
    class Vector4;
}  // namespace eg

namespace omega {class Frame;}
omega::Frame& operator <<(omega::Frame& f, const eg::Vector2& Ref);
omega::Frame& operator >>(omega::Frame& f, eg::Vector2& Ref);

omega::Frame& operator <<(omega::Frame& f, const eg::Vector3& Ref);
omega::Frame& operator >>(omega::Frame& f, eg::Vector3& Ref);

omega::Frame& operator <<(omega::Frame& f, const eg::Vector4& Ref);
omega::Frame& operator >>(omega::Frame& f, eg::Vector4& Ref);


namespace eg
{

////////////////////////////////////////////////////////////
//! Vector2 class
    class Vector2
    {
    public:
        // Members
        float x, y;

        //! Constructors
        Vector2();

        //! Constructor with initializing array of floats
        Vector2(const float *in);

        //! Constructor with initializing float values
        Vector2(float inX, float inY);

        //! Constructor with initializing Vector2
        Vector2(const Vector2& v);

        //! This allows us to convert Vector3 to Vector2. z value will be lost
        Vector2(const Vector3& v);

        //! This allows us to convert Vector4 to Vector2. z and w values will be lost
        Vector2(const Vector4& v);

        //! Array indexing
        float& operator [] (unsigned int i);

        //! Array indexing
        const float& operator [](unsigned int i) const;

        //! Add a Vector2 to this one
        Vector2& operator +=(const Vector2& v);

        //! Subtract a Vector2 from this one
        Vector2& operator -=(const Vector2& v);

        //! Multiply the Vector2 by a float
        Vector2& operator *=(float f);

        //! Multiply the Vector2 by a Vector2
        Vector2& operator *=(const Vector2& v);

        //! Divide the Vector2 by a float
        Vector2& operator /=(float f);

        Vector2& operator /=(const Vector2& v);

        //! Are these two Vector2's equal?
        friend bool operator ==(const Vector2& a, const Vector2& b);

        //! Are these two Vector2's not equal?
        friend bool operator !=(const Vector2& a, const Vector2& b);

        //! Negate this vector
        friend Vector2 operator -(const Vector2& a);

        //! Add two Vector2's
        friend Vector2 operator +(const Vector2& a, const Vector2& b);

        //! Subtract one Vector2 from another
        friend Vector2 operator -(const Vector2& a, const Vector2& b);

        //! Multiply Vector2 by a float
        friend Vector2 operator *(const Vector2& v, float f);

        //! Multiply Vector2 by a float
        friend Vector2 operator *(float f, const Vector2& v);

        //! Multiply Vector2 by a Vector2
        friend Vector2 operator *(const Vector2& v0, const Vector2& v1);

        //! Divide Vector2 by a float
        friend Vector2 operator /(const Vector2& v, float f);

        friend Vector2 operator /(const Vector2& v1, const Vector2& v2);

        friend omega::Frame& operator >>(omega::Frame& f, eg::Vector2& Ref);

        friend std::ostream& operator <<(std::ostream& o, const Vector2& value);
        friend std::istream& operator >>(std::istream& i, Vector2& value);

        //! Make this absolute Vector2
        Vector2& abs();

        //! Are these two Vector2's almost equal?
        bool almost_eq(const Vector2& b, float Error = kEpsilon);

        //! Set x and y values
        void set(float xIn, float yIn);

        //! Initialize with array of floats
        void set(const float *In);

        void set(const Vector2& v);

        //! Return array of foats
        const float *getF() const;

        //! Return array of foats
        float *getF();

        //! Get length of a Vector2
        float length() const;

        //! Get squared length of a Vector2
        float  lengthSqr() const;

        //! Does Vector2 equal to (0, 0)?
        bool isZero() const;

        //! Normalize a Vector2
        Vector2& normalize();

        //! Wrap vector when is above one
        Vector2& wrap();

        //! Return string representation of this vector in the form [x: %.2f, y: %.2f, z: %.2f].
        std::string Print() const;

        /*!
        This will parse the string into 2 floats.
        \param pValue string in the format [%s, %s], which converts to x, y.
               For every missing component it will default to 0.0f
        */
        void set(const char *pValue);
    };

    ////////////////////////////////////////////////////////////
    //! Vector3 class
    class Vector3
    {
    public:
        //! Members
        float x, y, z;

        //! Constructors
        Vector3();

        Vector3(float v);

        Vector3(const float *v);

        //! Constructor with initializing float values
        Vector3(float inX, float inY, float inZ);

        //! Constructor with initializing Vector3
        Vector3(const Vector3& v);

        //! Constructor with initializing Vector2
        explicit Vector3(const Vector2& v);

        //! Constructor with initializing Vector4
        explicit Vector3(const Vector4& v);

        //! Array indexing
        float& operator [](unsigned int i);

        //! Array indexing
        const float& operator [](unsigned int i) const;

        //! Assign from a float
        Vector3& operator =(const float v);

        //! Assign from a Vector2
        Vector3& operator =(const Vector2& v);

        //! Assign from a Vector2
        Vector3& operator =(const Vector3& v);

        //! Assign from a Vector4
        Vector3& operator =(const Vector4& v);

        //! Add a Vector3 to this one
        Vector3& operator +=(const Vector3& v);

        //! Subtract a Vector3 from this one
        Vector3& operator -=(const Vector3& v);

        //! Multiply the Vector3 by a float
        Vector3& operator *=(float f);

        //! Multiply the Vector3 by a Vector3
        Vector3& operator *=(const Vector3& vIn);

        //! Divide the Vector3 by a float
        Vector3& operator /=(float f);

        Vector3& operator /=(const Vector3& v);

        //! Are these two Vector3's almost equal?
        bool almost_eq(const Vector3& b, float Error = kEpsilon);

        void clamp(const Vector3& Low, const Vector3& High);

        //! Are these two Vector3's equal?
        friend bool operator ==(const Vector3& a, const Vector3& b);

        //! Are these two Vector3's not equal?
        friend bool operator !=(const Vector3& a, const Vector3& b);

        //! Negate a Vector3
        friend Vector3 operator -(const Vector3& a);

        //! Add two Vector3's
        friend Vector3 operator +(const Vector3& a, const Vector3& b);

        //! Subtract one Vector3 from another
        friend Vector3 operator -(const Vector3& a, const Vector3& b);

        //! Multiply the Vector3 by a Vector3
        friend Vector3 operator *(const Vector3& a, const Vector3& b);

        //! Multiply Vector3 by a float
        friend Vector3 operator *(const Vector3& v, float f);

        //! Multiply Vector3 by a float
        friend Vector3 operator *(float f, const Vector3& v);

        //! Divide Vector3 by a float
        friend Vector3 operator /(const Vector3& v, float f);

        friend Vector3 operator /(const Vector3& v1, const Vector3& v2);

        friend omega::Frame& operator >>(omega::Frame& f, eg::Vector3& Ref);

        friend std::ostream& operator <<(std::ostream& o, const Vector3& value);
        friend std::istream& operator >>(std::istream& i, Vector3& value);

        //! Make this absolute Vector3
        Vector3& abs();

        // Set Values
        void set(float xIn, float yIn, float zIn);

        void set(const float *In);

        void set(const Vector3& v);

        //! This is only useful in converting colors (from 0 to 255) into float (from 0 to 1)
        void setb(unsigned char r, unsigned char g, unsigned char b);

        const float *getF() const;

        float *getF();

        //! Reverse this vector
        void reverse();

        //! Get length of a Vector3
        float length() const;

        //! Get squared length of a Vector3
        float  lengthSqr() const;

        //! Does Vector3 equal (0, 0, 0)?
        bool isZero() const;

        //! Normalize a Vector3
        Vector3& normalize();

        //! Wrap vector when is above one
        Vector3& wrap();

        //! Return string representation of this vector in the form [x: %.2f, y: %.2f, z: %.2f].
        std::string Print() const;
        /*!
        This will parse the string into 3 floats.
        \param a_Value string i the format [%s, %s, %s], which converts to x, y, z.
               For every missing component it will default to 0
        */
        void set(const char *pValue);
    };


    /** \addtogroup Math */
    //@{

    //! Re-using Vector3 as the color rgb (float)
    typedef Vector3 Color3;
    //@}


    ////////////////////////////////////////////////////////////
    //! Vector4 class
    class Vector4
    {
    public:
        // Members
        float x, y, z, w;

        //! Constructors
        Vector4();

        Vector4(float v);

        Vector4(const float *v);

        //! Constructor with initializing float values
        Vector4(float inX, float inY, float inZ, float inW);

        //! Constructor with initializing Vector4
        Vector4(const Vector4& v);

        //! Constructor with initializing Vector3
        explicit Vector4(const Vector3& v);

        //! Constructor with initializing Vector3, but will set the w component
        explicit Vector4(const Vector3& v, float vw);

        //! Constructor with initializing Vector2
        explicit Vector4(const Vector2& v);

        //! Array indexing
        float& operator [](unsigned int i);

        //! Array indexing
        const float& operator [](unsigned int i) const;

        //! Assign from a float
        Vector4& operator =(const float v);

        //! Assign from a Vector3
        Vector4& operator =(const Vector4& v);

        //! Assign from a Vector3
        Vector4& operator =(const Vector3& v);

        //! Assign from a Vector2
        Vector4& operator =(const Vector2& v);

        //! Add a Vector4 to this one
        Vector4& operator +=(const Vector4& v);

        //! Subtract a Vector4 from this one
        Vector4& operator -=(const Vector4& v);

        //! Multiply a Vector4 from this one
        Vector4& operator *=(const Vector4& v);

        //! Multiply the Vector4 by a float
        Vector4& operator *=(float f);

        //! Divide the Vector4 by a float
        Vector4& operator /=(float f);

        Vector4& operator /=(const Vector4& v);

        //! Are these two Vector4's equal?
        friend bool operator ==(const Vector4& a, const Vector4& b);

        //! Are these two Vector4's not equal?
        friend bool operator !=(const Vector4& a, const Vector4& b);

        //! Negate a Vector4
        friend Vector4 operator -(const Vector4& a);

        //! Add two Vector4's
        friend Vector4 operator +(const Vector4& a, const Vector4& b);

        //! Subtract one Vector4 from another
        friend Vector4 operator -(const Vector4& a, const Vector4& b);

        //! Multiply the Vector4 by a Vector4
        friend Vector4 operator *(const Vector4& a, const Vector4& b);

        //! Multiply Vector4 by a float
        friend Vector4 operator *(const Vector4& v, float f);

        //! Multiply Vector4 by a float
        friend Vector4 operator *(float f, const Vector4& v);

        //! Divide Vector4 by a float
        friend Vector4 operator /(const Vector4& v, float f);

        friend Vector4 operator /(const Vector4& v1, const Vector4& v2);

        friend omega::Frame& operator >>(omega::Frame& f, eg::Vector4& Ref);

        friend std::ostream& operator <<(std::ostream& o, const Vector4& value);
        friend std::istream& operator >>(std::istream& i, Vector4& value);

        Vector4& abs();

        //! Set Values
        void set(float xIn, float yIn, float zIn, float wIn);

        void set(const float *In);

        void set(const Vector4& v);

        const float *getF() const;

        float *getF();

        void set(const Vector3& colorIn, float wIn);

        //! This is only usefull in converting colors (from 0 to 255) into float (from 0 to 1)
        void setb(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

        //! Reverse this vector
        void reverse();

        //! Get length of a Vector4
        float length() const;

        //! Get squared length of a Vector4
        float lengthSqr() const;

        //! Are these two Vector4's almost equal?
        bool almost_eq(const Vector4& b, float Error = kEpsilon);

        void clamp(const Vector4& Low, const Vector4& High);

        //! Does Vector4 equal (0, 0, 0, 0)?
        bool isZero() const;

        //! Normalize a Vector4
        Vector4& normalize();

        //! Wrap vector when is above one
        Vector4& wrap();

        //! Return string representation of this vector in the form [x: %.2f, y: %.2f, z: %.2f, w: %.2f].
        std::string Print() const;
        /*!
        This will parse the string into 3 floats.
        \param a_Value string i the format [%s, %s, %s, %s], which converts to x, y, z, w.
               For every missing component it will default to 0, except w which will be 1
        */
        void set(const char *pValue);
    };


    /** \addtogroup Math */
    //@{

    //! Re-using Vector3 as the color rgba (float)
    typedef Vector4 Color4_t;

    namespace math
    {
        // left,up, width, height
        typedef Vector4 Size_t;
        // left, up, right, bottom
        typedef Vector4 Rect_t;
    } // namespace math


    ////////////////////////////////////////////////////////////
    // Miscellaneous vector functions
    //

    //! So our template below will work
    float Normalized(float a);

    //! Normalize 2d vector.
    Vector2 Normalized(const Vector2& a);

    //! Normalize 3d vector.
    Vector3 Normalized(const Vector3& a);

    //! Normalize 4d vector.
    Vector4 Normalized(const Vector4& a);

    //! This is here so our template function AlmostEq will work
    float DotProduct(float a, float b);

    //! Dot Product between 2d vector.
    float DotProduct(const Vector2& a, const Vector2& b);

    //! Dot Product between 3d vector.
    float DotProduct(const Vector3& a, const Vector3& b);

    //! Dot Product between 4d vector.
    float DotProduct(const Vector4& a, const Vector4& b);

    //! This will return cross product based on 2 3d vectors.
    //! going clockwise, it will return up vector
    Vector3 CrossProduct(const Vector3& a, const Vector3& b);

    //! This will return cross product created from 3 points.
    Vector3 CrossProduct(const Vector3& a, const Vector3& b, const Vector3& c);

    //! This will return cross product normalized, created from 3 points.
    Vector3 CrossProductN(const Vector3& a, const Vector3& b, const Vector3& c);

    //! Comparison of 2 2d vectors based on same small epsilon value.
    bool NearlyEquals(const Vector2& a, const Vector2& b, float r);

    //! Comparison of 2 3d vectors based on same small epsilon value.
    bool NearlyEquals(const Vector3& a, const Vector3& b, float r);

    //! Comparison of 2 4d vectors based on same small epsilon value.
    bool NearlyEquals(const Vector4& a, const Vector4& b, float r);

    //! Return shortest angle between 2 vectors in radians
    float Angle(const Vector3& v1, const Vector3& v2);

    //! Return shortest angle between 2 vectors in degrees
    float AngleD(const Vector3& v1, const Vector3& v2);

    //! Comparison of 2 values based on same small epsilon value.
    template <typename T> bool AlmostEq(const T& a, const T& b, float epsilon);

    template <typename T> bool AlmostEq(const T& a, const T& b)
    {
        return AlmostEq(a, b, kEpsilon);
    }

    //! Return length of a value
    template <typename T> inline float Length(const T& a);

    //! This will return normalized direction vector
    template <typename T> T Direction(const T& from, const T& to);

    //! Calculate length between to and from
    template <typename T> float Distance(const T& from, const T& to);

    //! Clamp Value to be between Low and High.
    template <class T> T Clamp(T& value, const T& low, const T& high);

    //! Swap 2 values between each other.
    template <class T> void Swap(T &a, T &b);
    //@}

} // namespace eg


#define MATH_VECTOR_INCLUDE_IMPLEMENTATION
#include "VectorConst.h"
#include "VectorImpl.h"
#undef MATH_VECTOR_INCLUDE_IMPLEMENTATION

#endif // MATH_VECTOR_H

