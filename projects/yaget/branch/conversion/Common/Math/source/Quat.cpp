#include "Precompiled.h"

#if 0
#include "Quat.h"


const UOMath::mthQuat&  UOMath::mthQuat::Identity()
{
    static UOMath::mthQuat m_Identity(0.0f, 0.0f, 0.0f, 1.0f);
    return m_Identity;
};


float	UOMath::mthQuat::Dot(const UOMath::mthQuat& q) const
{
	return DotProduct(Axis(), q.Axis()) + w*q.w;
}


void UOMath::mthQuat::SetEulerRad(const UOMath::vector3& a_Angles)
{
	SetEuler(a_Angles.x, a_Angles.y, a_Angles.z);
}


void UOMath::mthQuat::SetEulerDeg(const UOMath::vector3& a_Angles)
{
	SetEuler(UOMath::DegToRad(a_Angles.x), UOMath::DegToRad(a_Angles.y), UOMath::DegToRad(a_Angles.z));
}


/**********************************************************************************
Description: SetEuler - converts euler angles into a quaternion

      Input: a_Yaw - yaw angle (around Y-axis) in radians
             a_Pitch - pitch angle (around X-axis) in radians
             a_Roll - roll angle (around Z-axis) in radians
***********************************************************************************/
void UOMath::mthQuat::SetEuler(float a_Yaw, float a_Pitch, float a_Roll)
{
	a_Yaw /= 2.f;
	a_Pitch /= 2.f;
	a_Roll /= 2.f;
    float cosY = cosf(a_Yaw);
    float sinY = sinf(a_Yaw);
    float cosP = cosf(a_Pitch);
    float sinP = sinf(a_Pitch);
    float cosR = cosf(a_Roll);
    float sinR = sinf(a_Roll);
    x = cosR * sinP * cosY + sinR * cosP * sinY;
    y = cosR * cosP * sinY - sinR * sinP * cosY;
    z = sinR * cosP * cosY - cosR * sinP * sinY;
    w = cosR * cosP * cosY + sinR * sinP * sinY;
}

void UOMath::mthQuat::SetAxisAngle (const UOMath::vector3& a_Axis, float a_Angle)
{
  float s = sinf(a_Angle/2);
  x = a_Axis.x * s;
  y = a_Axis.y * s;
  z = a_Axis.z * s;
  w = cosf(a_Angle/2);
}


/**********************************************************************************
Description: Normalize the quaternion
***********************************************************************************/
void UOMath::mthQuat::Normalize()
{
    float invLen = 1.0f / sqrtf(w*w + x*x + y*y + z*z);
    if (invLen == 0) return;

    x *= invLen;
    y *= invLen;
    z *= invLen;
    w *= invLen;
}


void UOMath::mthQuat::Conjugate()
{
	x *= -1;
	y *= -1;
	z *= -1;
}


void UOMath::mthQuat::Inverse()
{
	// make sure that this quat is normalized
	// so we can just use Conjugate to inverse this quat
	Normalize();
	Conjugate();
}


UOMath::mthQuat UOMath::mthQuat::Multiply(const UOMath::mthQuat& a)
{
    UOMath::mthQuat r;
	r.w     = (w*a.w) - DotProduct(Axis(), a.Axis());
    r.Axis()= (a.Axis()*w) + (Axis() * a.w) + CrossProduct(Axis(), a.Axis());
    return r;
}


void UOMath::mthQuat::Create(const UOMath::matrix44& a_Matrix)
{
	matrix33 rotMatrix = a_Matrix.GetRotation();
	Create(rotMatrix);
}



