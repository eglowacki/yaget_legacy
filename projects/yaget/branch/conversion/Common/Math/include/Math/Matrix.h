///////////////////////////////////////////////////////////////////////
// Matrix.h
//
//  Copyright 11/3/2005 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:          Partially based on the "Game Programing Gems"
//                  ource by Dante Treglia II and Mark A. DeLoura.
//                  C++ Matrix Library.
//                  Some extra functionality added/changed by Edgar Glowacki
//
//  #include <Math\Matrix.h>
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef MATH_MATRIX_H
#define MATH_MATRIX_H
#pragma once



#include "Vector.h"
//#include <Math/Quat.h>

namespace eg {class Quaternion;}

namespace eg
{
////////////////////////////////////////////////////////////
//! Matrix33 class
class Matrix33
{
public:
    // Members
    Vector3 row[3];

public:
    //! Constructors
    Matrix33();

    //! Constructor with initializing array of floats
    Matrix33(const float *v);

    //! Constructor with initializing Matrix33
    //! Constructor with initializing value
    Matrix33(float v);

    //! Constructor with initializing Matrix33
    Matrix33(const Matrix33& m);

    //! Constructor with initializing Vector3's
    Matrix33(const Vector3& v0, const Vector3& v1, const Vector3& v2);

    //! This will take the mthQuat as parameter and create rotation matrix from it
    //Matrix33(const mthQuat& a_Quat);

    //! Array indexing
    Vector3& operator [] (unsigned int i);

    //! Array indexing
    const Vector3& operator [] (unsigned int i) const;

    //! Assign
    Matrix33& operator = (const Matrix33& m);

    //! Add a Matrix33 to this one
    Matrix33& operator += (const Matrix33& m);

    //! Subtract a Matrix33 from this one
    Matrix33& operator -= (const Matrix33& m);

    //! Multiply the Matrix33 by another Matrix33
    Matrix33& operator *= (const Matrix33& m);

    //! Multiply the Matrix33 by a float
    Matrix33& operator *= (float f);

    //! Are these two Matrix33's equal?
    friend bool operator == (const Matrix33& a, const Matrix33& b);

    //! Are these two Matrix33's not equal?
    friend bool operator != (const Matrix33& a, const Matrix33& b);

    //! Add two Matrix33's
    friend Matrix33 operator + (const Matrix33& a, const Matrix33& b);

    //! Subtract one Matrix33 from another
    friend Matrix33 operator - (const Matrix33& a, const Matrix33& b);

    //! Multiply Matrix33 by another Matrix33
    friend Matrix33 operator * (const Matrix33& a, const Matrix33& b);

    //! Multiply a Vector3 by this Matrix33
    friend Vector3 operator * (const Matrix33& m, const Vector3& v);

    //! Multiply a Vector3 by this Matrix33
    friend Vector3 operator * (const Vector3& v, const Matrix33& m);

    //! Multiply Matrix33 by a float
    friend Matrix33 operator * (const Matrix33& m, float f);

    //! Multiply Matrix33 by a float
    friend Matrix33 operator * (float f, const Matrix33& m);

    friend std::ostream& operator <<(std::ostream& o, const Matrix33& value);
    friend std::istream& operator >>(std::istream& i, Matrix33& value);

    // Methods
    void Set(const float *in);

    //! Set Matrix33 to the identity matrix
    Matrix33& Identity();

