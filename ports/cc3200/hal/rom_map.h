//*****************************************************************************
//
//  rom_map.h
//
//  Macros to facilitate calling functions in the ROM when they are
//  available and in flash otherwise.
//
//  Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
//
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//    Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the
//    distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

//*****************************************************************************
//
// THIS IS AN AUTO-GENERATED FILE.  DO NOT EDIT BY HAND.
//
//*****************************************************************************

#ifndef __ROM_MAP_H__
#define __ROM_MAP_H__
#ifndef DEBUG
#include "rom.h"
#endif
#include "rom_patch.h"

//*****************************************************************************
//
// Macros for the Interrupt API.
//
//*****************************************************************************
#ifdef ROM_IntEnable
#define MAP_IntEnable \
        ROM_IntEnable
#else
#define MAP_IntEnable \
        IntEnable
#endif
#ifdef ROM_IntMasterEnable
#define MAP_IntMasterEnable \
        ROM_IntMasterEnable
#else
#define MAP_IntMasterEnable \
        IntMasterEnable
#endif
#ifdef ROM_IntMasterDisable
#define MAP_IntMasterDisable \
        ROM_IntMasterDisable
#else
#define MAP_IntMasterDisable \
        IntMasterDisable
#endif
#ifdef ROM_IntDisable
#define MAP_IntDisable \
        ROM_IntDisable
#else
#define MAP_IntDisable \
        IntDisable
#endif
#ifdef ROM_IntPriorityGroupingSet
#define MAP_IntPriorityGroupingSet \
        ROM_IntPriorityGroupingSet
#else
#define MAP_IntPriorityGroupingSet \
        IntPriorityGroupingSet
#endif
#ifdef ROM_IntPriorityGroupingGet
#define MAP_IntPriorityGroupingGet \
        ROM_IntPriorityGroupingGet
#else
#define MAP_IntPriorityGroupingGet \
        IntPriorityGroupingGet
#endif
#ifdef ROM_IntPrioritySet
#define MAP_IntPrioritySet \
        ROM_IntPrioritySet
#else
#define MAP_IntPrioritySet \
        IntPrioritySet
#endif
#ifdef ROM_IntPriorityGet
#define MAP_IntPriorityGet \
        ROM_IntPriorityGet
#else
#define MAP_IntPriorityGet \
        IntPriorityGet
#endif
#ifdef ROM_IntPendSet
#define MAP_IntPendSet \
        ROM_IntPendSet
#else
#define MAP_IntPendSet \
        IntPendSet
#endif
#ifdef ROM_IntPendClear
#define MAP_IntPendClear \
        ROM_IntPendClear
#else
#define MAP_IntPendClear \
        IntPendClear
#endif
#ifdef ROM_IntPriorityMaskSet
#define MAP_IntPriorityMaskSet \
        ROM_IntPriorityMaskSet
#else
#define MAP_IntPriorityMaskSet \
        IntPriorityMaskSet
#endif
#ifdef ROM_IntPriorityMaskGet
#define MAP_IntPriorityMaskGet \
        ROM_IntPriorityMaskGet
#else
#define MAP_IntPriorityMaskGet \
        IntPriorityMaskGet
#endif
#ifdef ROM_IntRegister
#define MAP_IntRegister \
        ROM_IntRegister
#else
#define MAP_IntRegister \
        IntRegister
#endif
#ifdef ROM_IntUnregister
#define MAP_IntUnregister \
        ROM_IntUnregister
#else
#define MAP_IntUnregister \
        IntUnregister
#endif
#ifdef ROM_IntVTableBaseSet
#define MAP_IntVTableBaseSet \
        ROM_IntVTableBaseSet
#else
#define MAP_IntVTableBaseSet \
        IntVTableBaseSet
#endif

//*****************************************************************************
//
// Macros for the Timer API.
//
//*****************************************************************************
#ifdef ROM_TimerEnable
#define MAP_TimerEnable \
        ROM_TimerEnable
#else
#define MAP_TimerEnable \
        TimerEnable
#endif
#ifdef ROM_TimerDisable
#define MAP_TimerDisable \
        ROM_TimerDisable
#else
#define MAP_TimerDisable \
        TimerDisable
#endif
#ifdef ROM_TimerConfigure
#define MAP_TimerConfigure \
        ROM_TimerConfigure
#else
#define MAP_TimerConfigure \
        TimerConfigure
#endif
#ifdef ROM_TimerControlLevel
#define MAP_TimerControlLevel \
        ROM_TimerControlLevel
#else
#define MAP_TimerControlLevel \
        TimerControlLevel
#endif
#ifdef ROM_TimerControlEvent
#define MAP_TimerControlEvent \
        ROM_TimerControlEvent
#else
#define MAP_TimerControlEvent \
        TimerControlEvent
#endif
#ifdef ROM_TimerControlStall
#define MAP_TimerControlStall \
        ROM_TimerControlStall
#else
#define MAP_TimerControlStall \
        TimerControlStall
#endif
#ifdef ROM_TimerPrescaleSet
#define MAP_TimerPrescaleSet \
        ROM_TimerPrescaleSet
#else
#define MAP_TimerPrescaleSet \
        TimerPrescaleSet
#endif
#ifdef ROM_TimerPrescaleGet
#define MAP_TimerPrescaleGet \
        ROM_TimerPrescaleGet
#else
#define MAP_TimerPrescaleGet \
        TimerPrescaleGet
#endif
#ifdef ROM_TimerPrescaleMatchSet
#define MAP_TimerPrescaleMatchSet \
        ROM_TimerPrescaleMatchSet
#else
#define MAP_TimerPrescaleMatchSet \
        TimerPrescaleMatchSet
#endif
#ifdef ROM_TimerPrescaleMatchGet
#define MAP_TimerPrescaleMatchGet \
        ROM_TimerPrescaleMatchGet
#else
#define MAP_TimerPrescaleMatchGet \
        TimerPrescaleMatchGet
#endif
#ifdef ROM_TimerLoadSet
#define MAP_TimerLoadSet \
        ROM_TimerLoadSet
#else
#define MAP_TimerLoadSet \
        TimerLoadSet
#endif
#ifdef ROM_TimerLoadGet
#define MAP_TimerLoadGet \
        ROM_TimerLoadGet
#else
#define MAP_TimerLoadGet \
        TimerLoadGet
#endif
#ifdef ROM_TimerValueGet
#define MAP_TimerValueGet \
        ROM_TimerValueGet
#else
#define MAP_TimerValueGet \
        TimerValueGet
#endif
#ifdef ROM_TimerMatchSet
#define MAP_TimerMatchSet \
        ROM_TimerMatchSet
#else
#define MAP_TimerMatchSet \
        TimerMatchSet
#endif
#ifdef ROM_TimerMatchGet
#define MAP_TimerMatchGet \
        ROM_TimerMatchGet
#else
#define MAP_TimerMatchGet \
        TimerMatchGet
#endif
#ifdef ROM_TimerIntRegister
#define MAP_TimerIntRegister \
        ROM_TimerIntRegister
#else
#define MAP_TimerIntRegister \
        TimerIntRegister
#endif
#ifdef ROM_TimerIntUnregister
#define MAP_TimerIntUnregister \
        ROM_TimerIntUnregister
#else
#define MAP_TimerIntUnregister \
        TimerIntUnregister
#endif
#ifdef ROM_TimerIntEnable
#define MAP_TimerIntEnable \
        ROM_TimerIntEnable
#else
#define MAP_TimerIntEnable \
        TimerIntEnable
#endif
#ifdef ROM_TimerIntDisable
#define MAP_TimerIntDisable \
        ROM_TimerIntDisable
#else
#define MAP_TimerIntDisable \
        TimerIntDisable
#endif
#ifdef ROM_TimerIntStatus
#define MAP_TimerIntStatus \
        ROM_TimerIntStatus
#else
#define MAP_TimerIntStatus \
        TimerIntStatus
#endif
#ifdef ROM_TimerIntClear
#define MAP_TimerIntClear \
        ROM_TimerIntClear
#else
#define MAP_TimerIntClear \
        TimerIntClear
#endif
#ifdef ROM_TimerDMAEventSet
#define MAP_TimerDMAEventSet \
        ROM_TimerDMAEventSet
#else
#define MAP_TimerDMAEventSet \
        TimerDMAEventSet
#endif
#ifdef ROM_TimerDMAEventGet
#define MAP_TimerDMAEventGet \
        ROM_TimerDMAEventGet
#else
#define MAP_TimerDMAEventGet \
        TimerDMAEventGet
#endif

//*****************************************************************************
//
// Macros for the UART API.
//
//*****************************************************************************
#ifdef ROM_UARTParityModeSet
#define MAP_UARTParityModeSet \
        ROM_UARTParityModeSet
#else
#define MAP_UARTParityModeSet \
        UARTParityModeSet
#endif
#ifdef ROM_UARTParityModeGet
#define MAP_UARTParityModeGet \
        ROM_UARTParityModeGet
#else
#define MAP_UARTParityModeGet \
        UARTParityModeGet
#endif
#ifdef ROM_UARTFIFOLevelSet
#define MAP_UARTFIFOLevelSet \
        ROM_UARTFIFOLevelSet
#else
#define MAP_UARTFIFOLevelSet \
        UARTFIFOLevelSet
#endif
#ifdef ROM_UARTFIFOLevelGet
#define MAP_UARTFIFOLevelGet \
        ROM_UARTFIFOLevelGet
#else
#define MAP_UARTFIFOLevelGet \
        UARTFIFOLevelGet
#endif
#ifdef ROM_UARTConfigSetExpClk
#define MAP_UARTConfigSetExpClk \
        ROM_UARTConfigSetExpClk
#else
#define MAP_UARTConfigSetExpClk \
        UARTConfigSetExpClk
#endif
#ifdef ROM_UARTConfigGetExpClk
#define MAP_UARTConfigGetExpClk \
        ROM_UARTConfigGetExpClk
#else
#define MAP_UARTConfigGetExpClk \
        UARTConfigGetExpClk
#endif
#ifdef ROM_UARTEnable
#define MAP_UARTEnable \
        ROM_UARTEnable
#else
#define MAP_UARTEnable \
        UARTEnable
#endif
#ifdef ROM_UARTDisable
#define MAP_UARTDisable \
        ROM_UARTDisable
#else
#define MAP_UARTDisable \
        UARTDisable
#endif
#ifdef ROM_UARTFIFOEnable
#define MAP_UARTFIFOEnable \
        ROM_UARTFIFOEnable
#else
#define MAP_UARTFIFOEnable \
        UARTFIFOEnable
#endif
#ifdef ROM_UARTFIFODisable
#define MAP_UARTFIFODisable \
        ROM_UARTFIFODisable
#else
#define MAP_UARTFIFODisable \
        UARTFIFODisable
#endif
#ifdef ROM_UARTCharsAvail
#define MAP_UARTCharsAvail \
        ROM_UARTCharsAvail
#else
#define MAP_UARTCharsAvail \
        UARTCharsAvail
#endif
#ifdef ROM_UARTSpaceAvail
#define MAP_UARTSpaceAvail \
        ROM_UARTSpaceAvail
#else
#define MAP_UARTSpaceAvail \
        UARTSpaceAvail
#endif
#ifdef ROM_UARTCharGetNonBlocking
#define MAP_UARTCharGetNonBlocking \
        ROM_UARTCharGetNonBlocking
#else
#define MAP_UARTCharGetNonBlocking \
        UARTCharGetNonBlocking
#endif
#ifdef ROM_UARTCharGet
#define MAP_UARTCharGet \
        ROM_UARTCharGet
#else
#define MAP_UARTCharGet \
        UARTCharGet
#endif
#ifdef ROM_UARTCharPutNonBlocking
#define MAP_UARTCharPutNonBlocking \
        ROM_UARTCharPutNonBlocking
#else
#define MAP_UARTCharPutNonBlocking \
        UARTCharPutNonBlocking
#endif
#ifdef ROM_UARTCharPut
#define MAP_UARTCharPut \
        ROM_UARTCharPut
#else
#define MAP_UARTCharPut \
        UARTCharPut
#endif
#ifdef ROM_UARTBreakCtl
#define MAP_UARTBreakCtl \
        ROM_UARTBreakCtl
