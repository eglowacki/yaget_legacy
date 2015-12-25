#include "YagetVersion.h"
#include <UnitTest++.h>



TEST(Version)
{
    yaget::Version myVersion(yaget::YagetVersion);

    CHECK(yaget::YagetVersion == myVersion);

    CHECK(yaget::ver::is_major(myVersion));
    CHECK(yaget::ver::is_minor(myVersion));
    CHECK(yaget::ver::is_build(myVersion));
    CHECK(yaget::ver::is_mm(myVersion));

    yaget::Version badVersion(10000, 20000, 30000);

    CHECK(!(yaget::YagetVersion == badVersion));
    CHECK(!yaget::ver::is_major(badVersion));
    CHECK(!yaget::ver::is_minor(badVersion));
    CHECK(!yaget::ver::is_build(badVersion));
    CHECK(!yaget::ver::is_mm(badVersion));

    std::string textVersion = yaget::ver::to_string(badVersion);
    CHECK(textVersion == "10000.20000.30000");
}
