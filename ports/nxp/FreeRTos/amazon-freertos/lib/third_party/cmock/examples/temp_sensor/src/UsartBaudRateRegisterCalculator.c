#include "Types.h"
#include "UsartBaudRateRegisterCalculator.h"

uint8 UsartModel_CalculateBaudRateRegisterSetting(uint32 masterClock, uint32 baudRate)
{
  uint32 registerSetting = ((masterClock * 10) / (baudRate * 16));

  if ((registerSetting % 10) >= 5)
  {
    registerSetting = (registerSetting / 10) + 1;
  }
  else
  {
    registerSetting /= 10;
  }

  return (uint8)registerSetting;
}
