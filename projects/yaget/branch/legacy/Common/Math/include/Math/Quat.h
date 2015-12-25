///////////////////////////////////////////////////////////////////////
// Quat.h
//
//  Copyright 11/15/2005 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//
//
//  #include <Math/Quat.h>
//
//////////////////////////////////////////////////////////////////////
//! \file


#ifndef MATH_QUAT_H
#define MATH_QUAT_H
#pragma once


#include <Math\Matrix.h>


namespace eg
{
    class Quaternion
    {
    public:
        float x, y, z, w;

    public:
        //! Empty ctor
        Quaternion();
        Quaternion(const Vector4& source);
        //! Create quat with rotation rad (in radians) about axis
        Quaternion(const Vector3& axis, float rad);

        //! Initialize with 4 float
        Quaternion(float xv, float yv, float zv, float wv);

        //! Copy constructor
        Quaternion(const Quaternion& q);

        //@{
        //! Return axis representing this Quaternion
        const Vector3& Axis() const;
        Vector3& Axis();
        //@}

        //@{
        //! Equality operators
        bool operator ==(const Quaternion& v) const;
        bool operator !=(const Quaternion& v) const;
        //@}

        //! Assignment operator
        Quaternion& operator =(const Quaternion& a);
        //Quaternion  operator -(const Quaternion& a) { return mthQuat(x - a.x, y - a.y, z - a.z, w - a.w);}
        //Quaternion  operator *(float f)          { return mthQuat(x * f, y * f, z * f, w * f);}
        //Quaternion  operator *(const Quaternion& a) {return Multiply(a);};
        //Quaternion  operator /(float f)          { return mthQuat(x / f, y / f, z / f, w / f);}

        //! Add Quaternion to this one
        Quaternion& operator +=(const Quaternion& a);

        //! Subtract Quaternion from this one
        Quaternion& operator -=(const Quaternion& a);

        //! Multiply this Quaternion by another one
        Quaternion& operator *=(const Quaternion& a);

        Quaternion& operator /=(float f);

        //! Multiply this Quaternion by scalar
        Quaternion& operator *=(float f);

        //! Rotate Vector3 by this Quaternion
        Vector3 operator *(const Vector3& v) const;

        //! Set this Quaternion to new values
        void Set(float xx, float yv, float zv, float wv);

        //! Set Euler in radians with Vector3 for yaw, pitch roll
        void SetEulerRad(const Vector3& angles);
        //! Set Euler in degrees with Vector3 for yaw, pitch roll
        void SetEulerDeg(const Vector3& angles);

        //! Normalize this Quaternion
        void Normalize();

        //! Inverse this Quaternion
        void Inverse();

        //! negate axis part
        void Conjugate();

        //! Decompose quaternion into axis and angle
        void Decompose(Vector3& axis, float& angle) const;

        friend std::ostream& operator <<(std::ostream& o, const Quaternion& value);
        friend std::istream& operator >>(std::istream& i, Quaternion& value);

        friend Quaternion operator *(float v, const Quaternion& q);
        friend Quaternion operator *(const Quaternion& q, float v);
        friend Quaternion operator /(float v, const Quaternion& q);
        friend Quaternion operator /(const Quaternion& q, float v);
        friend Quaternion operator *(const Quaternion& q1, const Quaternion& q2);
        friend Quaternion operator +(const Quaternion& q1, const Quaternion& q2);
        friend Quaternion operator -(const Quaternion& q1, const Quaternion& q2);
    };

    float DotProduct(const Quaternion& q1, const Quaternion& q2);

    /*!
    Logarithm of a quaternion, given as:
    Qlog(q) = v*a where q = [cos(a),v*sin(a)]
    */
    Quaternion Qlog(const Quaternion& q);

    /*!
    e^quaternion given as:
    Qexp(v*a) = [cos(a),vsin(a)]
    */
    Quaternion Qexp(const Quaternion& q);

    /*!
    Linear interpolation between two quaternions
    */
    Quaternion Qlerp(const Quaternion& q1, const Quaternion& q2, float t);

    /*!
    Spherical linear interpolation between two quaternions
    */
    Quaternion Qslerp(const Quaternion& q1, const Quaternion& q2, float t);

