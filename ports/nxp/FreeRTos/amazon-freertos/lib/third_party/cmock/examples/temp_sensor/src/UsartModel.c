#include "Types.h"
#include "UsartModel.h"
#include "ModelConfig.h"
#include "UsartBaudRateRegisterCalculator.h"
#include "TemperatureFilter.h"
#include <stdio.h>
#include <math.h>

char formattedTemperature[32];
char* wakeup = "It's Awesome Time!\n";

uint8 UsartModel_GetBaudRateRegisterSetting(void)
{
  return UsartModel_CalculateBaudRateRegisterSetting(MASTER_CLOCK, USART0_BAUDRATE);
}

char* UsartModel_GetFormattedTemperature(void)
{
  float temperature = TemperatureFilter_GetTemperatureInCelcius();
  if (temperature == -INFINITY)
  {
    sprintf(formattedTemperature, "%s", "Temperature sensor failure!\n");
  }
  else
  {
    sprintf(formattedTemperature, "%.1f C\n", temperature);
  }
  return formattedTemperature;
}

char* UsartModel_GetWakeupMessage(void)
{
  return wakeup;
}
