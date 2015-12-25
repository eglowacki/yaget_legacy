#ifndef _MTH_BOUNDING_H_
#define _MTH_BOUNDING_H_


#if 0

//////////////////////////////////////////////////////////////////////
//
//	(c) 2004 Electronic Arts Inc.
//
//	#include <mthBounding.h>
//
//	           DESCRIPTION: This file provides bounding objects used in collision
//							and representation for the bounding volume
//
//	       ORIGINAL AUTHOR: eglowacki
//
//	ORIGINAL CREATION DATE: 6/14/2004
//
//
//////////////////////////////////////////////////////////////////////
//! \file

#include <vector>
#include "Util.h"

namespace UOMath
{
	class Ray;

	class AABBox
	{
	public:
		AABBox();
		AABBox(const UOMath::vector3& a_MinCorner, const UOMath::vector3& a_MaxCorner);
		//! Set minimum and maximum extends fro this bounding box
		void SetExtends(const UOMath::vector3& a_MinCorner, const UOMath::vector3& a_MaxCorner);
		//! This will make bounding box infinity in size
		void MakeInfinity();
		//! This will take all kist of points and create bounding box
		void Expand(const std::vector<UOMath::vector3>& a_Source);
		//! Add single pos and recalculate extends if needed.
		void Add(const UOMath::vector3& Pos);
		//! Add source aa bounding box to this and grow if necessary
		AABBox& Add(const UOMath::AABBox& a_Source);
		//! This will move entrire bounding box by the amount
		void Move(const UOMath::vector3& a_ByAmount);

		//! Minimum extend of this box, which is closer lower left corner
		const UOMath::vector3& Min() const
		{
			return m_Min;
		}

		//! Maximum extend of this box, which is further upper right corner
		const UOMath::vector3& Max() const
		{
			return m_Max;
		}

		//! Center of the boundig box
		UOMath::vector3 Center() const
		{
			return m_Min + (Size() / 2.0f);
		}

		//! Return size of this Extends in x, y and z direction, which is diagonal line from min to max
		UOMath::vector3 Size() const
		{
			return m_Max - m_Min;
		}

		/*!
		This will return TRUE, if the ray intersects this box
		\param a_Ray in the same coordinate space as this box
		\return TRUE ray intersects this box, otherwise FALSE
		*/
		bool IsIntersect(const UOMath::Ray &a_Ray) const;
		/*!
		This will return true if 2 aabb intersect eachother
		\return TRUE a_AABBox intersects this box, otherwise FALSE
		*/
		bool IsIntersect(const UOMath::AABBox& a_AABBox) const;
		/*!
		This will return TRUE, if the ray intersects this box
		including calculating intersection point
		\param a_Ray in the same coordinate space as this box
		\param a_Intersection [OUT] intersection of ray and box in 3d coordinates
		\return TRUE ray intersects this box, otherwise FALSE
		*/
		bool IsHit(const UOMath::Ray &a_Ray, UOMath::vector3& a_Intersection) const;
		/*!
		This will return TRUE, if the sphere intersects this box
		including calculating intersection point
		\param a_Sphere in the same coordinate space as this box
		\param a_Intersection [OUT] intersection of ray and box in 3d coordinates
		\return TRUE sphere intersects this box, otherwise FALSE
		NOTE a_Intersection is not implemented yet!!!
		*/
		bool IsHit(const UOMath::Sphere& a_Sphere, UOMath::vector3& a_Intersection) const;

	private:
		UOMath::vector3 m_Min, m_Max;
	};

} // namespace UOMath

#endif // 0

#endif // _MTH_BOUNDING_H_
