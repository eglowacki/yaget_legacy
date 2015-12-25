// Test Math module
#include "BaseRegistrate.h"
#include "Math/Vector.h"
#include "Registrate.h"
#include "File/ArchiveHelpers.h"
#include "File/VirtualFileSystem.h"
#include <UnitTest++.h>

using namespace eg;

TEST(Math_VectorInits)
{
    //-------------------------------------------------------------------------------------
    Vector2 value2 = v2::ZERO();
    CHECK_EQUAL(value2, Vector2(0, 0));

    Vector3 value3 = v3::ZERO();
    CHECK_EQUAL(value3, Vector3(0, 0, 0));

    Vector4 value4 = v4::ZERO();
    CHECK_EQUAL(value4, Vector4(0, 0, 0, 0));

    //-------------------------------------------------------------------------------------
    value2 = v2::ONE();
    CHECK_EQUAL(value2, Vector2(1, 1));

    value3 = v3::ONE();
    CHECK_EQUAL(value3, Vector3(1, 1, 1));

    value4 = v4::ONE();
    CHECK_EQUAL(value4, Vector4(1, 1, 1, 1));

    //-------------------------------------------------------------------------------------
    value2 = v2::XONE();
    CHECK_EQUAL(value2, Vector2(1, 0));

    value3 = v3::XONE();
    CHECK_EQUAL(value3, Vector3(1, 0, 0));

    value4 = v4::XONE();
    CHECK_EQUAL(value4, Vector4(1, 0, 0, 0));

    //-------------------------------------------------------------------------------------
    value2 = v2::YONE();
    CHECK_EQUAL(value2, Vector2(0, 1));

    value3 = v3::YONE();
    CHECK_EQUAL(value3, Vector3(0, 1, 0));

    value4 = v4::YONE();
    CHECK_EQUAL(value4, Vector4(0, 1, 0, 0));

    //-------------------------------------------------------------------------------------
    value3 = v3::ZONE();
    CHECK_EQUAL(value3, Vector3(0, 0, 1));

    value4 = v4::ZONE();
    CHECK_EQUAL(value4, Vector4(0, 0, 1, 0));

    //-------------------------------------------------------------------------------------
    value4 = v4::WONE();
    CHECK_EQUAL(value4, Vector4(0, 0, 0, 1));
}


TEST(Math_VectorAssigments)
{
    Vector2 value2(1, 2);
    Vector3 value3(3, 4, 5);
    Vector4 value4(6, 7, 8, 9);

    value2 = value3;
    CHECK_EQUAL(value2, Vector2(3, 4));

    value3 = value4;
    CHECK_EQUAL(value3, Vector3(6, 7, 8));

    value3 = value2;
    CHECK_EQUAL(value3, Vector3(3, 4, 0));

    value3.set(3, 4, 5);
    value4 = value3;
    CHECK_EQUAL(value4, Vector4(3, 4, 5, 1));

    value2.set(1, 2);
    value4 = value2;
    CHECK_EQUAL(value4, Vector4(1, 2, 0, 1));

    value2.set("");
    CHECK_EQUAL(value2, Vector2(0, 0));
    value2.set("10");
    CHECK_EQUAL(value2, Vector2(10, 0));
    value2.set("10, 11");
    CHECK_EQUAL(value2, Vector2(10, 11));

    const float checkedValue[4] = {10, 11, 12, 13};
    const float *pValue2 = value2.getF();
    CHECK_ARRAY_EQUAL(pValue2, checkedValue, 2);

    value3.set("");
    CHECK_EQUAL(value3, Vector3(0, 0, 0));
    value3.set("10, 11");
    CHECK_EQUAL(value3, Vector3(10, 11, 0));
    value3.set("10, 11, 12");
    CHECK_EQUAL(value3, Vector3(10, 11, 12));

    const float *pValue3 = value3.getF();
    CHECK_ARRAY_EQUAL(pValue3, checkedValue, 3);

    value4.set("");
    CHECK_EQUAL(value4, Vector4(0, 0, 0, 0));
    value4.set("10, 11");
    CHECK_EQUAL(value4, Vector4(10, 11, 0, 0));
    value4.set("10, 11, 12");
    CHECK_EQUAL(value4, Vector4(10, 11, 12, 0));
    value4.set("10, 11, 12, 13");
    CHECK_EQUAL(value4, Vector4(10, 11, 12, 13));

    const float *pValue4 = value4.getF();
    CHECK_ARRAY_EQUAL(pValue4, checkedValue, 4);
}



TEST(Math_Streams)
{
    VirtualFileSystem& fileSystem = REGISTRATE(VirtualFileSystem);

    CHECK(fileSystem.RegisterFactory("math", "MemoryFile"));

    Vector2 value2(1, 2);
    Vector3 value3(3, 4, 5);
    Vector4 value4(6, 7, 8, 9);

    //-------------------------------------------------------------------------------------
    VirtualFileFactory::ostream_t os = fileSystem.AttachFileStream("UnitTest.math");
    CHECK(os != 0);

    (*os) << value2;
    (*os) << value2;

    VirtualFileFactory::istream_t is = fileSystem.GetFileStream("UnitTest.math");
    CHECK(is != 0);

    Vector2 newAValue2 = v2::ZERO();
    Vector2 newBValue2 = v2::ZERO();

    (*is) >> newAValue2;
    (*is) >> newBValue2;

    CHECK_EQUAL(value2, newAValue2);
    CHECK_EQUAL(value2, newBValue2);

    //-------------------------------------------------------------------------------------
    os = fileSystem.AttachFileStream("UnitTest.math");
    CHECK(os != 0);

    (*os) << value3;
    (*os) << value3;

    is = fileSystem.GetFileStream("UnitTest.math");
    CHECK(is != 0);

    Vector3 newAValue3 = v3::ZERO();
    Vector3 newBValue3 = v3::ZERO();

    (*is) >> newAValue3;
    (*is) >> newBValue3;

    CHECK_EQUAL(value3, newAValue3);
    CHECK_EQUAL(value3, newBValue3);

    //-------------------------------------------------------------------------------------
    os = fileSystem.AttachFileStream("UnitTest.math");
    CHECK(os != 0);

    (*os) << value4;
    (*os) << value4;

    is = fileSystem.GetFileStream("UnitTest.math");
    CHECK(is != 0);

    Vector4 newAValue4 = v4::ZERO();
    Vector4 newBValue4 = v4::ZERO();

    (*is) >> newAValue4;
    (*is) >> newBValue4;

    CHECK_EQUAL(value4, newAValue4);
    CHECK_EQUAL(value4, newBValue4);
}

