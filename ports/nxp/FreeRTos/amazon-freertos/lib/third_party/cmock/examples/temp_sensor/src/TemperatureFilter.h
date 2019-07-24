#ifndef _TEMPERATUREFILTER_H
#define _TEMPERATUREFILTER_H

#include "Types.h"

void TemperatureFilter_Init(void);
float TemperatureFilter_GetTemperatureInCelcius(void);
void TemperatureFilter_ProcessInput(float temperature);

#endif // _TEMPERATUREFILTER_H
