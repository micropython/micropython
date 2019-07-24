#include "unity.h"
#include "Types.h"
#include "UsartModel.h"
#include "ModelConfig.h"
#include "MockTemperatureFilter.h"
#include "MockUsartBaudRateRegisterCalculator.h"
#include <math.h>

void setUp(void)
{
}

void tearDown(void)
{
}

void testGetBaudRateRegisterSettingShouldReturnAppropriateBaudRateRegisterSetting(void)
{
  uint8 dummyRegisterSetting = 17;
  UsartModel_CalculateBaudRateRegisterSetting_ExpectAndReturn(MASTER_CLOCK, USART0_BAUDRATE, dummyRegisterSetting);

  TEST_ASSERT_EQUAL(dummyRegisterSetting, UsartModel_GetBaudRateRegisterSetting());
}

void testGetFormattedTemperatureFormatsTemperatureFromCalculatorAppropriately(void)
{
  TemperatureFilter_GetTemperatureInCelcius_ExpectAndReturn(25.0f);
  TEST_ASSERT_EQUAL_STRING("25.0 C\n", UsartModel_GetFormattedTemperature());
}

void testShouldReturnErrorMessageUponInvalidTemperatureValue(void)
{
  TemperatureFilter_GetTemperatureInCelcius_ExpectAndReturn(-INFINITY);
  TEST_ASSERT_EQUAL_STRING("Temperature sensor failure!\n", UsartModel_GetFormattedTemperature());
}

void testShouldReturnWakeupMessage(void)
{
  TEST_ASSERT_EQUAL_STRING("It's Awesome Time!\n", UsartModel_GetWakeupMessage());
}