void UOMath::mthQuat::Create(const UOMath::matrix33& a_Matrix)
{
	float trace = a_Matrix[0][0] + a_Matrix[1][1] + a_Matrix[2][2] + 1.0f;
	if ( trace > 0.001f)//M_EPSILON )
	{
		float s = 0.5f / UOMath::Sqrt(trace);
		w = 0.25f / s;
		x = ( a_Matrix[1][2] - a_Matrix[2][1] ) * s;
		y = ( a_Matrix[2][0] - a_Matrix[0][2] ) * s;
		z = ( a_Matrix[0][1] - a_Matrix[1][0] ) * s;
	}
	else
	{
		if ( a_Matrix[0][0] > a_Matrix[1][1] && a_Matrix[0][0] > a_Matrix[2][2] )
		{
			// this needs to be looked to make sure that we have this math here corectly
			// since we are in column major, but memory layout is in row major
			float s = 2.0f * UOMath::Sqrt( 1.0f + a_Matrix[0][0] - a_Matrix[1][1] - a_Matrix[2][2]);
			x = 0.25f * s;
			y = (a_Matrix[0][1] + a_Matrix[1][0] ) / s;
			z = (a_Matrix[0][2] + a_Matrix[2][0] ) / s;
			w = (a_Matrix[1][2] - a_Matrix[2][1] ) / s;
		}
		else if (a_Matrix[1][1] > a_Matrix[2][2])
		{
			// this needs to be looked to make sure that we have this math here corectly
			// since we are in column major, but memory layout is in row major
			float s = 2.0f * UOMath::Sqrt( 1.0f + a_Matrix[1][1] - a_Matrix[0][0] - a_Matrix[2][2]);
			x = (a_Matrix[0][1] + a_Matrix[1][0] ) / s;
			y = 0.25f * s;
			z = (a_Matrix[1][2] + a_Matrix[2][1] ) / s;
			w = (a_Matrix[0][2] - a_Matrix[2][0] ) / s;
		}
		else
		{
			float s = 2.0f * UOMath::Sqrt( 1.0f + a_Matrix[2][2] - a_Matrix[0][0] - a_Matrix[1][1] );
			x = (a_Matrix[0][2] + a_Matrix[2][0] ) / s;
			y = (a_Matrix[1][2] + a_Matrix[2][1] ) / s;
			z = 0.25f * s;
			w = (a_Matrix[0][1] - a_Matrix[1][0] ) / s;
		}
	}
}

void UOMath::mthQuat::LookAt(const UOMath::vector3& a_EyePoint, const UOMath::vector3& a_LookAtPoint, const UOMath::vector3& a_UpVector)
{
	// this is fixed version of creating quat from eye point and look at point
	UOMath::vector3 direction = a_LookAtPoint - a_EyePoint;
	direction.normalize();
	nGZNumerics::tQuaternion lookAtQuat = nGZNumerics::MatrixToQuaternion(nGZNumerics::tMatrixCreate33::LookAt(direction.getGZ(), a_UpVector.getGZ()));
	x = lookAtQuat[0];
	y = lookAtQuat[1];
	z = lookAtQuat[2];
	w = lookAtQuat[3];



	/********************************************
    //##!!! THIS NEEDS TO BE FIXXED

    // First, create a rotation matrix, then convert the
    //  matrix to a quaternion
	UOMath::matrix44 sunRotMatrix = UOMath::RotateDegMatrix44('x', -90);
    UOMath::vector3 forwardaxis = UOMath::vector3(1,0,-1) - eyept;
    forwardaxis.normalize();

    UOMath::vector3 crossAxis = UOMath::CrossProduct(upvec, forwardaxis);
    crossAxis.normalize();

    UOMath::vector3 upAxis = UOMath::vector3::YONE;//UOMath::CrossProduct(forwardaxis, crossAxis);

    UOMath::matrix33 matrix;
    matrix[0][0] = crossAxis.x;
    matrix[1][0] = crossAxis.y;
    matrix[2][0] = crossAxis.z;

    matrix[0][1] = upAxis.x;
    matrix[1][1] = upAxis.y;
    matrix[2][1] = upAxis.z;

    matrix[0][2] = forwardaxis.x;
    matrix[1][2] = forwardaxis.y;
    matrix[2][2] = forwardaxis.z;

    UOMath::matrix33 temp = matrix * sunRotMatrix.GetRotation();
    //matrix.transpose();

    this->Create(temp);
    this->Normalize();
	*************************************************/
}

//#ifdef FU_DEBUG
//
//
//    void     UOMath::mthQuat::SetAxisAngle( utlVector& a_Axis, float a_Angle) { RtQuatRotate(RwQuat(), a_Axis.RwVect(), a_Angle, rwCOMBINEREPLACE); }
//    void     UOMath::mthQuat::GetAxisAngle( utlVector& a_Axis, float& a_Angle){ RtQuatQueryRotate(RwQuat(), a_Axis.RwVect(), &a_Angle); }
//
//    void     UOMath::mthQuat::SetAxisRadian(utlVector& a_Axis, float a_Angle) { RtQuatRotate(RwQuat(), a_Axis.RwVect(), a_Angle * MTH_RAD_TO_DEG, rwCOMBINEREPLACE); }
//    void     UOMath::mthQuat::GetAxisRadian(utlVector& a_Axis, float& a_Angle){ RtQuatQueryRotate(RwQuat(), a_Axis.RwVect(), &a_Angle); a_Angle *= MTH_DEG_TO_RAD; }
//
//#endif // FU_DEBUG
//
#endif // 0

