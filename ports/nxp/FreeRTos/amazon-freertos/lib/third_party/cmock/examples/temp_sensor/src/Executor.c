#include "Types.h"
#include "Executor.h"
#include "Model.h"
#include "UsartConductor.h"
#include "TimerConductor.h"
#include "AdcConductor.h"
#include "IntrinsicsWrapper.h"


void Executor_Init(void)
{
  Model_Init();
  UsartConductor_Init();
  AdcConductor_Init();
  TimerConductor_Init();
  Interrupt_Enable();
}

bool Executor_Run(void)
{
  UsartConductor_Run();
  TimerConductor_Run();
  AdcConductor_Run();
  return TRUE;
}
