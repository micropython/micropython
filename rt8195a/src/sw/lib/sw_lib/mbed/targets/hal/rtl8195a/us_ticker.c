/* mbed Microcontroller Library
 *******************************************************************************
 * Copyright (c) 2014, Realtek Semiconductor Corp.
 * All rights reserved.
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *******************************************************************************
 */
#include "objects.h"
#include <stddef.h>
#include "us_ticker_api.h"
#include "PeripheralNames.h"

#define SYS_TIM_ID      1   // the G-Timer ID for System
#define APP_TIM_ID      2   // the G-Timer ID for Application

static int us_ticker_inited = 0;
static TIMER_ADAPTER TimerAdapter;

extern HAL_TIMER_OP HalTimerOp;

VOID _us_ticker_irq_handler(IN  VOID *Data)
{
    us_ticker_irq_handler();
}

void us_ticker_init(void) 
{
    
    if (us_ticker_inited) return;
    us_ticker_inited = 1;

    // Initial a G-Timer
    TimerAdapter.IrqDis = 1;    // Disable Irq
    TimerAdapter.IrqHandle.IrqFun = (IRQ_FUN) _us_ticker_irq_handler;
    TimerAdapter.IrqHandle.IrqNum = TIMER2_7_IRQ;
    TimerAdapter.IrqHandle.Priority = 0x20;
    TimerAdapter.IrqHandle.Data = (u32)NULL;
    TimerAdapter.TimerId = APP_TIM_ID;
    TimerAdapter.TimerIrqPriority = 0;
    TimerAdapter.TimerLoadValueUs = 1;
    TimerAdapter.TimerMode = FREE_RUN_MODE; // Countdown Free Run

    HalTimerOp.HalTimerInit((VOID*) &TimerAdapter);

    DBG_TIMER_INFO("%s: Timer_Id=%d\n", __FUNCTION__, APP_TIM_ID);
}

uint32_t us_ticker_read() 
{
    uint32_t tick_cnt;
    uint64_t us_tick;

    //1 Our G-timer  resolution is ~31 us (1/32K), and is a countdown timer
    if (!us_ticker_inited) 
        us_ticker_init();
    tick_cnt =  HalTimerOp.HalTimerReadCount(SYS_TIM_ID);
    us_tick = TIMER_TICK_US*(0xffffffff - tick_cnt);
    // TODO: handle overflow
    return ((uint32_t)us_tick);
}

void us_ticker_set_interrupt(timestamp_t timestamp) 
{
    uint32_t tick_cnt;
    uint32_t cur_tick_cnt;
    uint32_t time_def;

    tick_cnt = (uint32_t)timestamp/TIMER_TICK_US;
    cur_tick_cnt = 0xffffffff - HalTimerOp.HalTimerReadCount(SYS_TIM_ID);
    if (tick_cnt > cur_tick_cnt) {
        time_def = tick_cnt - cur_tick_cnt;
    }
    else {
        time_def = 0xffffffff - cur_tick_cnt + tick_cnt;
    }

    TimerAdapter.IrqDis = 0;    // Enable Irq
    TimerAdapter.TimerLoadValueUs = time_def * TIMER_TICK_US;
    TimerAdapter.TimerMode = USER_DEFINED; // Countdown Free Run

    HalTimerOp.HalTimerInit((VOID*) &TimerAdapter);
}

void us_ticker_disable_interrupt(void) 
{
    HalTimerOp.HalTimerDis((u32)TimerAdapter.TimerId);
}

void us_ticker_clear_interrupt(void) 
{
    HalTimerOp.HalTimerIrqClear((u32)TimerAdapter.TimerId);
}