    //! Transpose the Matrix33
    Matrix33& Transpose();
    //
    // Invert the Matrix33
    Matrix33& Inverse();
};


//! Return Transpose of Matrix33.
Matrix33 TransposeMatrix33(const Matrix33& m);
//! Return Inverse of Matrix33.
Matrix33 InverseMatrix33(const Matrix33& m);

#if 0

/** \addtogroup Math */
//@{
//! Return Identity Matrix33.
Matrix33 IdentityMatrix33();
//! Return Rotated Matrix33.
Matrix33 RotateRadMatrix33(float rad);
//! Return Translation Matrix33.
Matrix33 TranslateMatrix33(float x, float y);
//! Return Scale Matrix33.
Matrix33 ScaleMatrix33(float x, float y, float z = 1.0);
//! Return matrix which will rotate point 'From' orientation to 'To' orientation.
Matrix33 FromToMatrix33(const Vector3& From, const Vector3& To);

//@}

#endif // 0

////////////////////////////////////////////////////////////
//! Matrix44 class
class Matrix44
{
public:
    // Members
    Vector4 row[4];

public:
    //! Constructors
    Matrix44();

    //! Constructor with initializing value
    Matrix44(float v);

    //! Constructor with initializing array of floats
    Matrix44(const float *v);

    //! Constructor with initializing Matrix44
    Matrix44(const Matrix44& m);

    //! Constructor with initializing Vector4's
    Matrix44(const Vector4& v0, const Vector4& v1, const Vector4& v2, const Vector4& v3);

    //! Constructor with initializing Matrix33
    explicit Matrix44(const Matrix33& m);

    /// This will take the mthQuat and Vector3 as parameters and create matrix from it
    /// \param a_Quat quaternion representing orientation
    /// \param a_Position position
    Matrix44(const Quaternion& quat, const Vector3& position = v3::ZERO());

    //! Operators
    //! Array indexing
    Vector4& operator [](unsigned int i);

    //! Array indexing
    const Vector4& operator [](unsigned int i) const;

    //! Assign
    Matrix44& operator =(const Matrix44& m);

    //! Assign a Matrix33 to the Matrix44
    Matrix44& operator =(const Matrix33& m);

    //! Add a Matrix44 to this one
    Matrix44& operator +=(const Matrix44& m);

    //! Subtract a Matrix44 from this one
    Matrix44& operator -=(const Matrix44& m);

    //! Multiply the Matrix44 by another Matrix44
    Matrix44& operator *=(const Matrix44& m);

    //! Multiply the Matrix44 by another Matrix33
    Matrix44& operator *=(const Matrix33& m);

    //! Multiply the Matrix44 by a float
    Matrix44& operator *=(float f);

    //! Are these two Matrix44's equal?
    friend bool operator ==(const Matrix44& a, const Matrix44& b);

    //! Are these two Matrix44's not equal?
    friend bool  operator !=(const Matrix44& a, const Matrix44& b);

    //! Add two Matrix44's
    friend Matrix44 operator +(const Matrix44& a, const Matrix44& b);

    //! Subtract one Matrix44 from another
    friend Matrix44 operator -(const Matrix44& a, const Matrix44& b);

    //! Multiply Matrix44 by another Matrix44
    friend Matrix44 operator *(const Matrix44& a, const Matrix44& b);

    //! Multiply Matrix44 by another Matrix33
    friend Matrix44 operator *(const Matrix44& a, const Matrix33& b);

    //! Multiply a Vector3 by this Matrix44
    friend Vector3 operator *(const Matrix44& m, const Vector3& v);

    //! Multiply a Vector3 by this Matrix44
    friend Vector3 operator *(const Vector3& v, const Matrix44& m);

    //! Multiply a Vector4 by this Matrix44 (used in left handed coordinates system - D3D)
    friend Vector4 operator *(const Matrix44& m, const Vector4& v);

    //! Multiply a Vector4 by this Matrix44 (used in right handed coordinates system - OpenGL)
    friend Vector4 operator *(const Vector4& v, const Matrix44& m);

    //! Multiply Matrix44 by a float
    friend Matrix44 operator *(const Matrix44& m, float f);

    //! Set Matrix44 to the identity matrix
    friend Matrix44 operator *(float f, const Matrix44& m);

    friend std::ostream& operator <<(std::ostream& o, const Matrix44& value);
    friend std::istream& operator >>(std::istream& i, Matrix44& value);

