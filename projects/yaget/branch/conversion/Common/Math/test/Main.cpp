#include "Precompiled.h"
#include "CppUnitTest.h"
#include "CppUnitTestAssert.h"
#include "Math/Quat.h"
#include "Math/Matrix.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace eg;
                

//namespace Microsoft {
//    namespace VisualStudio {
//        namespace CppUnitTestFramework

template<> inline std::wstring Microsoft::VisualStudio::CppUnitTestFramework::ToString<Vector3>(const Vector3& t) { RETURN_WIDE_STRING(t.Print().c_str()); }
template<> inline std::wstring Microsoft::VisualStudio::CppUnitTestFramework::ToString<Vector4>(const Vector4& t) { RETURN_WIDE_STRING(t.Print().c_str()); }
template<> inline std::wstring Microsoft::VisualStudio::CppUnitTestFramework::ToString<Quaternion>(const Quaternion& t) { RETURN_WIDE_STRING(t.Print().c_str()); }

namespace UnitTest_Math
{		
    TEST_CLASS(YAGET_MATH_TEST)
	{
	public:
		
		TEST_METHOD(Vector3_Inits)
		{
			// TODO: Your TEST_METHOD code here
            Vector3 TEST_METHODValue = v3::ZERO();

            Assert::AreEqual(Vector3(0, 0, 0), TEST_METHODValue);

            TEST_METHODValue = v3::ONE();
            Assert::AreEqual(Vector3(1, 1, 1), TEST_METHODValue);

            TEST_METHODValue = v3::XONE();
            Assert::AreEqual(Vector3(1, 0, 0), TEST_METHODValue);

            TEST_METHODValue = v3::YONE();
            Assert::AreEqual(Vector3(0, 1, 0), TEST_METHODValue);

            TEST_METHODValue = v3::ZONE();
            Assert::AreEqual(Vector3(0, 0, 1), TEST_METHODValue);

            TEST_METHODValue.setb(0, 128, 255);

            Assert::AreEqual(0.0f, TEST_METHODValue.x, 0.002f);
            Assert::AreEqual(0.5f, TEST_METHODValue.y, 0.002f);
            Assert::AreEqual(1.0f, TEST_METHODValue.z, 0.002f);
        }

        //-------------------------------------------------------------------------------------
        TEST_METHOD(Vector3_Operations)
        {
            Vector3 value1(1, 2, 3);
            Vector3 value2(4, 5, 6);

            Vector3 value3 = value1 + value2;
            Assert::AreEqual(Vector3(5, 7, 9), value3);

            Vector3 value4 = value1 - value2;
            Assert::AreEqual(Vector3(-3, -3, -3), value4);

            Vector3 value5 = value1 * value2;
            Assert::AreEqual(Vector3(4, 10, 18), value5);


            Vector3 value6 = value1 / value2;
            Assert::AreEqual(0.25f, value6.x, 0.002f);
            Assert::AreEqual(0.4f, value6.y, 0.002f);
            Assert::AreEqual(0.5f, value6.z, 0.002f);

            Vector3 value7 = value1 * 2;
            Assert::AreEqual(Vector3(2, 4, 6), value7);

            Vector3 value8 = 2 * value1;
            Assert::AreEqual(Vector3(2, 4, 6), value8);

            Vector3 value9(-1, -3, 4);
            value9.abs();
            Assert::AreEqual(Vector3(1, 3, 4), value9);

            Vector3 value10(-1, -3, 4);
            value10.reverse();
            Assert::AreEqual(Vector3(1, 3, -4), value10);

            Vector3 value11(10, 0, 0);
            value11.normalize();
            Assert::IsTrue(AlmostEq(Vector3(1, 0, 0), value11));

            Vector3 value12(10.2, 0.4f, -1);
            value12.wrap();
            Assert::IsTrue(AlmostEq(Vector3(0.2f, 0.4f, 0), value12));

            Vector3 value13(1, 0, 0);
            Vector3 value14(0, 0, 1);
            float d = DotProduct(value13, value14);
            Assert::AreEqual(0, d, 0.002f);

            value13.set(1, 0, 0);
            value14.set(1, 0, 0);
            d = DotProduct(value13, value14);
            Assert::AreEqual(1, d, 0.002f);

            value13.set(1, 0, 0);
            value14.set(-1, 0, 0);
            d = DotProduct(value13, value14);
            Assert::AreEqual(-1, d, 0.002f);

            value13.set(1, 0, 0);
            value14.set(0, 0, 1);
            Vector3 value15 = CrossProduct(value14, value13);
            Assert::AreEqual(Vector3(0, 1, 0), value15);

            value13.set(1, 0, 0);
            value14.set(0, 0, 1);
            value10.set(0, 0, 0);
            value15 = CrossProduct(value14, value13, value10);
            Assert::AreEqual(Vector3(0, 1, 0), value15);

            value13.set(10, 0, 0);
            value14.set(0, 0, 10);
            value10.set(0, 0, 0);
            value15 = CrossProductN(value14, value13, value10);
            Assert::AreEqual(Vector3(0, 1, 0), value15);

            value13.set(1, 0, 0);
            value14.set(0, 0, 1);
            float a = AngleD(value13, value14);
            Assert::AreEqual(90, a, 0.002f);

            value14.set(0, 0, 1);
            d = Length(value14);
            Assert::AreEqual(1, d, 0.002f);

            value13.set(1, 0, 0);
            value14.set(12, 0, 0);
            Vector3 value16 = Direction(value13, value14);
            Assert::IsTrue(AlmostEq(Vector3(1, 0, 0), value16));

            value16 = Direction(value14, value13);
            Assert::IsTrue(AlmostEq(Vector3(-1, 0, 0), value16));

            float c = 10;
            c = Clamp<float>(c, -1, 1);
            Assert::AreEqual(1.0f, c);

            c = -10;
            c = Clamp<float>(c, -1, 1);
            Assert::AreEqual(-1.0f, c);
        }
        //-------------------------------------------------------------------------------------
        TEST_METHOD(Vector3_Serialization)
        {
            const Vector3 TEST_METHODPattern(1, 2, 3);

            Vector3 TEST_METHODValue = TEST_METHODPattern;
            std::ostringstream os;
            os << TEST_METHODValue;
            std::string str = os.str();

            TEST_METHODValue = v3::ZERO();

            std::istringstream iss(str);
            iss >> TEST_METHODValue;

            Assert::AreEqual(TEST_METHODPattern, TEST_METHODValue);

            TEST_METHODValue = v3::ZERO();
            std::istringstream iss2("1 2 3");
            iss2 >> TEST_METHODValue;

            Assert::AreEqual(TEST_METHODPattern, TEST_METHODValue);
        }

        //-------------------------------------------------------------------------------------
        TEST_METHOD(Matrix33_Inits)
        {
            Matrix33 m = m33::Identity();
            Assert::AreEqual(v3::XONE(), m[0]);
            Assert::AreEqual(v3::YONE(), m[1]);
            Assert::AreEqual(v3::ZONE(), m[2]);
        }

        //-------------------------------------------------------------------------------------
        TEST_METHOD(Matrix33_Streams)
        {

            Matrix33 TEST_METHODValue(Vector3(1, 2, 3), Vector3(4, 5, 6), Vector3(7, 8, 9));

            std::ostringstream os;
            os << TEST_METHODValue;
            std::string str = os.str();

            Matrix33 newValue(0.0f);

            std::istringstream iss(str);
            iss >> newValue;

            Assert::AreEqual(Vector3(1, 2, 3), newValue[0]);
            Assert::AreEqual(Vector3(4, 5, 6), newValue[1]);
            Assert::AreEqual(Vector3(7, 8, 9), newValue[2]);
        }

        //-------------------------------------------------------------------------------------
        TEST_METHOD(Matrix44_Inits)
        {
            Matrix44 m = m44::Identity();
            Assert::AreEqual(v4::XONE(), m[0]);
            Assert::AreEqual(v4::YONE(), m[1]);
            Assert::AreEqual(v4::ZONE(), m[2]);
            Assert::AreEqual(v4::WONE(), m[3]);
        }

        //-------------------------------------------------------------------------------------
        TEST_METHOD(Matrix44_Streams)
        {

            Matrix44 TEST_METHODValue(Vector4(1, 2, 3, 4), Vector4(5, 6, 7, 8), Vector4(9, 10, 11, 12), Vector4(13, 14, 15, 16));

            std::ostringstream os;
            os << TEST_METHODValue;
            std::string str = os.str();

            Matrix44 newValue(0.0f);

            std::istringstream iss(str);
            iss >> newValue;

            Assert::AreEqual(Vector4(1, 2, 3, 4), newValue[0]);
            Assert::AreEqual(Vector4(5, 6, 7, 8), newValue[1]);
            Assert::AreEqual(Vector4(9, 10, 11, 12), newValue[2]);
            Assert::AreEqual(Vector4(13, 14, 15, 16), newValue[3]);
        }

        //-------------------------------------------------------------------------------------
        TEST_METHOD(Matrix44_VectorTranslate)
        {
            Matrix44 m = m44::Identity();
            m.SetTranslate(Vector3(1, 2, 3));

            Vector3 v = v3::ZERO();
            v = m * v;

            Assert::AreEqual(Vector3(1, 2, 3), v);
        }

        TEST_METHOD(Matrix44_VectorRotate)
        {
            // rotation around arbitrary axis
            Matrix44 m = RotateDegMatrix44(v3::YONE(), 90);
            Vector3 v = v3::XONE();
            v = m * v;
            Assert::IsTrue(AlmostEq(Vector3(0, 0, -1), v));

            m = RotateDegMatrix44(v3::ZONE(), 90);
            v = v3::XONE();
            v = m * v;
            Assert::IsTrue(AlmostEq(Vector3(0, 1, 0), v));

            m = RotateDegMatrix44(v3::XONE(), 90);
            v = v3::YONE();
            v = m * v;
            Assert::IsTrue(AlmostEq(Vector3(0, 0, 1), v));

            // rotation using euler angles
            v = v3::XONE();
            m = EulerXYZMatrix44(Vector3(0, DegToRad(90), 0));
            v = m * v;
            Assert::IsTrue(AlmostEq(Vector3(0, 0, -1), v));

            v = v3::XONE();
            m = EulerXYZMatrix44(Vector3(0, 0, DegToRad(90)));
            v = m * v;
            Assert::IsTrue(AlmostEq(Vector3(0, 1, 0), v));

            v = v3::YONE();
            m = EulerXYZMatrix44(Vector3(DegToRad(90), 0, 0));
            v = m * v;
            Assert::IsTrue(AlmostEq(Vector3(0, 0, 1), v));
        }

        TEST_METHOD(Matrix_Inverse)
        {
            Vector3 v = v3::XONE();
            Matrix44 m = RotateDegMatrix44(v3::YONE(), 90);
            v = m * v;
            m = InverseMatrix44(m);
            v = m * v;
            Assert::IsTrue(AlmostEq(v3::XONE(), v));

            m = RotateDegMatrix44(v3::YONE(), 90);
            m = m * InverseMatrix44(m);
            v = v3::XONE();
            v = m * v;
            Assert::IsTrue(AlmostEq(v3::XONE(), v));

            Matrix44 refm = RotateDegMatrix44(v3::YONE(), 90);
            m = InverseMatrix44(refm);
            m = InverseMatrix44(m);
            v = v3::XONE();
            v = m * v;

            Vector3 v1 = v3::XONE();
            v1 = refm * v1;
            Assert::IsTrue(AlmostEq(v1, v));
        }

        TEST_METHOD(Matrix_Transpose)
        {
            Vector3 v = v3::XONE();
            Matrix44 m = RotateDegMatrix44(v3::YONE(), 90);
            v = m * v;
            m = TransposeMatrix44(m);
            v = m * v;
            Assert::IsTrue(AlmostEq(v3::XONE(), v));

            m = RotateDegMatrix44(v3::YONE(), 90);
            m = m * TransposeMatrix44(m);
            v = v3::XONE();
            v = m * v;
            Assert::IsTrue(AlmostEq(v3::XONE(), v));

            Matrix44 refm = RotateDegMatrix44(v3::YONE(), 90);
            m = TransposeMatrix44(refm);
            m = TransposeMatrix44(m);
            v = v3::XONE();
            v = m * v;

            Vector3 v1 = v3::XONE();
            v1 = refm * v1;
            Assert::IsTrue(AlmostEq(v1, v));
        }

        TEST_METHOD(Matrix_Normals)
        {
            Vector3 n = v3::XONE();
            Matrix44 m = RotateDegMatrix44(v3::YONE(), 90);
            m = NormalMatrix44(m);
            n = m * n;
            Assert::IsTrue(AlmostEq(Vector3(0, 0, -1), n));
        }

        TEST_METHOD(Quaternion_Basic)
        {
            Quaternion q(v3::YONE(), DegToRad(90));
            Vector3 v = v3::XONE();
            v = q * v;
            Assert::IsTrue(AlmostEq(Vector3(0, 0, -1), v));

            v = v3::XONE();
            Quaternion qa(v3::YONE(), DegToRad(50));
            Quaternion qb(v3::YONE(), DegToRad(40));
            Quaternion qc = qa * qb;
            v = qc * v;
            Assert::IsTrue(AlmostEq(Vector3(0, 0, -1), v));
        }

        TEST_METHOD(Fixing_Rotations)
        {
        }
    };
}

