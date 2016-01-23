/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "rtl8195a.h"
#include "hal_ssi.h"

VOID HalSsiOpInit(VOID *Adaptor)
{
    PHAL_SSI_OP pHalSsiOp = (PHAL_SSI_OP) Adaptor;

    pHalSsiOp->HalSsiPinmuxEnable            = HalSsiPinmuxEnableRtl8195a;
    pHalSsiOp->HalSsiEnable                  = HalSsiEnableRtl8195a;
    pHalSsiOp->HalSsiDisable                 = HalSsiDisableRtl8195a;
    pHalSsiOp->HalSsiInit                    = HalSsiInitRtl8195a;
    pHalSsiOp->HalSsiSetSclkPolarity         = HalSsiSetSclkPolarityRtl8195a;
    pHalSsiOp->HalSsiSetSclkPhase            = HalSsiSetSclkPhaseRtl8195a;
    pHalSsiOp->HalSsiWrite                   = HalSsiWriteRtl8195a;
    pHalSsiOp->HalSsiRead                    = HalSsiReadRtl8195a;
    pHalSsiOp->HalSsiGetRxFifoLevel          = HalSsiGetRxFifoLevelRtl8195a;
    pHalSsiOp->HalSsiGetTxFifoLevel          = HalSsiGetTxFifoLevelRtl8195a;
    pHalSsiOp->HalSsiGetStatus               = HalSsiGetStatusRtl8195a;
    pHalSsiOp->HalSsiGetInterruptStatus      = HalSsiGetInterruptStatusRtl8195a;
    pHalSsiOp->HalSsiLoadSetting             = HalSsiLoadSettingRtl8195a;
    pHalSsiOp->HalSsiSetInterruptMask        = HalSsiSetInterruptMaskRtl8195a;
    pHalSsiOp->HalSsiGetInterruptMask        = HalSsiGetInterruptMaskRtl8195a;
    pHalSsiOp->HalSsiSetDeviceRole           = HalSsiSetDeviceRoleRtl8195a;
    pHalSsiOp->HalSsiWriteable               = HalSsiWriteableRtl8195a;
    pHalSsiOp->HalSsiReadable                = HalSsiReadableRtl8195a;
    pHalSsiOp->HalSsiBusy                    = HalSsiBusyRtl8195a;
    pHalSsiOp->HalSsiInterruptEnable         = HalSsiInterruptEnableRtl8195a;
    pHalSsiOp->HalSsiInterruptDisable        = HalSsiInterruptDisableRtl8195a;
    pHalSsiOp->HalSsiReadInterrupt           = HalSsiReadInterruptRtl8195a;
    pHalSsiOp->HalSsiSetRxFifoThresholdLevel = HalSsiSetRxFifoThresholdLevelRtl8195a;
    pHalSsiOp->HalSsiSetTxFifoThresholdLevel = HalSsiSetTxFifoThresholdLevelRtl8195a;
    pHalSsiOp->HalSsiWriteInterrupt          = HalSsiWriteInterruptRtl8195a;
    pHalSsiOp->HalSsiGetRawInterruptStatus   = HalSsiGetRawInterruptStatusRtl8195a;
    pHalSsiOp->HalSsiGetSlaveEnableRegister  = HalSsiGetSlaveEnableRegisterRtl8195a;
    pHalSsiOp->HalSsiSetSlaveEnableRegister  = HalSsiSetSlaveEnableRegisterRtl8195a;
}
