#include "unity.h"
#include "Types.h"
#include "TimerInterruptHandler.h"
#include "AT91SAM7X256.h"

AT91S_TC  TimerCounter0Peripheral;

void setUp(void)
{
}

void tearDown(void)
{
}

void testSetAndGetSystemTime(void)
{
  Timer_SetSystemTime(0);
  TEST_ASSERT_EQUAL(0, Timer_GetSystemTime());

  Timer_SetSystemTime(129837);
  TEST_ASSERT_EQUAL(129837, Timer_GetSystemTime());

  Timer_SetSystemTime(UINT32_MAX);
  TEST_ASSERT_EQUAL(UINT32_MAX, Timer_GetSystemTime());
}

void testInterruptHandlerShouldIncrementSystemTimeOnlyIfStatusHasCompareRegisterCOverflowBitSet(void)
{
  Timer_SetSystemTime(0);
  AT91C_BASE_TC0->TC_SR = 0;
  Timer_InterruptHandler();
  TEST_ASSERT_EQUAL(0, Timer_GetSystemTime());

  Timer_SetSystemTime(0);
  AT91C_BASE_TC0->TC_SR = ~AT91C_TC_CPCS;
  Timer_InterruptHandler();
  TEST_ASSERT_EQUAL(0, Timer_GetSystemTime());

  Timer_SetSystemTime(0);
  AT91C_BASE_TC0->TC_SR = AT91C_TC_CPCS;
  Timer_InterruptHandler();
  TEST_ASSERT(Timer_GetSystemTime() > 0);

  Timer_SetSystemTime(0);
  AT91C_BASE_TC0->TC_SR = 0xffffffff;
  Timer_InterruptHandler();
  TEST_ASSERT(Timer_GetSystemTime() > 0);
}

void testInterruptHandlerShouldIncrementSystemTimerBy_10(void)
{
  Timer_SetSystemTime(0);
  AT91C_BASE_TC0->TC_SR = AT91C_TC_CPCS;
  Timer_InterruptHandler();
  TEST_ASSERT_EQUAL(10, Timer_GetSystemTime());

  AT91C_BASE_TC0->TC_SR = AT91C_TC_CPCS;
  Timer_InterruptHandler();
  TEST_ASSERT_EQUAL(20, Timer_GetSystemTime());

  Timer_SetSystemTime(39426857);
  AT91C_BASE_TC0->TC_SR = AT91C_TC_CPCS;
  Timer_InterruptHandler();
  TEST_ASSERT_EQUAL(39426867, Timer_GetSystemTime());
}
