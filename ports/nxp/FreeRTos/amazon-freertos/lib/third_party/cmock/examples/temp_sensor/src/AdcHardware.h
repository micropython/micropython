#ifndef _ADCHARDWARE_H
#define _ADCHARDWARE_H

void AdcHardware_Init(void);
void AdcHardware_StartConversion(void);
bool AdcHardware_GetSampleComplete(void);
uint16 AdcHardware_GetSample(void);

#endif // _ADCHARDWARE_H
