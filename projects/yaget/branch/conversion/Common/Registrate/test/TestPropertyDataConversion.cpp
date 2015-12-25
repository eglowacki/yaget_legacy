// Test Property Data conversion
#include "BaseRegistrate.h"
//#include "Math/Vector.h"
#include "Property/PropertyDataConversion.h"
//#include "Registrate.h"
//#include "File/ArchiveHelpers.h"
//#include "File/VirtualFileSystem.h"
//#include <boost/bind.hpp>
#include <UnitTest++.h>

using namespace eg;



TEST(PropertyDataConversion)
{
    int32_t initialInt32 = -5;
    std::string stringValue = ConvertProperty(initialInt32);
    int32_t convertedInt32 = ConvertProperty<int32_t>(stringValue);
    CHECK_EQUAL(initialInt32, convertedInt32);

    uint32_t initialUInt32 = 5;
    stringValue = ConvertProperty(initialUInt32);
    uint32_t convertedUInt32 = ConvertProperty<uint32_t>(stringValue);
    CHECK_EQUAL(initialUInt32, convertedUInt32);

    bool initialBool = true;
    stringValue = ConvertProperty(initialBool);
    bool convertedBool = ConvertProperty<bool>(stringValue);
    CHECK_EQUAL(initialBool, convertedBool);

    convertedBool = ConvertProperty<bool>(std::string("false"));
    CHECK_EQUAL(false, convertedBool);

    convertedBool = ConvertProperty<bool>(std::string("tRue"));
    CHECK_EQUAL(true, convertedBool);

    convertedBool = ConvertProperty<bool>(std::string("1"));
    CHECK_EQUAL(true, convertedBool);

    convertedBool = ConvertProperty<bool>(std::string("garbage"));
    CHECK_EQUAL(false, convertedBool);
}
