///////////////////////////////////////////////////////////////////////
// VectorImpl.h
//
//  Copyright 10/2/2005 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//    Vectro2|3|4 implementation. Do not include this file.
//
//  #include "VectorImpl.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef MATH_VECTOR_IMPL_H
#define MATH_VECTOR_IMPL_H

#ifndef MATH_VECTOR_INCLUDE_IMPLEMENTATION
    #error "Do not include this file explicitly."
#endif // MATH_VECTOR_INCLUDE_IMPLEMENTATION

namespace eg
{
    extern void GetFloatValues(const char *pValue, float *a_FloatValues, size_t numValues);

    //----------------------------------------------------------------------
    // Vector2
    //----------------------------------------------------------------------
    inline Vector2::Vector2()
    {
    }

    inline Vector2::Vector2(const float *in): x(in[0]), y(in[1])
    {
    }

    inline Vector2::Vector2(float inX, float inY): x(inX), y(inY)
    {
    }

    inline Vector2::Vector2(const Vector2& v): x(v.x), y(v.y)
    {
    }

    inline Vector2::Vector2(const Vector3& v) : x(v.x), y(v.y)
    {
    }

    inline Vector2::Vector2(const Vector4& v) : x(v.x), y(v.y)
    {
    }

    inline float& Vector2::operator [] (unsigned int i)
    {
        return *(&x+i);
    }

    inline const float& Vector2::operator [](unsigned int i) const
    {
        return *(&x+i);
    }

    inline Vector2& Vector2::operator +=(const Vector2& v)
    {
        x += v.x;
        y += v.y;
        return *this;
    }