    /*!
    This version of slerp, used by squad, does not check for theta > 90.
    */
    Quaternion QslerpNoInvert(const Quaternion& q1, const Quaternion& q2, float t);

    /*
    Spherical cubic interpolation
    */
    Quaternion Qsquad(const Quaternion& q1, const Quaternion& q2, const Quaternion& a, const Quaternion& b, float t);

    /*
    Given 3 quaternions, qn-1,qn and qn+1, calculate a control point to be used in spline interpolation
    */
    Quaternion Qspline(const Quaternion& qnm1, const Quaternion& qn, const Quaternion& qnp1);

    //! Create Quaternion from 3x3 matrix
    Quaternion CreateFromMatrix33(const Matrix33& matrix);

    //! Create Quaternion from 4x4 matrix
    Quaternion CreateFromMatrix44(const Matrix44& matrix);



} // namespace eg




#define MATH_QUAT_INCLUDE_IMPLEMENTATION
//#include "QuatConst.h"
#include "QuatImpl.h"
#undef MATH_QUAT_INCLUDE_IMPLEMENTATION

#endif // MATH_QUAT_H

#if 0


#include "Matrix.h"

namespace eg
{

    class mthQuat
    {
    public:

        float x, y, z, w;

        /***************************************
        Constructor(s)
        ***************************************/
        mthQuat()  {
#ifdef __DEBUG
            x = y = z = w = 6969.7171f;
#endif
        }

        mthQuat(const UOMath::vector3& a_Axis, float a_Angle)
        {
            SetAxisAngle(a_Axis, a_Angle);
        }
        mthQuat(float _x, float _y, float _z, float _w) { x = _x; y = _y; z = _z; w = _w;}
        mthQuat(const mthQuat &q)                       { x = q.x; y = q.y; z = q.z; w = q.w;}
        mthQuat(const UOMath::matrix44& a_Matrix)
        {
            Create(a_Matrix);
        }
        mthQuat(const UOMath::matrix33& a_Matrix)
        {
            Create(a_Matrix);
        }
        mthQuat(const nGZNumerics::tQuaternion a_q){x = a_q[0]; y = a_q[1]; z = a_q[2]; w = a_q[3];}

        /***************************************
        Convience
        ***************************************/
        static const mthQuat& Identity();
        const UOMath::vector3& Axis() const {return *((UOMath::vector3*)&x);}
        UOMath::vector3& Axis() {return *((UOMath::vector3*)&x);}

        /***************************************
        Operators
        ***************************************/
        bool operator==(const mthQuat& v) { return((x == v.x) && (y == v.y) && (z == v.z) && (w == v.w));}
        bool operator!=(const mthQuat& v) { return((x != v.x) && (y != v.y) && (z != v.z) && (w != v.w));}

        mthQuat& operator= (const mthQuat& a) { x = a.x; y = a.y; z = a.z; w = a.w; return *this;}
        mthQuat  operator- (const mthQuat& a) { return mthQuat(x - a.x, y - a.y, z - a.z, w - a.w);}
        mthQuat  operator* (float f)          { return mthQuat(x * f, y * f, z * f, w * f);}
        mthQuat  operator/ (float f)          { return mthQuat(x / f, y / f, z / f, w / f);}

        mthQuat& operator+=(const mthQuat& a) { x += a.x; y += a.y; z += a.z; w += a.w; return *this;}
        mthQuat& operator-=(const mthQuat& a) { x -= a.x; y -= a.y; z -= a.z; w -= a.w; return *this;}
        mthQuat& operator*=(float f)    { x *= f; y *= f; z *= f; w *= f; return *this;}

        vector3 operator* (vector3 V)
        {
            mthQuat Q = *this;
            // form [0,v].~q directly
            UOMath::mthQuat R
            (
            V.x * Q.w + V.z * Q.y - V.y * Q.z,
            V.y * Q.w + V.x * Q.z - V.z * Q.x,
            V.z * Q.w + V.y * Q.x - V.x * Q.y,
            V.x * Q.x + V.y * Q.y + V.z * Q.z
            );

            // return the last three components of q.([0,v].~q) as a vector
            UOMath::vector3 newVec
            (
            Q.w * R.x + Q.x * R.w + Q.y * R.z - Q.z * R.y,
            Q.w * R.y + Q.y * R.w + Q.z * R.x - Q.x * R.z,
            Q.w * R.z + Q.z * R.w + Q.x * R.y - Q.y * R.x
            );

            return newVec;
        }


