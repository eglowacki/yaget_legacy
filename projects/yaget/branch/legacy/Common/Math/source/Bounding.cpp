#include "Precompiled.h"

#if 0

#include "Bounding.h"
#include "Util.h"


UOMath::AABBox::AABBox() :
m_Min(999999, 999999, 999999),
m_Max(-999999, -999999, -999999)
{
}


UOMath::AABBox::AABBox(const UOMath::vector3& a_MinCorner, const UOMath::vector3& a_MaxCorner) :
m_Min(a_MinCorner),
m_Max(a_MaxCorner)
{
}


void UOMath::AABBox::SetExtends(const UOMath::vector3& a_MinCorner, const UOMath::vector3& a_MaxCorner)
{
	m_Min = a_MinCorner;
	m_Max = a_MaxCorner;
}


void UOMath::AABBox::MakeInfinity()
{
	SetExtends(UOMath::vector3(-999999, -999999, -999999), UOMath::vector3(999999, 999999, 999999));
}


void UOMath::AABBox::Add(const UOMath::vector3& Pos)
{
	if (Pos.x < m_Min.x)
	{
		m_Min.x = Pos.x;
	}
	if (Pos.x > m_Max.x)
	{
		m_Max.x = Pos.x;
	}

	if (Pos.y < m_Min.y)
	{
		m_Min.y = Pos.y;
	}
	if (Pos.y > m_Max.y)
	{
		m_Max.y = Pos.y;
	}

	if (Pos.z < m_Min.z)
	{
		m_Min.z = Pos.z;
	}
	if (Pos.z > m_Max.z)
	{
		m_Max.z = Pos.z;
	}
}


UOMath::AABBox& UOMath::AABBox::Add(const UOMath::AABBox& a_Source)
{
	if (a_Source.m_Min.x < m_Min.x)
	{
		m_Min.x = a_Source.m_Min.x;
	}

	if (a_Source.m_Min.y < m_Min.y)
	{
		m_Min.y = a_Source.m_Min.y;
	}

	if (a_Source.m_Min.z < m_Min.z)
	{
		m_Min.z = a_Source.m_Min.z;
	}

	if (a_Source.m_Max.x > m_Max.x)
	{
		m_Max.x = a_Source.m_Max.x;
	}

	if (a_Source.m_Max.y > m_Max.y)
	{
		m_Max.y = a_Source.m_Max.y;
	}

	if (a_Source.m_Max.z > m_Max.z)
	{
		m_Max.z = a_Source.m_Max.z;
	}

	return *this;
}


void UOMath::AABBox::Expand(const std::vector<UOMath::vector3>& a_Source)
{
	for (std::vector<UOMath::vector3>::const_iterator it = a_Source.begin(); it != a_Source.end(); ++it)
	{
		Add(*it);
	}
}



void UOMath::AABBox::Move(const UOMath::vector3& a_ByAmount)
{
	m_Min += a_ByAmount;
	m_Max += a_ByAmount;
}


//! integer representation of a floating-point value
#define IR(x)	((unsigned int&)x)

bool UOMath::AABBox::IsHit(const UOMath::Ray &a_Ray, UOMath::vector3& a_Intersection) const
{
	const float kRAYAABB_EPSILON = 0.00001f;
	bool inside = true;
	UOMath::vector3 maxT(-1.0f);

	// Find candidate planes.
	for (unsigned int i = 0; i < 3; i++)
	{
		if (a_Ray.Origin[i] < m_Min[i])
		{
			a_Intersection[i] = m_Min[i];
			inside = false;

			// Calculate T distances to candidate planes
			if (IR(a_Ray.Dir[i]))
			{
				maxT[i] = (m_Min[i] - a_Ray.Origin[i]) / a_Ray.Dir[i];
			}
		}
		else if (a_Ray.Origin[i] > m_Max[i])
		{
			a_Intersection[i] = m_Max[i];
			inside = false;

			// Calculate T distances to candidate planes
			if (IR(a_Ray.Dir[i]))
			{
				maxT[i] = (m_Max[i] - a_Ray.Origin[i]) / a_Ray.Dir[i];
			}
		}
	}

	// Ray origin inside bounding box
	if (inside)
	{
		a_Intersection = a_Ray.Origin;
		return true;
	}

	// Get largest of the maxT's for final choice of intersection
	unsigned int whichPlane = 0;
	if (maxT[1] > maxT[whichPlane])
	{
		whichPlane = 1;
	}

	if (maxT[2] > maxT[whichPlane])
	{
		whichPlane = 2;
	}

	// Check final candidate actually inside box
	if (IR(maxT[whichPlane])&0x80000000)
	{
		return false;
	}

	for (unsigned int i = 0; i < 3; i++)
	{
		if (i != whichPlane)
		{
			a_Intersection[i] = a_Ray.Origin[i] + maxT[whichPlane] * a_Ray.Dir[i];

			if (a_Intersection[i] < m_Min[i] - kRAYAABB_EPSILON || a_Intersection[i] > m_Max[i] + kRAYAABB_EPSILON)
			{
				return false;
			}
		}
	}

	return true;	// ray hits box
}