#else
#define MAP_UARTBreakCtl \
        UARTBreakCtl
#endif
#ifdef ROM_UARTBusy
#define MAP_UARTBusy \
        ROM_UARTBusy
#else
#define MAP_UARTBusy \
        UARTBusy
#endif
#ifdef ROM_UARTIntRegister
#define MAP_UARTIntRegister \
        ROM_UARTIntRegister
#else
#define MAP_UARTIntRegister \
        UARTIntRegister
#endif
#ifdef ROM_UARTIntUnregister
#define MAP_UARTIntUnregister \
        ROM_UARTIntUnregister
#else
#define MAP_UARTIntUnregister \
        UARTIntUnregister
#endif
#ifdef ROM_UARTIntEnable
#define MAP_UARTIntEnable \
        ROM_UARTIntEnable
#else
#define MAP_UARTIntEnable \
        UARTIntEnable
#endif
#ifdef ROM_UARTIntDisable
#define MAP_UARTIntDisable \
        ROM_UARTIntDisable
#else
#define MAP_UARTIntDisable \
        UARTIntDisable
#endif
#ifdef ROM_UARTIntStatus
#define MAP_UARTIntStatus \
        ROM_UARTIntStatus
#else
#define MAP_UARTIntStatus \
        UARTIntStatus
#endif
#ifdef ROM_UARTIntClear
#define MAP_UARTIntClear \
        ROM_UARTIntClear
#else
#define MAP_UARTIntClear \
        UARTIntClear
#endif
#ifdef ROM_UARTDMAEnable
#define MAP_UARTDMAEnable \
        ROM_UARTDMAEnable
#else
#define MAP_UARTDMAEnable \
        UARTDMAEnable
#endif
#ifdef ROM_UARTDMADisable
#define MAP_UARTDMADisable \
        ROM_UARTDMADisable
#else
#define MAP_UARTDMADisable \
        UARTDMADisable
#endif
#ifdef ROM_UARTRxErrorGet
#define MAP_UARTRxErrorGet \
        ROM_UARTRxErrorGet
#else
#define MAP_UARTRxErrorGet \
        UARTRxErrorGet
#endif
#ifdef ROM_UARTRxErrorClear
#define MAP_UARTRxErrorClear \
        ROM_UARTRxErrorClear
#else
#define MAP_UARTRxErrorClear \
        UARTRxErrorClear
#endif
#ifdef ROM_UARTModemControlSet
#define MAP_UARTModemControlSet \
        ROM_UARTModemControlSet
#else
#define MAP_UARTModemControlSet \
        UARTModemControlSet
#endif
#ifdef ROM_UARTModemControlClear
#define MAP_UARTModemControlClear \
        ROM_UARTModemControlClear
#else
#define MAP_UARTModemControlClear \
        UARTModemControlClear
#endif
#ifdef ROM_UARTModemControlGet
#define MAP_UARTModemControlGet \
        ROM_UARTModemControlGet
#else
#define MAP_UARTModemControlGet \
        UARTModemControlGet
#endif
#ifdef ROM_UARTModemStatusGet
#define MAP_UARTModemStatusGet \
        ROM_UARTModemStatusGet
#else
#define MAP_UARTModemStatusGet \
        UARTModemStatusGet
#endif
#ifdef ROM_UARTFlowControlSet
#define MAP_UARTFlowControlSet \
        ROM_UARTFlowControlSet
#else
#define MAP_UARTFlowControlSet \
        UARTFlowControlSet
#endif
#ifdef ROM_UARTFlowControlGet
#define MAP_UARTFlowControlGet \
        ROM_UARTFlowControlGet
#else
#define MAP_UARTFlowControlGet \
        UARTFlowControlGet
#endif
#ifdef ROM_UARTTxIntModeSet
#define MAP_UARTTxIntModeSet \
        ROM_UARTTxIntModeSet
#else
#define MAP_UARTTxIntModeSet \
        UARTTxIntModeSet
#endif
#ifdef ROM_UARTTxIntModeGet
#define MAP_UARTTxIntModeGet \
        ROM_UARTTxIntModeGet
#else
#define MAP_UARTTxIntModeGet \
        UARTTxIntModeGet
#endif

//*****************************************************************************
//
// Macros for the uDMA API.
//
//*****************************************************************************
#ifdef ROM_uDMAChannelTransferSet
#define MAP_uDMAChannelTransferSet \
        ROM_uDMAChannelTransferSet
#else
#define MAP_uDMAChannelTransferSet \
        uDMAChannelTransferSet
#endif
#ifdef ROM_uDMAEnable
#define MAP_uDMAEnable \
        ROM_uDMAEnable
#else
#define MAP_uDMAEnable \
        uDMAEnable
#endif
#ifdef ROM_uDMADisable
#define MAP_uDMADisable \
        ROM_uDMADisable
#else
#define MAP_uDMADisable \
        uDMADisable
#endif
#ifdef ROM_uDMAErrorStatusGet
#define MAP_uDMAErrorStatusGet \
        ROM_uDMAErrorStatusGet
#else
#define MAP_uDMAErrorStatusGet \
        uDMAErrorStatusGet
#endif
#ifdef ROM_uDMAErrorStatusClear
#define MAP_uDMAErrorStatusClear \
        ROM_uDMAErrorStatusClear
#else
#define MAP_uDMAErrorStatusClear \
        uDMAErrorStatusClear
#endif
#ifdef ROM_uDMAChannelEnable
#define MAP_uDMAChannelEnable \
        ROM_uDMAChannelEnable
#else
#define MAP_uDMAChannelEnable \
        uDMAChannelEnable
#endif
#ifdef ROM_uDMAChannelDisable
#define MAP_uDMAChannelDisable \
        ROM_uDMAChannelDisable
#else
#define MAP_uDMAChannelDisable \
        uDMAChannelDisable
#endif
#ifdef ROM_uDMAChannelIsEnabled
#define MAP_uDMAChannelIsEnabled \
        ROM_uDMAChannelIsEnabled
#else
#define MAP_uDMAChannelIsEnabled \
        uDMAChannelIsEnabled
#endif
#ifdef ROM_uDMAControlBaseSet
#define MAP_uDMAControlBaseSet \
        ROM_uDMAControlBaseSet
#else
#define MAP_uDMAControlBaseSet \
        uDMAControlBaseSet
#endif
#ifdef ROM_uDMAControlBaseGet
#define MAP_uDMAControlBaseGet \
        ROM_uDMAControlBaseGet
#else
#define MAP_uDMAControlBaseGet \
        uDMAControlBaseGet
#endif
#ifdef ROM_uDMAChannelRequest
#define MAP_uDMAChannelRequest \
        ROM_uDMAChannelRequest
#else
#define MAP_uDMAChannelRequest \
        uDMAChannelRequest
#endif
#ifdef ROM_uDMAChannelAttributeEnable
#define MAP_uDMAChannelAttributeEnable \
        ROM_uDMAChannelAttributeEnable
#else
#define MAP_uDMAChannelAttributeEnable \
        uDMAChannelAttributeEnable
#endif
#ifdef ROM_uDMAChannelAttributeDisable
#define MAP_uDMAChannelAttributeDisable \
        ROM_uDMAChannelAttributeDisable
#else
#define MAP_uDMAChannelAttributeDisable \
        uDMAChannelAttributeDisable
#endif
#ifdef ROM_uDMAChannelAttributeGet
#define MAP_uDMAChannelAttributeGet \
        ROM_uDMAChannelAttributeGet
#else
#define MAP_uDMAChannelAttributeGet \
        uDMAChannelAttributeGet
#endif
#ifdef ROM_uDMAChannelControlSet
#define MAP_uDMAChannelControlSet \
        ROM_uDMAChannelControlSet
#else
#define MAP_uDMAChannelControlSet \
        uDMAChannelControlSet
#endif
#ifdef ROM_uDMAChannelSizeGet
#define MAP_uDMAChannelSizeGet \
        ROM_uDMAChannelSizeGet
#else
#define MAP_uDMAChannelSizeGet \
        uDMAChannelSizeGet
#endif
#ifdef ROM_uDMAChannelModeGet
#define MAP_uDMAChannelModeGet \
        ROM_uDMAChannelModeGet
#else
#define MAP_uDMAChannelModeGet \
        uDMAChannelModeGet
#endif
#ifdef ROM_uDMAIntStatus
#define MAP_uDMAIntStatus \
        ROM_uDMAIntStatus
#else
#define MAP_uDMAIntStatus \
        uDMAIntStatus
#endif
#ifdef ROM_uDMAIntClear
#define MAP_uDMAIntClear \
        ROM_uDMAIntClear
#else
#define MAP_uDMAIntClear \
        uDMAIntClear
#endif
#ifdef ROM_uDMAControlAlternateBaseGet
#define MAP_uDMAControlAlternateBaseGet \
        ROM_uDMAControlAlternateBaseGet
#else
#define MAP_uDMAControlAlternateBaseGet \
        uDMAControlAlternateBaseGet
#endif
#ifdef ROM_uDMAChannelScatterGatherSet
#define MAP_uDMAChannelScatterGatherSet \
        ROM_uDMAChannelScatterGatherSet
#else
#define MAP_uDMAChannelScatterGatherSet \
        uDMAChannelScatterGatherSet
#endif
#ifdef ROM_uDMAChannelAssign
#define MAP_uDMAChannelAssign \
        ROM_uDMAChannelAssign
#else
#define MAP_uDMAChannelAssign \
        uDMAChannelAssign
#endif
#ifdef ROM_uDMAIntRegister
#define MAP_uDMAIntRegister \
        ROM_uDMAIntRegister
#else
#define MAP_uDMAIntRegister \
        uDMAIntRegister
#endif
#ifdef ROM_uDMAIntUnregister
#define MAP_uDMAIntUnregister \
        ROM_uDMAIntUnregister
#else
#define MAP_uDMAIntUnregister \
        uDMAIntUnregister
#endif

//*****************************************************************************
//
// Macros for the Watchdog API.
//
//*****************************************************************************
#ifdef ROM_WatchdogIntClear
#define MAP_WatchdogIntClear \
        ROM_WatchdogIntClear
#else
#define MAP_WatchdogIntClear \
        WatchdogIntClear
#endif
#ifdef ROM_WatchdogRunning
#define MAP_WatchdogRunning \
        ROM_WatchdogRunning
#else
#define MAP_WatchdogRunning \
        WatchdogRunning
#endif
#ifdef ROM_WatchdogEnable
#define MAP_WatchdogEnable \
        ROM_WatchdogEnable
#else
#define MAP_WatchdogEnable \
        WatchdogEnable
#endif
#ifdef ROM_WatchdogLock
#define MAP_WatchdogLock \
        ROM_WatchdogLock
#else
#define MAP_WatchdogLock \
        WatchdogLock
#endif
#ifdef ROM_WatchdogUnlock
#define MAP_WatchdogUnlock \
        ROM_WatchdogUnlock
#else
#define MAP_WatchdogUnlock \
        WatchdogUnlock
#endif
#ifdef ROM_WatchdogLockState
#define MAP_WatchdogLockState \
        ROM_WatchdogLockState
#else
#define MAP_WatchdogLockState \
        WatchdogLockState
#endif
#ifdef ROM_WatchdogReloadSet
#define MAP_WatchdogReloadSet \
        ROM_WatchdogReloadSet
#else
#define MAP_WatchdogReloadSet \
        WatchdogReloadSet
#endif
#ifdef ROM_WatchdogReloadGet
#define MAP_WatchdogReloadGet \
        ROM_WatchdogReloadGet
#else
#define MAP_WatchdogReloadGet \
        WatchdogReloadGet
#endif
#ifdef ROM_WatchdogValueGet
#define MAP_WatchdogValueGet \
        ROM_WatchdogValueGet
#else
#define MAP_WatchdogValueGet \
        WatchdogValueGet
#endif
#ifdef ROM_WatchdogIntStatus
#define MAP_WatchdogIntStatus \
        ROM_WatchdogIntStatus
#else
#define MAP_WatchdogIntStatus \
        WatchdogIntStatus
#endif
#ifdef ROM_WatchdogStallEnable
#define MAP_WatchdogStallEnable \
        ROM_WatchdogStallEnable
