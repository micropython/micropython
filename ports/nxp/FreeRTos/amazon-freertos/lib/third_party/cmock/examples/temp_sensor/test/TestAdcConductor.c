#include "unity.h"
#include "UnityHelper.h"
#include "Types.h"
#include "Types.h"
#include "AdcConductor.h"
#include "MockAdcModel.h"
#include "MockAdcHardware.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void testInitShouldCallHardwareInit(void)
{
  AdcHardware_Init_Expect();
  AdcConductor_Init();
}

void testRunShouldNotDoAnythingIfItIsNotTime(void)
{
  AdcModel_DoGetSample_ExpectAndReturn(FALSE);

  AdcConductor_Run();
}

void testRunShouldNotPassAdcResultToModelIfSampleIsNotComplete(void)
{
  AdcModel_DoGetSample_ExpectAndReturn(TRUE);
  AdcHardware_GetSampleComplete_ExpectAndReturn(FALSE);

  AdcConductor_Run();
}

void testRunShouldGetLatestSampleFromAdcAndPassItToModelAndStartNewConversionWhenItIsTime(void)
{
  AdcModel_DoGetSample_ExpectAndReturn(TRUE);
  AdcHardware_GetSampleComplete_ExpectAndReturn(TRUE);
  AdcHardware_GetSample_ExpectAndReturn(293U);
  AdcModel_ProcessInput_Expect(293U);
  AdcHardware_StartConversion_Expect();

  AdcConductor_Run();
}

void testJustHereToTest_Should_ProperlyPassAStructAndVerifyIt(void)
{
    EXAMPLE_STRUCT_T TestStruct;
    TestStruct.x = 5;
    TestStruct.y = 7;

    AdcModel_DoNothingExceptTestASpecialType_ExpectAndReturn(TestStruct, TRUE);

    TEST_ASSERT_TRUE(AdcConductor_JustHereToTest());
}

//void testJustHereToTest_Should_FailThisTestIfYouUncommentXIsBecauseItsWrong(void)
//{
//    EXAMPLE_STRUCT_T TestStruct;
//    TestStruct.x = 6;
//    TestStruct.y = 7;
//
//    AdcModel_DoNothingExceptTestASpecialType_ExpectAndReturn(TestStruct, TRUE);
//
//    TEST_ASSERT_TRUE(AdcConductor_JustHereToTest());
//}
//
//void testJustHereToTest_Should_FailThisTestIfYouUncommentYIsBecauseItsWrong(void)
//{
//    EXAMPLE_STRUCT_T TestStruct;
//    TestStruct.x = 5;
//    TestStruct.y = 8;
//
//    AdcModel_DoNothingExceptTestASpecialType_ExpectAndReturn(TestStruct, TRUE);
//
//    TEST_ASSERT_TRUE(AdcConductor_JustHereToTest());
//}

void test_AdcConductor_AlsoHereToTest_Should_ProperlyReturnAStructAsExpected1(void)
{
    EXAMPLE_STRUCT_T TestStruct;
    TestStruct.x = 99;
    TestStruct.y = 1;

    AdcModel_DoNothingExceptReturnASpecialType_ExpectAndReturn(TestStruct);

    TEST_ASSERT_TRUE(AdcConductor_AlsoHereToTest());
}

void test_AdcConductor_AlsoHereToTest_Should_ProperlyReturnAStructAsExpected2(void)
{
    EXAMPLE_STRUCT_T TestStruct;
    TestStruct.x = 98;
    TestStruct.y = 1;

    AdcModel_DoNothingExceptReturnASpecialType_ExpectAndReturn(TestStruct);

    TEST_ASSERT_FALSE(AdcConductor_AlsoHereToTest());
}

void test_AdcConductor_YetAnotherTest_Should_VerifyThatPointersToStructsAreTestable(void)
{
    uint32 TestNum = 3;

    AdModel_DoNothingExceptTestPointers_ExpectAndReturn(&TestNum, TRUE);

    TEST_ASSERT_TRUE(AdcConductor_YetAnotherTest());
}

//void test_AdcConductor_YetAnotherTest_Should_FailIfYouUncommentThisTestBecauseTheValuePointedToIsWrong(void)
//{
//    uint32 TestNum = 7;
//
//    AdModel_DoNothingExceptTestPointers_ExpectAndReturn(&TestNum, FALSE);
//
//    TEST_ASSERT_FALSE(AdcConductor_YetAnotherTest());
//}

