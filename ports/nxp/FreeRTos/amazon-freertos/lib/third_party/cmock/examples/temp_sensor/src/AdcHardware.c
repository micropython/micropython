#include "Types.h"
#include "AdcHardware.h"
#include "AdcHardwareConfigurator.h"
#include "AdcTemperatureSensor.h"

void AdcHardware_Init(void)
{
  Adc_Reset();
  Adc_ConfigureMode();
  Adc_EnableTemperatureChannel();
  Adc_StartTemperatureSensorConversion();
}

void AdcHardware_StartConversion(void)
{
  Adc_StartTemperatureSensorConversion();
}

bool AdcHardware_GetSampleComplete(void)
{
  return Adc_TemperatureSensorSampleReady();
}

uint16 AdcHardware_GetSample(void)
{
  return Adc_ReadTemperatureSensor();
}
