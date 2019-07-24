#include "unity.h"
#include "Types.h"
#include "TemperatureCalculator.h"
#include <math.h>

void setUp(void)
{
}

void tearDown(void)
{
}

void testTemperatureCalculatorShouldCalculateTemperatureFromMillivolts(void)
{
  float result;

  // Series resistor is 5k Ohms; Reference voltage is 3.0V
  // R(t) = A * e^(B*t); R is resistance of thermisor; t is temperature in C
  result = TemperatureCalculator_Calculate(1000);
  TEST_ASSERT_FLOAT_WITHIN(0.01f, 25.0f, result);

  result = TemperatureCalculator_Calculate(2985);
  TEST_ASSERT_FLOAT_WITHIN(0.01f, 68.317f, result);

  result = TemperatureCalculator_Calculate(3);
  TEST_ASSERT_FLOAT_WITHIN(0.01f, -19.96f, result);
}

void testShouldReturnNegativeInfinityWhen_0_millivoltsInput(void)
{
  TEST_ASSERT_FLOAT_WITHIN(0.0000001f, -INFINITY, TemperatureCalculator_Calculate(0));
}
