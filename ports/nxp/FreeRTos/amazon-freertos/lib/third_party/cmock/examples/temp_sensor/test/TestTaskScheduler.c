#include "unity.h"
#include "Types.h"
#include "TaskScheduler.h"

void setUp(void)
{
  TaskScheduler_Init();
}

void tearDown(void)
{
}

void testShouldScheduleUsartTaskAfter1000ms(void)
{
  TEST_ASSERT_EQUAL(FALSE, TaskScheduler_DoUsart());

  TaskScheduler_Update(999);
  TEST_ASSERT_EQUAL(FALSE, TaskScheduler_DoUsart());

  TaskScheduler_Update(1000);
  TEST_ASSERT_EQUAL(TRUE, TaskScheduler_DoUsart());
}

void testShouldClearUsartDoFlagAfterReported(void)
{
  TEST_ASSERT_EQUAL(FALSE, TaskScheduler_DoUsart());
  TaskScheduler_Update(1000);
  TEST_ASSERT_EQUAL(TRUE, TaskScheduler_DoUsart());
  TEST_ASSERT_EQUAL(FALSE, TaskScheduler_DoUsart());
}

void testShouldScheduleUsartTaskEvery1000ms(void)
{
  TEST_ASSERT_EQUAL(FALSE, TaskScheduler_DoUsart());

  TaskScheduler_Update(1300);
  TEST_ASSERT_EQUAL(TRUE, TaskScheduler_DoUsart());

  TaskScheduler_Update(2000);
  TEST_ASSERT_EQUAL(TRUE, TaskScheduler_DoUsart());

  TaskScheduler_Update(3100);
  TEST_ASSERT_EQUAL(TRUE, TaskScheduler_DoUsart());
}

void testShouldScheduleUsartTaskOnlyOncePerPeriod(void)
{
  TEST_ASSERT_EQUAL(FALSE, TaskScheduler_DoUsart());
  TaskScheduler_Update(1000);
  TEST_ASSERT_EQUAL(TRUE, TaskScheduler_DoUsart());
  TaskScheduler_Update(1001);
  TEST_ASSERT_EQUAL(FALSE, TaskScheduler_DoUsart());
  TaskScheduler_Update(1999);
  TEST_ASSERT_EQUAL(FALSE, TaskScheduler_DoUsart());
  TaskScheduler_Update(2000);
  TEST_ASSERT_EQUAL(TRUE, TaskScheduler_DoUsart());
}

void testShouldScheduleAdcTaskAfter100ms(void)
{
  TEST_ASSERT_EQUAL(FALSE, TaskScheduler_DoAdc());

  TaskScheduler_Update(99);
  TEST_ASSERT_EQUAL(FALSE, TaskScheduler_DoAdc());

  TaskScheduler_Update(100);
  TEST_ASSERT_EQUAL(TRUE, TaskScheduler_DoAdc());
}

void testShouldClearAdcDoFlagAfterReported(void)
{
  TEST_ASSERT_EQUAL(FALSE, TaskScheduler_DoAdc());
  TaskScheduler_Update(100);
  TEST_ASSERT_EQUAL(TRUE, TaskScheduler_DoAdc());
  TEST_ASSERT_EQUAL(FALSE, TaskScheduler_DoAdc());
}

void testShouldScheduleAdcTaskEvery100ms(void)
{
  TEST_ASSERT_EQUAL(FALSE, TaskScheduler_DoAdc());

  TaskScheduler_Update(121);
  TEST_ASSERT_EQUAL(TRUE, TaskScheduler_DoAdc());

  TaskScheduler_Update(200);
  TEST_ASSERT_EQUAL(TRUE, TaskScheduler_DoAdc());

  TaskScheduler_Update(356);
  TEST_ASSERT_EQUAL(TRUE, TaskScheduler_DoAdc());
}

void testShouldScheduleAdcTaskOnlyOncePerPeriod(void)
{
  TEST_ASSERT_EQUAL(FALSE, TaskScheduler_DoAdc());
  TaskScheduler_Update(100);
  TEST_ASSERT_EQUAL(TRUE, TaskScheduler_DoAdc());
  TaskScheduler_Update(101);
  TEST_ASSERT_EQUAL(FALSE, TaskScheduler_DoAdc());
  TaskScheduler_Update(199);
  TEST_ASSERT_EQUAL(FALSE, TaskScheduler_DoAdc());
  TaskScheduler_Update(200);
  TEST_ASSERT_EQUAL(TRUE, TaskScheduler_DoAdc());
}