        mthQuat  operator* (const mthQuat &a) {return Multiply(a);};
        mthQuat& operator*=(const mthQuat &a) {  *this = *this * a;  return *this;}

        mthQuat Multiply(const mthQuat& target);

        /***************************************
        Functions
        ***************************************/
        void Conjugate();
        void Inverse();

        void SetIdentity(){x =0; y=0; z= 0; w = 1.0f;}
        void Set          (float a_x, float a_y, float a_z ,float a_w) {x = a_x; y = a_y; z = a_z; w = a_w;};

        void SetEuler     (float a_Yaw, float a_Pitch, float a_Roll);
        //! Same as above, but uses vector3 for a_Yaw, a_Pitch and a_Roll, using Radians
        void SetEulerRad  (const UOMath::vector3& a_Angles);
        //! Same as above, but uses vector3 for a_Yaw, a_Pitch and a_Roll, using degrees
        void SetEulerDeg  (const UOMath::vector3& a_Angles);
        void SetAxisAngle(const UOMath::vector3& a_Axis, float a_Angle);
        void Normalize();

        float Dot(const mthQuat& q) const;

        void Create(const UOMath::matrix44& a_Matrix);
        void Create(const UOMath::matrix33& a_Matrix);

        //! Decompose this quat into rotation axis and rotation angle around the axis
        void Decompose(UOMath::vector3& a_Axis, float& a_Angle) const
        {
            float scale = UOMath::Sqrt(x*x + y*y + z*z);
            // if scale is 0, then there is no rotation
            // and we set axis to point up (z)
            if (UOMath::IsAEq(scale,  0.00001f))
            {
                a_Axis.set(0, 0, 1);
            }
            else
            {
                a_Axis.set(x/scale, y/scale, z/scale);
            }

            a_Angle = 2 * UOMath::Acos(w);
        }

        //! Return rotation angle around the Z axis
        float GetAngleZ() const
        {
            UOMath::vector3 axis;
            float angle;
            Decompose(axis, angle);
            return angle;
        }

        void LookAt(const UOMath::vector3& eyept, const UOMath::vector3& lookatpt, const UOMath::vector3& upvec = UOMath::vector3::ZONE);

        inline nGZNumerics::tQuaternion getGZ() const
        {
            return nGZNumerics::tQuaternion(x, y, z, w);
        }
    };

    inline mthQuat operator -(const mthQuat& q1, const mthQuat& q2)
    {
        mthQuat ret;
        ret.x = q1.x - q2.x;
        ret.y = q1.y - q2.y;
        ret.z = q1.z - q2.z;
        ret.w = q1.w - q2.w;

        return ret;
    }

    inline mthQuat operator +(const mthQuat& q1, const mthQuat& q2)
    {
        mthQuat ret;
        ret.x = q1.x + q2.x;
        ret.y = q1.y + q2.y;
        ret.z = q1.z + q2.z;
        ret.w = q1.w + q2.w;

        return ret;
    }


    inline mthQuat operator *(const mthQuat& q1, const mthQuat& q2)
    {
        mthQuat ret;

        ret.x = q1.y*q2.z - q1.z*q2.y + q1.w*q2.x + q1.x*q2.w;
        ret.y = q1.z*q2.x - q1.x*q2.z + q1.w*q2.y + q1.y*q2.w;
        ret.z = q1.x*q2.y - q1.y*q2.x + q1.w*q2.z + q1.z*q2.w;
        ret.w = q1.w*q2.w - q1.x*q2.x - q1.y*q2.y - q1.z*q2.z;

        return ret;
    }

    inline mthQuat operator *(const mthQuat& q, Float32 v)
    {
        mthQuat ret;

        ret.x = q.x*v;
        ret.y = q.y*v;
        ret.z = q.z*v;
        ret.w = q.w*v;

        return ret;
    }

    inline mthQuat operator *(Float32 v, const mthQuat& q)
    {
        return q*v;
    }

