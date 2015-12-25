///////////////////////////////////////////////////////////////////////
// MatrixImpl.h
//
//  Copyright 11/3/2005 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//    Matrix33|44 implementation. Do not include this file.
//
//  #include "MatrixImpl.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef MATH_MATRIX_IMPL_H
#define MATH_MATRIX_IMPL_H

#ifndef MATH_MATRIX_INCLUDE_IMPLEMENTATION
    #error "Do not include this file explicitly."
#endif // MATH_MATRIX_INCLUDE_IMPLEMENTATION

namespace eg
{
    //----------------------------------------------------------------------
    // Matrix33
    //----------------------------------------------------------------------
    inline Matrix33::Matrix33()
    {
    }

    inline Matrix33::Matrix33(const float *v)
    {
        row[0].set(v[0], v[1], v[2]);
        row[1].set(v[3], v[4], v[5]);
        row[2].set(v[6], v[7], v[8]);
    }

    inline Matrix33::Matrix33(float v)
    {
        row[0].set(v, v, v);
        row[1].set(v, v, v);
        row[2].set(v, v, v);
    }

    inline Matrix33::Matrix33(const Matrix33& m)
    {
        row[0] = m[0];
        row[1] = m[1];
        row[2] = m[2];
    }

    inline Matrix33::Matrix33(const Vector3& v0, const Vector3& v1, const Vector3& v2)
    {
        row[0] = v0;
        row[1] = v1;
        row[2] = v2;
    }


    //inline Matrix33::Matrix33(const mthQuat& a_Quat);

    inline Vector3& Matrix33::operator [] (unsigned int i)
    {
        return(Vector3&)row[i];
    }

    inline const Vector3& Matrix33::operator [] (unsigned int i) const
    {
        return(Vector3&)row[i];
    }

    inline Matrix33& Matrix33::operator = (const Matrix33& m)
    {
        row[0] = m[0];
        row[1] = m[1];
        row[2] = m[2];
        return *this;
    }

    inline Matrix33& Matrix33::operator += (const Matrix33& m)
    {
        row[0] += m[0];
        row[1] += m[1];
        row[2] += m[2];
        return *this;
    }

    inline Matrix33& Matrix33::operator -= (const Matrix33& m)
    {
        row[0] -= m[0];
        row[1] -= m[1];
        row[2] -= m[2];
        return *this;
    }

    inline Matrix33& Matrix33::operator *= (const Matrix33& m)
    {
        Matrix33 t;
        for (unsigned int r = 0; r < 3; r++)
        {
            for (unsigned int c = 0; c < 3; c++)
            {
                float f = 0;

                f += (m[0][r] * row[c][0]);
                f += (m[1][r] * row[c][1]);
                f += (m[2][r] * row[c][2]);

                t[c][r] = f;
            }
        }

        (*this) = t;
        return *this;
    }

    inline Matrix33& Matrix33::operator *= (float f)
    {
        row[0] *= f;
        row[1] *= f;
        row[2] *= f;
        return *this;
    }

    inline bool operator == (const Matrix33& a, const Matrix33& b)
    {
        return((a[0] == b[0]) && (a[1] == b[1]) && (a[2] == b[2]));
    }

    inline bool operator != (const Matrix33& a, const Matrix33& b)
    {
        return((a[0] != b[0]) || (a[1] != b[1]) || (a[2] != b[2]));
    }

    inline Matrix33 operator + (const Matrix33& a, const Matrix33& b)
    {
        Matrix33 ret(a);
        ret += b;
        return ret;
    }

    inline Matrix33 operator - (const Matrix33& a, const Matrix33& b)
    {
        Matrix33 ret(a);
        ret -= b;
        return ret;
    }

    inline Matrix33 operator * (const Matrix33& a, const Matrix33& b)
    {
        Matrix33 ret(a);
        ret *= b;
        return ret;
    }