#else
#define MAP_WatchdogStallEnable \
        WatchdogStallEnable
#endif
#ifdef ROM_WatchdogStallDisable
#define MAP_WatchdogStallDisable \
        ROM_WatchdogStallDisable
#else
#define MAP_WatchdogStallDisable \
        WatchdogStallDisable
#endif
#ifdef ROM_WatchdogIntRegister
#define MAP_WatchdogIntRegister \
        ROM_WatchdogIntRegister
#else
#define MAP_WatchdogIntRegister \
        WatchdogIntRegister
#endif
#ifdef ROM_WatchdogIntUnregister
#define MAP_WatchdogIntUnregister \
        ROM_WatchdogIntUnregister
#else
#define MAP_WatchdogIntUnregister \
        WatchdogIntUnregister
#endif

//*****************************************************************************
//
// Macros for the I2C API.
//
//*****************************************************************************
#ifdef ROM_I2CIntRegister
#define MAP_I2CIntRegister \
        ROM_I2CIntRegister
#else
#define MAP_I2CIntRegister \
        I2CIntRegister
#endif
#ifdef ROM_I2CIntUnregister
#define MAP_I2CIntUnregister \
        ROM_I2CIntUnregister
#else
#define MAP_I2CIntUnregister \
        I2CIntUnregister
#endif
#ifdef ROM_I2CTxFIFOConfigSet
#define MAP_I2CTxFIFOConfigSet \
        ROM_I2CTxFIFOConfigSet
#else
#define MAP_I2CTxFIFOConfigSet \
        I2CTxFIFOConfigSet
#endif
#ifdef ROM_I2CTxFIFOFlush
#define MAP_I2CTxFIFOFlush \
        ROM_I2CTxFIFOFlush
#else
#define MAP_I2CTxFIFOFlush \
        I2CTxFIFOFlush
#endif
#ifdef ROM_I2CRxFIFOConfigSet
#define MAP_I2CRxFIFOConfigSet \
        ROM_I2CRxFIFOConfigSet
#else
#define MAP_I2CRxFIFOConfigSet \
        I2CRxFIFOConfigSet
#endif
#ifdef ROM_I2CRxFIFOFlush
#define MAP_I2CRxFIFOFlush \
        ROM_I2CRxFIFOFlush
#else
#define MAP_I2CRxFIFOFlush \
        I2CRxFIFOFlush
#endif
#ifdef ROM_I2CFIFOStatus
#define MAP_I2CFIFOStatus \
        ROM_I2CFIFOStatus
#else
#define MAP_I2CFIFOStatus \
        I2CFIFOStatus
#endif
#ifdef ROM_I2CFIFODataPut
#define MAP_I2CFIFODataPut \
        ROM_I2CFIFODataPut
#else
#define MAP_I2CFIFODataPut \
        I2CFIFODataPut
#endif
#ifdef ROM_I2CFIFODataPutNonBlocking
#define MAP_I2CFIFODataPutNonBlocking \
        ROM_I2CFIFODataPutNonBlocking
#else
#define MAP_I2CFIFODataPutNonBlocking \
        I2CFIFODataPutNonBlocking
#endif
#ifdef ROM_I2CFIFODataGet
#define MAP_I2CFIFODataGet \
        ROM_I2CFIFODataGet
#else
#define MAP_I2CFIFODataGet \
        I2CFIFODataGet
#endif
#ifdef ROM_I2CFIFODataGetNonBlocking
#define MAP_I2CFIFODataGetNonBlocking \
        ROM_I2CFIFODataGetNonBlocking
#else
#define MAP_I2CFIFODataGetNonBlocking \
        I2CFIFODataGetNonBlocking
#endif
#ifdef ROM_I2CMasterBurstLengthSet
#define MAP_I2CMasterBurstLengthSet \
        ROM_I2CMasterBurstLengthSet
#else
#define MAP_I2CMasterBurstLengthSet \
        I2CMasterBurstLengthSet
#endif
#ifdef ROM_I2CMasterBurstCountGet
#define MAP_I2CMasterBurstCountGet \
        ROM_I2CMasterBurstCountGet
#else
#define MAP_I2CMasterBurstCountGet \
        I2CMasterBurstCountGet
#endif
#ifdef ROM_I2CMasterGlitchFilterConfigSet
#define MAP_I2CMasterGlitchFilterConfigSet \
        ROM_I2CMasterGlitchFilterConfigSet
#else
#define MAP_I2CMasterGlitchFilterConfigSet \
        I2CMasterGlitchFilterConfigSet
#endif
#ifdef ROM_I2CSlaveFIFOEnable
#define MAP_I2CSlaveFIFOEnable \
        ROM_I2CSlaveFIFOEnable
#else
#define MAP_I2CSlaveFIFOEnable \
        I2CSlaveFIFOEnable
#endif
#ifdef ROM_I2CSlaveFIFODisable
#define MAP_I2CSlaveFIFODisable \
        ROM_I2CSlaveFIFODisable
#else
#define MAP_I2CSlaveFIFODisable \
        I2CSlaveFIFODisable
#endif
#ifdef ROM_I2CMasterBusBusy
#define MAP_I2CMasterBusBusy \
        ROM_I2CMasterBusBusy
#else
#define MAP_I2CMasterBusBusy \
        I2CMasterBusBusy
#endif
#ifdef ROM_I2CMasterBusy
#define MAP_I2CMasterBusy \
        ROM_I2CMasterBusy
#else
#define MAP_I2CMasterBusy \
        I2CMasterBusy
#endif
#ifdef ROM_I2CMasterControl
#define MAP_I2CMasterControl \
        ROM_I2CMasterControl
#else
#define MAP_I2CMasterControl \
        I2CMasterControl
#endif
#ifdef ROM_I2CMasterDataGet
#define MAP_I2CMasterDataGet \
        ROM_I2CMasterDataGet
#else
#define MAP_I2CMasterDataGet \
        I2CMasterDataGet
#endif
#ifdef ROM_I2CMasterDataPut
#define MAP_I2CMasterDataPut \
        ROM_I2CMasterDataPut
#else
#define MAP_I2CMasterDataPut \
        I2CMasterDataPut
#endif
#ifdef ROM_I2CMasterDisable
#define MAP_I2CMasterDisable \
        ROM_I2CMasterDisable
#else
#define MAP_I2CMasterDisable \
        I2CMasterDisable
#endif
#ifdef ROM_I2CMasterEnable
#define MAP_I2CMasterEnable \
        ROM_I2CMasterEnable
#else
#define MAP_I2CMasterEnable \
        I2CMasterEnable
#endif
#ifdef ROM_I2CMasterErr
#define MAP_I2CMasterErr \
        ROM_I2CMasterErr
#else
#define MAP_I2CMasterErr \
        I2CMasterErr
#endif
#ifdef ROM_I2CMasterIntClear
#define MAP_I2CMasterIntClear \
        ROM_I2CMasterIntClear
#else
#define MAP_I2CMasterIntClear \
        I2CMasterIntClear
#endif
#ifdef ROM_I2CMasterIntDisable
#define MAP_I2CMasterIntDisable \
        ROM_I2CMasterIntDisable
#else
#define MAP_I2CMasterIntDisable \
        I2CMasterIntDisable
#endif
#ifdef ROM_I2CMasterIntEnable
#define MAP_I2CMasterIntEnable \
        ROM_I2CMasterIntEnable
#else
#define MAP_I2CMasterIntEnable \
        I2CMasterIntEnable
#endif
#ifdef ROM_I2CMasterIntStatus
#define MAP_I2CMasterIntStatus \
        ROM_I2CMasterIntStatus
#else
#define MAP_I2CMasterIntStatus \
        I2CMasterIntStatus
#endif
#ifdef ROM_I2CMasterIntEnableEx
#define MAP_I2CMasterIntEnableEx \
        ROM_I2CMasterIntEnableEx
#else
#define MAP_I2CMasterIntEnableEx \
        I2CMasterIntEnableEx
#endif
#ifdef ROM_I2CMasterIntDisableEx
#define MAP_I2CMasterIntDisableEx \
        ROM_I2CMasterIntDisableEx
#else
#define MAP_I2CMasterIntDisableEx \
        I2CMasterIntDisableEx
#endif
#ifdef ROM_I2CMasterIntStatusEx
#define MAP_I2CMasterIntStatusEx \
        ROM_I2CMasterIntStatusEx
#else
#define MAP_I2CMasterIntStatusEx \
        I2CMasterIntStatusEx
#endif
#ifdef ROM_I2CMasterIntClearEx
#define MAP_I2CMasterIntClearEx \
        ROM_I2CMasterIntClearEx
#else
#define MAP_I2CMasterIntClearEx \
        I2CMasterIntClearEx
#endif
#ifdef ROM_I2CMasterTimeoutSet
#define MAP_I2CMasterTimeoutSet \
        ROM_I2CMasterTimeoutSet
#else
#define MAP_I2CMasterTimeoutSet \
        I2CMasterTimeoutSet
#endif
#ifdef ROM_I2CSlaveACKOverride
#define MAP_I2CSlaveACKOverride \
        ROM_I2CSlaveACKOverride
#else
#define MAP_I2CSlaveACKOverride \
        I2CSlaveACKOverride
#endif
#ifdef ROM_I2CSlaveACKValueSet
#define MAP_I2CSlaveACKValueSet \
        ROM_I2CSlaveACKValueSet
#else
#define MAP_I2CSlaveACKValueSet \
        I2CSlaveACKValueSet
#endif
#ifdef ROM_I2CMasterLineStateGet
#define MAP_I2CMasterLineStateGet \
        ROM_I2CMasterLineStateGet
#else
#define MAP_I2CMasterLineStateGet \
        I2CMasterLineStateGet
#endif
#ifdef ROM_I2CMasterSlaveAddrSet
#define MAP_I2CMasterSlaveAddrSet \
        ROM_I2CMasterSlaveAddrSet
#else
#define MAP_I2CMasterSlaveAddrSet \
        I2CMasterSlaveAddrSet
#endif
#ifdef ROM_I2CSlaveDataGet
#define MAP_I2CSlaveDataGet \
        ROM_I2CSlaveDataGet
#else
#define MAP_I2CSlaveDataGet \
        I2CSlaveDataGet
#endif
#ifdef ROM_I2CSlaveDataPut
#define MAP_I2CSlaveDataPut \
        ROM_I2CSlaveDataPut
#else
#define MAP_I2CSlaveDataPut \
        I2CSlaveDataPut
#endif
#ifdef ROM_I2CSlaveDisable
#define MAP_I2CSlaveDisable \
        ROM_I2CSlaveDisable
#else
#define MAP_I2CSlaveDisable \
        I2CSlaveDisable
#endif
#ifdef ROM_I2CSlaveEnable
#define MAP_I2CSlaveEnable \
        ROM_I2CSlaveEnable
#else
#define MAP_I2CSlaveEnable \
        I2CSlaveEnable
#endif
#ifdef ROM_I2CSlaveInit
#define MAP_I2CSlaveInit \
        ROM_I2CSlaveInit
#else
#define MAP_I2CSlaveInit \
        I2CSlaveInit
#endif
#ifdef ROM_I2CSlaveAddressSet
#define MAP_I2CSlaveAddressSet \
        ROM_I2CSlaveAddressSet
#else
#define MAP_I2CSlaveAddressSet \
        I2CSlaveAddressSet
#endif
#ifdef ROM_I2CSlaveIntClear
#define MAP_I2CSlaveIntClear \
        ROM_I2CSlaveIntClear
#else
#define MAP_I2CSlaveIntClear \
        I2CSlaveIntClear
#endif
#ifdef ROM_I2CSlaveIntDisable
#define MAP_I2CSlaveIntDisable \
        ROM_I2CSlaveIntDisable
#else
#define MAP_I2CSlaveIntDisable \
        I2CSlaveIntDisable
#endif
#ifdef ROM_I2CSlaveIntEnable
#define MAP_I2CSlaveIntEnable \
        ROM_I2CSlaveIntEnable
#else
#define MAP_I2CSlaveIntEnable \
        I2CSlaveIntEnable
