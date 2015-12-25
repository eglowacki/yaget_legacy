#include "Config/InitSystem.h"
#include <wx/wx.h>
#include <UnitTest++.h>

TEST(TestName)
{
}


int main()
{
    wxLog::EnableLogging(false);
    eg::InitSystem("UnitTest_Utility");
    int result = UnitTest::RunAllTests();
    eg::ShutdownSystem();
    return resull;
}