    inline Vector3 operator * (const Matrix33& m, const Vector3& v)
    {
        Vector3 ret;
        ret.x = v.x * m[0][0] + v.y * m[1][0] + v.z * m[2][0];
        ret.y = v.x * m[0][1] + v.y * m[1][1] + v.z * m[2][1];
        ret.z = v.x * m[0][2] + v.y * m[1][2] + v.z * m[2][2];
        return ret;
    }

    inline Vector3 operator * (const Vector3& v, const Matrix33& m)
    {
        Vector3 ret;
        ret.x = DotProduct(m[0], v);
        ret.y = DotProduct(m[1], v);
        ret.z = DotProduct(m[2], v);
        return ret;
    }

    inline Matrix33 operator * (const Matrix33& m, float f)
    {
        Matrix33 ret(m);
        ret *= f;
        return ret;
    }

    inline Matrix33 operator * (float f, const Matrix33& m)
    {
        Matrix33 ret(m);
        ret *= f;
        return ret;
    }

    inline std::ostream& operator <<(std::ostream& o, const Matrix33& value)
    {
        return o << value[0] << value[1] << value[2];
    }

    inline std::istream& operator >>(std::istream& i, Matrix33& value)
    {
        return i >> value[0] >> value[1] >> value[2];
    }

    inline void Matrix33::Set(const float *In)
    {
        row[0].set(In[0], In[1], In[2]);
        row[1].set(In[3], In[4], In[5]);
        row[2].set(In[6], In[7], In[8]);
    }

    inline Matrix33& Matrix33::Identity()
    {
        row[0].set(1.0f, 0.0f, 0.0f);
        row[1].set(0.0f, 1.0f, 0.0f);
        row[2].set(0.0f, 0.0f, 1.0f);
        return *this;
    }

    /*
    inline Matrix33& Matrix33::Transpose()
    {
        float t;
        for (unsigned int c = 0; c < 3; c++)
        {
            for (unsigned int r = c + 1; r < 3; r++)
            {
                t = row[c][r];
                row[c][r] = row[r][c];
                row[r][c] = t;
            }
        }

        return *this;

    }

    inline Matrix33& Matrix33::Inverse()
    {
        return *this;
    }
    */

    inline Matrix33 TransposeMatrix33(const Matrix33& source)
    {
        Matrix33 newTransposed = source;

        const int kMSize = 3;
        for (int iRow = 0; iRow < kMSize; iRow++)
        {
            for (int iCol = 0; iCol < kMSize; iCol++)
            {
                newTransposed.row[iRow][iCol] = newTransposed.row[iCol][iRow];
            }
        }
        /*
        for (uint32_t c = 0; c < kMSize; c++)
        {
            for (uint32_t r = c + 1; r < kMSize; r++)
            {
                float t = newTransposed.row[c][r];
                newTransposed.row[c][r] = newTransposed.row[r][c];
                newTransposed.row[r][c] = t;
            }
        }
        */

        return newTransposed;
    }

    inline Matrix33 InverseMatrix33(const Matrix33& source)
    {
        Matrix33 m = source;

        const int kMSize = 3;
        int indxc[kMSize], indxr[kMSize], ipiv[kMSize];
        int icol, irow;
        float big, dum, pivinv;

        icol = irow = 0;
        ipiv[0] = ipiv[1] = ipiv[2] = 0;

        for (int i = 0; i < kMSize; i++) // compile with loop unrolling
        {
            big = 0;
            for (int j = 0; j < kMSize; j++)
            {
                if (ipiv[j] != 1)
                {
                    for (int k = 0; k < kMSize; k++)
                    {
                        if (!ipiv[k])
                        {
                            if ((dum = fabs(m[j][k])) >= big)
                            {
                                big = dum;
                                irow = j;
                                icol = k;
                            }
                        }
                        else if (ipiv[k] > 1)
                        {
                            return m33::Identity();
                        }
                    }
                }
            }

            ++ipiv[icol];
            if (irow != icol)
            {
                for (int l = 0; l < kMSize; l++)
                {
                    float tmp = m[irow][l];
                    m[irow][l] = m[icol][l];
                    m[icol][l] = tmp;
                }
            }

            indxr[i] = irow;
            indxc[i] = icol;
            if ((dum = m[icol][icol])==0)
            {
                return m33::Identity();
            }

            pivinv = 1 / dum;
            m[icol][icol] = 1;
            for (int l = 0; l < kMSize; l++)
            {
                m[icol][l] *= pivinv;
            }

            for (int ll = 0; ll < kMSize; ll++)
            {
                if (ll != icol)
                {
                    dum = m[ll][icol];
                    m[ll][icol] = 0;
                    for (int l = 0; l < kMSize; l++)
                    {
                        m[ll][l] -= m[icol][l]*dum;
                    }
                }
            }
        }

        for (int l = 2; l >= 0; l--)
        {
            if (indxr[l] != indxc[l])
            {
                for (int k = 0; k < kMSize; k++)
                {
                    float tmp = m[k][indxr[l]];
                    m[k][indxr[l]] = m[k][indxc[l]];
                    m[k][indxc[l]] = tmp;
                }
            }
        }

        return m;
    }

