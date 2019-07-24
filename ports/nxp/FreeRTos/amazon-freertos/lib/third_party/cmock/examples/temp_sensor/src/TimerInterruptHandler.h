#ifndef _TIMERINTERRUPTHANDLER_H
#define _TIMERINTERRUPTHANDLER_H

#include "Types.h"

void Timer_SetSystemTime(uint32 time);
uint32 Timer_GetSystemTime(void);
void Timer_InterruptHandler(void);

#endif // _TIMERINTERRUPTHANDLER_H
