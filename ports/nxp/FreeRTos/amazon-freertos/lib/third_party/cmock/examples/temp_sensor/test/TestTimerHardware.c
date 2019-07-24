#include "unity.h"
#include "Types.h"
#include "TimerHardware.h"
#include "MockTimerConfigurator.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void testInitShouldDelegateAppropriatelyToConfigurator(void)
{
  Timer_EnablePeripheralClocks_Expect();
  Timer_Reset_Expect();
  Timer_ConfigureMode_Expect();
  Timer_ConfigurePeriod_Expect();
  Timer_EnableOutputPin_Expect();
  Timer_Enable_Expect();
  Timer_ConfigureInterruptHandler_Expect();
  Timer_Start_Expect();

  TimerHardware_Init();
}