#endif
#ifdef ROM_I2CSlaveIntClearEx
#define MAP_I2CSlaveIntClearEx \
        ROM_I2CSlaveIntClearEx
#else
#define MAP_I2CSlaveIntClearEx \
        I2CSlaveIntClearEx
#endif
#ifdef ROM_I2CSlaveIntDisableEx
#define MAP_I2CSlaveIntDisableEx \
        ROM_I2CSlaveIntDisableEx
#else
#define MAP_I2CSlaveIntDisableEx \
        I2CSlaveIntDisableEx
#endif
#ifdef ROM_I2CSlaveIntEnableEx
#define MAP_I2CSlaveIntEnableEx \
        ROM_I2CSlaveIntEnableEx
#else
#define MAP_I2CSlaveIntEnableEx \
        I2CSlaveIntEnableEx
#endif
#ifdef ROM_I2CSlaveIntStatus
#define MAP_I2CSlaveIntStatus \
        ROM_I2CSlaveIntStatus
#else
#define MAP_I2CSlaveIntStatus \
        I2CSlaveIntStatus
#endif
#ifdef ROM_I2CSlaveIntStatusEx
#define MAP_I2CSlaveIntStatusEx \
        ROM_I2CSlaveIntStatusEx
#else
#define MAP_I2CSlaveIntStatusEx \
        I2CSlaveIntStatusEx
#endif
#ifdef ROM_I2CSlaveStatus
#define MAP_I2CSlaveStatus \
        ROM_I2CSlaveStatus
#else
#define MAP_I2CSlaveStatus \
        I2CSlaveStatus
#endif
#ifdef ROM_I2CMasterInitExpClk
#define MAP_I2CMasterInitExpClk \
        ROM_I2CMasterInitExpClk
#else
#define MAP_I2CMasterInitExpClk \
        I2CMasterInitExpClk
#endif

//*****************************************************************************
//
// Macros for the SPI API.
//
//*****************************************************************************
#ifdef ROM_SPIEnable
#define MAP_SPIEnable \
        ROM_SPIEnable
#else
#define MAP_SPIEnable \
        SPIEnable
#endif
#ifdef ROM_SPIDisable
#define MAP_SPIDisable \
        ROM_SPIDisable
#else
#define MAP_SPIDisable \
        SPIDisable
#endif
#ifdef ROM_SPIReset
#define MAP_SPIReset \
        ROM_SPIReset
#else
#define MAP_SPIReset \
        SPIReset
#endif
#ifdef ROM_SPIConfigSetExpClk
#define MAP_SPIConfigSetExpClk \
        ROM_SPIConfigSetExpClk
#else
#define MAP_SPIConfigSetExpClk \
        SPIConfigSetExpClk
#endif
#ifdef ROM_SPIDataGetNonBlocking
#define MAP_SPIDataGetNonBlocking \
        ROM_SPIDataGetNonBlocking
#else
#define MAP_SPIDataGetNonBlocking \
        SPIDataGetNonBlocking
#endif
#ifdef ROM_SPIDataGet
#define MAP_SPIDataGet \
        ROM_SPIDataGet
#else
#define MAP_SPIDataGet \
        SPIDataGet
#endif
#ifdef ROM_SPIDataPutNonBlocking
#define MAP_SPIDataPutNonBlocking \
        ROM_SPIDataPutNonBlocking
#else
#define MAP_SPIDataPutNonBlocking \
        SPIDataPutNonBlocking
#endif
#ifdef ROM_SPIDataPut
#define MAP_SPIDataPut \
        ROM_SPIDataPut
#else
#define MAP_SPIDataPut \
        SPIDataPut
#endif
#ifdef ROM_SPIFIFOEnable
#define MAP_SPIFIFOEnable \
        ROM_SPIFIFOEnable
#else
#define MAP_SPIFIFOEnable \
        SPIFIFOEnable
#endif
#ifdef ROM_SPIFIFODisable
#define MAP_SPIFIFODisable \
        ROM_SPIFIFODisable
#else
#define MAP_SPIFIFODisable \
        SPIFIFODisable
#endif
#ifdef ROM_SPIFIFOLevelSet
#define MAP_SPIFIFOLevelSet \
        ROM_SPIFIFOLevelSet
#else
#define MAP_SPIFIFOLevelSet \
        SPIFIFOLevelSet
#endif
#ifdef ROM_SPIFIFOLevelGet
#define MAP_SPIFIFOLevelGet \
        ROM_SPIFIFOLevelGet
#else
#define MAP_SPIFIFOLevelGet \
        SPIFIFOLevelGet
#endif
#ifdef ROM_SPIWordCountSet
#define MAP_SPIWordCountSet \
        ROM_SPIWordCountSet
#else
#define MAP_SPIWordCountSet \
        SPIWordCountSet
#endif
#ifdef ROM_SPIIntRegister
#define MAP_SPIIntRegister \
        ROM_SPIIntRegister
#else
#define MAP_SPIIntRegister \
        SPIIntRegister
#endif
#ifdef ROM_SPIIntUnregister
#define MAP_SPIIntUnregister \
        ROM_SPIIntUnregister
#else
#define MAP_SPIIntUnregister \
        SPIIntUnregister
#endif
#ifdef ROM_SPIIntEnable
#define MAP_SPIIntEnable \
        ROM_SPIIntEnable
#else
#define MAP_SPIIntEnable \
        SPIIntEnable
#endif
#ifdef ROM_SPIIntDisable
#define MAP_SPIIntDisable \
        ROM_SPIIntDisable
#else
#define MAP_SPIIntDisable \
        SPIIntDisable
#endif
#ifdef ROM_SPIIntStatus
#define MAP_SPIIntStatus \
        ROM_SPIIntStatus
#else
#define MAP_SPIIntStatus \
        SPIIntStatus
#endif
#ifdef ROM_SPIIntClear
#define MAP_SPIIntClear \
        ROM_SPIIntClear
#else
#define MAP_SPIIntClear \
        SPIIntClear
#endif
#ifdef ROM_SPIDmaEnable
#define MAP_SPIDmaEnable \
        ROM_SPIDmaEnable
#else
#define MAP_SPIDmaEnable \
        SPIDmaEnable
#endif
#ifdef ROM_SPIDmaDisable
#define MAP_SPIDmaDisable \
        ROM_SPIDmaDisable
#else
#define MAP_SPIDmaDisable \
        SPIDmaDisable
#endif
#ifdef ROM_SPICSEnable
#define MAP_SPICSEnable \
        ROM_SPICSEnable
#else
#define MAP_SPICSEnable \
        SPICSEnable
#endif
#ifdef ROM_SPICSDisable
#define MAP_SPICSDisable \
        ROM_SPICSDisable
#else
#define MAP_SPICSDisable \
        SPICSDisable
#endif
#ifdef ROM_SPITransfer
#define MAP_SPITransfer \
        ROM_SPITransfer
#else
#define MAP_SPITransfer \
        SPITransfer
#endif

//*****************************************************************************
//
// Macros for the CAM API.
//
//*****************************************************************************
#ifdef ROM_CameraReset
#define MAP_CameraReset \
        ROM_CameraReset
#else
#define MAP_CameraReset \
        CameraReset
#endif
#ifdef ROM_CameraParamsConfig
#define MAP_CameraParamsConfig \
        ROM_CameraParamsConfig
#else
#define MAP_CameraParamsConfig \
        CameraParamsConfig
#endif
#ifdef ROM_CameraXClkConfig
#define MAP_CameraXClkConfig \
        ROM_CameraXClkConfig
#else
#define MAP_CameraXClkConfig \
        CameraXClkConfig
#endif
#ifdef ROM_CameraXClkSet
#define MAP_CameraXClkSet \
        ROM_CameraXClkSet
#else
#define MAP_CameraXClkSet \
        CameraXClkSet
#endif
#ifdef ROM_CameraDMAEnable
#define MAP_CameraDMAEnable \
        ROM_CameraDMAEnable
#else
#define MAP_CameraDMAEnable \
        CameraDMAEnable
#endif
#ifdef ROM_CameraDMADisable
#define MAP_CameraDMADisable \
        ROM_CameraDMADisable
#else
#define MAP_CameraDMADisable \
        CameraDMADisable
#endif
#ifdef ROM_CameraThresholdSet
#define MAP_CameraThresholdSet \
        ROM_CameraThresholdSet
#else
#define MAP_CameraThresholdSet \
        CameraThresholdSet
#endif
#ifdef ROM_CameraIntRegister
#define MAP_CameraIntRegister \
        ROM_CameraIntRegister
#else
#define MAP_CameraIntRegister \
        CameraIntRegister
#endif
#ifdef ROM_CameraIntUnregister
#define MAP_CameraIntUnregister \
        ROM_CameraIntUnregister
#else
#define MAP_CameraIntUnregister \
        CameraIntUnregister
#endif
#ifdef ROM_CameraIntEnable
#define MAP_CameraIntEnable \
        ROM_CameraIntEnable
#else
#define MAP_CameraIntEnable \
        CameraIntEnable
#endif
#ifdef ROM_CameraIntDisable
#define MAP_CameraIntDisable \
        ROM_CameraIntDisable
#else
#define MAP_CameraIntDisable \
        CameraIntDisable
#endif
#ifdef ROM_CameraIntStatus
#define MAP_CameraIntStatus \
        ROM_CameraIntStatus
#else
#define MAP_CameraIntStatus \
        CameraIntStatus
#endif
#ifdef ROM_CameraIntClear
#define MAP_CameraIntClear \
        ROM_CameraIntClear
#else
#define MAP_CameraIntClear \
        CameraIntClear
#endif
#ifdef ROM_CameraCaptureStop
#define MAP_CameraCaptureStop \
        ROM_CameraCaptureStop
#else
#define MAP_CameraCaptureStop \
        CameraCaptureStop
#endif
#ifdef ROM_CameraCaptureStart
#define MAP_CameraCaptureStart \
        ROM_CameraCaptureStart
#else
#define MAP_CameraCaptureStart \
        CameraCaptureStart
#endif
#ifdef ROM_CameraBufferRead
#define MAP_CameraBufferRead \
        ROM_CameraBufferRead
#else
#define MAP_CameraBufferRead \
        CameraBufferRead
#endif

//*****************************************************************************
//
// Macros for the FLASH API.
//
//*****************************************************************************
#ifdef ROM_FlashDisable
#define MAP_FlashDisable \
        ROM_FlashDisable
#else
#define MAP_FlashDisable \
        FlashDisable
#endif
#ifdef ROM_FlashErase
#define MAP_FlashErase \
        ROM_FlashErase
#else
#define MAP_FlashErase \
        FlashErase
#endif
#ifdef ROM_FlashMassErase
#define MAP_FlashMassErase \
        ROM_FlashMassErase
#else
#define MAP_FlashMassErase \
        FlashMassErase
#endif
#ifdef ROM_FlashMassEraseNonBlocking
#define MAP_FlashMassEraseNonBlocking \
        ROM_FlashMassEraseNonBlocking
#else
#define MAP_FlashMassEraseNonBlocking \
        FlashMassEraseNonBlocking
#endif
#ifdef ROM_FlashEraseNonBlocking
#define MAP_FlashEraseNonBlocking \
        ROM_FlashEraseNonBlocking
#else
#define MAP_FlashEraseNonBlocking \
        FlashEraseNonBlocking
#endif
#ifdef ROM_FlashProgram
#define MAP_FlashProgram \
        ROM_FlashProgram
#else
#define MAP_FlashProgram \
        FlashProgram
#endif
#ifdef ROM_FlashProgramNonBlocking
#define MAP_FlashProgramNonBlocking \
        ROM_FlashProgramNonBlocking
#else
#define MAP_FlashProgramNonBlocking \
        FlashProgramNonBlocking
#endif
#ifdef ROM_FlashIntRegister
#define MAP_FlashIntRegister \
        ROM_FlashIntRegister
#else
#define MAP_FlashIntRegister \
        FlashIntRegister
#endif
#ifdef ROM_FlashIntUnregister
#define MAP_FlashIntUnregister \
        ROM_FlashIntUnregister
#else
#define MAP_FlashIntUnregister \
        FlashIntUnregister