    //----------------------------------------------------------------------
    // Matrix33 End
    //----------------------------------------------------------------------


    //----------------------------------------------------------------------
    // Matrix44
    //----------------------------------------------------------------------
    inline Matrix44::Matrix44()
    {
    }

    inline Matrix44::Matrix44(float v)
    {
        row[0].set(v, v, v, v);
        row[1].set(v, v, v, v);
        row[2].set(v, v, v, v);
        row[3].set(v, v, v, v);
    }

    inline Matrix44::Matrix44(const float *v)
    {
        row[0].set(v[0], v[1], v[2], v[3]);
        row[1].set(v[4], v[5], v[6], v[7]);
        row[2].set(v[8], v[9], v[10], v[11]);
        row[3].set(v[12], v[13], v[14], v[15]);
    }

    inline Matrix44::Matrix44(const Matrix44& m)
    {
        row[0] = m[0];
        row[1] = m[1];
        row[2] = m[2];
        row[3] = m[3];
    }

    inline Matrix44::Matrix44(const Vector4& v0, const Vector4& v1,
                              const Vector4& v2, const Vector4& v3)
    {
        row[0] = v0;
        row[1] = v1;
        row[2] = v2;
        row[3] = v3;
    }

    inline Matrix44::Matrix44(const Matrix33& m)
    {
        row[0].set(m[0], 0);
        row[1].set(m[1], 0);
        row[2].set(m[2], 0);
        row[3].set(0.0, 0.0, 0.0, 1.0);
    }
                                                                  
    inline Vector4& Matrix44::operator [] (unsigned int i)
    {
        return row[i];
    }

    inline const Vector4& Matrix44::operator [] (unsigned int i) const
    {
        return row[i];
    }

    inline Matrix44& Matrix44::operator = (const Matrix44& m)
    {
        row[0] = m[0];
        row[1] = m[1];
        row[2] = m[2];
        row[3] = m[3];
        return *this;
    }

    inline Matrix44& Matrix44::operator = (const Matrix33& m)
    {
        row[0] = m[0];
        row[1] = m[1];
        row[2] = m[2];
        row[3].set(0.0, 0.0, 0.0, 1.0);
        return *this;
    }

    inline Matrix44& Matrix44::operator += (const Matrix44& m)
    {
        row[0] += m[0];
        row[1] += m[1];
        row[2] += m[2];
        row[3] += m[3];
        return *this;
    }

    inline Matrix44& Matrix44::operator -= (const Matrix44& m)
    {
        row[0] -= m[0];
        row[1] -= m[1];
        row[2] -= m[2];
        row[3] -= m[3];
        return *this;
    }

    inline Matrix44& Matrix44::operator *= (const Matrix44& m)
    {
        Matrix44 t;
        for (unsigned int r = 0; r < 4; r++)
        {
            for (unsigned int c = 0; c < 4; c++)
            {
                float f = 0;

                f += (m[0][r] * row[c][0]);
                f += (m[1][r] * row[c][1]);
                f += (m[2][r] * row[c][2]);
                f += (m[3][r] * row[c][3]);

                t[c][r] = f;
            }
        }

        (*this) = t;
        return *this;
    }

