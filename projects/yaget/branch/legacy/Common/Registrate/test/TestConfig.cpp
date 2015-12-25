// Test Config functions to read and write
#include "BaseRegistrate.h"
#include "Config/ConfigHelper.h"
#include "Math/Vector.h"
#include <wx/sstream.h>
#include <wx/fileconf.h>
#include <UnitTest++.h>

TEST(ConfigHelpers_ReadWrite)
{
    std::string configData = "[Section]\n" \
                                "Integer = 1\n" \
                                "String = \"Hello There\"\n" \
                                "Float = 1.5\n" \
                                "Boolean = true\n" \
                                "Vector2 = 1, 2\n" \
                                "StringArray = Hello, 1, Edgar\n";
    eg::config::Use(configData);

    CHECK_EQUAL(eg::config::ReadInt("Section/Integer", 2), 1);
    eg::config::WriteInt("Section/Integer", 2);
    CHECK_EQUAL(eg::config::ReadInt("Section/Integer", 1), 2);

    CHECK_EQUAL(eg::config::ReadString("Section/String", ""), "Hello There");
    eg::config::WriteString("Section/String", "Good Buy");
    CHECK_EQUAL(eg::config::ReadString("Section/String", ""), "Good Buy");

    CHECK(eg::config::ReadBool("Section/Boolean", false));
    eg::config::WriteBool("Section/Boolean", false);
    CHECK(eg::config::ReadBool("Section/Boolean", true) == false);

    CHECK_EQUAL(eg::config::ReadV2("Section/Vector2"), eg::Vector2(1, 2));
    eg::config::WriteV2("Section/Vector2", eg::Vector2(3, 4));
    CHECK_EQUAL(eg::config::ReadV2("Section/Vector2"), eg::Vector2(3, 4));

    std::vector<std::string> testValue = eg::config::ReadStringArray("Section/StringArray");
    CHECK_EQUAL(testValue.size(), (size_t)3);
    CHECK_EQUAL(testValue[0], std::string("Hello"));
    CHECK_EQUAL(testValue[1], std::string("1"));
    CHECK_EQUAL(testValue[2], std::string("Edgar"));

    wxStringOutputStream os;
    static_cast<wxFileConfig *>(wxConfigBase::Get(false))->Save(os);
    std::string iniValues = (const char *)os.GetString().c_str();
}

TEST(ConfigHelpers_MacroSubstitution)
{
    std::string configData = "[Folders]\n" \
                                "RootFolder = c:/Assets\n" \
                                "Images = $(RootFolder)/targa\n";
    eg::config::Use(configData);

    CHECK_EQUAL("c:\\Assets\\targa\\", eg::config::ReadPath("Folders/Images"));
}

