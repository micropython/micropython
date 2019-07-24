#include "ProductionCode.h"
#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP(ProductionCode);

//sometimes you may want to get at local data in a module.
//for example: If you plan to pass by reference, this could be useful
//however, it should often be avoided
extern int Counter;

TEST_SETUP(ProductionCode)
{
  //This is run before EACH TEST
  Counter = 0x5a5a;
}

TEST_TEAR_DOWN(ProductionCode)
{
}

TEST(ProductionCode, FindFunction_WhichIsBroken_ShouldReturnZeroIfItemIsNotInList_WhichWorksEvenInOurBrokenCode)
{
  //All of these should pass
  TEST_ASSERT_EQUAL(0, FindFunction_WhichIsBroken(78));
  TEST_ASSERT_EQUAL(0, FindFunction_WhichIsBroken(1));
  TEST_ASSERT_EQUAL(0, FindFunction_WhichIsBroken(33));
  TEST_ASSERT_EQUAL(0, FindFunction_WhichIsBroken(999));
  TEST_ASSERT_EQUAL(0, FindFunction_WhichIsBroken(-1));
}

TEST(ProductionCode, FindFunction_WhichIsBroken_ShouldReturnTheIndexForItemsInList_WhichWillFailBecauseOurFunctionUnderTestIsBroken)
{
  // You should see this line fail in your test summary
  TEST_ASSERT_EQUAL(1, FindFunction_WhichIsBroken(34));

  // Notice the rest of these didn't get a chance to run because the line above failed.
  // Unit tests abort each test function on the first sign of trouble.
  // Then NEXT test function runs as normal.
  TEST_ASSERT_EQUAL(8, FindFunction_WhichIsBroken(8888));
}

TEST(ProductionCode, FunctionWhichReturnsLocalVariable_ShouldReturnTheCurrentCounterValue)
{
    //This should be true because setUp set this up for us before this test
    TEST_ASSERT_EQUAL_HEX(0x5a5a, FunctionWhichReturnsLocalVariable());

    //This should be true because we can still change our answer
    Counter = 0x1234;
    TEST_ASSERT_EQUAL_HEX(0x1234, FunctionWhichReturnsLocalVariable());
}

TEST(ProductionCode, FunctionWhichReturnsLocalVariable_ShouldReturnTheCurrentCounterValueAgain)
{
    //This should be true again because setup was rerun before this test (and after we changed it to 0x1234)
    TEST_ASSERT_EQUAL_HEX(0x5a5a, FunctionWhichReturnsLocalVariable());
}

TEST(ProductionCode, FunctionWhichReturnsLocalVariable_ShouldReturnCurrentCounter_ButFailsBecauseThisTestIsActuallyFlawed)
{
    //Sometimes you get the test wrong.  When that happens, you get a failure too... and a quick look should tell
    // you what actually happened...which in this case was a failure to setup the initial condition.
    TEST_ASSERT_EQUAL_HEX(0x1234, FunctionWhichReturnsLocalVariable());
}
