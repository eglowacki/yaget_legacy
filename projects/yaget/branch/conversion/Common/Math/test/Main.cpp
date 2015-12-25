// test.cpp

//#ifdef _DEBUG
//#pragma comment(lib, "Math_d")
//#else // release
//#pragma comment(lib, "Math")
//#endif // debug/release


#include "Math/Matrix.h"
#include "Math/Quat.h"
#include <fstream>
#include <UnitTest++.h>
#include <XmlTestReporter.h>

using namespace eg;

SUITE(MathLibrary)
{
    //-------------------------------------------------------------------------------------
    TEST(Vector3_Inits)
    {
        Vector3 testValue = v3::ZERO();
        CHECK_EQUAL(Vector3(0, 0, 0), testValue);

        testValue = v3::ONE();
        CHECK_EQUAL(Vector3(1, 1, 1), testValue);

        testValue = v3::XONE();
        CHECK_EQUAL(Vector3(1, 0, 0), testValue);

        testValue = v3::YONE();
        CHECK_EQUAL(Vector3(0, 1, 0), testValue);

        testValue = v3::ZONE();
        CHECK_EQUAL(Vector3(0, 0, 1), testValue);

        testValue.setb(0, 128, 255);
        CHECK_CLOSE(0, testValue.x, 0.002f);
        CHECK_CLOSE(0.5f, testValue.y, 0.002f);
        CHECK_CLOSE(1, testValue.z, 0.002f);
    }

    //-------------------------------------------------------------------------------------
    TEST(Vector3_Operations)
    {
        Vector3 value1(1, 2, 3);
        Vector3 value2(4, 5, 6);

        Vector3 value3 = value1 + value2;
        CHECK_EQUAL(Vector3(5, 7, 9), value3);

        Vector3 value4 = value1 - value2;
        CHECK_EQUAL(Vector3(-3, -3, -3), value4);

        Vector3 value5 = value1 * value2;
        CHECK_EQUAL(Vector3(4, 10, 18), value5);


        Vector3 value6 = value1 / value2;
        CHECK_CLOSE(0.25f, value6.x, 0.002f);
        CHECK_CLOSE(0.4f, value6.y, 0.002f);
        CHECK_CLOSE(0.5f, value6.z, 0.002f);

        Vector3 value7 = value1 * 2;
        CHECK_EQUAL(Vector3(2, 4, 6), value7);

        Vector3 value8 = 2 * value1;
        CHECK_EQUAL(Vector3(2, 4, 6), value8);

        Vector3 value9(-1, -3, 4);
        value9.abs();
        CHECK_EQUAL(Vector3(1, 3, 4), value9);

        Vector3 value10(-1, -3, 4);
        value10.reverse();
        CHECK_EQUAL(Vector3(1, 3, -4), value10);

        Vector3 value11(10, 0, 0);
        value11.normalize();
        CHECK(AlmostEq(Vector3(1, 0, 0), value11));

        Vector3 value12(10.2, 0.4f, -1);
        value12.wrap();
        CHECK(AlmostEq(Vector3(0.2f, 0.4f, 0), value12));

        Vector3 value13(1, 0, 0);
        Vector3 value14(0, 0, 1);
        float d = DotProduct(value13, value14);
        CHECK_CLOSE(0, d, 0.002f);

        value13.set(1, 0, 0);
        value14.set(1, 0, 0);
        d = DotProduct(value13, value14);
        CHECK_CLOSE(1, d, 0.002f);

        value13.set(1, 0, 0);
        value14.set(-1, 0, 0);
        d = DotProduct(value13, value14);
        CHECK_CLOSE(-1, d, 0.002f);

        value13.set(1, 0, 0);
        value14.set(0, 0, 1);
        Vector3 value15 = CrossProduct(value14, value13);
        CHECK_EQUAL(Vector3(0, 1, 0), value15);

        value13.set(1, 0, 0);
        value14.set(0, 0, 1);
        value10.set(0, 0, 0);
        value15 = CrossProduct(value14, value13, value10);
        CHECK_EQUAL(Vector3(0, 1, 0), value15);

        value13.set(10, 0, 0);
        value14.set(0, 0, 10);
        value10.set(0, 0, 0);
        value15 = CrossProductN(value14, value13, value10);
        CHECK_EQUAL(Vector3(0, 1, 0), value15);

        value13.set(1, 0, 0);
        value14.set(0, 0, 1);
        float a = AngleD(value13, value14);
        CHECK_CLOSE(90, a, 0.002f);

        value14.set(0, 0, 1);
        d = Length(value14);
        CHECK_CLOSE(1, d, 0.002f);

        value13.set(1, 0, 0);
        value14.set(12, 0, 0);
        Vector3 value16 = Direction(value13, value14);
        CHECK(AlmostEq(Vector3(1, 0, 0), value16));

        value16 = Direction(value14, value13);
        CHECK(AlmostEq(Vector3(-1, 0, 0), value16));

        float c = 10;
        c = Clamp<float>(c, -1, 1);
        CHECK_EQUAL(1, c);

        c = -10;
        c = Clamp<float>(c, -1, 1);
        CHECK_EQUAL(-1, c);
    }

    //-------------------------------------------------------------------------------------
    TEST(Vector3_Serialization)
    {
        const Vector3 testPattern(1, 2, 3);

        Vector3 testValue = testPattern;
        std::ostringstream os;
        os << testValue;
        std::string str = os.str();

        testValue = v3::ZERO();

        std::istringstream iss(str);
        iss >> testValue;

        CHECK_EQUAL(testPattern, testValue);

        testValue = v3::ZERO();
        std::istringstream iss2("1 2 3");
        iss2 >> testValue;

        CHECK_EQUAL(testPattern, testValue);
    }

    //-------------------------------------------------------------------------------------
    TEST(Matrix33_Inits)
    {
        Matrix33 m = m33::Identity();
        CHECK_EQUAL(v3::XONE(), m[0]);
        CHECK_EQUAL(v3::YONE(), m[1]);
        CHECK_EQUAL(v3::ZONE(), m[2]);
    }

    //-------------------------------------------------------------------------------------
    TEST(Matrix33_Streams)
    {

        Matrix33 testValue(Vector3(1, 2, 3), Vector3(4, 5, 6), Vector3(7, 8, 9));

        std::ostringstream os;
        os << testValue;
        std::string str = os.str();

        Matrix33 newValue(0.0f);

        std::istringstream iss(str);
        iss >> newValue;

        CHECK_EQUAL(Vector3(1, 2, 3), newValue[0]);
        CHECK_EQUAL(Vector3(4, 5, 6), newValue[1]);
        CHECK_EQUAL(Vector3(7, 8, 9), newValue[2]);
    }

    //-------------------------------------------------------------------------------------
    TEST(Matrix44_Inits)
    {
        Matrix44 m = m44::Identity();
        CHECK_EQUAL(v4::XONE(), m[0]);
        CHECK_EQUAL(v4::YONE(), m[1]);
        CHECK_EQUAL(v4::ZONE(), m[2]);
        CHECK_EQUAL(v4::WONE(), m[3]);
    }

    //-------------------------------------------------------------------------------------
    TEST(Matrix44_Streams)
    {

        Matrix44 testValue(Vector4(1, 2, 3, 4), Vector4(5, 6, 7, 8), Vector4(9, 10, 11, 12), Vector4(13, 14, 15, 16));

        std::ostringstream os;
        os << testValue;
        std::string str = os.str();

        Matrix44 newValue(0.0f);

        std::istringstream iss(str);
        iss >> newValue;

        CHECK_EQUAL(Vector4(1, 2, 3, 4), newValue[0]);
        CHECK_EQUAL(Vector4(5, 6, 7, 8), newValue[1]);
        CHECK_EQUAL(Vector4(9, 10, 11, 12), newValue[2]);
        CHECK_EQUAL(Vector4(13, 14, 15, 16), newValue[3]);
    }

    //-------------------------------------------------------------------------------------
    TEST(Matrix44_VectorTranslate)
    {
        Matrix44 m = m44::Identity();
        m.SetTranslate(Vector3(1, 2, 3));

        Vector3 v = v3::ZERO();
        v = m * v;

        CHECK_EQUAL(Vector3(1, 2, 3), v);
    }


    TEST(Matrix44_VectorRotate)
    {
        // rotation around arbitrary axis
        Matrix44 m = RotateDegMatrix44(v3::YONE(), 90);
        Vector3 v = v3::XONE();
        v = m * v;
        CHECK(AlmostEq(Vector3(0, 0, -1), v));

        m = RotateDegMatrix44(v3::ZONE(), 90);
        v = v3::XONE();
        v = m * v;
        CHECK(AlmostEq(Vector3(0, 1, 0), v));

        m = RotateDegMatrix44(v3::XONE(), 90);
        v = v3::YONE();
        v = m * v;
        CHECK(AlmostEq(Vector3(0, 0, 1), v));

        // rotation using euler angles
        v = v3::XONE();
        m = EulerXYZMatrix44(Vector3(0, DegToRad(90), 0));
        v = m * v;
        CHECK(AlmostEq(Vector3(0, 0, -1), v));

        v = v3::XONE();
        m = EulerXYZMatrix44(Vector3(0, 0, DegToRad(90)));
        v = m * v;
        CHECK(AlmostEq(Vector3(0, 1, 0), v));

        v = v3::YONE();
        m = EulerXYZMatrix44(Vector3(DegToRad(90), 0, 0));
        v = m * v;
        CHECK(AlmostEq(Vector3(0, 0, 1), v));
    }

    TEST(Matrix_Inverse)
    {
        Vector3 v = v3::XONE();
        Matrix44 m = RotateDegMatrix44(v3::YONE(), 90);
        v = m * v;
        m = InverseMatrix44(m);
        v = m * v;
        CHECK(AlmostEq(v3::XONE(), v));

        m = RotateDegMatrix44(v3::YONE(), 90);
        m = m * InverseMatrix44(m);
        v = v3::XONE();
        v = m * v;
        CHECK(AlmostEq(v3::XONE(), v));

        Matrix44 refm = RotateDegMatrix44(v3::YONE(), 90);
        m = InverseMatrix44(refm);
        m = InverseMatrix44(m);
        v = v3::XONE();
        v = m * v;

        Vector3 v1 = v3::XONE();
        v1 = refm * v1;
        CHECK(AlmostEq(v1, v));
    }

    TEST(Matrix_Transpose)
    {
        Vector3 v = v3::XONE();
        Matrix44 m = RotateDegMatrix44(v3::YONE(), 90);
        v = m * v;
        m = TransposeMatrix44(m);
        v = m * v;
        CHECK(AlmostEq(v3::XONE(), v));

        m = RotateDegMatrix44(v3::YONE(), 90);
        m = m * TransposeMatrix44(m);
        v = v3::XONE();
        v = m * v;
        CHECK(AlmostEq(v3::XONE(), v));

        Matrix44 refm = RotateDegMatrix44(v3::YONE(), 90);
        m = TransposeMatrix44(refm);
        m = TransposeMatrix44(m);
        v = v3::XONE();
        v = m * v;

        Vector3 v1 = v3::XONE();
        v1 = refm * v1;
        CHECK(AlmostEq(v1, v));
    }

    TEST(Matrix_Normals)
    {
        Vector3 n = v3::XONE();
        Matrix44 m = RotateDegMatrix44(v3::YONE(), 90);
        m = NormalMatrix44(m);
        n = m * n;
        CHECK(AlmostEq(Vector3(0, 0, -1), n));
    }

    TEST(Quaternion)
    {
        Quaternion q(v3::YONE(), DegToRad(90));
        Vector3 v = v3::XONE();
        v = q * v;
        CHECK(AlmostEq(Vector3(0, 0, -1), v));

        v = v3::XONE();
        Quaternion qa(v3::YONE(), DegToRad(50));
        Quaternion qb(v3::YONE(), DegToRad(40));
        Quaternion qc = qa * qb;
        v = qc * v;
        CHECK(AlmostEq(Vector3(0, 0, -1), v));
    }

    TEST(Fixing_Rotations)
    {
    }
}




int main()
{
    int result = UnitTest::RunAllTests();
    return result;
}
