#ifndef _MTH_INTERSECTIONS_H_
#define _MTH_INTERSECTIONS_H_


#if 0

//////////////////////////////////////////////////////////////////////
//
//	(c) 2003 Electronic Arts Inc.
//
//	           DESCRIPTION: This header provides simple intersection methods
//
//	       ORIGINAL AUTHOR: eglowacki
//
//	ORIGINAL CREATION DATE: 1/5/2004
//
//	               $Author: jrein $
//	                 $Date: 2004/06/02 $
//	             $Revision: #2 $
//	               $Change: 45105 $
//
//////////////////////////////////////////////////////////////////////

#include "Util.h"

namespace UOMath
{
	/// NOTE: This methods are _NOT_ optimized yet.


	/// This will check if point is inside the triangle.
	/// \param p0 triangle point 0
	/// \param p1 triangle point 1
	/// \param p2 triangle point 2
	/// \param p point to test if it's inside the triangle
	/// \return TRUE if point is inside the triangle, otherwise FALSE
	bool IsPointInsideTri(const UOMath::vector3& p0,
						  const UOMath::vector3& p1,
						  const UOMath::vector3& p2,
						  const UOMath::vector3& p);

	/*!
	This will check if point is inside the triangle.
	\param p0 triangle point 0
	\param p1 triangle point 1
	\param p2 triangle point 2
	\param p point to test if it's inside the triangle
	\param IntersectionPoint [OUT] intersection point if accured
	\return TRUE if point is inside the triangle, otherwise FALSE

	NOTE: about point p, this will convert that point into ray with origin
	at p and direction 0, 0, -1, which means it will poind down
	*/
	bool IsPointInsideTri(const UOMath::vector3& p0,
						  const UOMath::vector3& p1,
						  const UOMath::vector3& p2,
						  const UOMath::vector3& p,
						  UOMath::vector3& IntersectionPoint);

	/*!
	This will check if point is inside the triangle.
	\param p0 triangle point 0
	\param p1 triangle point 1
	\param p2 triangle point 2
	\param p point to test if it's inside the triangle, which z will be modified to be very high
	\param IntersectionPoint [OUT] intersection point if accured
	\return TRUE if point is inside the triangle, otherwise FALSE

	NOTE: about point p, this will convert that point into ray with origin
	at p and direction 0, 0, -1, which means it will poind down
	*/
	bool IsHighPointInsideTri(const UOMath::vector3& p0,
							  const UOMath::vector3& p1,
							  const UOMath::vector3& p2,
							  const UOMath::vector2& p,
							  UOMath::vector3& IntersectionPoint);

	/// This will check if Ray intersects inside the triangle.
	/// \param p0 triangle point 0
	/// \param p1 triangle point 1
	/// \param p2 triangle point 2
	/// \param Ray ray to test if intersects the triangle
	/// \return TRUE if ray intersects, otherwise FALSE
	bool IsRayIntersectTri(const UOMath::vector3& p0,
						   const UOMath::vector3& p1,
						   const UOMath::vector3& p2,
						   const UOMath::Ray& Ray,
						   UOMath::vector3& IntersectionPoint);

	/// This will check if point is inside the quad.
	/// \param p0 triangle point 0, upper left corner
	/// \param p1 triangle point 1, upper right corner
	/// \param p2 triangle point 2, lower right corner
	/// \param p3 triangle point 3, lower left corner
	/// \param p point to test if it's inside the triangle
	/// \return TRUE if point is inside the triangle, otherwise FALSE
	bool IsPointInsideQuad(const UOMath::vector3& p0,
						   const UOMath::vector3& p1,
						   const UOMath::vector3& p2,
						   const UOMath::vector3& p3,
						   const UOMath::vector3& p);

	/// This will check if ray intersects the quad.
	/// \param p0 triangle point 0, upper left corner
	/// \param p1 triangle point 1, upper right corner
	/// \param p2 triangle point 2, lower right corner
	/// \param p3 triangle point 3, lower left corner
	/// \param Ray ray to test if intersects the quad
	/// \return TRUE if ray intersects, otherwise FALSE
	bool IsRayIntersectQuad(const UOMath::vector3& p0,
						   const UOMath::vector3& p1,
						   const UOMath::vector3& p2,
						   const UOMath::vector3& p3,
						   const UOMath::Ray& Ray,
						   UOMath::vector3& IntersectionPoint);

	/*!
	This will return closest distance from point to line segment
	\param a_Point point to calculate distance from the line
	\param a_Line0 beginning endpoint of the line
	\param a_Line0 ending endpoint of the line
	\return closest distance from point to line segment
	*/
	Float32 DistancePointToLine(const UOMath::vector3& a_Point,
								const UOMath::vector3& a_Line0,
								const UOMath::vector3& a_Line1);

	/*!
	This will return closest distance from point to line segment as a squared value
	\param a_Point point to calculate distance from the line
	\param a_Line0 beginning endpoint of the line
	\param a_Line0 ending endpoint of the line
	\return closest distance from point to line segment as a squared value
	NOTE: This avoids the square root
	*/
	Float32 DistancePointToLineSq(const UOMath::vector3& a_Point,
								  const UOMath::vector3& a_Line0,
								  const UOMath::vector3& a_Line1);

} // namespace UOMath

#endif // 0

#endif // _MTH_INTERSECTIONS_H_

