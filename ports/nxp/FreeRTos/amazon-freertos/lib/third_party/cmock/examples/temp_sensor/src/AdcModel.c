#include "AdcModel.h"
#include "TaskScheduler.h"
#include "TemperatureCalculator.h"
#include "TemperatureFilter.h"

bool AdcModel_DoGetSample(void)
{
  return TaskScheduler_DoAdc();
}

void AdcModel_ProcessInput(uint16 millivolts)
{
  TemperatureFilter_ProcessInput(TemperatureCalculator_Calculate(millivolts));
}

bool AdcModel_DoNothingExceptTestASpecialType(EXAMPLE_STRUCT_T ExampleStruct)
{
    //This doesn't really do anything. it's only here to make sure I can compare a struct.
    return FALSE;
}
bool AdModel_DoNothingExceptTestPointers(uint32* pExample)
{
    //This doesn't really do anything. it's only here to make sure I can compare a pointer value.
    return FALSE;
}

EXAMPLE_STRUCT_T AdcModel_DoNothingExceptReturnASpecialType(void)
{
    EXAMPLE_STRUCT_T example; //again, this just is here to test that I can return a struct
    example.x = 99;
    example.y = 1;
    return example;
}
