#ifndef _ADCMODEL_H
#define _ADCMODEL_H

#include "Types.h"

bool AdcModel_DoGetSample(void);
void AdcModel_ProcessInput(uint16 millivolts);

bool AdcModel_DoNothingExceptTestASpecialType(EXAMPLE_STRUCT_T ExampleStruct);
bool AdModel_DoNothingExceptTestPointers(uint32* pExample);
EXAMPLE_STRUCT_T AdcModel_DoNothingExceptReturnASpecialType(void);

#endif // _ADCMODEL_H