#endif
#ifdef ROM_FlashIntEnable
#define MAP_FlashIntEnable \
        ROM_FlashIntEnable
#else
#define MAP_FlashIntEnable \
        FlashIntEnable
#endif
#ifdef ROM_FlashIntDisable
#define MAP_FlashIntDisable \
        ROM_FlashIntDisable
#else
#define MAP_FlashIntDisable \
        FlashIntDisable
#endif
#ifdef ROM_FlashIntStatus
#define MAP_FlashIntStatus \
        ROM_FlashIntStatus
#else
#define MAP_FlashIntStatus \
        FlashIntStatus
#endif
#ifdef ROM_FlashIntClear
#define MAP_FlashIntClear \
        ROM_FlashIntClear
#else
#define MAP_FlashIntClear \
        FlashIntClear
#endif
#ifdef ROM_FlashProtectGet
#define MAP_FlashProtectGet \
        ROM_FlashProtectGet
#else
#define MAP_FlashProtectGet \
        FlashProtectGet
#endif

//*****************************************************************************
//
// Macros for the Pin API.
//
//*****************************************************************************
#ifdef ROM_PinModeSet
#define MAP_PinModeSet \
        ROM_PinModeSet
#else
#define MAP_PinModeSet \
        PinModeSet
#endif
#ifdef ROM_PinDirModeSet
#define MAP_PinDirModeSet \
        ROM_PinDirModeSet
#else
#define MAP_PinDirModeSet \
        PinDirModeSet
#endif
#ifdef ROM_PinDirModeGet
#define MAP_PinDirModeGet \
        ROM_PinDirModeGet
#else
#define MAP_PinDirModeGet \
        PinDirModeGet
#endif
#ifdef ROM_PinModeGet
#define MAP_PinModeGet \
        ROM_PinModeGet
#else
#define MAP_PinModeGet \
        PinModeGet
#endif
#ifdef ROM_PinConfigGet
#define MAP_PinConfigGet \
        ROM_PinConfigGet
#else
#define MAP_PinConfigGet \
        PinConfigGet
#endif
#ifdef ROM_PinConfigSet
#define MAP_PinConfigSet \
        ROM_PinConfigSet
#else
#define MAP_PinConfigSet \
        PinConfigSet
#endif
#ifdef ROM_PinTypeUART
#define MAP_PinTypeUART \
        ROM_PinTypeUART
#else
#define MAP_PinTypeUART \
        PinTypeUART
#endif
#ifdef ROM_PinTypeI2C
#define MAP_PinTypeI2C \
        ROM_PinTypeI2C
#else
#define MAP_PinTypeI2C \
        PinTypeI2C
#endif
#ifdef ROM_PinTypeSPI
#define MAP_PinTypeSPI \
        ROM_PinTypeSPI
#else
#define MAP_PinTypeSPI \
        PinTypeSPI
#endif
#ifdef ROM_PinTypeI2S
#define MAP_PinTypeI2S \
        ROM_PinTypeI2S
#else
#define MAP_PinTypeI2S \
        PinTypeI2S
#endif
#ifdef ROM_PinTypeTimer
#define MAP_PinTypeTimer \
        ROM_PinTypeTimer
#else
#define MAP_PinTypeTimer \
        PinTypeTimer
#endif
#ifdef ROM_PinTypeCamera
#define MAP_PinTypeCamera \
        ROM_PinTypeCamera
#else
#define MAP_PinTypeCamera \
        PinTypeCamera
#endif
#ifdef ROM_PinTypeGPIO
#define MAP_PinTypeGPIO \
        ROM_PinTypeGPIO
#else
#define MAP_PinTypeGPIO \
        PinTypeGPIO
#endif
#ifdef ROM_PinTypeADC
#define MAP_PinTypeADC \
        ROM_PinTypeADC
#else
#define MAP_PinTypeADC \
        PinTypeADC
#endif
#ifdef ROM_PinTypeSDHost
#define MAP_PinTypeSDHost \
        ROM_PinTypeSDHost
#else
#define MAP_PinTypeSDHost \
        PinTypeSDHost
#endif

//*****************************************************************************
//
// Macros for the SYSTICK API.
//
//*****************************************************************************
#ifdef ROM_SysTickEnable
#define MAP_SysTickEnable \
        ROM_SysTickEnable
#else
#define MAP_SysTickEnable \
        SysTickEnable
#endif
#ifdef ROM_SysTickDisable
#define MAP_SysTickDisable \
        ROM_SysTickDisable
#else
#define MAP_SysTickDisable \
        SysTickDisable
#endif
#ifdef ROM_SysTickIntRegister
#define MAP_SysTickIntRegister \
        ROM_SysTickIntRegister
#else
#define MAP_SysTickIntRegister \
        SysTickIntRegister
#endif
#ifdef ROM_SysTickIntUnregister
#define MAP_SysTickIntUnregister \
        ROM_SysTickIntUnregister
#else
#define MAP_SysTickIntUnregister \
        SysTickIntUnregister
#endif
#ifdef ROM_SysTickIntEnable
#define MAP_SysTickIntEnable \
        ROM_SysTickIntEnable
#else
#define MAP_SysTickIntEnable \
        SysTickIntEnable
#endif
#ifdef ROM_SysTickIntDisable
#define MAP_SysTickIntDisable \
        ROM_SysTickIntDisable
#else
#define MAP_SysTickIntDisable \
        SysTickIntDisable
#endif
#ifdef ROM_SysTickPeriodSet
#define MAP_SysTickPeriodSet \
        ROM_SysTickPeriodSet
#else
#define MAP_SysTickPeriodSet \
        SysTickPeriodSet
#endif
#ifdef ROM_SysTickPeriodGet
#define MAP_SysTickPeriodGet \
        ROM_SysTickPeriodGet
#else
#define MAP_SysTickPeriodGet \
        SysTickPeriodGet
#endif
#ifdef ROM_SysTickValueGet
#define MAP_SysTickValueGet \
        ROM_SysTickValueGet
#else
#define MAP_SysTickValueGet \
        SysTickValueGet
#endif

//*****************************************************************************
//
// Macros for the UTILS API.
//
//*****************************************************************************
#ifdef ROM_UtilsDelay
#define MAP_UtilsDelay \
        ROM_UtilsDelay
#else
#define MAP_UtilsDelay \
        UtilsDelay
#endif

//*****************************************************************************
//
// Macros for the I2S API.
//
//*****************************************************************************
#ifdef ROM_I2SEnable
#define MAP_I2SEnable \
        ROM_I2SEnable
#else
#define MAP_I2SEnable \
        I2SEnable
#endif
#ifdef ROM_I2SDisable
#define MAP_I2SDisable \
        ROM_I2SDisable
#else
#define MAP_I2SDisable \
        I2SDisable
#endif
#ifdef ROM_I2SDataPut
#define MAP_I2SDataPut \
        ROM_I2SDataPut
#else
#define MAP_I2SDataPut \
        I2SDataPut
#endif
#ifdef ROM_I2SDataPutNonBlocking
#define MAP_I2SDataPutNonBlocking \
        ROM_I2SDataPutNonBlocking
#else
#define MAP_I2SDataPutNonBlocking \
        I2SDataPutNonBlocking
#endif
#ifdef ROM_I2SDataGet
#define MAP_I2SDataGet \
        ROM_I2SDataGet
#else
#define MAP_I2SDataGet \
        I2SDataGet
#endif
#ifdef ROM_I2SDataGetNonBlocking
#define MAP_I2SDataGetNonBlocking \
        ROM_I2SDataGetNonBlocking
#else
#define MAP_I2SDataGetNonBlocking \
        I2SDataGetNonBlocking
#endif
#ifdef ROM_I2SConfigSetExpClk
#define MAP_I2SConfigSetExpClk \
        ROM_I2SConfigSetExpClk
#else
#define MAP_I2SConfigSetExpClk \
        I2SConfigSetExpClk
#endif
#ifdef ROM_I2STxFIFOEnable
#define MAP_I2STxFIFOEnable \
        ROM_I2STxFIFOEnable
#else
#define MAP_I2STxFIFOEnable \
        I2STxFIFOEnable
#endif
#ifdef ROM_I2STxFIFODisable
#define MAP_I2STxFIFODisable \
        ROM_I2STxFIFODisable
#else
#define MAP_I2STxFIFODisable \
        I2STxFIFODisable
#endif
#ifdef ROM_I2SRxFIFOEnable
#define MAP_I2SRxFIFOEnable \
        ROM_I2SRxFIFOEnable
#else
#define MAP_I2SRxFIFOEnable \
        I2SRxFIFOEnable
#endif
#ifdef ROM_I2SRxFIFODisable
#define MAP_I2SRxFIFODisable \
        ROM_I2SRxFIFODisable
#else
#define MAP_I2SRxFIFODisable \
        I2SRxFIFODisable
#endif
#ifdef ROM_I2STxFIFOStatusGet
#define MAP_I2STxFIFOStatusGet \
        ROM_I2STxFIFOStatusGet
#else
#define MAP_I2STxFIFOStatusGet \
        I2STxFIFOStatusGet
#endif
#ifdef ROM_I2SRxFIFOStatusGet
#define MAP_I2SRxFIFOStatusGet \
        ROM_I2SRxFIFOStatusGet
#else
#define MAP_I2SRxFIFOStatusGet \
        I2SRxFIFOStatusGet
#endif
#ifdef ROM_I2SSerializerConfig
#define MAP_I2SSerializerConfig \
        ROM_I2SSerializerConfig
#else
#define MAP_I2SSerializerConfig \
        I2SSerializerConfig
#endif
#ifdef ROM_I2SIntEnable
#define MAP_I2SIntEnable \
        ROM_I2SIntEnable
#else
#define MAP_I2SIntEnable \
        I2SIntEnable
#endif
#ifdef ROM_I2SIntDisable
#define MAP_I2SIntDisable \
        ROM_I2SIntDisable
#else
#define MAP_I2SIntDisable \
        I2SIntDisable
#endif
#ifdef ROM_I2SIntStatus
#define MAP_I2SIntStatus \
        ROM_I2SIntStatus
#else
#define MAP_I2SIntStatus \
        I2SIntStatus
#endif
#ifdef ROM_I2SIntClear
#define MAP_I2SIntClear \
        ROM_I2SIntClear
#else
#define MAP_I2SIntClear \
        I2SIntClear
#endif
#ifdef ROM_I2SIntRegister
#define MAP_I2SIntRegister \
        ROM_I2SIntRegister
#else
#define MAP_I2SIntRegister \
        I2SIntRegister
#endif
#ifdef ROM_I2SIntUnregister
#define MAP_I2SIntUnregister \
        ROM_I2SIntUnregister
#else
#define MAP_I2SIntUnregister \
        I2SIntUnregister
#endif

//*****************************************************************************
//
// Macros for the GPIO API.
//
//*****************************************************************************
#ifdef ROM_GPIODirModeSet
#define MAP_GPIODirModeSet \
        ROM_GPIODirModeSet
#else
#define MAP_GPIODirModeSet \
        GPIODirModeSet
#endif
#ifdef ROM_GPIODirModeGet
#define MAP_GPIODirModeGet \
        ROM_GPIODirModeGet
#else
#define MAP_GPIODirModeGet \
        GPIODirModeGet
#endif
#ifdef ROM_GPIOIntTypeSet
#define MAP_GPIOIntTypeSet \
        ROM_GPIOIntTypeSet
#else
#define MAP_GPIOIntTypeSet \
        GPIOIntTypeSet
#endif
#ifdef ROM_GPIODMATriggerEnable
#define MAP_GPIODMATriggerEnable \
        ROM_GPIODMATriggerEnable
#else
#define MAP_GPIODMATriggerEnable \
        GPIODMATriggerEnable
#endif
#ifdef ROM_GPIODMATriggerDisable
#define MAP_GPIODMATriggerDisable \
        ROM_GPIODMATriggerDisable
#else
#define MAP_GPIODMATriggerDisable \
        GPIODMATriggerDisable
#endif
#ifdef ROM_GPIOIntTypeGet
#define MAP_GPIOIntTypeGet \
        ROM_GPIOIntTypeGet
#else
#define MAP_GPIOIntTypeGet \
        GPIOIntTypeGet
