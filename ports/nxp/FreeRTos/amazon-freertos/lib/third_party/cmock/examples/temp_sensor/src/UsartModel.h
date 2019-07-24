#ifndef _USARTMODEL_H
#define _USARTMODEL_H

#include "Types.h"

uint8 UsartModel_GetBaudRateRegisterSetting(void);
char* UsartModel_GetFormattedTemperature(void);
char* UsartModel_GetWakeupMessage(void);

#endif // _USARTMODEL_H
