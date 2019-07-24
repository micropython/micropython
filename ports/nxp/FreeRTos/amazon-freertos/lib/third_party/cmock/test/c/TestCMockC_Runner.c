/* ==========================================
    CMock Project - Automatic Mock Generation for C
    Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
    [Released under MIT License. Please refer to license.txt for details]
========================================== */

#include "unity.h"
#include <setjmp.h>
#include <stdio.h>

extern void setUp(void);
extern void tearDown(void);

extern void test_MemNewWillReturnNullIfGivenIllegalSizes(void);
extern void test_MemChainWillReturnNullAndDoNothingIfGivenIllegalInformation(void);
extern void test_MemNextWillReturnNullIfGivenABadRoot(void);
extern void test_ThatWeCanClaimAndChainAFewElementsTogether(void);
extern void test_MemEndOfChain(void);
extern void test_ThatCMockStopsReturningMoreDataWhenItRunsOutOfMemory(void);
extern void test_ThatCMockStopsReturningMoreDataWhenAskForMoreThanItHasLeftEvenIfNotAtExactEnd(void);
extern void test_ThatWeCanAskForAllSortsOfSizes(void);

int main(void)
{
  Unity.TestFile = "TestCMock.c";
  UnityBegin(Unity.TestFile);

  RUN_TEST(test_MemNewWillReturnNullIfGivenIllegalSizes, 21);
  RUN_TEST(test_MemChainWillReturnNullAndDoNothingIfGivenIllegalInformation, 32);
  RUN_TEST(test_MemNextWillReturnNullIfGivenABadRoot, 46);
  RUN_TEST(test_ThatWeCanClaimAndChainAFewElementsTogether, 57);
  RUN_TEST(test_MemEndOfChain, 282);
  RUN_TEST(test_ThatCMockStopsReturningMoreDataWhenItRunsOutOfMemory, 139);
  RUN_TEST(test_ThatCMockStopsReturningMoreDataWhenAskForMoreThanItHasLeftEvenIfNotAtExactEnd, 185);
  RUN_TEST(test_ThatWeCanAskForAllSortsOfSizes, 233);

  UnityEnd();
  return 0;
}
