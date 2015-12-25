// Partialy based on the "Game Programing Gems" source
// by Dante Treglia II and Mark A. DeLoura.
// C++ Matrix Library
// Some extra functionality added/changed by Edgar Glowacki 2003
// (c) 2004 Electronic Arts Inc.
#include "Precompiled.h"
#include "Math/Matrix.h"
#include "Math/Quat.h"

namespace eg {
#if 0


////////////////////////////////////////////////////////////
// matrix33 class
//

matrix33::matrix33(const Quat& q)
{
    /*
	col[0][0] = 1 - (2 * a_Quat.y * 2) - (2 * a_Quat.z * 2);
	col[1][0] = (2 * a_Quat.x * a_Quat.y) + (2 * a_Quat.w * a_Quat.z);
	col[2][0] = (2 * a_Quat.x * a_Quat.z) - (2 * a_Quat.w * a_Quat.y);
	col[0][1] = (2 * a_Quat.x * a_Quat.y) - (2 * a_Quat.w * a_Quat.z);
	col[1][1] = 1 - (2 * a_Quat.x * 2) - (2 * a_Quat.z * 2);
	col[2][1] = (2 * a_Quat.y * a_Quat.z) + (2 * a_Quat.w * a_Quat.x);
	col[0][2] = (2 * a_Quat.x * a_Quat.z) + (2 * a_Quat.w * a_Quat.y);
	col[1][2] = (2 * a_Quat.y * a_Quat.z) - (2 * a_Quat.w * a_Quat.x);
	col[2][2] = 1 - (2 * a_Quat.x * 2) - (2 * a_Quat.y * 2);
    */
    float xx = q.x * q.x * 2;
    float xy = q.x * q.y * 2;
    float xz = q.x * q.z * 2;
    float xw = q.x * q.w * 2;
    float yy = q.y * q.y * 2;
    float yz = q.y * q.z * 2;
    float yw = q.y * q.w * 2;
    float zz = q.z * q.z * 2;
    float zw = q.z * q.w * 2;

    col[0][0] = 1 - yy - zz;
    col[1][0] =     xy - zw;
    col[2][0] =     xz + yw;
    col[0][1] =     xy + zw;
    col[1][1] = 1 - xx - zz;
    col[2][1] =     yz - xw;
    col[0][2] =     xz - yw;
    col[1][2] =     yz + xw;
    col[2][2] = 1 - xx - yy;
}


// Multiply the matrix33 by another matrix33
matrix33& matrix33::operator *= (const matrix33& m)
{
	matrix33 t;

	for ( unsigned int r = 0; r < 3; r++ )
	{
		for ( unsigned int c = 0; c < 3; c++ )
		{
			float f = 0;

			f += col[0][r] * m[c][0];
			f += col[1][r] * m[c][1];
			f += col[2][r] * m[c][2];

			t[c][r] = f;
		}
	}

	*this = t;

	return *this;
}


// Invert the matrix33
matrix33& matrix33::invert()
{
	matrix33 a(*this);
	matrix33 b(IdentityMatrix33());

	unsigned int c, r;
	unsigned int cc;
	unsigned int rowMax; // Points to max abs value row in this column
	unsigned int row;
	float tmp;

	// Go through columns
	for ( c=0; c<3; c++ )
	{
		// Find the row with max value in this column
		rowMax = c;
		for ( r=c+1; r<3; r++ )
		{
			if ( fabs(a[c][r]) > fabs(a[c][rowMax]) )
			{
				rowMax = r;
			}
		}

		// If the max value here is 0, we can't invert.  Return identity.
		if ( a[rowMax][c] == 0.0F )
			return(identity());

		// Swap row "rowMax" with row "c"
		for ( cc=0; cc<3; cc++ )
		{
			tmp = a[cc][c];
			a[cc][c] = a[cc][rowMax];
			a[cc][rowMax] = tmp;
			tmp = b[cc][c];
			b[cc][c] = b[cc][rowMax];
			b[cc][rowMax] = tmp;
		}

		// Now everything we do is on row "c".
		// Set the max cell to 1 by dividing the entire row by that value
		tmp = a[c][c];
		for ( cc=0; cc<3; cc++ )
		{
			a[cc][c] /= tmp;
			b[cc][c] /= tmp;
		}

		// Now do the other rows, so that this column only has a 1 and 0's
		for ( row = 0; row < 3; row++ )
		{
			if ( row != c )
			{
				tmp = a[c][row];
				for ( cc=0; cc<3; cc++ )
				{
					a[cc][row] -= a[cc][c] * tmp;
					b[cc][row] -= b[cc][c] * tmp;
				}
			}
		}

	}

	*this = b;

	return *this;
}

// Return a matrix33 set to the identity matrix
matrix33 IdentityMatrix33()
{
	matrix33 ret;

	return ret.identity();
}

// Return the transpose of the matrix33
matrix33 TransposeMatrix33(const matrix33& m)
{
	matrix33 ret(m);

	return ret.transpose();
}

// Return the inverted matrix33
matrix33 InvertMatrix33(const matrix33& m)
{
	matrix33 ret(m);

	return ret.invert();
}

// Return a 2D rotation matrix33
matrix33 RotateRadMatrix33(float rad)
{
	matrix33 ret;
	float sinA, cosA;

	sinA = Sin(rad);
	cosA = Cos(rad);

	ret[0][0] = cosA; ret[1][0] = -sinA; ret[2][0] = 0.0F;
	ret[0][1] = sinA; ret[1][1] =  cosA; ret[2][1] = 0.0F;
	ret[0][2] = 0.0F; ret[1][2] =  0.0F; ret[2][2] = 1.0F;

	return ret;
}

// Return a 2D translation matrix33
matrix33 TranslateMatrix33(float x, float y)
{
	matrix33 ret;

	ret.identity();
	ret[2][0] = x;
	ret[2][1] = y;

	return ret;
}

// Return a 2D/3D scale matrix33
matrix33 ScaleMatrix33(float x, float y, float z)
{
	matrix33 ret;

	ret.identity();
	ret[0][0] = x;
	ret[1][1] = y;
	ret[2][2] = z;

	return ret;
}


////////////////////////////////////////////////////////////
// matrix44 class
//


/*
matrix44::matrix44(const nGZCompGeom::tOrthogonalTransform& a_GZMatrix)
{
	const float *gzElements = a_GZMatrix.AsMatrix().Elements();
	set(gzElements);
	transpose();
}
*/
#endif // 0


Matrix44::Matrix44(const Quaternion& quat, const Vector3& position)
{
    (*this) = m44::Identity();

	Vector3 axis(v3::ZONE());
	float angle = 0;
	quat.Decompose(axis, angle);
    Matrix44 rotMatrix = RotateRadMatrix44(axis, angle);
	Matrix44 posMatrix = TranslateMatrix44(position);
    (*this) = rotMatrix * posMatrix;

	/*
	// NOTE! it's not working corectly
	col[0][0] = 1 - (2 * (a_Quat.y * a_Quat.y)) - (2 * (a_Quat.z * a_Quat.z));
	col[1][0] = (2 * a_Quat.x * a_Quat.y) + (2 * a_Quat.w * a_Quat.z);
	col[2][0] = (2 * a_Quat.x * a_Quat.z) - (2 * a_Quat.w * a_Quat.y);

	col[0][1] = (2 * a_Quat.x * a_Quat.y) - (2 * a_Quat.w * a_Quat.z);
	col[1][1] = 1 - (2 * (a_Quat.x * a_Quat.x)) - (2 * (a_Quat.z * a_Quat.z));
	col[2][1] = (2 * a_Quat.y * a_Quat.z) + (2 * a_Quat.w * a_Quat.x);

	col[0][2] = (2 * a_Quat.x * a_Quat.z) + (2 * a_Quat.w * a_Quat.y);
	col[1][2] = (2 * a_Quat.y * a_Quat.z) - (2 * a_Quat.w * a_Quat.x);
	col[2][2] = 1 - (2 * (a_Quat.x * a_Quat.x)) - (2 * (a_Quat.y * a_Quat.y));

	col[3][0] = a_Position.x;
	col[3][1] = a_Position.y;
	col[3][2] = a_Position.z;
	*/
}


#if 0
//! Help in creating texture translation matrix
matrix44 Tex2DTRanslateMatrix44(float u, float v)
{
	matrix44 ret;

	ret.identity();
	ret[2][0] = u;
	ret[2][1] = v;
	ret[2][2] = 1;

	return ret;
}


matrix44 Tex2DTRanslateMatrix44(const vector3& NewPos)
{
    return Tex2DTRanslateMatrix44(NewPos.x, NewPos.y);
}


matrix44 Tex2DTRanslateMatrix44(const vector2& NewPos)
{
    return Tex2DTRanslateMatrix44(NewPos.x, NewPos.y);
}


matrix44 Tex2DScaleMatrix44(float x, float y)
{
	matrix44 ret;

	ret.identity();
	ret[0][0] = x;
	ret[1][1] = y;

	return ret;
}


matrix44 Tex2DScaleMatrix44(const vector2& NewScale)
{
    return Tex2DScaleMatrix44(NewScale.x, NewScale.y);
}


//! z component is disregarded.
matrix44 Tex2DScaleMatrix44(const vector3& NewScale)
{
    return Tex2DScaleMatrix44(NewScale.x, NewScale.y);
}



// Return a frustum matrix44 given the left, right, bottom, top,
// near, and far values for the frustum boundaries.
matrix44 FrustumMatrix44(float l, float r, float b, float t, float n, float f)
{
	matrix44 ret;
	float width = r-l;
	float height = t-b;
	float depth = f-n;

	ret[0][0] = (2*n) / width;
	ret[0][1] = 0.0F;
	ret[0][2] = 0.0F;
	ret[0][3] = 0.0F;

	ret[1][0] = 0.0F;
	ret[1][1] = (2*n) / height;
	ret[1][2] = 0.0F;
	ret[1][3] = 0.0F;

	ret[2][0] = (r + l) / width;
	ret[2][1] = (t + b) / height;
	ret[2][2] = -(f + n) / depth;
	ret[2][3] = -1.0F;

	ret[3][0] = 0.0F;
	ret[3][1] = 0.0F;
	ret[3][2] = -(2*f*n) / depth;
	ret[3][3] = 0.0F;

	return ret;
}


// Return an orthographic matrix44 given the left, right, bottom, top,
//   near, and far values for the frustum boundaries.
matrix44 OrthoMatrix44(float l, float r, float b, float t, float n, float f)
{
	matrix44 ret;
	float width = r-l;
	float height = t-b;
	float depth = f-n;

	ret[0][0] = 2.0F / width;
	ret[0][1] = 0.0F;
	ret[0][2] = 0.0F;
	ret[0][3] = 0.0F;

	ret[1][0] = 0.0F;
	ret[1][1] = 2.0F / height;
	ret[1][2] = 0.0F;
	ret[1][3] = 0.0F;

	ret[2][0] = 0.0F;
	ret[2][1] = 0.0F;
	ret[2][2] = -(2.0F) / depth;
	ret[2][3] = 0.0F;

	ret[3][0] = -(r + l) / width;
	ret[3][1] = -(t + b) / height;
	ret[3][2] = -(f + n) / depth;
	ret[3][3] = 1.0F;

	return ret;
}

// Return an orientation matrix using 3 basis normalized vectors
matrix44 OrthoNormalMatrix44(const vector3 &xdir, const vector3 &ydir, const vector3 &zdir)
{
	matrix44 ret;

	ret[0] = (vector4)xdir;
	ret[1] = (vector4)ydir;
	ret[2] = (vector4)zdir;
	ret[3].set(0.0, 0.0, 0.0, 1.0);

	return ret;
}


// Return matrix representing projector (light)
matrix44 ProjectorMatrixLH44(const vector3& a_Eye, const vector3& a_LookAt, const vector3& a_UpVector, float a_Angle, float a_Aspect, float a_Near, float a_Far, const matrix44 a_ObjectMatrix)
{
	matrix44 lookAtMatrix = LookAtMatrixLH44(a_Eye, a_LookAt, a_UpVector);
	matrix44 perspectiveFovMatrix = PerspectiveMatrixLH44(a_Angle, a_Aspect, a_Near, a_Far);

	return perspectiveFovMatrix * lookAtMatrix * a_ObjectMatrix;
}


// This will create orientation matrix33 of world oriented billboard with y being up (0, 1, 0).
// ViewVector is the view from bilboard pointing towards the viewer.
// So if you are using camera view vector, then it needs to be reverse.
// If you multiply this matrix with point, it will rotate that point, so
// it will always face camera with y beying UP. One use is for the trees.
matrix33 BillboardWorldMatrix33(const vector3& ViewVector)
{
	vector3 right = CrossProduct(ViewVector, vector3::ZONE);
	right.normalize();
	vector3 normal = CrossProduct(vector3::ZONE, right);
	normal.normalize();

	return matrix33(right, normal, vector3::ZONE);
}


matrix33 BillboardWorldMatrix33(const vector3& FromVector, const vector3& ToVector)
{
	vector3 view(ToVector - FromVector);
	view *= -1.0f;
	view.normalize();
	return BillboardWorldMatrix33(view);
}

// This will extract view vector from ViewMatrix[2], reverse,
// normalize, and call above method.
matrix33 BillboardWorldMatrix33(const matrix44& ViewMatrix)
{
	vector3 view(ViewMatrix[2]);
	// reverse the normal
	view *= -1.0f;
	view.normalize();
	return BillboardWorldMatrix33(view);
}


// This will create orientation matrix33 of view oriented billboard.
// ViewVector is the view from billboard pointing towards the viewer.
// So if you are using camera view vector, then it needs to be reverse.
// If you multiply this matrix with point, it will rotate that point, so
// it will always face camera and it will be perpendicular to the viewer,
matrix33 BillboardViewMatrix33(const vector3& ViewVector)
{
	vector3 right = CrossProduct(ViewVector, vector3::ZONE);
	right.normalize();
	//vector3 up = CrossProduct(ViewVector, right);
	vector3 up = CrossProduct(right, ViewVector);
	up.normalize();

	return matrix33(right, ViewVector, up);
}

matrix33 BillboardViewMatrix33(const vector3& FromVector, const vector3& ToVector)
{
	vector3 view(ToVector - FromVector);
	view *= -1.0f;
	view.normalize();
	return BillboardViewMatrix33(view);
}

// This will extract view vector from ViewMatrix[2], reverse,
// normalize, and call above method.
matrix33 BillboardViewMatrix33(const matrix44& ViewMatrix)
{
	assert(0 && "This is not working");
	vector3 view(ViewMatrix[2]);
	// reverse the normal
	//view.x *= -1.0f;
	view.normalize();
	return BillboardViewMatrix33(view);
}



// A function for creating a rotation matrix that rotates a vector called
// "From" into another vector called "To".
// Input : From, ToTo which both must be *normalized* non-zero vectors
// Output: matrix33 -- a 3x3 matrix in colum-major form.
matrix33 FromToMatrix33(const vector3& From, const vector3& To)
{
	matrix33 mtx;
	vector3 v = CrossProduct(From, To);
	float e = DotProduct(From, To);
	float f= (e < 0)? -e:e;
	if (f > 1.0 - kEpsilon)	   // "from" and "to"-vector almost parallel
	{
		vector3 x;
		x.x = (From.x > 0.0) ? From.x : -From.x;
		x.y = (From.y > 0.0) ? From.y : -From.y;
		x.z = (From.z > 0.0) ? From.z : -From.z;

		if (x.x < x.y)
		{
			if (x.x < x.z)
				x.set(1, 0, 0);
			else
				x.set(0, 0, 1);
		}
		else
		{
			if (x.y < x.z)
				x.set(0, 1, 0);
			else
				x.set(0, 0, 1);
		}

		vector3 u(x - From);
		vector3 v(x - To);

		float c1 = 2.0f / DotProduct(u, u);
		float c2 = 2.0f / DotProduct(v, v);
		float c3 = c1 * c2  * DotProduct(u, v);

		mtx[0][0] =  - c1 * u.x * u.x
					 - c2 * v.x * v.x
					 + c3 * v.x * u.x;

		mtx[0][1] =  - c1 * u.x * u.y
					 - c2 * v.x * v.y
					 + c3 * v.x * u.y;

		mtx[0][2] =  - c1 * u.x * u.z
					 - c2 * v.x * v.z
					 + c3 * v.x * u.z;
		mtx[0][0] += 1.0f;

		mtx[1][0] =  - c1 * u.y * u.x
					 - c2 * v.y * v.x
					 + c3 * v.y * u.x;

		mtx[1][1] =  - c1 * u.y * u.y
					 - c2 * v.y * v.y
					 + c3 * v.y * u.y;

		mtx[1][2] =  - c1 * u.y * u.z
					 - c2 * v.y * v.z
					 + c3 * v.y * u.z;

		mtx[1][1] += 1.0;

		mtx[2][0] =  - c1 * u.z * u.x
					 - c2 * v.z * v.x
					 + c3 * v.z * u.x;

		mtx[2][1] =  - c1 * u.z * u.y
					 - c2 * v.z * v.y
					 + c3 * v.z * u.y;

		mtx[2][2] =  - c1 * u.z * u.z
					 - c2 * v.z * v.z
					 + c3 * v.z * u.z;
		mtx[2][2] += 1.0;
	}
	else
	{
		// the most common case, unless "from"="to", or "from"=-"to"
		float h = 1.0f/(1.0f + e);
		mtx[0][0] = e + h * v.x * v.x;
		mtx[0][1] = h * v.x * v.y - v.z;
		mtx[0][2] = h * v.x * v.z + v.y;

		mtx[1][0] = h * v.x * v.y + v.z;
		mtx[1][1] = e + h * v.y * v.y;
		mtx[1][2] = h * v.y * v.z - v.x;

		mtx[2][0] = h * v.x * v.z - v.y;
		mtx[2][1] = h * v.y * v.z + v.x;
		mtx[2][2] = e + h * v.z * v.z;
	}

	return mtx;
}
#endif // 0

} // namespace eg



