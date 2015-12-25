///////////////////////////////////////////////////////////////////////
// QuatImpl.h
//
//  Copyright 11/15/2005 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Quaternion implementation
//
//
//  #include <Math/QuatImpl.h>
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef MATH_QUAT_IMPL_H
#define MATH_QUAT_IMPL_H

#ifndef MATH_QUAT_INCLUDE_IMPLEMENTATION
    #error "Do not include this file explicitly."
#endif // MATH_QUAT_INCLUDE_IMPLEMENTATION

namespace eg
{

    inline Quaternion::Quaternion()
    {
    }

    inline Quaternion::Quaternion(const Vector4& source)
    : x(source.x), y(source.y), z(source.z), w(source.w)
    {
    }

    inline Quaternion::Quaternion(const Vector3& axis, float rad)
    {
        Matrix44 m = RotateRadMatrix44(axis, rad);
        *this = CreateFromMatrix44(m);
    }

    inline Quaternion::Quaternion(float xv, float yv, float zv, float wv)
    : x(xv), y(yv), z(zv), w(wv)
    {
    }

    inline Quaternion::Quaternion(const Quaternion& q)
    : x(q.x), y(q.y), z(q.z), w(q.w)
    {
    }

    inline const Vector3& Quaternion::Axis() const
    {
        return *(Vector3*)&x;
    }

    inline Vector3& Quaternion::Axis()
    {
        return *(Vector3*)&x;
    }

    inline bool Quaternion::operator ==(const Quaternion& v) const
    {
        return (x == v.x) && (y == v.y) && (z == v.z) && (w == v.w);
    }

    inline bool Quaternion::operator !=(const Quaternion& v) const
    {
        return !((*this) == v);
    }

    inline Quaternion& Quaternion::operator =(const Quaternion& a)
    {
        x = a.x;
        y = a.y;
        z = a.z;
        w = a.w;
        return *this;
    }

    //Quaternion  operator -(const Quaternion& a) { return mthQuat(x - a.x, y - a.y, z - a.z, w - a.w);}
    //Quaternion  operator *(float f)          { return mthQuat(x * f, y * f, z * f, w * f);}
    //Quaternion  operator *(const Quaternion& a) {return Multiply(a);};
    //Quaternion  operator /(float f)          { return mthQuat(x / f, y / f, z / f, w / f);}

    inline Quaternion& Quaternion::operator +=(const Quaternion& a)
    {
        x += a.x;
        y += a.y;
        z += a.z;
        w += a.w;

        return *this;
    }

    inline Quaternion& Quaternion::operator -=(const Quaternion& a)
    {
        x -= a.x;
        y -= a.y;
        z -= a.z;
        w -= a.w;

        return *this;
    }

    inline Quaternion& Quaternion::operator /=(float f)
    {
        x /= f;
        y /= f;
        z /= f;
        w /= f;
        return *this;
    }

    inline Quaternion& Quaternion::operator *=(const Quaternion& a)
    {
        Quaternion r;
        r.w     = (w * a.w) - DotProduct(Axis(), a.Axis());
        r.Axis()= (a.Axis() * w) + (Axis() * a.w) + CrossProduct(Axis(), a.Axis());
        (*this) = r;

        return *this;
    }

    inline Quaternion& Quaternion::operator *=(float f)
    {
        x *= f;
        y *= f;
        z *= f;
        w *= f;

        return *this;
    }


    inline Vector3 Quaternion::operator *(const Vector3& v) const
    {
        Quaternion Q = *this;
        // from [0,v].~q directly
        Quaternion R(v.x * Q.w + v.z * Q.y - v.y * Q.z,
                     v.y * Q.w + v.x * Q.z - v.z * Q.x,
                     v.z * Q.w + v.y * Q.x - v.x * Q.y,
                     v.x * Q.x + v.y * Q.y + v.z * Q.z);

        // return the last three components of q.([0,v].~q) as a vector
        Vector3 newVec(Q.w * R.x + Q.x * R.w + Q.y * R.z - Q.z * R.y,
                       Q.w * R.y + Q.y * R.w + Q.z * R.x - Q.x * R.z,
                       Q.w * R.z + Q.z * R.w + Q.x * R.y - Q.y * R.x);

        return newVec;
    }

    inline void Quaternion::Set(float xv, float yv, float zv, float wv)
    {
        x = xv;
        y = yv;
        z = zv;
        w = wv;
    }

    inline void Quaternion::Normalize()
    {
        float invLen = 1.0f / Sqrt(w*w + x*x + y*y + z*z);
        if (invLen == 0)
        {
            return;
        }

        x *= invLen;
        y *= invLen;
        z *= invLen;
        w *= invLen;
    }