#endif
#ifdef ROM_GPIOIntEnable
#define MAP_GPIOIntEnable \
        ROM_GPIOIntEnable
#else
#define MAP_GPIOIntEnable \
        GPIOIntEnable
#endif
#ifdef ROM_GPIOIntDisable
#define MAP_GPIOIntDisable \
        ROM_GPIOIntDisable
#else
#define MAP_GPIOIntDisable \
        GPIOIntDisable
#endif
#ifdef ROM_GPIOIntStatus
#define MAP_GPIOIntStatus \
        ROM_GPIOIntStatus
#else
#define MAP_GPIOIntStatus \
        GPIOIntStatus
#endif
#ifdef ROM_GPIOIntClear
#define MAP_GPIOIntClear \
        ROM_GPIOIntClear
#else
#define MAP_GPIOIntClear \
        GPIOIntClear
#endif
#ifdef ROM_GPIOIntRegister
#define MAP_GPIOIntRegister \
        ROM_GPIOIntRegister
#else
#define MAP_GPIOIntRegister \
        GPIOIntRegister
#endif
#ifdef ROM_GPIOIntUnregister
#define MAP_GPIOIntUnregister \
        ROM_GPIOIntUnregister
#else
#define MAP_GPIOIntUnregister \
        GPIOIntUnregister
#endif
#ifdef ROM_GPIOPinRead
#define MAP_GPIOPinRead \
        ROM_GPIOPinRead
#else
#define MAP_GPIOPinRead \
        GPIOPinRead
#endif
#ifdef ROM_GPIOPinWrite
#define MAP_GPIOPinWrite \
        ROM_GPIOPinWrite
#else
#define MAP_GPIOPinWrite \
        GPIOPinWrite
#endif

//*****************************************************************************
//
// Macros for the AES API.
//
//*****************************************************************************
#ifdef ROM_AESConfigSet
#define MAP_AESConfigSet \
        ROM_AESConfigSet
#else
#define MAP_AESConfigSet \
        AESConfigSet
#endif
#ifdef ROM_AESKey1Set
#define MAP_AESKey1Set \
        ROM_AESKey1Set
#else
#define MAP_AESKey1Set \
        AESKey1Set
#endif
#ifdef ROM_AESKey2Set
#define MAP_AESKey2Set \
        ROM_AESKey2Set
#else
#define MAP_AESKey2Set \
        AESKey2Set
#endif
#ifdef ROM_AESKey3Set
#define MAP_AESKey3Set \
        ROM_AESKey3Set
#else
#define MAP_AESKey3Set \
        AESKey3Set
#endif
#ifdef ROM_AESIVSet
#define MAP_AESIVSet \
        ROM_AESIVSet
#else
#define MAP_AESIVSet \
        AESIVSet
#endif
#ifdef ROM_AESTagRead
#define MAP_AESTagRead \
        ROM_AESTagRead
#else
#define MAP_AESTagRead \
        AESTagRead
#endif
#ifdef ROM_AESDataLengthSet
#define MAP_AESDataLengthSet \
        ROM_AESDataLengthSet
#else
#define MAP_AESDataLengthSet \
        AESDataLengthSet
#endif
#ifdef ROM_AESAuthDataLengthSet
#define MAP_AESAuthDataLengthSet \
        ROM_AESAuthDataLengthSet
#else
#define MAP_AESAuthDataLengthSet \
        AESAuthDataLengthSet
#endif
#ifdef ROM_AESDataReadNonBlocking
#define MAP_AESDataReadNonBlocking \
        ROM_AESDataReadNonBlocking
#else
#define MAP_AESDataReadNonBlocking \
        AESDataReadNonBlocking
#endif
#ifdef ROM_AESDataRead
#define MAP_AESDataRead \
        ROM_AESDataRead
#else
#define MAP_AESDataRead \
        AESDataRead
#endif
#ifdef ROM_AESDataWriteNonBlocking
#define MAP_AESDataWriteNonBlocking \
        ROM_AESDataWriteNonBlocking
#else
#define MAP_AESDataWriteNonBlocking \
        AESDataWriteNonBlocking
#endif
#ifdef ROM_AESDataWrite
#define MAP_AESDataWrite \
        ROM_AESDataWrite
#else
#define MAP_AESDataWrite \
        AESDataWrite
#endif
#ifdef ROM_AESDataProcess
#define MAP_AESDataProcess \
        ROM_AESDataProcess
#else
#define MAP_AESDataProcess \
        AESDataProcess
#endif
#ifdef ROM_AESDataMAC
#define MAP_AESDataMAC \
        ROM_AESDataMAC
#else
#define MAP_AESDataMAC \
        AESDataMAC
#endif
#ifdef ROM_AESDataProcessAE
#define MAP_AESDataProcessAE \
        ROM_AESDataProcessAE
#else
#define MAP_AESDataProcessAE \
        AESDataProcessAE
#endif
#ifdef ROM_AESIntStatus
#define MAP_AESIntStatus \
        ROM_AESIntStatus
#else
#define MAP_AESIntStatus \
        AESIntStatus
#endif
#ifdef ROM_AESIntEnable
#define MAP_AESIntEnable \
        ROM_AESIntEnable
#else
#define MAP_AESIntEnable \
        AESIntEnable
#endif
#ifdef ROM_AESIntDisable
#define MAP_AESIntDisable \
        ROM_AESIntDisable
#else
#define MAP_AESIntDisable \
        AESIntDisable
#endif
#ifdef ROM_AESIntClear
#define MAP_AESIntClear \
        ROM_AESIntClear
#else
#define MAP_AESIntClear \
        AESIntClear
#endif
#ifdef ROM_AESIntRegister
#define MAP_AESIntRegister \
        ROM_AESIntRegister
#else
#define MAP_AESIntRegister \
        AESIntRegister
#endif
#ifdef ROM_AESIntUnregister
#define MAP_AESIntUnregister \
        ROM_AESIntUnregister
#else
#define MAP_AESIntUnregister \
        AESIntUnregister
#endif
#ifdef ROM_AESDMAEnable
#define MAP_AESDMAEnable \
        ROM_AESDMAEnable
#else
#define MAP_AESDMAEnable \
        AESDMAEnable
#endif
#ifdef ROM_AESDMADisable
#define MAP_AESDMADisable \
        ROM_AESDMADisable
#else
#define MAP_AESDMADisable \
        AESDMADisable
#endif

//*****************************************************************************
//
// Macros for the DES API.
//
//*****************************************************************************
#ifdef ROM_DESConfigSet
#define MAP_DESConfigSet \
        ROM_DESConfigSet
#else
#define MAP_DESConfigSet \
        DESConfigSet
#endif
#ifdef ROM_DESDataRead
#define MAP_DESDataRead \
        ROM_DESDataRead
#else
#define MAP_DESDataRead \
        DESDataRead
#endif
#ifdef ROM_DESDataReadNonBlocking
#define MAP_DESDataReadNonBlocking \
        ROM_DESDataReadNonBlocking
#else
#define MAP_DESDataReadNonBlocking \
        DESDataReadNonBlocking
#endif
#ifdef ROM_DESDataProcess
#define MAP_DESDataProcess \
        ROM_DESDataProcess
#else
#define MAP_DESDataProcess \
        DESDataProcess
#endif
#ifdef ROM_DESDataWrite
#define MAP_DESDataWrite \
        ROM_DESDataWrite
#else
#define MAP_DESDataWrite \
        DESDataWrite
#endif
#ifdef ROM_DESDataWriteNonBlocking
#define MAP_DESDataWriteNonBlocking \
        ROM_DESDataWriteNonBlocking
#else
#define MAP_DESDataWriteNonBlocking \
        DESDataWriteNonBlocking
#endif
#ifdef ROM_DESDMADisable
#define MAP_DESDMADisable \
        ROM_DESDMADisable
#else
#define MAP_DESDMADisable \
        DESDMADisable
#endif
#ifdef ROM_DESDMAEnable
#define MAP_DESDMAEnable \
        ROM_DESDMAEnable
#else
#define MAP_DESDMAEnable \
        DESDMAEnable
#endif
#ifdef ROM_DESIntClear
#define MAP_DESIntClear \
        ROM_DESIntClear
#else
#define MAP_DESIntClear \
        DESIntClear
#endif
#ifdef ROM_DESIntDisable
#define MAP_DESIntDisable \
        ROM_DESIntDisable
#else
#define MAP_DESIntDisable \
        DESIntDisable
#endif
#ifdef ROM_DESIntEnable
#define MAP_DESIntEnable \
        ROM_DESIntEnable
#else
#define MAP_DESIntEnable \
        DESIntEnable
#endif
#ifdef ROM_DESIntRegister
#define MAP_DESIntRegister \
        ROM_DESIntRegister
#else
#define MAP_DESIntRegister \
        DESIntRegister
#endif
#ifdef ROM_DESIntStatus
#define MAP_DESIntStatus \
        ROM_DESIntStatus
#else
#define MAP_DESIntStatus \
        DESIntStatus
#endif
#ifdef ROM_DESIntUnregister
#define MAP_DESIntUnregister \
        ROM_DESIntUnregister
#else
#define MAP_DESIntUnregister \
        DESIntUnregister
#endif
#ifdef ROM_DESIVSet
#define MAP_DESIVSet \
        ROM_DESIVSet
#else
#define MAP_DESIVSet \
        DESIVSet
#endif
#ifdef ROM_DESKeySet
#define MAP_DESKeySet \
        ROM_DESKeySet
#else
#define MAP_DESKeySet \
        DESKeySet
#endif
#ifdef ROM_DESDataLengthSet
#define MAP_DESDataLengthSet \
        ROM_DESDataLengthSet
#else
#define MAP_DESDataLengthSet \
        DESDataLengthSet
#endif

//*****************************************************************************
//
// Macros for the SHAMD5 API.
//
//*****************************************************************************
#ifdef ROM_SHAMD5ConfigSet
#define MAP_SHAMD5ConfigSet \
        ROM_SHAMD5ConfigSet
#else
#define MAP_SHAMD5ConfigSet \
        SHAMD5ConfigSet
#endif
#ifdef ROM_SHAMD5DataProcess
#define MAP_SHAMD5DataProcess \
        ROM_SHAMD5DataProcess
#else
#define MAP_SHAMD5DataProcess \
        SHAMD5DataProcess
#endif
#ifdef ROM_SHAMD5DataWrite
#define MAP_SHAMD5DataWrite \
        ROM_SHAMD5DataWrite
#else
#define MAP_SHAMD5DataWrite \
        SHAMD5DataWrite
#endif
#ifdef ROM_SHAMD5DataWriteNonBlocking
#define MAP_SHAMD5DataWriteNonBlocking \
        ROM_SHAMD5DataWriteNonBlocking
#else
#define MAP_SHAMD5DataWriteNonBlocking \
        SHAMD5DataWriteNonBlocking
#endif
#ifdef ROM_SHAMD5DMADisable
#define MAP_SHAMD5DMADisable \
        ROM_SHAMD5DMADisable
#else
#define MAP_SHAMD5DMADisable \
        SHAMD5DMADisable
#endif
#ifdef ROM_SHAMD5DMAEnable
#define MAP_SHAMD5DMAEnable \
        ROM_SHAMD5DMAEnable
#else
#define MAP_SHAMD5DMAEnable \
        SHAMD5DMAEnable
#endif
#ifdef ROM_SHAMD5DataLengthSet
#define MAP_SHAMD5DataLengthSet \
        ROM_SHAMD5DataLengthSet
#else
#define MAP_SHAMD5DataLengthSet \
        SHAMD5DataLengthSet
#endif
#ifdef ROM_SHAMD5HMACKeySet
#define MAP_SHAMD5HMACKeySet \
        ROM_SHAMD5HMACKeySet
#else
#define MAP_SHAMD5HMACKeySet \
        SHAMD5HMACKeySet
#endif
#ifdef ROM_SHAMD5HMACPPKeyGenerate
#define MAP_SHAMD5HMACPPKeyGenerate \
        ROM_SHAMD5HMACPPKeyGenerate
#else
#define MAP_SHAMD5HMACPPKeyGenerate \
        SHAMD5HMACPPKeyGenerate
