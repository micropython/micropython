#include "unity.h"
#include "Types.h"
#include "AdcHardwareConfigurator.h"
#include "AT91SAM7X256.h"
#include "ModelConfig.h"

AT91S_ADC AdcPeripheral;

void setUp(void)
{

}

void tearDown(void)
{
}

void testResetShouldResetTheAdcConverterPeripheral(void)
{
  AT91C_BASE_ADC->ADC_CR = 0;
  Adc_Reset();
  TEST_ASSERT_EQUAL(AT91C_ADC_SWRST, AT91C_BASE_ADC->ADC_CR);
}

void testConfigureModeShouldSetAdcModeRegisterAppropriately(void)
{
  uint32 prescaler = (MASTER_CLOCK / (2 * 2000000)) - 1; // 5MHz ADC clock

  AT91C_BASE_ADC->ADC_MR = 0;

  Adc_ConfigureMode();

  TEST_ASSERT_EQUAL(prescaler, (AT91C_BASE_ADC->ADC_MR & AT91C_ADC_PRESCAL) >> 8);
}

void testEnableTemperatureChannelShouldEnableTheAppropriateAdcInput(void)
{
  AT91C_BASE_ADC->ADC_CHER = 0;

  Adc_EnableTemperatureChannel();

  TEST_ASSERT_EQUAL(0x1 << 4, AT91C_BASE_ADC->ADC_CHER);
}