    /*
    mthQuat operator /(const mthQuat& q, Float32 v)
    {
        mthQuat ret;

        ret.x = q.x/v;
        ret.y = q.y/v;
        ret.z = q.z/v;
        ret.w = q.w/v;

        return ret;
    }
    */


    /*!
    Logarithm of a quaternion, given as:
    Qlog(q) = v*a where q = [cos(a),v*sin(a)]
    */
    inline UOMath::mthQuat Qlog(const UOMath::mthQuat& q)
    {
        Float32 a = UOMath::Acos(q.w);
        Float32 sina = UOMath::Sin(a);
        UOMath::mthQuat ret;
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

    /*!
    e^quaternion given as:
    Qexp(v*a) = [cos(a),vsin(a)]
    */
    inline UOMath::mthQuat Qexp(const UOMath::mthQuat& q)
    {
        Float32 a = UOMath::Sqrt(q.x*q.x + q.y*q.y + q.z*q.z);
        Float32 sina = UOMath::Sin(a);
        Float32 cosa = UOMath::Cos(a);
        UOMath::mthQuat ret;

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

    /*!
    Linear interpolation between two quaternions
    */
    inline UOMath::mthQuat Qlerp(const UOMath::mthQuat& q1, const UOMath::mthQuat& q2, Float32 t)
    {
        UOMath::mthQuat ret;
        ret = q1 + t*(q2 - q1);
        // return value must be normalized
        ret.Normalize();
        return ret;
    }

    /*!
    Spherical linear interpolation between two quaternions
    */
    inline UOMath::mthQuat Qslerp(const UOMath::mthQuat& q1, const UOMath::mthQuat& q2, Float32 t)
    {
        UOMath::mthQuat q3;
        //Float32 dot = q1.x*q2.x + q1.y*q2.y + q1.z*q2.z + q1.w*q2.w;
        Float32 dot = q1.Dot(q2);

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
            Float32 angle = UOMath::Acos(dot);
            Float32 sina = UOMath::Sin(angle);
            Float32 sinat = UOMath::Sin(angle * t);
            Float32 sinaomt = UOMath::Sin(angle*(1 - t));
            return (q1*sinaomt + q3*sinat) / sina;
        }

        //if the angle is small, use linear interpolation
        return Qlerp(q1, q3, t);
    }

    /*!
    This version of slerp, used by squad, does not check for theta > 90.
    */
    inline UOMath::mthQuat QslerpNoInvert(const UOMath::mthQuat& q1, const UOMath::mthQuat& q2, Float32 t)
    {
        //Float32 dot = q1.x*q2.x + q1.y*q2.y + q1.z*q2.z + q1.w*q2.w;
        Float32 dot = q1.Dot(q2);

        if (dot > -0.95f && dot < 0.95f)
        {
            Float32 angle = UOMath::Acos(dot);
            Float32 sina = UOMath::Sin(angle);
            Float32 sinat = UOMath::Sin(angle*t);
            Float32 sinaomt = UOMath::Sin(angle*(1 - t));
            return (q1*sinaomt + q2*sinat) / sina;
        }

        // if the angle is small, use linear interpolation
        return Qlerp(q1,q2,t);
    }


    /*
    Spherical cubic interpolation
    */
    inline UOMath::mthQuat Qsquad(const UOMath::mthQuat& q1, const UOMath::mthQuat& q2, const UOMath::mthQuat& a, const UOMath::mthQuat& b, Float32 t)
    {
        UOMath::mthQuat c = QslerpNoInvert(q1, q2, t);
        UOMath::mthQuat d = QslerpNoInvert(a, b, t);
        return QslerpNoInvert(c, d, 2*t*(1 - t));
    }


    /*
    Given 3 quaternions, qn-1,qn and qn+1, calculate a control point to be used in spline interpolation
    */
    inline UOMath::mthQuat Qspline(const UOMath::mthQuat& qnm1, const UOMath::mthQuat& qn, const UOMath::mthQuat& qnp1)
    {
        UOMath::mthQuat qni;

        qni.x = -qn.x;
        qni.y = -qn.y;
        qni.z = -qn.z;
        qni.w = qn.w;

        return qn*Qexp((Qlog(qni*qnm1) + Qlog(qni*qnp1)) / -4);
    }

} // namespace eg

#endif // 0