    inline void Quaternion::Inverse()
    {
        // make sure that this quat is normalized
        // so we can just use Conjugate to inverse this quat
        Normalize();
        Conjugate();
    }

    inline void Quaternion::Conjugate()
    {
        x *= -1;
        y *= -1;
        z *= -1;
    }

    inline void Quaternion::Decompose(Vector3& axis, float& angle) const
    {
        float scale = Sqrt(x*x + y*y + z*z);
        // if scale is 0, then there is no rotation
        // and we set axis to point up (z)
        if (IsAEq(scale,  0.00001f))
        {
            axis.set(0, 0, 1);
        }
        else
        {
            axis.set(x/scale, y/scale, z/scale);
        }

        angle = 2 * Acos(w);
    }

    inline void Quaternion::SetEulerRad(const Vector3& angles)
    {
        float yaw = angles.x / 2.0f;
        float pitch = angles.y / 2.0f;
        float roll = angles.z / 2.0f;

        float cosY = Cos(yaw);
        float sinY = Sin(yaw);
        float cosP = Cos(pitch);
        float sinP = Sin(pitch);
        float cosR = Cos(roll);
        float sinR = Sin(roll);

        x = cosR * sinP * cosY + sinR * cosP * sinY;
        y = cosR * cosP * sinY - sinR * sinP * cosY;
        z = sinR * cosP * cosY - cosR * sinP * sinY;
        w = cosR * cosP * cosY + sinR * sinP * sinY;
    }

    inline void Quaternion::SetEulerDeg(const Vector3& angles)
    {
        Vector3 radAngles;
        radAngles.x = DegToRad(angles.x);
        radAngles.x = DegToRad(angles.y);
        radAngles.x = DegToRad(angles.z);

        SetEulerRad(radAngles);
    }

    inline Quaternion operator /(float v, const Quaternion& q)
    {
        return q / v;
    }

    inline Quaternion operator /(const Quaternion& q, float v)
    {
        Quaternion retValue(q);
        retValue /= v;
        return retValue;
    }

    inline Quaternion operator *(float v, const Quaternion& q)
    {
        return q * v;
    }

    inline Quaternion operator *(const Quaternion& q, float v)
    {
        Quaternion retValue(q);
        retValue *= v;
        return retValue;
    }

    inline Quaternion operator *(const Quaternion& q1, const Quaternion& q2)
    {
        Quaternion retValue(q1);
        retValue *= q2;
        return retValue;
    }

    inline Quaternion operator +(const Quaternion& q1, const Quaternion& q2)
    {
        Quaternion retValue(q1);
        retValue += q2;
        return retValue;
    }

    inline Quaternion operator -(const Quaternion& q1, const Quaternion& q2)
    {
        Quaternion retValue(q1);
        retValue -= q2;
        return retValue;
    }

    inline float DotProduct(const Quaternion& q1, const Quaternion& q2)
    {
        return DotProduct(q1.Axis(), q2.Axis()) + q1.w*q2.w;
    }

    inline Quaternion Qlog(const Quaternion& q)
    {
        float a = Acos(q.w);
        float sina = Sin(a);
        Quaternion ret;
        ret.w = 0;
        if (sina > 0)
        {
            ret.x = a * q.x / sina;
            ret.y = a * q.y / sina;
            ret.z = a * q.z / sina;
        }
        else
        {
            ret.x=ret.y=ret.z=0;
        }
        return ret;
    }

    inline Quaternion Qexp(const Quaternion& q)
    {
        float a = Sqrt(q.x*q.x + q.y*q.y + q.z*q.z);
        float sina = Sin(a);
        float cosa = Cos(a);
        Quaternion ret;

        ret.w = cosa;
        if(a > 0)
        {
            ret.x = sina * q.x / a;
            ret.y = sina * q.y / a;
            ret.z = sina * q.z / a;
        }
        else
        {
            ret.x = ret.y = ret.z = 0;
        }

        return ret;
    }

    inline Quaternion Qlerp(const Quaternion& q1, const Quaternion& q2, float t)
    {
        Quaternion ret;
        ret = q1 + t*(q2 - q1);
        // return value must be normalized
        ret.Normalize();
        return ret;
    }

