#include "Types.h"
#include "TemperatureCalculator.h"
#include <math.h>

#ifndef logl
#define logl log
#endif

float TemperatureCalculator_Calculate(uint16 millivolts)
{
  const double supply_voltage = 3.0;
  const double series_resistance = 5000;
  const double coefficient_A = 316589.698;
  const double coefficient_B = -0.1382009;
  double sensor_voltage = ((double)millivolts / 1000);
  double resistance;
  
  if (millivolts == 0)
  {
    return -INFINITY;
  }

  // Series resistor is 5k Ohms; Reference voltage is 3.0V
  // R(t) = A * e^(B*t); R is resistance of thermisor; t is temperature in C
  resistance = ((supply_voltage * series_resistance) / sensor_voltage) - series_resistance;
  return (float)(logl(resistance / coefficient_A) / coefficient_B);
}
