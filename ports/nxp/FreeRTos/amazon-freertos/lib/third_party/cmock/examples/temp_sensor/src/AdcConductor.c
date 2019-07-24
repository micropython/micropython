#include "Types.h"
#include "AdcConductor.h"
#include "AdcModel.h"
#include "AdcHardware.h"

void AdcConductor_Init(void)
{
  AdcHardware_Init();
}

void AdcConductor_Run(void)
{
  if (AdcModel_DoGetSample() && AdcHardware_GetSampleComplete())
  {
    AdcModel_ProcessInput(AdcHardware_GetSample());
    AdcHardware_StartConversion();
  }
}

bool AdcConductor_JustHereToTest(void)
{
    EXAMPLE_STRUCT_T ExampleStruct;
    ExampleStruct.x = 5;
    ExampleStruct.y = 7;

    return AdcModel_DoNothingExceptTestASpecialType(ExampleStruct);
}

bool AdcConductor_AlsoHereToTest(void)
{
    EXAMPLE_STRUCT_T example = AdcModel_DoNothingExceptReturnASpecialType();

    return ((example.x == 99) && (example.y == 1));
}

bool AdcConductor_YetAnotherTest(void)
{
    uint32 example = 3;

    return AdModel_DoNothingExceptTestPointers(&example);
}