    inline Matrix44 &Matrix44::operator *= (const Matrix33& m)
    {
        return(*this) *= Matrix44(m);
    }

    inline Matrix44& Matrix44::operator *= (float f)
    {
        row[0] *= f;
        row[1] *= f;
        row[2] *= f;
        row[3] *= f;
        return *this;
    }

    inline  bool operator == (const Matrix44& a, const Matrix44& b)
    {
        return((a[0] == b[0]) && (a[1] == b[1]) &&
               (a[2] == b[2]) && (a[3] == b[3]));
    }

    inline  bool operator != (const Matrix44& a, const Matrix44& b)
    {
        return((a[0] != b[0]) || (a[1] != b[1]) ||
               (a[2] != b[2]) || (a[3] != b[3]));
    }

    inline  Matrix44 operator + (const Matrix44& a, const Matrix44& b)
    {
        Matrix44 ret(a);
        ret += b;
        return ret;
    }

    inline  Matrix44 operator - (const Matrix44& a, const Matrix44& b)
    {
        Matrix44 ret(a);
        ret -= b;
        return ret;
    }

    inline  Matrix44 operator * (const Matrix44& a, const Matrix44& b)
    {
        Matrix44 ret(a);
        ret *= b;
        return ret;
    }

    inline  Matrix44 operator * (const Matrix44& a, const Matrix33& b)
    {
        Matrix44 ret(a);
        ret *= b;
        return ret;
    }

    inline  Vector3 operator * (const Matrix44& m, const Vector3& v)
    {
        Vector4 ret(v, 1.0f);
        ret = m * ret;
        return Vector3(ret.x, ret.y, ret.z);
    }

    inline  Vector3 operator * (const Vector3& v, const Matrix44& m)
    {
        Vector4 ret(v, 1.0f);
        ret = ret * m;
        return Vector3(ret.x, ret.y, ret.z);
    }

    inline Vector4 operator * (const Matrix44& m, const Vector4& v)
    {
        Vector4 ret;
        ret.x = m[3][0] + v.x * m[0][0] + v.y * m[1][0] + v.z * m[2][0];
        ret.y = m[3][1] + v.x * m[0][1] + v.y * m[1][1] + v.z * m[2][1];
        ret.z = m[3][2] + v.x * m[0][2] + v.y * m[1][2] + v.z * m[2][2];
        ret.w = m[3][3] + v.x * m[0][3] + v.y * m[1][3] + v.z * m[2][3];


        /*
        ret.x = v.x * m[0][0] + v.y * m[0][1] + v.z * m[0][2] + v.w * m[0][3];
        ret.y = v.x * m[1][0] + v.y * m[1][1] + v.z * m[1][2] + v.w * m[1][3];
        ret.z = v.x * m[2][0] + v.y * m[2][1] + v.z * m[2][2] + v.w * m[2][3];
        ret.w = v.x * m[3][0] + v.y * m[3][1] + v.z * m[3][2] + v.w * m[3][3];
        */
        return ret;
    }

    inline Vector4 operator * (const Vector4& v, const Matrix44& m)
    {
        Vector4 ret;
        ret.x = DotProduct(m[0], v);
        ret.y = DotProduct(m[1], v);
        ret.z = DotProduct(m[2], v);
        ret.w = DotProduct(m[3], v);
        return ret;
    }

    inline Matrix44 operator * (const Matrix44& m, float f)
    {
        Matrix44 ret(m);
        ret *= f;
        return ret;
    }

    inline Matrix44 operator * (float f, const Matrix44& m)
    {
        Matrix44 ret(m);
        ret *= f;
        return ret;
    }

    inline std::ostream& operator <<(std::ostream& o, const Matrix44& value)
    {
        return o << value[0] << value[1] << value[2] << value[3];
    }

    inline std::istream& operator >>(std::istream& i, Matrix44& value)
    {
        return i >> value[0] >> value[1] >> value[2] >> value[3];
    }

