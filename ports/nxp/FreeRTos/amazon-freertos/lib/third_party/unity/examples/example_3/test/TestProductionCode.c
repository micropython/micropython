
#include "ProductionCode.h"
#include "unity.h"

//sometimes you may want to get at local data in a module.
//for example: If you plan to pass by reference, this could be useful
//however, it should often be avoided
extern int Counter; 

void setUp(void)
{
  //This is run before EACH TEST
  Counter = 0x5a5a;
}

void tearDown(void)
{
}

void test_FindFunction_WhichIsBroken_ShouldReturnZeroIfItemIsNotInList_WhichWorksEvenInOurBrokenCode(void)
{
  //All of these should pass
  TEST_ASSERT_EQUAL(0, FindFunction_WhichIsBroken(78));
  TEST_ASSERT_EQUAL(0, FindFunction_WhichIsBroken(1));
  TEST_ASSERT_EQUAL(0, FindFunction_WhichIsBroken(33));
  TEST_ASSERT_EQUAL(0, FindFunction_WhichIsBroken(999));
  TEST_ASSERT_EQUAL(0, FindFunction_WhichIsBroken(-1));
}

void test_FindFunction_WhichIsBroken_ShouldReturnTheIndexForItemsInList_WhichWillFailBecauseOurFunctionUnderTestIsBroken(void)
{
  // You should see this line fail in your test summary
  TEST_ASSERT_EQUAL(1, FindFunction_WhichIsBroken(34));
  
  // Notice the rest of these didn't get a chance to run because the line above failed.  
  // Unit tests abort each test function on the first sign of trouble. 
  // Then NEXT test function runs as normal.
  TEST_ASSERT_EQUAL(8, FindFunction_WhichIsBroken(8888));
}

void test_FunctionWhichReturnsLocalVariable_ShouldReturnTheCurrentCounterValue(void)
{
    //This should be true because setUp set this up for us before this test
    TEST_ASSERT_EQUAL_HEX(0x5a5a, FunctionWhichReturnsLocalVariable());
    
    //This should be true because we can still change our answer
    Counter = 0x1234;
    TEST_ASSERT_EQUAL_HEX(0x1234, FunctionWhichReturnsLocalVariable());
}

void test_FunctionWhichReturnsLocalVariable_ShouldReturnTheCurrentCounterValueAgain(void)
{
    //This should be true again because setup was rerun before this test (and after we changed it to 0x1234)
    TEST_ASSERT_EQUAL_HEX(0x5a5a, FunctionWhichReturnsLocalVariable());
}

void test_FunctionWhichReturnsLocalVariable_ShouldReturnCurrentCounter_ButFailsBecauseThisTestIsActuallyFlawed(void)
{
    //Sometimes you get the test wrong.  When that happens, you get a failure too... and a quick look should tell
    // you what actually happened...which in this case was a failure to setup the initial condition.
    TEST_ASSERT_EQUAL_HEX(0x1234, FunctionWhichReturnsLocalVariable());
}
