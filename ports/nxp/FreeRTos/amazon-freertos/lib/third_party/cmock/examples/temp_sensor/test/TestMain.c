#include "unity.h"
#include "Types.h"
#include "MockExecutor.h"
#include "Main.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void testMainShouldCallExecutorInitAndContinueToCallExecutorRunUntilHalted(void)
{
  Executor_Init_Expect();
  Executor_Run_ExpectAndReturn(TRUE);
  Executor_Run_ExpectAndReturn(TRUE);
  Executor_Run_ExpectAndReturn(TRUE);
  Executor_Run_ExpectAndReturn(TRUE);
  Executor_Run_ExpectAndReturn(FALSE);
  
  AppMain();
}
