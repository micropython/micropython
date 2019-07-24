#include "Types.h"
#include "TimerHardware.h"
#include "TimerConfigurator.h"

void TimerHardware_Init(void)
{
  Timer_EnablePeripheralClocks();
  Timer_Reset();
  Timer_ConfigureMode();
  Timer_ConfigurePeriod();
  Timer_EnableOutputPin();
  Timer_Enable();
  Timer_ConfigureInterruptHandler();
  Timer_Start();
}
