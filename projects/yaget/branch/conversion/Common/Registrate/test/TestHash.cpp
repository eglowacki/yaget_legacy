// Test Hash class
#include "BaseRegistrate.h"
#include "Hash/Hash.h"
#include "Registrate.h"
#include "File/ArchiveHelpers.h"
#include "File/VirtualFileSystem.h"
#include <boost/serialization/version.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <UnitTest++.h>

using namespace eg;

TEST(HashClass)
{
    Hash hashValue;
    CHECK(hashValue.IsValid() == false);

    hashValue = Hash("HashValue");
    CHECK(hashValue.IsValid());

    Hash hashValue2 = Hash("HashValueSecondTest");
    CHECK(hashValue2.IsValid());

    VirtualFileSystem& fileSystem = REGISTRATE(VirtualFileSystem);

    CHECK(fileSystem.RegisterFactory("hash", "MemoryFile"));

    VirtualFileFactory::ostream_t os = fileSystem.AttachFileStream("UnitTest.hash");
    CHECK(os != 0);
    (*os) << hashValue << hashValue2;
    os.reset();

    VirtualFileFactory::istream_t is = fileSystem.GetFileStream("UnitTest.hash");
    CHECK(is != 0);
    Hash newHashValue;
    Hash newHashValue2;
    (*is) >> newHashValue >> newHashValue2;
    CHECK_EQUAL(hashValue, newHashValue);
    CHECK_EQUAL(hashValue2, newHashValue2);
}

