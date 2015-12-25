///////////////////////////////////////////////////////////////////////
// Util.h
//
//  Copyright 10/1/2005 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      This implements some of the geometry
//      helper objects, like Ray, Box, etc.
//
//  #include <Math\Util.h>
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef MATH_UTIL_H
#define MATH_UTIL_H
#pragma once

#if 0
#include "Matrix.h"


namespace eg
{
    /*!
    For more info lookup "Game Programming Gems 4"
    Critically Damped Easy-In/Easy-Out Smoothing.
    \param from start value
    \param to to end value
    \param vel [OUT] velocity at this point at this time
    \param smoothTime how long it takes to reach end value at maximum velocity
    \param deltaTime time for the last frame
    \return value between 'from' and 'to'

    \code
        // m_CurrEyePoint, m_DesiredEyePoint, m_Velocity and m_SmoothTime are member variables
        // m_CurrEyePoint(0, -5, 3)
        // m_DesiredEyePoint(0, -5, 3)
        // m_Velocity(0, 0, 0)
        // m_SmoothTime(0)

        void FooClass::SetNewEyePosition(const UOMath::vector3& a_NewEyePos)
        {
        // This is one way of doing:
        m_DesiredEyePoint = a_NewEyePos;
        UOMath::vector3 lineFromTo = m_CurrEyePoint - m_DesiredEyePoint;
        float distance = lineFromTo.length();
        // this speed can be set by the user for whatever the value is good
        const float kSpeed = 10.0f;
        m_SmoothTime = distance / kSpeed;
        }

        void FooClass::Update(float a_DeltaTime)
        {
        // m_SmoothTime was already calculated at the initial setting of m_DesiredEyePoint
            UOMath::vector3  newCurrEyePoint = SmoothCD(m_CurrEyePoint, m_DesiredEyePoint, m_Velocity, m_SmoothTime, a_DeltaTime);
            newCurrEyePoint += m_Velocity * a_DeltaTime;
            m_SmoothTime -= a_DeltaTime;
            m_CurrEyePoint = newCurrEyePoint;
        }
    \endcode
    */
    template <typename T>
    inline T SmoothCD(const T& a_From, const T& a_To, T& a_Vel, float a_SmoothTime, float a_DeltaTime)
    {
        float omega = 2.0f / a_SmoothTime;
        float x = omega * a_DeltaTime;
        float exp = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);
        T change = a_From - a_To;
        T temp = (a_Vel + omega * change) * a_DeltaTime;
        a_Vel = (a_Vel - omega * temp) * exp;
        return a_To + (change + temp) * exp;
    }

    /*!
    This will return hash value based on the string.
    \param a_String string to calculate hash value for
    \param a_CaseSensitive preserve and take into account case of the string
    \param a_InitValue arbitrary value. Hashing a key with different
           a_InitValue parameter values produces independent hash values,
           e.g. by using the previous hash value for a_InitValue.
    \return hash value of the string.
    NOTE: if a_CaseSensitive is FALSE, then this function incurs
    overhead of makeing string lower case
    */
    //Uint32 Hash32(const cRZString& a_String, bool a_CaseSensitive = true, Uint32 a_InitValue = 0);
    /*!
    This will return hash based on 2 values
    */
    //Uint32 Hash32(Uint32 a_ValueA, Uint32 a_ValueB);
    /*!
    Rerturn hash value based on some Uint32 number.
    */
    //Uint32 Hash32(Uint32 a_Value);


    /*!
    This simple class is used to represent ray, which has Origin and direction.
    Direction is normalized.
    */
    class Ray
    {
    public:
        Ray() : Origin(vector3::ZERO), Dir(vector3::ONE)
        {}

        Ray(const vector3& o, const vector3& d) : Origin(o), Dir(d)
        {}

        vector3 Origin;
        vector3 Dir;
    };


    /*!
    Calculate extends of the vertex position passed to this class.
    It's simple axis aligned bounding box where all the points
    are inside that box.
    */
    class Extends
    {
    public:
        Extends() : m_Min(999999, 999999, 999999), m_Max(-999999, -999999, -999999)
        {
        }

        //! Add single pos and recalculate extends if needed.
        void Add(const vector3& Pos)
        {
            if (Pos.x < m_Min.x)
                m_Min.x = Pos.x;
            if (Pos.y < m_Min.y)
                m_Min.y = Pos.y;
            if (Pos.z < m_Min.z)
                m_Min.z = Pos.z;

            if (Pos.x > m_Max.x)
                m_Max.x = Pos.x;
            if (Pos.y > m_Max.y)
                m_Max.y = Pos.y;
            if (Pos.z > m_Max.z)
                m_Max.z = Pos.z;
        }

        //! Minimum extend of this box.
        const vector3& Min() const
        {
            return m_Min;
        }

        //! Maximum extend of this box.
        const vector3& Max() const
        {
            return m_Max;
        }

        vector3 Center() const
        {

            return m_Min + (Size() / 2.0f);
        }

        //! Return size of this Extends in x, y and z direction
        vector3 Size() const
        {
            return m_Max - m_Min;
        }

        //! This will return Upper Left corner of this extend box
        vector3 GetUL() const
        {
            return vector3(m_Min.x, m_Max.y, m_Max.z);
        }

        //! This will return Upper Right corner of this extend box
        vector3 GetUR() const
        {
            return vector3(m_Max.x, m_Max.y, m_Max.z);
        }

        //! This will return Lower Right corner of this extend box
        vector3 GetLR() const
        {
            return vector3(m_Max.x, m_Max.y, m_Min.z);
        }

        //! This will return Lower Left corner of this extend box
        vector3 GetLL() const
        {
            return vector3(m_Min.x, m_Max.y, m_Min.z);
        }

    private:
        vector3 m_Min, m_Max;
    };


    /*!
    This hold the plane in the standard equation:
    Ax + By + CZ + D = 0
    where ABC are normal of the plane
    xyz is point on the plane
    and D is how far is the point of the plane (if xyz is on the plane, then D will be 0)
    or IOW:
    if Ax + By + Cz = 0 then point xyz lies on the plane.
    */
    class Plane
    {
    public:
        Plane() :
        m_D(0), m_Normal(vector3::ZONE)
        {
        }
        //! This ctor will create plane from ray, where plane is perpendicualr to the direction
        Plane(const UOMath::Ray& a_Ray);
        //! This allows us to create plane with one point and the plane normal
        Plane(const vector3& a_Normal, const vector3& a_Point);
        //! Create plane from this 3 points
        Plane(const vector3& p0, const vector3& p1, const vector3& p2)  :
        m_D(0), m_Normal(vector3::ZONE)
        {
            Set(p0, p1, p2);
        }

        //! Create plane from this 3 points
        void Set(const vector3& p0, const vector3& p1, const vector3& p2);
        //! Return Normal of the plane
        const vector3& Normal() const {return m_Normal;}
        //! Return D variable representing distance from origin to plane
        float D() const {return m_D;}

        /// This will return TRUE if ray intersects this plane and will put intersection point
        /// in to IntersectionPoint param, otherwise it will return FALSE and will
        /// not modify IntersectionPoint param.
        bool IsRayIntersection(const Ray& Ray, vector3& IntersectionPoint) const;
        /// Returns intersection point between Ray and this plane. If there was no intersection,
        /// it will return vector3(0, 0, 0).
        vector3 GetIntersectionPoint(const Ray& Ray)const;
        /*!
        This method will return TRUE, if point is on positive side of the plane, otherwise FALSE
        Positive side is considered towards normal of the plane.
        */
        bool IsPositive(const vector3& a_Point) const;

    private:
        vector3 m_Normal;
        float m_D;
    };


    /*!
    Helper class to be used in touching algorithm. This will contain
    Center position and radius.
    */
    class Sphere
    {
    public:
        Sphere() : m_CenterPoint(vector3::ZERO), m_Radius(0)
        {
        }

        Sphere(const vector3& CenterPoint, float Radius) :
        m_CenterPoint(CenterPoint), m_Radius(Radius)
        {
        }

        void Set(const vector3& CenterPoint, float Radius)
        {
            m_CenterPoint = CenterPoint;
            m_Radius = Radius;
        }

        const vector3& Center() const
        {
            return m_CenterPoint;
        }

        vector3& Center()
        {
            return m_CenterPoint;
        }

        float Radius() const
        {
            return m_Radius;
        }

        /*!
        This will test if the sphere intersects this ray
        */
        bool IsRayIntersection(const UOMath::Ray& a_Ray) const;

    private:
        vector3 m_CenterPoint;
        float m_Radius;
    };


    //! This will convert in to string
    std::string IntToString(int Value);

    //! This will convert float to string with desmo points, up to Precision
    std::string FloatToString(float Value, int Precision);


    // container for transformation
    struct cTransformationParams
    {
        cTransformationParams() :
            m_scale(1, 1, 1),
            m_rotation(0, 0, 0),
            m_offset(0, 0, 0),
            m_Valid(true)
        {}

        cTransformationParams(const cTransformationParams *a_Source) :
            m_scale(a_Source->m_scale),
            m_rotation(a_Source->m_rotation),
            m_offset(a_Source->m_offset),
            m_Valid(a_Source->m_Valid)
        {}

        cTransformationParams(const cTransformationParams& a_Source) :
            m_scale(a_Source.m_scale),
            m_rotation(a_Source.m_rotation),
            m_offset(a_Source.m_offset),
            m_Valid(a_Source.m_Valid)
        {}

        void Reset()
        {
            m_scale.set(1, 1, 1);
            m_rotation.set(0, 0, 0);
            m_offset.set(0, 0, 0);
            m_Valid = false;
        }

        void Set(const cTransformationParams *a_Source)
        {
            m_scale = a_Source->m_scale;
            m_rotation = a_Source->m_rotation;
            m_offset = a_Source->m_offset;
            m_Valid = a_Source->m_Valid;
        }

        std::string Print() const
        {
            return std::string("Scale: " + m_scale.Print() + ", Rotation: " + m_rotation.Print() + ", Offset: " + m_offset.Print());
        }

        void Validate()
        {
            if (m_scale.almost_eq(UOMath::vector3::ONE) &&
                m_rotation.almost_eq(UOMath::vector3::ZERO) &&
                m_offset.almost_eq(UOMath::vector3::ZERO))
            {
                m_Valid = false;
            }
            else
            {
                m_Valid = true;
            }
        }

        UOMath::vector3 m_scale;
        UOMath::vector3 m_rotation;
        UOMath::vector3 m_offset;
        //! This is used for optimizations, so if the values are defualt,
        //! we can find out very fast
        bool m_Valid;
    };


} // namespace eg

#endif // 0


#endif // MATH_UTIL_H
