#include "Types.h"
#include "TemperatureFilter.h"
#include <math.h>

static bool initialized;
static float temperatureInCelcius;

void TemperatureFilter_Init(void)
{
  initialized = FALSE;
  temperatureInCelcius = -INFINITY;
}

float TemperatureFilter_GetTemperatureInCelcius(void)
{
  return temperatureInCelcius;
}

void TemperatureFilter_ProcessInput(float temperature)
{
  if (!initialized)
  {
    temperatureInCelcius = temperature;
    initialized = TRUE;
  }
  else
  {
    if (temperature == +INFINITY ||
        temperature == -INFINITY ||
        temperature == +NAN ||
        temperature == -NAN)
    {
      initialized = FALSE;
      temperature = -INFINITY;
    }
    
    temperatureInCelcius = (temperatureInCelcius * 0.75f) + (temperature * 0.25);
  }
}
