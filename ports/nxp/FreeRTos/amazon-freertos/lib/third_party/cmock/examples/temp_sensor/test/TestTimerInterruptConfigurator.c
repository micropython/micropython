#include "unity.h"
#include "Types.h"
#include "TimerInterruptConfigurator.h"
#include "MockTimerInterruptHandler.h"
#include "AT91SAM7X256.h"

AT91S_AIC AicPeripheral;
AT91S_TC  TimerCounter0Peripheral;

void setUp(void)
{
}

void tearDown(void)
{
}

void test_TIMER0_ID_MASK_ShouldBeCorrect(void)
{
  TEST_ASSERT_EQUAL(((uint32)0x1) << AT91C_ID_TC0, TIMER0_ID_MASK);
}

void testDisableInterruptDisablesTimer0InterruptInTheInterruptController(void)
{
  AT91C_BASE_AIC->AIC_IDCR = 0;
  Timer_DisableInterrupt();
  TEST_ASSERT_EQUAL(TIMER0_ID_MASK, AT91C_BASE_AIC->AIC_IDCR);
}

void testResetSystemTimeDelegatesTo_Timer_SetSystemTime_Appropriately(void)
{
  Timer_SetSystemTime_Expect(0);
  Timer_ResetSystemTime();
}

void testConfigureInterruptShouldSetInterruptHandlerAppropriately(void)
{
  AT91C_BASE_AIC->AIC_SVR[AT91C_ID_TC0] = (uint32)NULL;
  Timer_ConfigureInterrupt();
  TEST_ASSERT_EQUAL((uint32)Timer_InterruptHandler, AT91C_BASE_AIC->AIC_SVR[AT91C_ID_TC0]);
}

void testConfigureInterruptShouldSetInterruptLevelInSourceModeRegisterAppropriately(void)
{
  AT91C_BASE_AIC->AIC_SMR[AT91C_ID_TC0] = 0;
  Timer_ConfigureInterrupt();
  TEST_ASSERT_EQUAL(
      AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL, 
      AT91C_BASE_AIC->AIC_SMR[AT91C_ID_TC0] & 0x00000060);
}

void testConfigureInterruptShouldSetInterruptPriorityInSourceModeRegisterAppropriately(void)
{
  AT91C_BASE_AIC->AIC_SMR[AT91C_ID_TC0] = 0;
  Timer_ConfigureInterrupt();
  TEST_ASSERT_EQUAL(1, AT91C_BASE_AIC->AIC_SMR[AT91C_ID_TC0] & 0x00000007);
}

void testConfigureInterruptShouldClearTimer0InterruptOnTheInterruptController(void)
{
  AT91C_BASE_AIC->AIC_ICCR = 0;
  Timer_ConfigureInterrupt();
  TEST_ASSERT_EQUAL(TIMER0_ID_MASK, AT91C_BASE_AIC->AIC_ICCR);
}

void testConfigureInterruptShouldEnableCompareInterruptForRegisterC(void)
{
  AT91C_BASE_TC0->TC_IER = 0;
  Timer_ConfigureInterrupt();
  TEST_ASSERT_EQUAL(AT91C_TC_CPCS, AT91C_BASE_TC0->TC_IER);
}

void testEnableInterruptShouldEnableTimer0InterruptsInInterruptCotroller(void)
{
  AT91C_BASE_AIC->AIC_IECR = 0;
  Timer_EnableInterrupt();
  TEST_ASSERT_EQUAL(TIMER0_ID_MASK, AT91C_BASE_AIC->AIC_IECR);
}
