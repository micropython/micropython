#include "Types.h"
#include "AdcTemperatureSensor.h"

static inline uint32 ConvertAdcCountsToPicovolts(uint32 counts); 
static inline uint16 ConvertPicovoltsToMillivolts(uint32 picovolts);

//
// PUBLIC METHODS
//

void Adc_StartTemperatureSensorConversion(void)
{
  AT91C_BASE_ADC->ADC_CR = AT91C_ADC_START;
}

bool Adc_TemperatureSensorSampleReady(void)
{
  return ((AT91C_BASE_ADC->ADC_SR & AT91C_ADC_EOC4) == AT91C_ADC_EOC4);
}

uint16 Adc_ReadTemperatureSensor(void)
{
  uint32 picovolts = ConvertAdcCountsToPicovolts(AT91C_BASE_ADC->ADC_CDR4);
  return ConvertPicovoltsToMillivolts(picovolts);
}

//
// PRIVATE HELPERS
//

static inline uint32 ConvertAdcCountsToPicovolts(uint32 counts)
{
  // ADC bit weight at 10-bit resolution with 3.0V reference = 2.9296875 mV/LSB
  uint32 picovoltsPerAdcCount = 2929688;

  // Shift decimal point by 6 places to preserve accuracy in fixed-point math
  return counts * picovoltsPerAdcCount;
}

static inline uint16 ConvertPicovoltsToMillivolts(uint32 picovolts)
{
  const uint32 halfMillivoltInPicovolts = 500000;
  const uint32 picovoltsPerMillivolt = 1000000;
    
  // Add 0.5 mV to result so that truncation yields properly rounded result
  picovolts += halfMillivoltInPicovolts;

  // Divide appropriately to convert to millivolts
  return (uint16)(picovolts / picovoltsPerMillivolt);
}

