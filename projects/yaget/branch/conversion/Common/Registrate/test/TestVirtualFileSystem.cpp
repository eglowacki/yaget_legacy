// Test Virtual File System module
#include "BaseRegistrate.h"
#include "Registrate.h"
#include "Math/Vector.h"
#include "File/VirtualFileSystem.h"
#include <UnitTest++.h>

using namespace eg;

TEST(VirtualFileSystem)
{
    VirtualFileSystem& fileSystem = REGISTRATE(VirtualFileSystem);

    CHECK(fileSystem.RegisterFactory("ut", "MemoryFile"));

    VirtualFileFactory::ostream_t os = fileSystem.AttachFileStream("UnitTest.ut");
    CHECK(os != 0);

    (*os) << Vector2(1, 2);
    (*os) << Vector3(3, 4, 5);
    (*os) << Vector4(6, 7, 8, 9);
    os.reset();

    VirtualFileFactory::istream_t is = fileSystem.GetFileStream("UnitTest.ut");
    CHECK(is != 0);

    Vector2 value2 = v2::ZERO();
    Vector3 value3 = v3::ZERO();
    Vector4 value4 = v4::ZERO();
    (*is) >> value2;
    (*is) >> value3;
    (*is) >> value4;
    CHECK_EQUAL(value2, Vector2(1, 2));
    CHECK_EQUAL(value3, Vector3(3, 4, 5));
    CHECK_EQUAL(value4, Vector4(6, 7, 8, 9));
}