    inline void Matrix44::Set(const float *v)
    {
        row[0].set(v[0], v[1], v[2], v[3]);
        row[1].set(v[4], v[5], v[6], v[7]);
        row[2].set(v[8], v[9], v[10], v[11]);
        row[3].set(v[12], v[13], v[14], v[15]);
    }

    inline const float *Matrix44::GetF() const
    {
        return reinterpret_cast<const float *>(&row[0]);
        //return ((const float *)&row[0]);
    }

    inline float *Matrix44::GetF()
    {
        return reinterpret_cast<float *>(&row[0]);
        //return ((float *)&row[0]);
    }

    inline size_t Matrix44::SizeOf()
    {
        return sizeof(float) * 16;
    }

    inline Matrix33 Matrix44::GetRotation() const
    {
        Matrix33 m;

        m[0] = row[0];
        m[1] = row[1];
        m[2] = row[2];

        return m;
    }

    inline Vector3 Matrix44::GetTranslate() const
    {
        Vector3 ret(row[3]);

        return ret;
    }

    inline void Matrix44::SetTranslate(const Vector3& newPos)
    {
        row[3].x = newPos.x;
        row[3].y = newPos.y;
        row[3].z = newPos.z;

    }

    inline Matrix44 TranslateMatrix44(const Vector2& newPos)
    {
        return TranslateMatrix44(newPos.x, newPos.y, 0);
    }

    inline Matrix44 TranslateMatrix44(const Vector3& newPos)
    {
        return TranslateMatrix44(newPos.x, newPos.y, newPos.z);
    }

    inline Matrix44 TranslateMatrix44(float x, float y, float z)
    {
        Matrix44 ret = m44::Identity();

        ret[3][0] = x;
        ret[3][1] = y;
        ret[3][2] = z;

        return ret;
    }

    inline Matrix44 TransposeMatrix44(const Matrix44& source)
    {
        Matrix44 newTransposed = source;

        for (uint32_t c = 0; c < 4; c++)
        {
            for (uint32_t r = c + 1; r < 4; r++)
            {
                float t = newTransposed.row[c][r];
                newTransposed.row[c][r] = newTransposed.row[r][c];
                newTransposed.row[r][c] = t;
            }
        }

        return newTransposed;
    }

    inline Matrix44 InverseMatrix44(const Matrix44& source)
    {
        Matrix44 m = source;

        int indxc[4], indxr[4], ipiv[4];
        int icol, irow;
        float big, dum, pivinv;

        icol = irow = 0;
        ipiv[0] = ipiv[1] = ipiv[2] = ipiv[3] = 0;

        for (int i = 0; i < 4; i++) // compile with loop unrolling
        {
            big = 0;
            for (int j = 0; j < 4; j++)
            {
                if (ipiv[j] != 1)
                {
                    for (int k = 0; k < 4; k++)
                    {
                        if (!ipiv[k])
                        {
                            if ((dum = fabs(m[j][k])) >= big)
                            {
                                big = dum;
                                irow = j;
                                icol = k;
                            }
                        }
                        else if (ipiv[k] > 1)
                        {
                            return m44::Identity();
                        }
                    }
                }
            }

            ++ipiv[icol];
            if (irow != icol)
            {
                for (int l = 0; l < 4; l++)
                {
                    float tmp = m[irow][l];
                    m[irow][l] = m[icol][l];
                    m[icol][l] = tmp;
                }
            }

            indxr[i] = irow;
            indxc[i] = icol;
            if ((dum = m[icol][icol])==0)
            {
                return m44::Identity();
            }

            pivinv = 1 / dum;
            m[icol][icol] = 1;
            for (int l = 0; l < 4; l++)
            {
                m[icol][l] *= pivinv;
            }

            for (int ll = 0; ll < 4; ll++)
            {
                if (ll != icol)
                {
                    dum = m[ll][icol];
                    m[ll][icol] = 0;
                    for (int l = 0; l < 4; l++)
                    {
                        m[ll][l] -= m[icol][l]*dum;
                    }
                }
            }
        }

        for (int l = 3; l >= 0; l--)
        {
            if (indxr[l] != indxc[l])
            {
                for (int k = 0; k < 4; k++)
                {
                    float tmp = m[k][indxr[l]];
                    m[k][indxr[l]] = m[k][indxc[l]];
                    m[k][indxc[l]] = tmp;
                }
            }
        }

        return m;
    }