#endif
#ifdef ROM_SHAMD5HMACPPKeySet
#define MAP_SHAMD5HMACPPKeySet \
        ROM_SHAMD5HMACPPKeySet
#else
#define MAP_SHAMD5HMACPPKeySet \
        SHAMD5HMACPPKeySet
#endif
#ifdef ROM_SHAMD5HMACProcess
#define MAP_SHAMD5HMACProcess \
        ROM_SHAMD5HMACProcess
#else
#define MAP_SHAMD5HMACProcess \
        SHAMD5HMACProcess
#endif
#ifdef ROM_SHAMD5IntClear
#define MAP_SHAMD5IntClear \
        ROM_SHAMD5IntClear
#else
#define MAP_SHAMD5IntClear \
        SHAMD5IntClear
#endif
#ifdef ROM_SHAMD5IntDisable
#define MAP_SHAMD5IntDisable \
        ROM_SHAMD5IntDisable
#else
#define MAP_SHAMD5IntDisable \
        SHAMD5IntDisable
#endif
#ifdef ROM_SHAMD5IntEnable
#define MAP_SHAMD5IntEnable \
        ROM_SHAMD5IntEnable
#else
#define MAP_SHAMD5IntEnable \
        SHAMD5IntEnable
#endif
#ifdef ROM_SHAMD5IntRegister
#define MAP_SHAMD5IntRegister \
        ROM_SHAMD5IntRegister
#else
#define MAP_SHAMD5IntRegister \
        SHAMD5IntRegister
#endif
#ifdef ROM_SHAMD5IntStatus
#define MAP_SHAMD5IntStatus \
        ROM_SHAMD5IntStatus
#else
#define MAP_SHAMD5IntStatus \
        SHAMD5IntStatus
#endif
#ifdef ROM_SHAMD5IntUnregister
#define MAP_SHAMD5IntUnregister \
        ROM_SHAMD5IntUnregister
#else
#define MAP_SHAMD5IntUnregister \
        SHAMD5IntUnregister
#endif
#ifdef ROM_SHAMD5ResultRead
#define MAP_SHAMD5ResultRead \
        ROM_SHAMD5ResultRead
#else
#define MAP_SHAMD5ResultRead \
        SHAMD5ResultRead
#endif

//*****************************************************************************
//
// Macros for the CRC API.
//
//*****************************************************************************
#ifdef ROM_CRCConfigSet
#define MAP_CRCConfigSet \
        ROM_CRCConfigSet
#else
#define MAP_CRCConfigSet \
        CRCConfigSet
#endif
#ifdef ROM_CRCDataProcess
#define MAP_CRCDataProcess \
        ROM_CRCDataProcess
#else
#define MAP_CRCDataProcess \
        CRCDataProcess
#endif
#ifdef ROM_CRCDataWrite
#define MAP_CRCDataWrite \
        ROM_CRCDataWrite
#else
#define MAP_CRCDataWrite \
        CRCDataWrite
#endif
#ifdef ROM_CRCResultRead
#define MAP_CRCResultRead \
        ROM_CRCResultRead
#else
#define MAP_CRCResultRead \
        CRCResultRead
#endif
#ifdef ROM_CRCSeedSet
#define MAP_CRCSeedSet \
        ROM_CRCSeedSet
#else
#define MAP_CRCSeedSet \
        CRCSeedSet
#endif

//*****************************************************************************
//
// Macros for the SDHOST API.
//
//*****************************************************************************
#ifdef ROM_SDHostCmdReset
#define MAP_SDHostCmdReset \
        ROM_SDHostCmdReset
#else
#define MAP_SDHostCmdReset \
        SDHostCmdReset
#endif
#ifdef ROM_SDHostInit
#define MAP_SDHostInit \
        ROM_SDHostInit
#else
#define MAP_SDHostInit \
        SDHostInit
#endif
#ifdef ROM_SDHostCmdSend
#define MAP_SDHostCmdSend \
        ROM_SDHostCmdSend
#else
#define MAP_SDHostCmdSend \
        SDHostCmdSend
#endif
#ifdef ROM_SDHostIntRegister
#define MAP_SDHostIntRegister \
        ROM_SDHostIntRegister
#else
#define MAP_SDHostIntRegister \
        SDHostIntRegister
#endif
#ifdef ROM_SDHostIntUnregister
#define MAP_SDHostIntUnregister \
        ROM_SDHostIntUnregister
#else
#define MAP_SDHostIntUnregister \
        SDHostIntUnregister
#endif
#ifdef ROM_SDHostIntEnable
#define MAP_SDHostIntEnable \
        ROM_SDHostIntEnable
#else
#define MAP_SDHostIntEnable \
        SDHostIntEnable
#endif
#ifdef ROM_SDHostIntDisable
#define MAP_SDHostIntDisable \
        ROM_SDHostIntDisable
#else
#define MAP_SDHostIntDisable \
        SDHostIntDisable
#endif
#ifdef ROM_SDHostIntStatus
#define MAP_SDHostIntStatus \
        ROM_SDHostIntStatus
#else
#define MAP_SDHostIntStatus \
        SDHostIntStatus
#endif
#ifdef ROM_SDHostIntClear
#define MAP_SDHostIntClear \
        ROM_SDHostIntClear
#else
#define MAP_SDHostIntClear \
        SDHostIntClear
#endif
#ifdef ROM_SDHostRespStatus
#define MAP_SDHostRespStatus \
        ROM_SDHostRespStatus
#else
#define MAP_SDHostRespStatus \
        SDHostRespStatus
#endif
#ifdef ROM_SDHostRespGet
#define MAP_SDHostRespGet \
        ROM_SDHostRespGet
#else
#define MAP_SDHostRespGet \
        SDHostRespGet
#endif
#ifdef ROM_SDHostBlockSizeSet
#define MAP_SDHostBlockSizeSet \
        ROM_SDHostBlockSizeSet
#else
#define MAP_SDHostBlockSizeSet \
        SDHostBlockSizeSet
#endif
#ifdef ROM_SDHostBlockCountSet
#define MAP_SDHostBlockCountSet \
        ROM_SDHostBlockCountSet
#else
#define MAP_SDHostBlockCountSet \
        SDHostBlockCountSet
#endif
#ifdef ROM_SDHostDataNonBlockingWrite
#define MAP_SDHostDataNonBlockingWrite \
        ROM_SDHostDataNonBlockingWrite
#else
#define MAP_SDHostDataNonBlockingWrite \
        SDHostDataNonBlockingWrite
#endif
#ifdef ROM_SDHostDataNonBlockingRead
#define MAP_SDHostDataNonBlockingRead \
        ROM_SDHostDataNonBlockingRead
#else
#define MAP_SDHostDataNonBlockingRead \
        SDHostDataNonBlockingRead
#endif
#ifdef ROM_SDHostDataWrite
#define MAP_SDHostDataWrite \
        ROM_SDHostDataWrite
#else
#define MAP_SDHostDataWrite \
        SDHostDataWrite
#endif
#ifdef ROM_SDHostDataRead
#define MAP_SDHostDataRead \
        ROM_SDHostDataRead
#else
#define MAP_SDHostDataRead \
        SDHostDataRead
#endif
#ifdef ROM_SDHostSetExpClk
#define MAP_SDHostSetExpClk \
        ROM_SDHostSetExpClk
#else
#define MAP_SDHostSetExpClk \
        SDHostSetExpClk
#endif

//*****************************************************************************
//
// Macros for the PRCM API.
//
//*****************************************************************************
#ifdef ROM_PRCMMCUReset
#define MAP_PRCMMCUReset \
        ROM_PRCMMCUReset
#else
#define MAP_PRCMMCUReset \
        PRCMMCUReset
#endif
#ifdef ROM_PRCMSysResetCauseGet
#define MAP_PRCMSysResetCauseGet \
        ROM_PRCMSysResetCauseGet
#else
#define MAP_PRCMSysResetCauseGet \
        PRCMSysResetCauseGet
#endif
#ifdef ROM_PRCMPeripheralClkEnable
#define MAP_PRCMPeripheralClkEnable \
        ROM_PRCMPeripheralClkEnable
#else
#define MAP_PRCMPeripheralClkEnable \
        PRCMPeripheralClkEnable
#endif
#ifdef ROM_PRCMPeripheralClkDisable
#define MAP_PRCMPeripheralClkDisable \
        ROM_PRCMPeripheralClkDisable
#else
#define MAP_PRCMPeripheralClkDisable \
        PRCMPeripheralClkDisable
#endif
#ifdef ROM_PRCMPeripheralReset
#define MAP_PRCMPeripheralReset \
        ROM_PRCMPeripheralReset
#else
#define MAP_PRCMPeripheralReset \
        PRCMPeripheralReset
#endif
#ifdef ROM_PRCMPeripheralStatusGet
#define MAP_PRCMPeripheralStatusGet \
        ROM_PRCMPeripheralStatusGet
#else
#define MAP_PRCMPeripheralStatusGet \
        PRCMPeripheralStatusGet
#endif
#ifdef ROM_PRCMI2SClockFreqSet
#define MAP_PRCMI2SClockFreqSet \
        ROM_PRCMI2SClockFreqSet
#else
#define MAP_PRCMI2SClockFreqSet \
        PRCMI2SClockFreqSet
#endif
#ifdef ROM_PRCMPeripheralClockGet
#define MAP_PRCMPeripheralClockGet \
        ROM_PRCMPeripheralClockGet
#else
#define MAP_PRCMPeripheralClockGet \
        PRCMPeripheralClockGet
#endif
#ifdef ROM_PRCMSleepEnter
#define MAP_PRCMSleepEnter \
        ROM_PRCMSleepEnter
#else
#define MAP_PRCMSleepEnter \
        PRCMSleepEnter
#endif
#ifdef ROM_PRCMDeepSleepEnter
#define MAP_PRCMDeepSleepEnter \
        ROM_PRCMDeepSleepEnter
#else
#define MAP_PRCMDeepSleepEnter \
        PRCMDeepSleepEnter
#endif
#ifdef ROM_PRCMSRAMRetentionEnable
#define MAP_PRCMSRAMRetentionEnable \
        ROM_PRCMSRAMRetentionEnable
#else
#define MAP_PRCMSRAMRetentionEnable \
        PRCMSRAMRetentionEnable
#endif
#ifdef ROM_PRCMSRAMRetentionDisable
#define MAP_PRCMSRAMRetentionDisable \
        ROM_PRCMSRAMRetentionDisable
#else
#define MAP_PRCMSRAMRetentionDisable \
        PRCMSRAMRetentionDisable
#endif
#ifdef ROM_PRCMLPDSEnter
#define MAP_PRCMLPDSEnter \
        ROM_PRCMLPDSEnter
#else
#define MAP_PRCMLPDSEnter \
        PRCMLPDSEnter
#endif
#ifdef ROM_PRCMLPDSIntervalSet
#define MAP_PRCMLPDSIntervalSet \
        ROM_PRCMLPDSIntervalSet
#else
#define MAP_PRCMLPDSIntervalSet \
        PRCMLPDSIntervalSet
#endif
#ifdef ROM_PRCMLPDSWakeupSourceEnable
#define MAP_PRCMLPDSWakeupSourceEnable \
        ROM_PRCMLPDSWakeupSourceEnable
#else
#define MAP_PRCMLPDSWakeupSourceEnable \
        PRCMLPDSWakeupSourceEnable
#endif
#ifdef ROM_PRCMLPDSWakeupCauseGet
#define MAP_PRCMLPDSWakeupCauseGet \
        ROM_PRCMLPDSWakeupCauseGet
#else
#define MAP_PRCMLPDSWakeupCauseGet \
        PRCMLPDSWakeupCauseGet
#endif
#ifdef ROM_PRCMLPDSWakeUpGPIOSelect
#define MAP_PRCMLPDSWakeUpGPIOSelect \
        ROM_PRCMLPDSWakeUpGPIOSelect
#else
#define MAP_PRCMLPDSWakeUpGPIOSelect \
        PRCMLPDSWakeUpGPIOSelect
#endif
#ifdef ROM_PRCMLPDSWakeupSourceDisable
#define MAP_PRCMLPDSWakeupSourceDisable \
        ROM_PRCMLPDSWakeupSourceDisable
