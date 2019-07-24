#include "unity.h"
#include "Types.h"
#include "AdcHardware.h"
#include "MockAdcHardwareConfigurator.h"
#include "MockAdcTemperatureSensor.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void testInitShouldDelegateToConfiguratorAndTemperatureSensor(void)
{
  Adc_Reset_Expect();
  Adc_ConfigureMode_Expect();
  Adc_EnableTemperatureChannel_Expect();
  Adc_StartTemperatureSensorConversion_Expect();

  AdcHardware_Init();
}

void testGetSampleCompleteShouldReturn_FALSE_WhenTemperatureSensorSampleReadyReturns_FALSE(void)
{
  Adc_TemperatureSensorSampleReady_ExpectAndReturn(FALSE);
  TEST_ASSERT(!AdcHardware_GetSampleComplete());
}

void testGetSampleCompleteShouldReturn_TRUE_WhenTemperatureSensorSampleReadyReturns_TRUE(void)
{
  Adc_TemperatureSensorSampleReady_ExpectAndReturn(TRUE);
  TEST_ASSERT(AdcHardware_GetSampleComplete());
}

void testGetSampleShouldDelegateToAdcTemperatureSensor(void)
{
  uint16 sample;
  Adc_ReadTemperatureSensor_ExpectAndReturn(847);

  sample = AdcHardware_GetSample();
  TEST_ASSERT_EQUAL(847, sample);
}