    inline Matrix44 NormalMatrix44(const Matrix44& source)
    {
        return TransposeMatrix44(InverseMatrix44(source));
    }

    inline Matrix44 RotateRadMatrix44(const Vector3& axis, float rad)
    {
        Matrix44 ret;
        Vector3 nrm = axis;

        nrm.normalize();
        float x = nrm.x;
        float y = nrm.y;
        float z = nrm.z;

        float sinA = Sin(rad);
        float cosA = Cos(rad);
        float invCosA = 1.0F - cosA;


        float xSq = x * x;
        float ySq = y * y;
        float zSq = z * z;

        ret[0][0] = (invCosA * xSq) + (cosA);
        ret[1][0] = (invCosA * x * y) - (sinA * z );
        ret[2][0] = (invCosA * x * z) + (sinA * y );
        ret[3][0] = 0.0F;

        ret[0][1] = (invCosA * x * y) + (sinA * z);
        ret[1][1] = (invCosA * ySq) + (cosA);
        ret[2][1] = (invCosA * y * z) - (sinA * x);
        ret[3][1] = 0.0F;

        ret[0][2] = (invCosA * x * z) - (sinA * y);
        ret[1][2] = (invCosA * y * z) + (sinA * x);
        ret[2][2] = (invCosA * zSq) + (cosA);
        ret[3][2] = 0.0F;

        ret[0][3] = 0.0F;
        ret[1][3] = 0.0F;
        ret[2][3] = 0.0F;
        ret[3][3] = 1.0F;

        return ret;
    }

    inline Matrix44 RotateDegMatrix44(const Vector3& axis, float deg)
    {
        return RotateRadMatrix44(axis, DegToRad(deg));
    }

    inline Matrix44 ScaleMatrix44(float x, float y, float z, float w)
    {
        Matrix44 ret = m44::Identity();

        ret[0][0] = x;
        ret[1][1] = y;
        ret[2][2] = z;
        ret[3][3] = w;

        return ret;
    }

    inline Matrix44 ScaleMatrix44(const Vector2& scaleValue)
    {
        return ScaleMatrix44(scaleValue.x, scaleValue.y, 1.0f, 1.0f);
    }

    inline Matrix44 ScaleMatrix44(const Vector3& scaleValue)
    {
        return ScaleMatrix44(scaleValue.x, scaleValue.y, scaleValue.z, 1.0f);
    }

    inline Matrix44 ScaleMatrix44(const Vector4& scaleValue)
    {
        return ScaleMatrix44(scaleValue.x, scaleValue.y, scaleValue.z, scaleValue.w);
    }

    inline Matrix44 EulerXYZMatrix44(const Vector3& euler)
    {
        float  ci, cj, ch, si, sj, sh, cc, cs, sc, ss;

        SinCos(euler.x, si, ci);
        SinCos(euler.y, sj, cj);
        SinCos(euler.z, sh, ch);

        cc = ci * ch;
        cs = ci * sh;
        sc = si * ch;
        ss = si * sh;

        Matrix44 result(Vector4(cj * ch     , cj * sh     , -sj      , 0),
                        Vector4(sj * sc - cs, sj * ss + cc,  cj * si , 0),
                        Vector4(sj * cc + ss, sj * cs - sc,  cj * ci , 0),
                        Vector4(0           , 0           ,  0       , 1));

        return result;
    }

