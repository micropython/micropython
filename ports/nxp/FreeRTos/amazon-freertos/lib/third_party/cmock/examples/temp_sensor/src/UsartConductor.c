#include "Types.h"
#include "UsartConductor.h"
#include "UsartHardware.h"
#include "UsartModel.h"
#include "TaskScheduler.h"

void UsartConductor_Init(void)
{
  UsartHardware_Init(UsartModel_GetBaudRateRegisterSetting());
  UsartHardware_TransmitString(UsartModel_GetWakeupMessage());
}

void UsartConductor_Run(void)
{
  char* temp;
  if (TaskScheduler_DoUsart())
  {
    temp = UsartModel_GetFormattedTemperature();
    UsartHardware_TransmitString(temp);
  }
}
