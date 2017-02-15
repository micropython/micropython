/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _HAL_TIMER_H_
#define _HAL_TIMER_H_
#include "basic_types.h"
#include "hal_platform.h"
#include "rtl8195a_timer.h"

#define GTIMER_CLK_HZ           (32768)
#define GTIMER_TICK_US          (1000000/GTIMER_CLK_HZ)

typedef enum _TIMER_MODE_ {
    FREE_RUN_MODE = 0,
    USER_DEFINED = 1
}TIMER_MODE, *PTIMER_MODE;


typedef struct _TIMER_ADAPTER_ {

    u32         TimerLoadValueUs;
    u32         TimerIrqPriority;
    TIMER_MODE  TimerMode;
    IRQ_HANDLE  IrqHandle;
    u8          TimerId;    
    u8          IrqDis;

}TIMER_ADAPTER, *PTIMER_ADAPTER;


typedef struct _HAL_TIMER_OP_ {
    u32  (*HalGetTimerId)(u32 *TimerId);
    BOOL (*HalTimerInit)(VOID *Data);
    u32  (*HalTimerReadCount)(u32 TimerId);
    VOID (*HalTimerIrqClear)(u32 TimerId);
    VOID (*HalTimerDis)(u32 TimerId);
    VOID (*HalTimerEn)(u32 TimerId);
    VOID (*HalTimerDumpReg)(u32 TimerId);
}HAL_TIMER_OP, *PHAL_TIMER_OP;

VOID HalTimerOpInit_Patch(
    IN  VOID *Data
);


//======================================================
// ROM Function prototype
_LONG_CALL_ VOID HalTimerOpInitV02(IN  VOID *Data);

#define HalTimerOpInit      HalTimerOpInit_Patch

#endif
