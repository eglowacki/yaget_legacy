#include "Precompiled.h"

#if 0

#include "Intersections.h"

using namespace std;
using namespace UOMath;


bool UOMath::IsPointInsideTri(const UOMath::vector3& p0,
							  const UOMath::vector3& p1,
							  const UOMath::vector3& p2,
							  const UOMath::vector3& p)
{
	vector3 n = CrossProductN(p0, p1, p2);

	vector3 ta = p1 - p0;
	vector3 pa = p - p0;
	vector3 crossA = CrossProduct(ta, pa);
	float d0 = DotProduct(crossA, n);

	vector3 tb = p2 - p1;
	vector3 pb = p - p1;
	vector3 crossB = CrossProduct(tb, pb);
	float d1 = DotProduct(crossB, n);

	vector3 tc = p0 - p2;
	vector3 pc = p - p2;
	vector3 crossC = CrossProduct(tc, pc);
	float d2 = DotProduct(crossC, n);

	return (d0 + 0.001f) >= 0.0f && (d1 + 0.001f) >= 0.0f && (d2 + 0.001f) >= 0.0f;

	/*
	vector3 n = CrossProduct(p0, p1, p2);

	float u0, u1, u2;
	float v0, v1, v2;

	// find axis which is dominant for computing u and v
	if (fabs(n.x) > fabs(n.y))
	{
		if (fabs(n.x) > fabs(n.z))
		{
			u0 = p.y - p0.y;
			u1 = p1.y - p0.y;
			u2 = p2.y - p0.y;

			v0 = p.z - p0.z;
			v1 = p1.z - p0.z;
			v2 = p2.z - p0.z;
		}
		else
		{
			u0 = p.x - p0.x;
			u1 = p1.x - p0.x;
			u2 = p2.x - p0.x;

			v0 = p.y - p0.y;
			v1 = p1.y - p0.y;
			v2 = p2.y - p0.y;
		}
	}
	else
	{
		if (fabs(n.y) > fabs(n.z))
		{
			u0 = p.x - p0.x;
			u1 = p1.x - p0.x;
			u2 = p2.x - p0.x;

			v0 = p.z - p0.z;
			v1 = p1.z - p0.z;
			v2 = p2.z - p0.z;
		}
		else
		{
			u0 = p.x - p0.x;
			u1 = p1.x - p0.x;
			u2 = p2.x - p0.x;

			v0 = p.y - p0.y;
			v1 = p1.y - p0.y;
			v2 = p2.y - p0.y;
		}
	}

	// make sure that denominator is valid
	float Denom = u1 * v2 - v1 * u2;
	if (IsAEq(Denom, 0.0f))
	{
		// no intersection
		return false;
	}

	Denom = 1.0f / Denom;

	// calculate barycentric coordinates, checking
	// for out of range at each calculation
	float Alpha = (u0 * v2 - v0 * u2) * Denom;
	if (!(Alpha >= 0))
	{
		// no intersection
		return false;
	}

	float Beta = (u1 * v0 - v1 * u0) * Denom;
	if (!(Beta >= 0))
	{
		// no intersection
		return false;
	}

	float Gamma = 1.0f - Alpha - Beta;
	if (!(Gamma >= 0))
	{
		// no intersection
		return false;
	}

	return true;
	*/
}


bool UOMath::IsPointInsideTri(const UOMath::vector3& p0,
							  const UOMath::vector3& p1,
							  const UOMath::vector3& p2,
							  const UOMath::vector3& p,
							  UOMath::vector3& IntersectionPoint)
{
	UOMath::Ray ray(p, UOMath::vector3(0, 0, -1));
	return IsRayIntersectTri(p0, p1, p2, ray, IntersectionPoint);
}



bool UOMath::IsHighPointInsideTri(const UOMath::vector3& p0,
								  const UOMath::vector3& p1,
								  const UOMath::vector3& p2,
								  const UOMath::vector2& p,
								  UOMath::vector3& IntersectionPoint)
{
	UOMath::Ray ray(UOMath::vector3(p.x, p.y, 1000.0f), UOMath::vector3(0, 0, -1));
	return IsRayIntersectTri(p0, p1, p2, ray, IntersectionPoint);
}


bool UOMath::IsRayIntersectTri(const UOMath::vector3& p0,
							   const UOMath::vector3& p1,
							   const UOMath::vector3& p2,
							   const UOMath::Ray& a_Ray,
							   UOMath::vector3& a_IntersectionPoint)
{
	UOMath::Plane plane(p0, p1, p2);
	//vector3 Point = plane.GetIntersectionPoint(a_Ray);
	vector3 point;
    if (plane.IsRayIntersection(a_Ray,  point))
	{
		if (IsPointInsideTri(p0, p1, p2, point))
		{
			a_IntersectionPoint = point;
			return true;
		}
	}

	return false;
}



bool UOMath::IsPointInsideQuad(const UOMath::vector3& p0,
							   const UOMath::vector3& p1,
							   const UOMath::vector3& p2,
							   const UOMath::vector3& p3,
							   const UOMath::vector3& p)
{
	bool result1 = IsPointInsideTri(p0, p1, p2, p);
	bool result2 = IsPointInsideTri(p0, p2, p3, p);

	return (result1 || result2);
}

bool UOMath::IsRayIntersectQuad(const UOMath::vector3& p0,
						   const UOMath::vector3& p1,
						   const UOMath::vector3& p2,
						   const UOMath::vector3& p3,
						   const UOMath::Ray& Ray,
						   UOMath::vector3& IntersectionPoint)
{
	if(IsRayIntersectTri(p0,p1,p2,Ray,IntersectionPoint)
		|| IsRayIntersectTri(p0,p2,p3,Ray,IntersectionPoint))
	{
		return true;
	}

	return false;

}


Float32 UOMath::DistancePointToLine(const UOMath::vector3& a_Point,
									const UOMath::vector3& a_Line0,
									const UOMath::vector3& a_Line1)
{
	UOMath::vector3 v = a_Line1 - a_Line0;
	UOMath::vector3 w = a_Point - a_Line0;
	Float32 c1 = UOMath::DotProduct(w, v);
    Float32 c2 = UOMath::DotProduct(v, v);
    Float32 b = c1 / c2;
	UOMath::vector3 Pb = a_Line0 + b * v;
	UOMath::vector3 d = a_Point - Pb;
	return d.length();
}


Float32 UOMath::DistancePointToLineSq(const UOMath::vector3& a_Point,
									  const UOMath::vector3& a_Line0,
									  const UOMath::vector3& a_Line1)
{
	UOMath::vector3 v = a_Line1 - a_Line0;
	UOMath::vector3 w = a_Point - a_Line0;
	Float32 c1 = UOMath::DotProduct(w, v);
	Float32 c2 = UOMath::DotProduct(v, v);
	Float32 b = c1 / c2;
	UOMath::vector3 Pb = a_Line0 + b * v;
	UOMath::vector3 d = a_Point - Pb;
	return d.lengthSqr();
}


#endif // 0

