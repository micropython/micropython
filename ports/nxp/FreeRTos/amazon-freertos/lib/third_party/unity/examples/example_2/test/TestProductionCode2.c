#include "ProductionCode2.h"
#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP(ProductionCode2);

/* These should be ignored because they are commented out in various ways:
#include "whatever.h"
*/
//#include "somethingelse.h"

TEST_SETUP(ProductionCode2)
{
}

TEST_TEAR_DOWN(ProductionCode2)
{
}

TEST(ProductionCode2, IgnoredTest)
{
    TEST_IGNORE_MESSAGE("This Test Was Ignored On Purpose");
}

TEST(ProductionCode2, AnotherIgnoredTest)
{
    TEST_IGNORE_MESSAGE("These Can Be Useful For Leaving Yourself Notes On What You Need To Do Yet");
}

TEST(ProductionCode2, ThisFunctionHasNotBeenTested_NeedsToBeImplemented)
{
    TEST_IGNORE(); //Like This
}
