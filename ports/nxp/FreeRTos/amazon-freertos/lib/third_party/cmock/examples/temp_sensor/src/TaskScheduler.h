#ifndef _TASKSCHEDULER_H
#define _TASKSCHEDULER_H

#include "Types.h"

void TaskScheduler_Init(void);
void TaskScheduler_Update(uint32 time);
bool TaskScheduler_DoUsart(void);
bool TaskScheduler_DoAdc(void);

#endif // _TASKSCHEDULER_H
