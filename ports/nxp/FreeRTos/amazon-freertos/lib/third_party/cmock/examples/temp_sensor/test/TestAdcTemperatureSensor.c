#include "unity.h"
#include "Types.h"
#include "AdcTemperatureSensor.h"
#include "AT91SAM7X256.h"

AT91S_ADC AdcPeripheral;

void setUp(void)
{
}

void tearDown(void)
{
}

void testShouldStartTemperatureSensorConversionWhenTriggered(void)
{
  AT91C_BASE_ADC->ADC_CR = 0;
  Adc_StartTemperatureSensorConversion();
  TEST_ASSERT_EQUAL(AT91C_ADC_START, AT91C_BASE_ADC->ADC_CR);
}

void testTemperatureSensorSampleReadyShouldReturnChannelConversionCompletionStatus(void)
{
  AT91C_BASE_ADC->ADC_SR = 0;
  TEST_ASSERT_EQUAL(FALSE, Adc_TemperatureSensorSampleReady());
  AT91C_BASE_ADC->ADC_SR = ~AT91C_ADC_EOC4;
  TEST_ASSERT_EQUAL(FALSE, Adc_TemperatureSensorSampleReady());
  AT91C_BASE_ADC->ADC_SR = AT91C_ADC_EOC4;
  TEST_ASSERT_EQUAL(TRUE, Adc_TemperatureSensorSampleReady());
  AT91C_BASE_ADC->ADC_SR = 0xffffffff;
  TEST_ASSERT_EQUAL(TRUE, Adc_TemperatureSensorSampleReady());
}

void testReadTemperatureSensorShouldFetchAndTranslateLatestReadingToMillivolts(void)
{
  uint16 result;

  // ADC bit weight at 10-bit resolution with 3.0V reference = 2.9296875 mV/LSB
  AT91C_BASE_ADC->ADC_CDR4 = 138;
  result = Adc_ReadTemperatureSensor();
  TEST_ASSERT_EQUAL(404, result);

  AT91C_BASE_ADC->ADC_CDR4 = 854;
  result = Adc_ReadTemperatureSensor();
  TEST_ASSERT_EQUAL(2502, result);
}
