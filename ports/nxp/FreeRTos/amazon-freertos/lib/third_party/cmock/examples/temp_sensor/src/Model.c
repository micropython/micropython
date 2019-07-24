#include "Model.h"
#include "TaskScheduler.h"
#include "TemperatureFilter.h"

void Model_Init(void)
{
  TaskScheduler_Init();
  TemperatureFilter_Init();
}