    inline Quaternion Qslerp(const Quaternion& q1, const Quaternion& q2, float t)
    {
        Quaternion q3;
        float dot = DotProduct(q1, q2);

        // dot = cos(theta)
        // if (dot < 0), q1 and q2 are more than 90 degrees apart,
        // so we can invert one to reduce spinning
        if (dot < 0)
        {
            dot = -dot;
            q3 = -1*q2;
        }
        else
        {
            q3 = q2;
        }


        if (dot < 0.95f)
        {
            float angle = Acos(dot);
            float sina = Sin(angle);
            float sinat = Sin(angle * t);
            float sinaomt = Sin(angle*(1 - t));
            return (q1*sinaomt + q3*sinat) / sina;
        }

        //if the angle is small, use linear interpolation
        return Qlerp(q1, q3, t);
    }

    inline Quaternion QslerpNoInvert(const Quaternion& q1, const Quaternion& q2, float t)
    {
        float dot = DotProduct(q1, q2);

        if (dot > -0.95f && dot < 0.95f)
        {
            float angle = Acos(dot);
            float sina = Sin(angle);
            float sinat = Sin(angle*t);
            float sinaomt = Sin(angle*(1 - t));
            return (q1*sinaomt + q2*sinat) / sina;
        }

        // if the angle is small, use linear interpolation
        return Qlerp(q1,q2,t);
    }

    inline Quaternion Qsquad(const Quaternion& q1, const Quaternion& q2, const Quaternion& a, const Quaternion& b, float t)
    {
        Quaternion c = QslerpNoInvert(q1, q2, t);
        Quaternion d = QslerpNoInvert(a, b, t);
        return QslerpNoInvert(c, d, 2*t*(1 - t));
    }

    inline Quaternion Qspline(const Quaternion& qnm1, const Quaternion& qn, const Quaternion& qnp1)
    {
        Quaternion qni;

        qni.x = -qn.x;
        qni.y = -qn.y;
        qni.z = -qn.z;
        qni.w = qn.w;

        return qn*Qexp((Qlog(qni*qnm1) + Qlog(qni*qnp1)) / -4);
    }


    inline Quaternion CreateFromMatrix33(const Matrix33& matrix)
    {
        Quaternion q;

        float trace = matrix[0][0] + matrix[1][1] + matrix[2][2] + 1.0f;
        if ( trace > kEpsilon)
        {
            float s = 0.5f / Sqrt(trace);
            q.w = 0.25f / s;
            q.x = ( matrix[1][2] - matrix[2][1] ) * s;
            q.y = ( matrix[2][0] - matrix[0][2] ) * s;
            q.z = ( matrix[0][1] - matrix[1][0] ) * s;
        }
        else
        {
            if ( matrix[0][0] > matrix[1][1] && matrix[0][0] > matrix[2][2] )
            {
                // this needs to be looked to make sure that we have this math here corectly
                // since we are in column major, but memory layout is in row major
                float s = 2.0f * Sqrt( 1.0f + matrix[0][0] - matrix[1][1] - matrix[2][2]);
                q.x = 0.25f * s;
                q.y = (matrix[0][1] + matrix[1][0] ) / s;
                q.z = (matrix[0][2] + matrix[2][0] ) / s;
                q.w = (matrix[1][2] - matrix[2][1] ) / s;
            }
            else if (matrix[1][1] > matrix[2][2])
            {
                // this needs to be looked to make sure that we have this math here corectly
                // since we are in column major, but memory layout is in row major
                float s = 2.0f * Sqrt( 1.0f + matrix[1][1] - matrix[0][0] - matrix[2][2]);
                q.x = (matrix[0][1] + matrix[1][0] ) / s;
                q.y = 0.25f * s;
                q.z = (matrix[1][2] + matrix[2][1] ) / s;
                q.w = (matrix[0][2] - matrix[2][0] ) / s;
            }
            else
            {
                float s = 2.0f * Sqrt( 1.0f + matrix[2][2] - matrix[0][0] - matrix[1][1] );
                q.x = (matrix[0][2] + matrix[2][0] ) / s;
                q.y = (matrix[1][2] + matrix[2][1] ) / s;
                q.z = 0.25f * s;
                q.w = (matrix[0][1] - matrix[1][0] ) / s;
            }
        }

        return q;
    }

    inline Quaternion CreateFromMatrix44(const Matrix44& matrix)
    {
        Matrix33 rotMatrix = matrix.GetRotation();
        return CreateFromMatrix33(rotMatrix);
    }

    inline std::ostream& operator <<(std::ostream& o, const Quaternion& value)
    {
        return o << " " << value.x << " " << value.y << " " << value.z << " " << value.w;
    }

    inline std::istream& operator >>(std::istream& i, Quaternion& value)
    {
        return i >> value.x >> value.y >> value.z >> value.w;
    }


} // namespace eg

#endif // MATH_QUAT_IMPL_H