    inline Vector2& Vector2::operator -=(const Vector2& v)
    {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    inline Vector2& Vector2::operator *=(float f)
    {
        x *= f;
        y *= f;
        return *this;
    }

    inline Vector2& Vector2::operator *=(const Vector2& v)
    {
        x *= v.x;
        y *= v.y;
        return *this;
    }

    inline Vector2& Vector2::operator /=(float f)
    {
        x /= f;
        y /= f;
        return *this;
    }

    inline Vector2& Vector2::operator /=(const Vector2& vIn)
    {
        x /= vIn.x;
        y /= vIn.y;
        return *this;
    }

    inline bool operator ==(const Vector2& a, const Vector2& b)
    {
        return ((a.x == b.x) && (a.y == b.y));
    }

    inline bool operator !=(const Vector2& a, const Vector2& b)
    {
        return ((a.x != b.x) || (a.y != b.y));
    }

    inline Vector2 operator -(const Vector2& a)
    {
        return Vector2(-a.x, -a.y);
    }

    inline Vector2 operator +(const Vector2& a, const Vector2& b)
    {
        Vector2 ret(a);
        ret += b;
        return ret;
    }

    inline Vector2 operator -(const Vector2& a, const Vector2& b)
    {
        Vector2 ret(a);
        ret -= b;
        return ret;
    }

    inline Vector2 operator *(const Vector2& v, float f)
    {
        return Vector2(f * v.x, f * v.y);
    }

    inline Vector2 operator *(float f, const Vector2& v)
    {
        return Vector2(f * v.x, f * v.y);
    }

    inline Vector2 operator *(const Vector2& v0, const Vector2& v1)
    {
        Vector2 ret(v0);
        ret *= v1;
        return ret;
    }

    inline Vector2 operator /(const Vector2& v, float f)
    {
        return Vector2(v.x / f, v.y / f);
    }

    inline Vector2 operator /(const Vector2& v1, const Vector2& v2)
    {
        Vector2 ret(v1);
        ret /= v2;
        return ret;
    }

    inline Vector2& Vector2::abs()
    {
        x = Abs(x);
        y = Abs(y);
        return *this;
    }

    inline bool Vector2::almost_eq(const Vector2& b, float Error)
    {
        return(IsAEq(x, b.x, Error) &&
               IsAEq(y, b.y, Error));
    }

    inline void Vector2::set(float xIn, float yIn)
    {
        x = xIn;
        y = yIn;
    }

    inline void Vector2::set(const float *In)
    {
        x = In[0];
        y = In[1];
    }

    inline void Vector2::set(const Vector2& v)
    {
        x = v.x;
        y = v.y;
    }

    inline const float *Vector2::getF() const
    {
        return static_cast<const float *>(&x);
    }

    inline float *Vector2::getF()
    {
        return static_cast<float *>(&x);
    }

    inline float Vector2::length() const
    {
        return Sqrt(x*x + y*y);
    }

    inline float Vector2::lengthSqr() const
    {
        return (x*x + y*y);
    }

    inline bool Vector2::isZero() const
    {
        return ((x == 0.0f) && (y == 0.0f));
    }

    inline Vector2& Vector2::normalize()
    {
        float m = length();

        if (m > 0.0f)
            m = 1.0f / m;
        else
            m = 0.0f;
        x *= m;
        y *= m;

        return *this;
    }

    inline Vector2& Vector2::wrap()
    {
        float integerPart;
        x = modff(x, &integerPart);
        y = modff(y, &integerPart);
        return *this;
    }

    inline void Vector2::set(const char *pValue)
    {
        float values[4] = {0, 0, 0, 1};
        GetFloatValues(pValue, values, 4);
        set(values);
    }

    inline std::ostream& operator <<(std::ostream& o, const Vector2& value)
    {
        return o << " " << value.x << " " << value.y;
    }

    inline std::istream& operator >>(std::istream& i, Vector2& value)
    {
        return i >> value.x >> value.y;
    }

    //----------------------------------------------------------------------
    // Vector2 End
    //----------------------------------------------------------------------


    //----------------------------------------------------------------------
    // Vector3
    //----------------------------------------------------------------------
    inline Vector3::Vector3()
    {
    }

    inline Vector3::Vector3(float v) : x(v), y(v), z(v)
    {
    }

    inline Vector3::Vector3(const float *v) : x(v[0]), y(v[1]), z(v[2])
    {
    }

    inline Vector3::Vector3(float inX, float inY, float inZ): x(inX), y(inY), z(inZ)
    {
    }

    inline Vector3::Vector3(const Vector4& v) : x(v.x), y(v.y), z(v.z)
    {
    }

    inline Vector3::Vector3(const Vector3& v) : x(v.x), y(v.y), z(v.z)
    {
    }

    inline Vector3::Vector3(const Vector2& v): x(v.x), y(v.y), z(0.0f)
    {
    }

    inline float& Vector3::operator [](unsigned int i)
    {
        return *(&x+i);
    }

    inline const float& Vector3::operator [](unsigned int i) const
    {
        return *(&x+i);
    }

    inline Vector3& Vector3::operator =(const float v)
    {
        x = v;
        y = v;
        z = v;
        return *this;
    }

    inline Vector3& Vector3::operator =(const Vector2& v)
    {
        x = v.x;
        y = v.y;
        z = 0.0f;
        return *this;
    }

    inline Vector3& Vector3::operator =(const Vector3& v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }

    inline Vector3& Vector3::operator =(const Vector4& v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }

    inline Vector3& Vector3::operator +=(const Vector3& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    inline Vector3& Vector3::operator -=(const Vector3& v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    inline Vector3& Vector3::operator *=(float f)
    {
        x *= f;
        y *= f;
        z *= f;
        return *this;
    }

    inline Vector3& Vector3::operator *=(const Vector3& vIn)
    {
        x *= vIn.x;
        y *= vIn.y;
        z *= vIn.z;
        return *this;
    }

    inline Vector3& Vector3::operator /=(float f)
    {
        x /= f;
        y /= f;
        z /= f;
        return *this;
    }

    inline Vector3& Vector3::operator /=(const Vector3& vIn)
    {
        x /= vIn.x;
        y /= vIn.y;
        z /= vIn.z;
        return *this;
    }

    inline bool Vector3::almost_eq(const Vector3& b, float Error)
    {
        return (IsAEq(x, b.x, Error) &&
                IsAEq(y, b.y, Error) &&
                IsAEq(z, b.z, Error));
    }

    inline void Vector3::clamp(const Vector3& Low, const Vector3& High)
    {
        if (x < Low.x)
            x = Low.x;
        else if (x > High.x)
            x = High.x;

        if (y < Low.y)
            y = Low.y;
        else if (y > High.y)
            y = High.y;

        if (z < Low.z)
            z = Low.z;
        else if (z > High.z)
            z = High.z;
    }

    inline bool operator ==(const Vector3& a, const Vector3& b)
    {
        return ((a.x == b.x) && (a.y == b.y) && (a.z == b.z));
    }

    inline bool operator !=(const Vector3& a, const Vector3& b)
    {
        return ((a.x != b.x) || (a.y != b.y) || (a.z != b.z));
    }

    inline Vector3 operator -(const Vector3& a)
    {
        return Vector3(-a.x, -a.y, -a.z);
    }

    inline Vector3 operator +(const Vector3& a, const Vector3& b)
    {
        Vector3 ret(a);
        ret += b;
        return ret;
    }

    inline Vector3 operator -(const Vector3& a, const Vector3& b)
    {
        Vector3 ret(a);
        ret -= b;
        return ret;
    }

    inline Vector3 operator *(const Vector3& a, const Vector3& b)
    {
        Vector3 ret(a);
        ret *= b;
        return ret;
    }

    inline Vector3 operator *(const Vector3& v, float f)
    {
        return Vector3(f * v.x, f * v.y, f * v.z);
    }

    inline Vector3 operator *(float f, const Vector3& v)
    {
        return Vector3(f * v.x, f * v.y, f * v.z);
    }

    inline Vector3 operator /(const Vector3& v, float f)
    {
        return Vector3(v.x / f, v.y / f, v.z / f);
    }

    inline Vector3 operator /(const Vector3& v1, const Vector3& v2)
    {
        Vector3 ret(v1);
        ret /= v2;
        return ret;
    }


    inline Vector3& Vector3::abs()
    {
        x = Abs(x);
        y = Abs(y);
        z = Abs(z);
        return *this;
    }

    inline void Vector3::set(float xIn, float yIn, float zIn)
    {
        x = xIn;
        y = yIn;
        z = zIn;
    }

    inline void Vector3::set(const float *In)
    {
        x = In[0];
        y = In[1];
        z = In[2];
    }

    inline void Vector3::set(const Vector3& v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
    }

    inline void Vector3::setb(unsigned char r, unsigned char g, unsigned char b)
    {
        const double kInverseRange = 1.0/255.0;
        x = static_cast<float>(r * kInverseRange);
        y = static_cast<float>(g * kInverseRange);
        z = static_cast<float>(b * kInverseRange);
    }


    inline const float *Vector3::getF() const
    {
        return static_cast<const float *>(&x);
    }

    inline float *Vector3::getF()
    {
        return static_cast<float *>(&x);
    }

    inline void Vector3::reverse()
    {
        (*this) *= -1;
    }

    inline float Vector3::length() const
    {
        return Sqrt(x*x + y*y + z*z);
    }

    inline float  Vector3::lengthSqr() const
    {
        return (x*x + y*y + z*z);
    }

    inline bool Vector3::isZero() const
    {
        return ((x == 0.0f) && (y == 0.0f) && (z == 0.0f));
    }

    inline Vector3& Vector3::normalize()
    {
        float m = length();
        if (m > 0.0f)
            m = 1.0f / m;
        else
            m = 0.0f;
        x *= m;
        y *= m;
        z *= m;
        return *this;
    }

    inline Vector3& Vector3::wrap()
    {
        float integerPart;
        x = modff(x, &integerPart);
        y = modff(y, &integerPart);
        z = modff(z, &integerPart);
        return *this;
    }

    inline void Vector3::set(const char *pValue)
    {
        float values[4] = {0, 0, 0, 1};
        GetFloatValues(pValue, values, 4);
        set(values);
    }

    inline std::ostream& operator <<(std::ostream& o, const Vector3& value)
    {
        return o << " " << value.x << " " << value.y << " " << value.z;
    }

    inline std::istream& operator >>(std::istream& i, Vector3& value)
    {
        return i >> value.x >> value.y >> value.z;
    }

    //----------------------------------------------------------------------
    // Vector3 End
    //----------------------------------------------------------------------


    //----------------------------------------------------------------------
    // Vector4
    //----------------------------------------------------------------------
    inline Vector4::Vector4()
    {
    };

    inline Vector4::Vector4(float v) : x(v), y(v), z(v), w(v)
    {
    };

    inline Vector4::Vector4(const float *v) : x(v[0]), y(v[1]), z(v[2]), w(v[3])
    {
    };

    inline Vector4::Vector4(float inX, float inY, float inZ, float inW): x(inX), y(inY), z(inZ), w(inW)
    {
    };

    inline Vector4::Vector4(const Vector4& v): x(v.x), y(v.y), z(v.z), w(v.w)
    {
    };

    inline Vector4::Vector4(const Vector3& v): x(v.x), y(v.y), z(v.z), w(1.0f)
    {
    };

    inline Vector4::Vector4(const Vector3& v, float vw): x(v.x), y(v.y), z(v.z), w(vw)
    {
    };

    inline Vector4::Vector4(const Vector2& v): x(v.x), y(v.y), z(0.0f), w(1.0f)
    {
    };

    inline float& Vector4::operator [](unsigned int i)
    {
        return (i == 0) ? x : (i == 1) ? y : (i == 2) ? z : w;
    }

    inline const float& Vector4::operator [](unsigned int i) const
    {
        return (i == 0) ? x : (i == 1) ? y : (i == 2) ? z : w;
    }

    inline Vector4& Vector4::operator =(const float v)
    {
        x = v;
        y = v;
        z = v;
        w = z;
        return *this;
    }

    inline Vector4& Vector4::operator =(const Vector4& v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
        w = v.w;
        return *this;
    }

    inline Vector4& Vector4::operator =(const Vector3& v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
        w = 1.0f;
        return *this;
    }

    inline Vector4& Vector4::operator =(const Vector2& v)
    {
        x = v.x;
        y = v.y;
        z = 0.0f;
        w = 1.0f;
        return *this;
    }

    inline Vector4& Vector4::operator +=(const Vector4& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
        w += v.w;
        return *this;
    }

    inline Vector4& Vector4::operator -=(const Vector4& v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        w -= v.w;
        return *this;
    }

    inline Vector4& Vector4::operator *=(const Vector4& v)
    {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        w *= v.w;
        return *this;
    }

    inline Vector4& Vector4::operator *=(float f)
    {
        x *= f;
        y *= f;
        z *= f;
        w *= f;
        return *this;
    }

    inline Vector4& Vector4::operator /=(float f)
    {
        x /= f;
        y /= f;
        z /= f;
        w /= f;
        return *this;
    }

    inline Vector4& Vector4::operator /=(const Vector4& vIn)
    {
        x /= vIn.x;
        y /= vIn.y;
        z /= vIn.z;
        w /= vIn.w;
        return *this;
    }

    inline bool operator ==(const Vector4& a, const Vector4& b)
    {
        return ((a.x == b.x) && (a.y == b.y) &&
                (a.z == b.z) && (a.w == b.w));
    }

    inline bool operator !=(const Vector4& a, const Vector4& b)
    {
        return ((a.x != b.x) || (a.y != b.y) ||
                (a.z != b.z) || (a.w != b.w));
    }

    inline Vector4 operator -(const Vector4& a)
    {
        return Vector4(-a.x, -a.y, -a.z, -a.w);
    }

    inline Vector4 operator +(const Vector4& a, const Vector4& b)
    {
        Vector4 ret(a);
        ret += b;
        return ret;
    }

    inline Vector4 operator -(const Vector4& a, const Vector4& b)
    {
        Vector4 ret(a);
        ret -= b;
        return ret;
    }

    inline Vector4 operator *(const Vector4& a, const Vector4& b)
    {
        Vector4 ret(a);
        ret *= b;
        return ret;
    }

    inline Vector4 operator *(const Vector4& v, float f)
    {
        return Vector4(f * v.x, f * v.y, f * v.z, f * v.w);
    }

    inline Vector4 operator *(float f, const Vector4& v)
    {
        return Vector4(f * v.x, f * v.y, f * v.z, f * v.w);
    }

    inline Vector4 operator /(const Vector4& v, float f)
    {
        return Vector4(v.x / f, v.y / f, v.z / f, v.w / f);
    }

    inline Vector4 operator /(const Vector4& v1, const Vector4& v2)
    {
        Vector4 ret(v1);
        ret /= v2;
        return ret;
    }


    inline Vector4& Vector4::abs()
    {
        x = Abs(x);
        y = Abs(y);
        z = Abs(z);
        w = Abs(w);
        return *this;
    }

    inline void Vector4::set(float xIn, float yIn, float zIn, float wIn)
    {
        x = xIn;
        y = yIn;
        z = zIn;
        w = wIn;
    }

    inline void Vector4::set(const float *In)
    {
        x = In[0];
        y = In[1];
        z = In[2];
        w = In[3];
    }

    inline void Vector4::set(const Vector4& v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
        w = v.w;
    }

    inline const float *Vector4::getF() const
    {
        return((const float *)&x);
    }

    inline float *Vector4::getF()
    {
        return((float *)&x);
    }

    inline void Vector4::set(const Vector3& colorIn, float wIn)
    {
        (*this) = colorIn;
        w = wIn;
    }

    inline void Vector4::setb(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
    {
        const float kInverseRange = 1.0f/255.0f;
        x = r * kInverseRange;
        y = g * kInverseRange;
        z = b * kInverseRange;
        w = a * kInverseRange;
    }

    inline void Vector4::reverse()
    {
        (*this) *= -1;
    }

    inline float Vector4::length() const
    {
        return Sqrt(x*x + y*y + z*z + w*w);
    }

    inline float Vector4::lengthSqr() const
    {
        return(x*x + y*y + z*z + w*w);
    }

    inline bool Vector4::almost_eq(const Vector4& b, float Error)
    {
        return(IsAEq(x, b.x, Error) &&
               IsAEq(y, b.y, Error) &&
               IsAEq(z, b.z, Error) &&
               IsAEq(w, b.w, Error));
    }

    inline void Vector4::clamp(const Vector4& Low, const Vector4& High)
    {
        if (x < Low.x)
            x = Low.x;
        else if (x > High.x)
            x = High.x;

        if (y < Low.y)
            y = Low.y;
        else if (y > High.y)
            y = High.y;

        if (z < Low.z)
            z = Low.z;
        else if (z > High.z)
            z = High.z;

        if (w < Low.w)
            w = Low.w;
        else if (w > High.w)
            w = High.w;
    }

    inline bool Vector4::isZero() const
    {
        return ((x == 0.0F) && (y == 0.0F) && (z == 0.0F) && (w == 0.0F));
    }

    inline Vector4& Vector4::normalize()
    {
        float m = length();
        if (m > 0.0f)
            m = 1.0f / m;
        else
            m = 0.0f;

        x *= m;
        y *= m;
        z *= m;
        w *= m;
        return *this;
    }

    inline Vector4& Vector4::wrap()
    {
        float integerPart;
        x = modff(x, &integerPart);
        y = modff(y, &integerPart);
        z = modff(z, &integerPart);
        w = modff(w, &integerPart);
        return *this;
    }

    inline void Vector4::set(const char *pValue)
    {
        float values[4] = {0, 0, 0, 1};
        GetFloatValues(pValue, values, 4);
        set(values);
    }

    inline std::ostream& operator <<(std::ostream& o, const Vector4& value)
    {
        return o << " " << value.x << " " << value.y << " " << value.z << " " << value.w;
    }

    inline std::istream& operator >>(std::istream& i, Vector4& value)
    {
        return i >> value.x >> value.y >> value.z >> value.w;
    }


    //----------------------------------------------------------------------
    // Vector4 End
    //----------------------------------------------------------------------

    template <typename T> inline bool AlmostEq(const T& a, const T& b, float epsilon)
    {
        T diff = a - b;  // difference

        return DotProduct(diff, diff) < (epsilon * epsilon);  // radius
    }

    template <typename T> inline float Length(const T& a)
    {
        float sql = DotProduct(a, a);
        return Sqrt(sql);
    }

    template <typename T> T Direction(const T& from, const T& to)
    {
        T dir = to - from;
        return Normalized(dir);
    }

    template <typename T> float Distance(const T& from, const T& to)
    {
        T distance = to - from;
        return Length(distance);
    }

    inline float Normalized(float a)
    {
        return a;
    }

    inline Vector2 Normalized(const Vector2& a)
    {
        Vector2 ret(a);
        return ret.normalize();
    }

    inline Vector3 Normalized(const Vector3& a)
    {
        Vector3 ret(a);
        return ret.normalize();
    }

    inline Vector4 Normalized(const Vector4& a)
    {
        Vector4 ret(a);
        return ret.normalize();
    }

    inline float DotProduct(float a, float b)
    {
        return a * b;
    }

    inline float DotProduct(const Vector2& a, const Vector2& b)
    {
        return a.x*b.x + a.y*b.y;
    }

    inline float DotProduct(const Vector3& a, const Vector3& b)
    {
        return a.x*b.x + a.y*b.y + a.z*b.z;
    }

    inline float DotProduct(const Vector4& a, const Vector4& b)
    {
        return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
    }

    inline Vector3 CrossProduct(const Vector3& a, const Vector3& b)
    {
        return Vector3(a.y*b.z - a.z*b.y,
                       a.z*b.x - a.x*b.z,
                       a.x*b.y - a.y*b.x);
    }

    inline Vector3 CrossProductN(const Vector3& a, const Vector3& b, const Vector3& c)
    {
        Vector3 Cross = CrossProduct(a, b, c);
        Cross.normalize();
        return Cross;
    }

    inline Vector3 CrossProduct(const Vector3& a, const Vector3& b, const Vector3& c)
    {
        Vector3 n_a = b - a;
        Vector3 n_c = c - b;
        return CrossProduct(n_a, n_c);
    }

    inline bool NearlyEquals( const Vector2& a, const Vector2& b, float r )
    {
        Vector2 diff = a - b;  // difference

        return(DotProduct(diff, diff) < r*r);  // radius
    }

    inline bool NearlyEquals( const Vector3& a, const Vector3& b, float r )
    {
        Vector3 diff = a - b;  // difference

        return(DotProduct(diff, diff) < r*r);  // radius
    }

    inline bool NearlyEquals( const Vector4& a, const Vector4& b, float r )
    {
        Vector4 diff = a - b;  // difference

        return(DotProduct(diff, diff) < r*r);  // radius
    }

    inline float Angle(const Vector3& v1, const Vector3& v2)
    {
        float dot = DotProduct(v1, v2);
        Clamp(dot, -1.0f, 1.0f);
        float angle = Acos(dot);
        return angle;
    }

    inline float AngleD(const Vector3& v1, const Vector3& v2)
    {
        float angle = Angle(v1, v2);

        float degAngle = RadToDeg(angle);
        return degAngle;
    }

    template <class _T> _T
    inline Clamp(_T& value, const _T& low, const _T& high)
    {
        if (value < low)
        {
            value = low;
        }
        else if (value > high)
        {
            value = high;
        }

        return value;
    }

    template <class _T>
    inline void Swap(_T &a, _T &b)
    {
        _T temp(a);
        a = b;
        b = temp;
    }



} // namespace eg

#endif // MATH_VECTOR_IMPL_H

