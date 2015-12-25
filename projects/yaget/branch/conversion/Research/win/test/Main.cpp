// test.cpp
#include <UnitTest++.h>

using namespace eg;

SUITE(SomeName)
{
    //-------------------------------------------------------------------------------------
    TEST(SomeTest)
    {
    }
}




int main()
{
    int result = UnitTest::RunAllTests();
    return result;
}
