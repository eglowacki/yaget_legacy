#include "Precompiled.h"
#include "Math\Vector.h"


namespace eg {


////////////////////////////////////////////////////////////
// Miscellaneous vector functions
//



/*!
This functions is provided in Utility projects, sicne I want to keep
Math lib not dependent on wxWidgets.
*/
extern void GetFloatValues(const char *pValue, float *a_FloatValues, size_t numValues);


////////////////////////////////////////////////////////////
// Debug functions
//
//! Return string represention of this vector in the form [x: %.2f, y: %.2f].
std::string Vector2::Print() const
{
	static char WorkBuffer[256];
	sprintf(WorkBuffer, "[x: %.2f, y: %.2f]", x, y);
	return std::string(WorkBuffer);
}


//! Return string represention of this vector in the form [x: %.2f, y: %.2f, z: %.2f].
std::string Vector3::Print() const
{
	static char WorkBuffer[256];
	sprintf(WorkBuffer, "[x: %.2f, y: %.2f, z: %.2f]", x, y, z);
	return std::string(WorkBuffer);
}

//! Return string represention of this vector in the form [x: %.2f, y: %.2f, z: %.2f, w: %.2f].
std::string Vector4::Print() const
{
	static char WorkBuffer[256];
	sprintf(WorkBuffer, "[x: %.2f, y: %.2f, z: %.2f, w: %.2f]", x, y, z, w);
	return std::string(WorkBuffer);
}


#if 0

void Vector3::set(const char *pValue)
{
	float values[4] = {0, 0, 0, 1};
	GetFloatValues(pValue, values, 4);
	set(values);
}


void Vector4::set(const char *pValue)
{
	float values[4] = {0, 0, 0, 1};
	GetFloatValues(pValue, values, 4);
	set(values);
}


void Vector2::set(const char *pValue)
{
	float values[4] = {0, 0, 0, 1};
	GetFloatValues(pValue, values, 4);
	set(values);
}


//#if 0
float Angle(const Vector3& v1, const Vector3& v2)
{
	float dot = DotProduct(v1, v2);
	Clamp(dot, -1.0f, 1.0f);
	float angle = Acos(dot);
	return angle;
}


float AngleD(const Vector3& v1, const Vector3& v2)
{
	float angle = Angle(v1, v2);

	float degAngle = RadToDeg(angle);
	return angle;
}

#endif // 0
} // namespace eg