    //! Methods
    //! set matrix using array of floats
    void Set(const float *v);

    //! Return pointer to floats. It's guarantied to be in continuous memory
    const float *GetF() const;

    //! Return pointer to floats. It's guarantied to be in continuous memory
    float *GetF();

    //! This will return data size for this Matrix44 in bytes
    static size_t SizeOf();

    //! Return upper 3x3 portion of a matrix which represents rotation
    Matrix33 GetRotation() const;

    //! Return translate portion of a matrix
    Vector3 GetTranslate() const;

    //! Set translate portion of this matrix. This does not change any other part or matrix.
    void SetTranslate(const Vector3& NewPos);
};


//! Translate matrix using 3 floats
Matrix44 TranslateMatrix44(float x, float y, float z);

//! Translate matrix using Vector3
Matrix44 TranslateMatrix44(const Vector3& newPos);

//! Translate matrix using Vector2, where z will be 0
Matrix44 TranslateMatrix44(const Vector2& newPos);

//! Transpose matrix. source can be same as the target
Matrix44 TransposeMatrix44(const Matrix44& source);

//! Invert matrix. source can be same as the target
Matrix44 InverseMatrix44(const Matrix44& source);

//! Invert/transpose source matrix. This is used for transdformation of normals and planes
Matrix44 NormalMatrix44(const Matrix44& source);

//@{
//! Rotate matrix around arbitrary axis
Matrix44 RotateRadMatrix44(const Vector3& axis, float rad);
inline Matrix33 RotateRadMatrix33(const Vector3& axis, float rad) {return RotateRadMatrix44(axis, rad).GetRotation();}
Matrix44 RotateDegMatrix44(const Vector3& axis, float deg);
inline Matrix33 RotateDegMatrix33(const Vector3& axis, float deg) {return RotateDegMatrix44(axis, deg).GetRotation();}
//@}



//@{
//! Scale matrix
Matrix44 ScaleMatrix44(float x, float y, float z, float w = 1.0);
inline Matrix33 ScaleMatrix33(float x, float y, float z, float w = 1.0) {return ScaleMatrix44(x, y, z, w).GetRotation();}
Matrix44 ScaleMatrix44(const Vector2& scaleValue);
inline Matrix33 ScaleMatrix33(const Vector2& scaleValue) {return ScaleMatrix44(scaleValue).GetRotation();}
Matrix44 ScaleMatrix44(const Vector3& scaleValue);
inline Matrix33 ScaleMatrix33(const Vector3& scaleValue) {return ScaleMatrix44(scaleValue).GetRotation();}
Matrix44 ScaleMatrix44(const Vector4& scaleValue);
inline Matrix33 ScaleMatrix33(const Vector4& scaleValue) {return ScaleMatrix44(scaleValue).GetRotation();}
//@}

//@{
//! Construct look at matrix
Matrix44 LookAtMatrixRH44(const Vector3& camPos, const Vector3& target, const Vector3& camUp);
Matrix44 LookAtMatrixLH44(const Vector3& camPos, const Vector3& target, const Vector3& camUp);
//@}

//@{
/// Create perspective matrix.
/// Return a perspective Matrix44 given the field-of-view in the Y
/// direction in degrees, the aspect ratio of Y/X, and near and
/// far plane distances.
Matrix44 PerspectiveMatrixRH44(float fovY, float aspect, float n, float f);
Matrix44 PerspectiveMatrixLH44(float fovY, float aspect, float n, float f);
//@}


#if 0


Matrix44 FrustumMatrix44(float l, float r, float b, float t, float n, float f);

/// Return a perspective Matrix44 given the field-of-view in the Y
/// direction in degrees, the aspect ratio of Y/X, and near and
/// far plane distances.
Matrix44 OrthoMatrix44(float l, float r, float b, float t, float n, float f);
Matrix44 OrthoNormalMatrix44(const Vector3& xdir, const Vector3& ydir, const Vector3& zdir);

/*!
Create matrix representing projector (light), which can be used to project texture
from light point of view on some arbitrary geometry
\param a_Eye position of the projector
\param a_LookAt where to look at
\param a_UpVector up vector of the projector
\param a_Angle view angle of the projector
\param a_Aspect aspect ratio of the projector
\param a_Near near plane of the projector
\param a_Far far plane of the projector
\param a_ObjectMatrix this will be concentate with projector matrix as the first one
\return matrix representing projector
*/
Matrix44 ProjectorMatrixLH44(const Vector3& a_Eye, const Vector3& a_LookAt, const Vector3& a_UpVector, float a_Angle, float a_Aspect, float a_Near, float a_Far, const Matrix44 a_ObjectMatrix);// = m44_I);


/// Some helper texture matrix construction routines.

/// Texture matrix under D3D used for translation, are
/// treated as a 3x3. So translation portion is in 20 and 21 position.
/// NOTE: index into matrix is 0 based.
Matrix44 Tex2DTRanslateMatrix44(float u, float v);
Matrix44 Tex2DTRanslateMatrix44(const vector2& NewPos);
//! z component is disregarded.
Matrix44 Tex2DTRanslateMatrix44(const Vector3& NewPos);

/// Create scaling matrix
Matrix44 Tex2DScaleMatrix44(float x, float y);
Matrix44 Tex2DScaleMatrix44(const vector2& NewScale);
//! z component is disregarded.
Matrix44 Tex2DScaleMatrix44(const Vector3& NewScale);

//! This helper method allow us to create bilboarding matricies

//
//  *** ALL OF THESE BILLBOARDING FUNCTIONS HAVE NOW BEEN CHANGED TO FLIP THE Y AND Z AXES!!!
//      -------------------------------------------------------------------------------------
//      This is done to comply with the final view orientation in GZ/UO2.  If you need to muck
//      around in the resultant matrix, be aware that the Y and Z axes are swapped.
//

/// This will create orientation Matrix33 of world oriented billboard with y being up (0, 1, 0).
/// ViewVector is the view from billboard pointing towards the viewer.
/// So if you are using camera view vector, then it needs to be reverse.
/// If you multiply this matrix with point, it will rotate that point, so
/// it will always face camera with y being UP. One use is for the trees.
Matrix33 BillboardWorldMatrix33(const Vector3& ViewVector);

/// This will extract view vector from ViewMatrix[2], reverse,
/// normalize, and call above method.
Matrix33 BillboardWorldMatrix33(const Matrix44& ViewMatrix);

Matrix33 BillboardWorldMatrix33(const Vector3& FromVector, const Vector3& ToVector);

/// This will create orientation Matrix33 of view oriented billboard.
/// ViewVector is the view from billboard pointing towards the viewer.
/// So if you are using camera view vector, then it needs to be reverse.
/// If you multiply this matrix with point, it will rotate that point, so
/// it will always face camera and it will be perpendicular to the viewer,
Matrix33 BillboardViewMatrix33(const Vector3& ViewVector);

/// This will create orientation Matrix33 of view oriented billboard.
/// FromVector is the ViewPoint and ToVevtor is the point we are looking at
/// and which will be used to create view matrix.
/// If you multiply this matrix with point, it will rotate that point, so
/// it will always face camera and it will be perpendicular to the viewer,
Matrix33 BillboardViewMatrix33(const Vector3& FromVector, const Vector3& ToVector);

/// This will extract view vector from ViewMatrix[2], reverse,
/// normalize, and call above method.
Matrix33 BillboardViewMatrix33(const Matrix44& ViewMatrix);
//@}


#endif // 0


} // namespace eg


#define MATH_MATRIX_INCLUDE_IMPLEMENTATION
#include "MatrixConst.h"
#include "MatrixImpl.h"
#undef MATH_MATRIX_INCLUDE_IMPLEMENTATION


#endif // _MTH_MATRIX_H_