#else
#define MAP_PRCMLPDSWakeupSourceDisable \
        PRCMLPDSWakeupSourceDisable
#endif
#ifdef ROM_PRCMHibernateEnter
#define MAP_PRCMHibernateEnter \
        ROM_PRCMHibernateEnter
#else
#define MAP_PRCMHibernateEnter \
        PRCMHibernateEnter
#endif
#ifdef ROM_PRCMHibernateWakeupSourceEnable
#define MAP_PRCMHibernateWakeupSourceEnable \
        ROM_PRCMHibernateWakeupSourceEnable
#else
#define MAP_PRCMHibernateWakeupSourceEnable \
        PRCMHibernateWakeupSourceEnable
#endif
#ifdef ROM_PRCMHibernateWakeupCauseGet
#define MAP_PRCMHibernateWakeupCauseGet \
        ROM_PRCMHibernateWakeupCauseGet
#else
#define MAP_PRCMHibernateWakeupCauseGet \
        PRCMHibernateWakeupCauseGet
#endif
#ifdef ROM_PRCMHibernateWakeUpGPIOSelect
#define MAP_PRCMHibernateWakeUpGPIOSelect \
        ROM_PRCMHibernateWakeUpGPIOSelect
#else
#define MAP_PRCMHibernateWakeUpGPIOSelect \
        PRCMHibernateWakeUpGPIOSelect
#endif
#ifdef ROM_PRCMHibernateWakeupSourceDisable
#define MAP_PRCMHibernateWakeupSourceDisable \
        ROM_PRCMHibernateWakeupSourceDisable
#else
#define MAP_PRCMHibernateWakeupSourceDisable \
        PRCMHibernateWakeupSourceDisable
#endif
#ifdef ROM_PRCMHibernateIntervalSet
#define MAP_PRCMHibernateIntervalSet \
        ROM_PRCMHibernateIntervalSet
#else
#define MAP_PRCMHibernateIntervalSet \
        PRCMHibernateIntervalSet
#endif
#ifdef ROM_PRCMSlowClkCtrGet
#define MAP_PRCMSlowClkCtrGet \
        ROM_PRCMSlowClkCtrGet
#else
#define MAP_PRCMSlowClkCtrGet \
        PRCMSlowClkCtrGet
#endif
#ifdef ROM_PRCMSlowClkCtrMatchSet
#define MAP_PRCMSlowClkCtrMatchSet \
        ROM_PRCMSlowClkCtrMatchSet
#else
#define MAP_PRCMSlowClkCtrMatchSet \
        PRCMSlowClkCtrMatchSet
#endif
#ifdef ROM_PRCMSlowClkCtrMatchGet
#define MAP_PRCMSlowClkCtrMatchGet \
        ROM_PRCMSlowClkCtrMatchGet
#else
#define MAP_PRCMSlowClkCtrMatchGet \
        PRCMSlowClkCtrMatchGet
#endif
#ifdef ROM_PRCMOCRRegisterWrite
#define MAP_PRCMOCRRegisterWrite \
        ROM_PRCMOCRRegisterWrite
#else
#define MAP_PRCMOCRRegisterWrite \
        PRCMOCRRegisterWrite
#endif
#ifdef ROM_PRCMOCRRegisterRead
#define MAP_PRCMOCRRegisterRead \
        ROM_PRCMOCRRegisterRead
#else
#define MAP_PRCMOCRRegisterRead \
        PRCMOCRRegisterRead
#endif
#ifdef ROM_PRCMIntRegister
#define MAP_PRCMIntRegister \
        ROM_PRCMIntRegister
#else
#define MAP_PRCMIntRegister \
        PRCMIntRegister
#endif
#ifdef ROM_PRCMIntUnregister
#define MAP_PRCMIntUnregister \
        ROM_PRCMIntUnregister
#else
#define MAP_PRCMIntUnregister \
        PRCMIntUnregister
#endif
#ifdef ROM_PRCMIntEnable
#define MAP_PRCMIntEnable \
        ROM_PRCMIntEnable
#else
#define MAP_PRCMIntEnable \
        PRCMIntEnable
#endif
#ifdef ROM_PRCMIntDisable
#define MAP_PRCMIntDisable \
        ROM_PRCMIntDisable
#else
#define MAP_PRCMIntDisable \
        PRCMIntDisable
#endif
#ifdef ROM_PRCMIntStatus
#define MAP_PRCMIntStatus \
        ROM_PRCMIntStatus
#else
#define MAP_PRCMIntStatus \
        PRCMIntStatus
#endif
#ifdef ROM_PRCMRTCInUseSet
#define MAP_PRCMRTCInUseSet \
        ROM_PRCMRTCInUseSet
#else
#define MAP_PRCMRTCInUseSet \
        PRCMRTCInUseSet
#endif
#ifdef ROM_PRCMRTCInUseGet
#define MAP_PRCMRTCInUseGet \
        ROM_PRCMRTCInUseGet
#else
#define MAP_PRCMRTCInUseGet \
        PRCMRTCInUseGet
#endif
#ifdef ROM_PRCMRTCSet
#define MAP_PRCMRTCSet \
        ROM_PRCMRTCSet
#else
#define MAP_PRCMRTCSet \
        PRCMRTCSet
#endif
#ifdef ROM_PRCMRTCGet
#define MAP_PRCMRTCGet \
        ROM_PRCMRTCGet
#else
#define MAP_PRCMRTCGet \
        PRCMRTCGet
#endif
#ifdef ROM_PRCMRTCMatchSet
#define MAP_PRCMRTCMatchSet \
        ROM_PRCMRTCMatchSet
#else
#define MAP_PRCMRTCMatchSet \
        PRCMRTCMatchSet
#endif
#ifdef ROM_PRCMRTCMatchGet
#define MAP_PRCMRTCMatchGet \
        ROM_PRCMRTCMatchGet
#else
#define MAP_PRCMRTCMatchGet \
        PRCMRTCMatchGet
#endif
#ifdef ROM_PRCMLPDSRestoreInfoSet
#define MAP_PRCMLPDSRestoreInfoSet \
        ROM_PRCMLPDSRestoreInfoSet
#else
#define MAP_PRCMLPDSRestoreInfoSet \
        PRCMLPDSRestoreInfoSet
#endif

#ifdef ROM_PRCMHIBRegRead
#define MAP_PRCMHIBRegRead \
        ROM_PRCMHIBRegRead
#else
#define MAP_PRCMHIBRegRead \
        PRCMHIBRegRead
#endif

#ifdef ROM_PRCMHIBRegWrite
#define MAP_PRCMHIBRegWrite \
        ROM_PRCMHIBRegWrite
#else
#define MAP_PRCMHIBRegWrite \
        PRCMHIBRegWrite
#endif

//*****************************************************************************
//
// Macros for the HWSPINLOCK API.
//
//*****************************************************************************
#ifdef ROM_HwSpinLockAcquire
#define MAP_HwSpinLockAcquire \
        ROM_HwSpinLockAcquire
#else
#define MAP_HwSpinLockAcquire \
        HwSpinLockAcquire
#endif
#ifdef ROM_HwSpinLockTryAcquire
#define MAP_HwSpinLockTryAcquire \
        ROM_HwSpinLockTryAcquire
#else
#define MAP_HwSpinLockTryAcquire \
        HwSpinLockTryAcquire
#endif
#ifdef ROM_HwSpinLockRelease
#define MAP_HwSpinLockRelease \
        ROM_HwSpinLockRelease
#else
#define MAP_HwSpinLockRelease \
        HwSpinLockRelease
#endif
#ifdef ROM_HwSpinLockTest
#define MAP_HwSpinLockTest \
        ROM_HwSpinLockTest
#else
#define MAP_HwSpinLockTest \
        HwSpinLockTest
#endif

//*****************************************************************************
//
// Macros for the ADC API.
//
//*****************************************************************************
#ifdef ROM_ADCEnable
#define MAP_ADCEnable \
        ROM_ADCEnable
#else
#define MAP_ADCEnable \
        ADCEnable
#endif
#ifdef ROM_ADCDisable
#define MAP_ADCDisable \
        ROM_ADCDisable
#else
#define MAP_ADCDisable \
        ADCDisable
#endif
#ifdef ROM_ADCChannelEnable
#define MAP_ADCChannelEnable \
        ROM_ADCChannelEnable
#else
#define MAP_ADCChannelEnable \
        ADCChannelEnable
#endif
#ifdef ROM_ADCChannelDisable
#define MAP_ADCChannelDisable \
        ROM_ADCChannelDisable
#else
#define MAP_ADCChannelDisable \
        ADCChannelDisable
#endif
#ifdef ROM_ADCIntRegister
#define MAP_ADCIntRegister \
        ROM_ADCIntRegister
#else
#define MAP_ADCIntRegister \
        ADCIntRegister
#endif
#ifdef ROM_ADCIntUnregister
#define MAP_ADCIntUnregister \
        ROM_ADCIntUnregister
#else
#define MAP_ADCIntUnregister \
        ADCIntUnregister
#endif
#ifdef ROM_ADCIntEnable
#define MAP_ADCIntEnable \
        ROM_ADCIntEnable
#else
#define MAP_ADCIntEnable \
        ADCIntEnable
#endif
#ifdef ROM_ADCIntDisable
#define MAP_ADCIntDisable \
        ROM_ADCIntDisable
#else
#define MAP_ADCIntDisable \
        ADCIntDisable
#endif
#ifdef ROM_ADCIntStatus
#define MAP_ADCIntStatus \
        ROM_ADCIntStatus
#else
#define MAP_ADCIntStatus \
        ADCIntStatus
#endif
#ifdef ROM_ADCIntClear
#define MAP_ADCIntClear \
        ROM_ADCIntClear
#else
#define MAP_ADCIntClear \
        ADCIntClear
#endif
#ifdef ROM_ADCDMAEnable
#define MAP_ADCDMAEnable \
        ROM_ADCDMAEnable
#else
#define MAP_ADCDMAEnable \
        ADCDMAEnable
#endif
#ifdef ROM_ADCDMADisable
#define MAP_ADCDMADisable \
        ROM_ADCDMADisable
#else
#define MAP_ADCDMADisable \
        ADCDMADisable
#endif
#ifdef ROM_ADCChannelGainSet
#define MAP_ADCChannelGainSet \
        ROM_ADCChannelGainSet
#else
#define MAP_ADCChannelGainSet \
        ADCChannelGainSet
#endif
#ifdef ROM_ADCChannleGainGet
#define MAP_ADCChannleGainGet \
        ROM_ADCChannleGainGet
#else
#define MAP_ADCChannleGainGet \
        ADCChannleGainGet
#endif
#ifdef ROM_ADCTimerConfig
#define MAP_ADCTimerConfig \
        ROM_ADCTimerConfig
#else
#define MAP_ADCTimerConfig \
        ADCTimerConfig
#endif
#ifdef ROM_ADCTimerEnable
#define MAP_ADCTimerEnable \
        ROM_ADCTimerEnable
#else
#define MAP_ADCTimerEnable \
        ADCTimerEnable
#endif
#ifdef ROM_ADCTimerDisable
#define MAP_ADCTimerDisable \
        ROM_ADCTimerDisable
#else
#define MAP_ADCTimerDisable \
        ADCTimerDisable
#endif
#ifdef ROM_ADCTimerReset
#define MAP_ADCTimerReset \
        ROM_ADCTimerReset
#else
#define MAP_ADCTimerReset \
        ADCTimerReset
#endif
#ifdef ROM_ADCTimerValueGet
#define MAP_ADCTimerValueGet \
        ROM_ADCTimerValueGet
#else
#define MAP_ADCTimerValueGet \
        ADCTimerValueGet
#endif
#ifdef ROM_ADCFIFOLvlGet
#define MAP_ADCFIFOLvlGet \
        ROM_ADCFIFOLvlGet
#else
#define MAP_ADCFIFOLvlGet \
        ADCFIFOLvlGet
#endif
#ifdef ROM_ADCFIFORead
#define MAP_ADCFIFORead \
        ROM_ADCFIFORead
#else
#define MAP_ADCFIFORead \
        ADCFIFORead
#endif

#endif // __ROM_MAP_H__