    inline Matrix44 LookAtMatrixRH44(const Vector3& camPos, const Vector3& target, const Vector3& camUp)
    {
        Matrix44 ret;

        Vector3 F = target - camPos;
        F.normalize();

        Vector3 S = CrossProduct(F, Normalized(camUp));
        S.normalize();

        Vector3 U = CrossProduct(S, F);
        U.normalize();

        ret[0][0] = S.x;
        ret[1][0] = S.y;
        ret[2][0] = S.z;
        ret[3][0] = 0.0;

        ret[0][1] = U.x;
        ret[1][1] = U.y;
        ret[2][1] = U.z;
        ret[3][1] = 0.0;

        ret[0][2] = -F.x;
        ret[1][2] = -F.y;
        ret[2][2] = -F.z;
        ret[3][2] = 0.0;

        ret[0][3] = 0.0F;
        ret[1][3] = 0.0F;
        ret[2][3] = 0.0F;
        ret[3][3] = 1.0F;

        ret *= TranslateMatrix44(-camPos.x, -camPos.y, -camPos.z);

        return ret;
    }

    inline Matrix44 LookAtMatrixLH44(const Vector3& camPos, const Vector3& target, const Vector3& camUp)
    {
        // this corresponds to D3D math
        Vector3 zAxis = target - camPos;
        zAxis.normalize();
        Vector3 xAxis = CrossProduct(Normalized(camUp), zAxis);
        xAxis.normalize();
        Vector3 yAxis = CrossProduct(zAxis, xAxis);

        Matrix44 ret;
        ret[0][0] = xAxis.x;
        ret[1][0] = xAxis.y;
        ret[2][0] = xAxis.z;
        ret[3][0] = -DotProduct(xAxis, camPos);

        ret[0][1] = yAxis.x;
        ret[1][1] = yAxis.y;
        ret[2][1] = yAxis.z;
        ret[3][1] = -DotProduct(yAxis, camPos);

        ret[0][2] = zAxis.x;
        ret[1][2] = zAxis.y;
        ret[2][2] = zAxis.z;
        ret[3][2] = -DotProduct(zAxis, camPos);

        ret[0][3] = 0.0f;
        ret[1][3] = 0.0f;
        ret[2][3] = 0.0f;
        ret[3][3] = 1.0f;

        return ret;
    }

    inline Matrix44 PerspectiveMatrixLH44(float fovY, float aspect, float n, float f)
    {
        float angle = fovY / 2.0f;
        angle = DegToRad(angle);

        float cot = Cos(angle) / Sin(angle);
        float Q1 = f / (f - n);
        float Q2 = -n * f / (f - n);

        Matrix44 ret;
        ret[0][0] = cot / aspect;
        ret[0][1] = 0.0f;
        ret[0][2] = 0.0f;
        ret[0][3] = 0.0f;

        ret[1][0] = 0.0f;
        ret[1][1] = cot;
        ret[1][2] = 0.0f;
        ret[1][3] = 0.0f;

        ret[2][0] = 0.0f;
        ret[2][1] = 0.0f;
        ret[2][2] = Q1;
        ret[2][3] = 1.0f;


        ret[3][0] = 0.0f;
        ret[3][1] = 0.0f;
        ret[3][2] = Q2;//-Q * n;
        ret[3][3] = 0.0f;

        return ret;
    }

    inline Matrix44 PerspectiveMatrixRH44(float fovY, float aspect, float n, float f)
    {
        float angle = fovY / 2.0f;
        angle = DegToRad(angle);

        float cot = Cos(angle) / Sin(angle);

        Matrix44 ret;
        ret[0][0] = cot / aspect;
        ret[0][1] = 0.0f;
        ret[0][2] = 0.0f;
        ret[0][3] = 0.0f;

        ret[1][0] = 0.0f;
        ret[1][1] = cot;
        ret[1][2] = 0.0f;
        ret[1][3] = 0.0f;

        ret[2][0] = 0.0f;
        ret[2][1] = 0.0f;
        ret[2][2] = -(f + n) / (f - n);
        ret[2][3] = -1.0f;

        ret[3][0] = 0.0f;
        ret[3][1] = 0.0f;
        ret[3][2] = -(2*f*n) / (f - n);
        ret[3][3] = 0.0f;

        return ret;
    }
    //----------------------------------------------------------------------
    // Matrix44 End
    //----------------------------------------------------------------------


} // namespace eg

#endif // MATH_MATRIX_IMPL_H

