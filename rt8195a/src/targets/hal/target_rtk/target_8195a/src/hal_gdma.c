/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#include "rtl8195a.h"
#include "hal_gdma.h"

VOID HalGdmaOpInit(
    IN  VOID *Data
)
{
    PHAL_GDMA_OP pHalGdmaOp = (PHAL_GDMA_OP) Data;

    pHalGdmaOp->HalGdmaOnOff = HalGdmaOnOffRtl8195a;
    pHalGdmaOp->HalGdamChInit = HalGdamChInitRtl8195a;
    pHalGdmaOp->HalGdmaChDis = HalGdmaChDisRtl8195a;
    pHalGdmaOp->HalGdmaChEn = HalGdmaChEnRtl8195a;
    pHalGdmaOp->HalGdmaChSeting = HalGdmaChSetingRtl8195a;
    pHalGdmaOp->HalGdmaChBlockSeting = HalGdmaChBlockSetingRtl8195a;
    pHalGdmaOp->HalGdmaChIsrEnAndDis = HalGdmaChIsrEnAndDisRtl8195a;
    pHalGdmaOp->HalGdmaChIsrClean = HalGdmaChIsrCleanRtl8195a;
    pHalGdmaOp->HalGdmaChCleanAutoSrc = HalGdmaChCleanAutoSrcRtl8195a;
    pHalGdmaOp->HalGdmaChCleanAutoDst = HalGdmaChCleanAutoDstRtl8195a;
}
