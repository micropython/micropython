#include "unity.h"
#include "Types.h"
#include "TimerModel.h"
#include "MockTaskScheduler.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void testUpdateTimeShouldDelegateToTaskScheduler(void)
{
  TaskScheduler_Update_Expect(19387L);
  TimerModel_UpdateTime(19387L);
}
