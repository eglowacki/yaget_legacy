#include "Precompiled.h"

#if 0
#include "Util.h"


using namespace UOMath;

Uint32 UOMath::Hash32(const cRZString& a_String, bool a_CaseSensitive, Uint32 a_InitValue)
{
	if (a_CaseSensitive)
	{
		return nGZNumerics::Hash32((Uint8 *)a_String.ToChar(), (Uint)a_String.Strlen(), a_InitValue);
	}

	cRZString tempString = a_String;
	tempString.MakeLower();
	return UOMath::Hash32(tempString, true);
}


Uint32 UOMath::Hash32(Uint32 a_ValueA, Uint32 a_ValueB)
{
	return nGZNumerics::Hash32(a_ValueA, a_ValueB);
}


Uint32 UOMath::Hash32(Uint32 a_Value)
{
	return nGZNumerics::Hash32(a_Value);
}


// This will return TRUE if ray intersects this plane and will put intersetion point
// in to IntersectionPoint param, otherwise it will return FALSE and will
// not modify IntersectionPoint param.
bool UOMath::Plane::IsRayIntersection(const Ray& Ray, vector3& IntersectionPoint) const
{
	float dot = DotProduct(m_Normal, Ray.Dir);
	// ray is parallel to the plane, so there is no intersection
	if (IsAEq(dot, 0.0f, 0.001f))
		return false;

	float Value = -(DotProduct(m_Normal, Ray.Origin) + m_D);
	float t = Value/dot;
	IntersectionPoint = Ray.Origin + (t * Ray.Dir);
	return true;
}


bool UOMath::Plane::IsPositive(const vector3& a_Point) const
{
	float Value = -(DotProduct(m_Normal, a_Point) + m_D);

	// if value is negative, then the point is on the postive side of the plane
	return Value < 0;
}


// Returns intersetion point between Ray and this plane. If there was no intersection,
// it will return vector3(0, 0, 0).
vector3 UOMath::Plane::GetIntersectionPoint(const Ray& Ray) const
{
	vector3 IntersectionPoint(vector3::ZERO);
	IsRayIntersection(Ray, IntersectionPoint);
	return IntersectionPoint;
}


UOMath::Plane::Plane(const UOMath::Ray& a_Ray) :
	m_Normal(a_Ray.Dir),
	m_D(0)
{
	m_D = DotProduct(-m_Normal, a_Ray.Origin);
}


UOMath::Plane::Plane(const vector3& a_Normal, const vector3& a_Point) :
	m_Normal(a_Normal),
	m_D(0)
{
	m_D = DotProduct(-m_Normal, a_Point);
}

void UOMath::Plane::Set(const vector3& p0, const vector3& p1, const vector3& p2)
{
	m_Normal = CrossProductN(p0, p1, p2);
	//m_D = DotProduct(p0, m_Normal);
	m_D = DotProduct(-m_Normal, p0);
}

bool UOMath::Sphere::IsRayIntersection(const UOMath::Ray& a_Ray) const
{
	const UOMath::vector3& vd = a_Ray.Dir;
	UOMath::vector3 vco = a_Ray.Origin - m_CenterPoint;

	float fA = vd.length();
	fA *= fA;

	float fB = 2.f * DotProduct(vco, vd);

	float fC = vco.length();
	fC = ( fC * fC ) - ( m_Radius * m_Radius );

    float fDiscriminant = ( fB * fB ) - ( 4.f * fA * fC );

	if( fDiscriminant < 0.f )
	{
		return false;
	}
	return true;
}

std::string UOMath::IntToString(int Value)
{
	static char WorkBuffer[128];
#ifdef K_MSVC
	sprintf(WorkBuffer, "%d", Value);
#else
	snprintf(WorkBuffer, 128, "%d", Value);
#endif
	return std::string(WorkBuffer);
}


std::string UOMath::FloatToString(float Value, int Precision)
{
	static char WorkBuffer[128];
#ifdef K_MSVC
	sprintf(WorkBuffer, "%.*f", Precision, Value);
#else
	snprintf(WorkBuffer, 128, "%.*f", Precision, Value);
#endif
	return std::string(WorkBuffer);
}

#endif // 0
