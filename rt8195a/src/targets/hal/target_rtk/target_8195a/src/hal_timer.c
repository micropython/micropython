/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#include "rtl8195a.h"

VOID 
HalTimerOpInit(
    IN  VOID *Data
)
{
    PHAL_TIMER_OP pHalTimerOp = (PHAL_TIMER_OP) Data;

    pHalTimerOp->HalGetTimerId = HalGetTimerIdRtl8195a;
    pHalTimerOp->HalTimerInit = HalTimerInitRtl8195a;
    pHalTimerOp->HalTimerReadCount = HalTimerReadCountRamRtl8195a;
    pHalTimerOp->HalTimerIrqClear = HalTimerIrqClearRtl8195a;
    pHalTimerOp->HalTimerDis = HalTimerDisRtl8195a;
    pHalTimerOp->HalTimerEn = HalTimerEnRtl8195a;
    pHalTimerOp->HalTimerDumpReg = HalTimerDumpRegRtl8195a;
}

