#include "Types.h"
#include "TimerModel.h"
#include "TaskScheduler.h"

void TimerModel_UpdateTime(uint32 systemTime)
{
  TaskScheduler_Update(systemTime);
}

