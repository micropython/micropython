/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _RTL8195A_TIMER_H_
#define _RTL8195A_TIMER_H_


#define TIMER_TICK_US               31

#define TIMER_LOAD_COUNT_OFF        0x00
#define TIMER_CURRENT_VAL_OFF       0x04
#define TIMER_CTL_REG_OFF           0x08
#define TIMER_EOI_OFF               0x0c
#define TIMER_INT_STATUS_OFF        0x10
#define TIMER_INTERVAL              0x14
#define TIMERS_INT_STATUS_OFF       0xa0
#define TIMERS_EOI_OFF              0xa4
#define TIMERS_RAW_INT_STATUS_OFF   0xa8
#define TIMERS_COMP_VER_OFF         0xac

#define HAL_TIMER_READ32(addr)            (*((volatile u32*)(TIMER_REG_BASE + addr)))//HAL_READ32(TIMER_REG_BASE, addr)
#define HAL_TIMER_WRITE32(addr, value)    ((*((volatile u32*)(TIMER_REG_BASE + addr))) = value)//HAL_WRITE32(TIMER_REG_BASE, addr, value)
#define HAL_TIMER_READ16(addr)            (*((volatile u16*)(TIMER_REG_BASE + addr)))//HAL_READ16(TIMER_REG_BASE, addr)
#define HAL_TIMER_WRITE16(addr, value)    ((*((volatile u16*)(TIMER_REG_BASE + addr))) = value)//HAL_WRITE16(TIMER_REG_BASE, addr, value)
#define HAL_TIMER_READ8(addr)             (*((volatile u8*)(TIMER_REG_BASE + addr)))//HAL_READ8(TIMER_REG_BASE, addr)
#define HAL_TIMER_WRITE8(addr, value)     ((*((volatile u8*)(TIMER_REG_BASE + addr))) = value)//HAL_WRITE8(TIMER_REG_BASE, addr, value)

u32
HalGetTimerIdRtl8195a(
    IN  u32     *TimerID
);

BOOL
HalTimerInitRtl8195a(
    IN  VOID    *Data
);

u32
HalTimerReadCountRtl8195a(
    IN  u32     TimerId
);

VOID
HalTimerIrqClearRtl8195a(
    IN  u32 TimerId
);

VOID
HalTimerDisRtl8195a(
    IN  u32 TimerId
);

VOID
HalTimerEnRtl8195a(
    IN  u32 TimerId
);

VOID
HalTimerDumpRegRtl8195a(
    IN  u32 TimerId
);


u32
HalTimerReadCountRamRtl8195a(
    IN  u32 TimerId
);

#endif //_RTL8195A_TIMER_H_