bool UOMath::AABBox::IsIntersect(const UOMath::Ray &a_Ray) const
{
	// this constants are used to make the ray long
	const float t0 = 0.0f;
	const float t1 = 10000.0f;
	float tmin, tmax, tymin, tymax, tzmin, tzmax;

	float divx = 1 / a_Ray.Dir.x;
	if (divx >= 0)
	{
		tmin = (m_Min.x - a_Ray.Origin.x) * divx;
		tmax = (m_Max.x - a_Ray.Origin.x) * divx;
	}
	else
	{
		tmin = (m_Max.x - a_Ray.Origin.x) * divx;
		tmax = (m_Min.x - a_Ray.Origin.x) * divx;
	}
	float divy = 1 / a_Ray.Dir.y;
	if (divy >= 0)
	{
		tymin = (m_Min.y - a_Ray.Origin.y) * divy;
		tymax = (m_Max.y - a_Ray.Origin.y) * divy;
	}
	else
	{
		tymin = (m_Max.y - a_Ray.Origin.y) * divy;
		tymax = (m_Min.y - a_Ray.Origin.y) * divy;
	}
	if ((tmin > tymax) || (tymin > tmax))
	{
		return false;
	}
	if (tymin > tmin)
	{
		tmin = tymin;
	}
	if (tymax < tmax)
		tmax = tymax;

	float divz = 1 / a_Ray.Dir.z;
	if (divz >= 0)
	{
		tzmin = (m_Min.z - a_Ray.Origin.z) * divz;
		tzmax = (m_Max.z - a_Ray.Origin.z) * divz;
	}
	else
	{
		tzmin = (m_Max.z - a_Ray.Origin.z) * divz;
		tzmax = (m_Min.z - a_Ray.Origin.z) * divz;
	}
	if ((tmin > tzmax) || (tzmin > tmax))
	{
		return false;
	}
	if (tzmin > tmin)
	{
		tmin = tzmin;
	}
	if (tzmax < tmax)
	{
		tmax = tzmax;
	}

	return((tmin < t1) && (tmax > t0));
}


bool UOMath::AABBox::IsHit(const UOMath::Sphere& a_Sphere, UOMath::vector3& a_Intersection) const
{
	float s;
	float distance = 0;

	// first get x component
	if (a_Sphere.Center().x < Min().x)
	{
		s = a_Sphere.Center().x - Min().x;
		distance += s * s;
	}
	else if (a_Sphere.Center().x > Max().x)
	{
		s = a_Sphere.Center().x - Max().x;
		distance += s * s;
	}

	// next get y component
	if (a_Sphere.Center().y < Min().y)
	{
		s = a_Sphere.Center().y - Min().y;
		distance += s * s;
	}
	else if (a_Sphere.Center().y > Max().y)
	{
		s = a_Sphere.Center().y - Max().y;
		distance += s * s;
	}

	// then get z component
	if (a_Sphere.Center().z < Min().z)
	{
		s = a_Sphere.Center().z - Min().z;
		distance += s * s;
	}
	else if (a_Sphere.Center().z > Max().z)
	{
		s = a_Sphere.Center().z - Max().z;
		distance += s * s;
	}

	return distance < a_Sphere.Radius() * a_Sphere.Radius();
}


bool UOMath::AABBox::IsIntersect(const UOMath::AABBox& a_AABBox) const
{
	if (Max().x < a_AABBox.Min().x ||
		a_AABBox.Max().x < Min().x ||
		Max().y < a_AABBox.Min().y ||
		a_AABBox.Max().y < Min().y ||
		Max().z < a_AABBox.Min().z ||
		a_AABBox.Max().z < Min().z)
	{
		return false;
	}

	return true;
}


#endif // 0

