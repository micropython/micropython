//*****************************************************************************
//
//  rom.h
//
//  Macros to facilitate calling functions in the ROM.
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

#ifndef __ROM_H__
#define __ROM_H__

//*****************************************************************************
//
// Pointers to the main API tables.
//
//*****************************************************************************
#define ROM_APITABLE            ((unsigned long *)0x0000040C)
#define ROM_VERSION             (ROM_APITABLE[0])
#define ROM_UARTTABLE           ((unsigned long *)(ROM_APITABLE[1]))
#define ROM_TIMERTABLE          ((unsigned long *)(ROM_APITABLE[2]))
#define ROM_WATCHDOGTABLE       ((unsigned long *)(ROM_APITABLE[3]))
#define ROM_INTERRUPTTABLE      ((unsigned long *)(ROM_APITABLE[4]))
#define ROM_UDMATABLE           ((unsigned long *)(ROM_APITABLE[5]))
#define ROM_PRCMTABLE           ((unsigned long *)(ROM_APITABLE[6]))
#define ROM_I2CTABLE            ((unsigned long *)(ROM_APITABLE[7]))
#define ROM_SPITABLE            ((unsigned long *)(ROM_APITABLE[8]))
#define ROM_CAMERATABLE         ((unsigned long *)(ROM_APITABLE[9]))
#define ROM_FLASHTABLE          ((unsigned long *)(ROM_APITABLE[10]))
#define ROM_PINTABLE            ((unsigned long *)(ROM_APITABLE[11]))
#define ROM_SYSTICKTABLE        ((unsigned long *)(ROM_APITABLE[12]))
#define ROM_UTILSTABLE          ((unsigned long *)(ROM_APITABLE[13]))
#define ROM_I2STABLE            ((unsigned long *)(ROM_APITABLE[14]))
#define ROM_HWSPINLOCKTABLE     ((unsigned long *)(ROM_APITABLE[15]))
#define ROM_GPIOTABLE           ((unsigned long *)(ROM_APITABLE[16]))
#define ROM_AESTABLE            ((unsigned long *)(ROM_APITABLE[17]))
#define ROM_DESTABLE            ((unsigned long *)(ROM_APITABLE[18]))
#define ROM_SHAMD5TABLE         ((unsigned long *)(ROM_APITABLE[19]))
#define ROM_CRCTABLE            ((unsigned long *)(ROM_APITABLE[20]))
#define ROM_SDHOSTTABLE         ((unsigned long *)(ROM_APITABLE[21]))
#define ROM_ADCTABLE            ((unsigned long *)(ROM_APITABLE[22]))

//*****************************************************************************
//
// Macros for calling ROM functions in the Interrupt API.
//
//*****************************************************************************
#if defined(TARGET_IS_CC3200)
#define ROM_IntEnable                                                         \
        ((void (*)(unsigned long ulInterrupt))ROM_INTERRUPTTABLE[0])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_IntMasterEnable                                                   \
        ((tBoolean (*)(void))ROM_INTERRUPTTABLE[1])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_IntMasterDisable                                                  \
        ((tBoolean (*)(void))ROM_INTERRUPTTABLE[2])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_IntDisable                                                        \
        ((void (*)(unsigned long ulInterrupt))ROM_INTERRUPTTABLE[3])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_IntPriorityGroupingSet                                            \
        ((void (*)(unsigned long ulBits))ROM_INTERRUPTTABLE[4])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_IntPriorityGroupingGet                                            \
        ((unsigned long (*)(void))ROM_INTERRUPTTABLE[5])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_IntPrioritySet                                                    \
        ((void (*)(unsigned long ulInterrupt,                                 \
                   unsigned char ucPriority))ROM_INTERRUPTTABLE[6])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_IntPriorityGet                                                    \
        ((long (*)(unsigned long ulInterrupt))ROM_INTERRUPTTABLE[7])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_IntPendSet                                                        \
        ((void (*)(unsigned long ulInterrupt))ROM_INTERRUPTTABLE[8])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_IntPendClear                                                      \
        ((void (*)(unsigned long ulInterrupt))ROM_INTERRUPTTABLE[9])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_IntPriorityMaskSet                                                \
        ((void (*)(unsigned long ulPriorityMask))ROM_INTERRUPTTABLE[10])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_IntPriorityMaskGet                                                \
        ((unsigned long (*)(void))ROM_INTERRUPTTABLE[11])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_IntRegister                                                       \
        ((void (*)(unsigned long ulInterrupt,                                 \
                   void (*pfnHandler)(void)))ROM_INTERRUPTTABLE[12])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_IntUnregister                                                     \
        ((void (*)(unsigned long ulInterrupt))ROM_INTERRUPTTABLE[13])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_IntVTableBaseSet                                                  \
        ((void (*)(unsigned long ulVtableBase))ROM_INTERRUPTTABLE[14])
#endif

//*****************************************************************************
//
// Macros for calling ROM functions in the Timer API.
//
//*****************************************************************************
#if defined(TARGET_IS_CC3200)
#define ROM_TimerEnable                                                       \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulTimer))ROM_TIMERTABLE[0])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_TimerDisable                                                      \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulTimer))ROM_TIMERTABLE[1])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_TimerConfigure                                                    \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulConfig))ROM_TIMERTABLE[2])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_TimerControlLevel                                                 \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulTimer,                                     \
                   tBoolean bInvert))ROM_TIMERTABLE[3])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_TimerControlEvent                                                 \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulTimer,                                     \
                   unsigned long ulEvent))ROM_TIMERTABLE[4])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_TimerControlStall                                                 \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulTimer,                                     \
                   tBoolean bStall))ROM_TIMERTABLE[5])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_TimerPrescaleSet                                                  \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulTimer,                                     \
                   unsigned long ulValue))ROM_TIMERTABLE[6])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_TimerPrescaleGet                                                  \
        ((unsigned long (*)(unsigned long ulBase,                             \
                            unsigned long ulTimer))ROM_TIMERTABLE[7])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_TimerPrescaleMatchSet                                             \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulTimer,                                     \
                   unsigned long ulValue))ROM_TIMERTABLE[8])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_TimerPrescaleMatchGet                                             \
        ((unsigned long (*)(unsigned long ulBase,                             \
                            unsigned long ulTimer))ROM_TIMERTABLE[9])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_TimerLoadSet                                                      \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulTimer,                                     \
                   unsigned long ulValue))ROM_TIMERTABLE[10])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_TimerLoadGet                                                      \
        ((unsigned long (*)(unsigned long ulBase,                             \
                            unsigned long ulTimer))ROM_TIMERTABLE[11])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_TimerValueGet                                                     \
        ((unsigned long (*)(unsigned long ulBase,                             \
                            unsigned long ulTimer))ROM_TIMERTABLE[12])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_TimerMatchSet                                                     \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulTimer,                                     \
                   unsigned long ulValue))ROM_TIMERTABLE[13])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_TimerMatchGet                                                     \
        ((unsigned long (*)(unsigned long ulBase,                             \
                            unsigned long ulTimer))ROM_TIMERTABLE[14])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_TimerIntRegister                                                  \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulTimer,                                     \
                   void (*pfnHandler)(void)))ROM_TIMERTABLE[15])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_TimerIntUnregister                                                \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulTimer))ROM_TIMERTABLE[16])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_TimerIntEnable                                                    \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulIntFlags))ROM_TIMERTABLE[17])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_TimerIntDisable                                                   \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulIntFlags))ROM_TIMERTABLE[18])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_TimerIntStatus                                                    \
        ((unsigned long (*)(unsigned long ulBase,                             \
                            tBoolean bMasked))ROM_TIMERTABLE[19])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_TimerIntClear                                                     \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulIntFlags))ROM_TIMERTABLE[20])
#endif

//*****************************************************************************
//
// Macros for calling ROM functions in the UART API.
//
//*****************************************************************************
#if defined(TARGET_IS_CC3200)
#define ROM_UARTParityModeSet                                                 \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulParity))ROM_UARTTABLE[0])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_UARTParityModeGet                                                 \
        ((unsigned long (*)(unsigned long ulBase))ROM_UARTTABLE[1])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_UARTFIFOLevelSet                                                  \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulTxLevel,                                   \
                   unsigned long ulRxLevel))ROM_UARTTABLE[2])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_UARTFIFOLevelGet                                                  \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long *pulTxLevel,                                 \
                   unsigned long *pulRxLevel))ROM_UARTTABLE[3])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_UARTConfigSetExpClk                                               \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulUARTClk,                                   \
                   unsigned long ulBaud,                                      \
                   unsigned long ulConfig))ROM_UARTTABLE[4])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_UARTConfigGetExpClk                                               \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulUARTClk,                                   \
                   unsigned long *pulBaud,                                    \
                   unsigned long *pulConfig))ROM_UARTTABLE[5])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_UARTEnable                                                        \
        ((void (*)(unsigned long ulBase))ROM_UARTTABLE[6])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_UARTDisable                                                       \
        ((void (*)(unsigned long ulBase))ROM_UARTTABLE[7])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_UARTFIFOEnable                                                    \
        ((void (*)(unsigned long ulBase))ROM_UARTTABLE[8])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_UARTFIFODisable                                                   \
        ((void (*)(unsigned long ulBase))ROM_UARTTABLE[9])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_UARTCharsAvail                                                    \
        ((tBoolean (*)(unsigned long ulBase))ROM_UARTTABLE[10])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_UARTSpaceAvail                                                    \
        ((tBoolean (*)(unsigned long ulBase))ROM_UARTTABLE[11])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_UARTCharGetNonBlocking                                            \
        ((long (*)(unsigned long ulBase))ROM_UARTTABLE[12])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_UARTCharGet                                                       \
        ((long (*)(unsigned long ulBase))ROM_UARTTABLE[13])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_UARTCharPutNonBlocking                                            \
        ((tBoolean (*)(unsigned long ulBase,                                  \
                       unsigned char ucData))ROM_UARTTABLE[14])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_UARTCharPut                                                       \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned char ucData))ROM_UARTTABLE[15])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_UARTBreakCtl                                                      \
        ((void (*)(unsigned long ulBase,                                      \
                   tBoolean bBreakState))ROM_UARTTABLE[16])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_UARTBusy                                                          \
        ((tBoolean (*)(unsigned long ulBase))ROM_UARTTABLE[17])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_UARTIntRegister                                                   \
        ((void (*)(unsigned long ulBase,                                      \
                   void(*pfnHandler)(void)))ROM_UARTTABLE[18])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_UARTIntUnregister                                                 \
        ((void (*)(unsigned long ulBase))ROM_UARTTABLE[19])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_UARTIntEnable                                                     \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulIntFlags))ROM_UARTTABLE[20])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_UARTIntDisable                                                    \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulIntFlags))ROM_UARTTABLE[21])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_UARTIntStatus                                                     \
        ((unsigned long (*)(unsigned long ulBase,                             \
                            tBoolean bMasked))ROM_UARTTABLE[22])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_UARTIntClear                                                      \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulIntFlags))ROM_UARTTABLE[23])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_UARTDMAEnable                                                     \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulDMAFlags))ROM_UARTTABLE[24])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_UARTDMADisable                                                    \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulDMAFlags))ROM_UARTTABLE[25])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_UARTRxErrorGet                                                    \
        ((unsigned long (*)(unsigned long ulBase))ROM_UARTTABLE[26])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_UARTRxErrorClear                                                  \
        ((void (*)(unsigned long ulBase))ROM_UARTTABLE[27])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_UARTModemControlSet                                               \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulControl))ROM_UARTTABLE[28])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_UARTModemControlClear                                             \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulControl))ROM_UARTTABLE[29])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_UARTModemControlGet                                               \
        ((unsigned long (*)(unsigned long ulBase))ROM_UARTTABLE[30])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_UARTModemStatusGet                                                \
        ((unsigned long (*)(unsigned long ulBase))ROM_UARTTABLE[31])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_UARTFlowControlSet                                                \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulMode))ROM_UARTTABLE[32])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_UARTFlowControlGet                                                \
        ((unsigned long (*)(unsigned long ulBase))ROM_UARTTABLE[33])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_UARTTxIntModeSet                                                  \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulMode))ROM_UARTTABLE[34])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_UARTTxIntModeGet                                                  \
        ((unsigned long (*)(unsigned long ulBase))ROM_UARTTABLE[35])
#endif

//*****************************************************************************
//
// Macros for calling ROM functions in the uDMA API.
//
//*****************************************************************************
#if defined(TARGET_IS_CC3200)
#define ROM_uDMAChannelTransferSet                                            \
        ((void (*)(unsigned long ulChannelStructIndex,                        \
                   unsigned long ulMode,                                      \
                   void *pvSrcAddr,                                           \
                   void *pvDstAddr,                                           \
                   unsigned long ulTransferSize))ROM_UDMATABLE[0])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_uDMAEnable                                                        \
        ((void (*)(void))ROM_UDMATABLE[1])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_uDMADisable                                                       \
        ((void (*)(void))ROM_UDMATABLE[2])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_uDMAErrorStatusGet                                                \
        ((unsigned long (*)(void))ROM_UDMATABLE[3])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_uDMAErrorStatusClear                                              \
        ((void (*)(void))ROM_UDMATABLE[4])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_uDMAChannelEnable                                                 \
        ((void (*)(unsigned long ulChannelNum))ROM_UDMATABLE[5])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_uDMAChannelDisable                                                \
        ((void (*)(unsigned long ulChannelNum))ROM_UDMATABLE[6])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_uDMAChannelIsEnabled                                              \
        ((tBoolean (*)(unsigned long ulChannelNum))ROM_UDMATABLE[7])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_uDMAControlBaseSet                                                \
        ((void (*)(void *pControlTable))ROM_UDMATABLE[8])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_uDMAControlBaseGet                                                \
        ((void * (*)(void))ROM_UDMATABLE[9])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_uDMAChannelRequest                                                \
        ((void (*)(unsigned long ulChannelNum))ROM_UDMATABLE[10])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_uDMAChannelAttributeEnable                                        \
        ((void (*)(unsigned long ulChannelNum,                                \
                   unsigned long ulAttr))ROM_UDMATABLE[11])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_uDMAChannelAttributeDisable                                       \
        ((void (*)(unsigned long ulChannelNum,                                \
                   unsigned long ulAttr))ROM_UDMATABLE[12])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_uDMAChannelAttributeGet                                           \
        ((unsigned long (*)(unsigned long ulChannelNum))ROM_UDMATABLE[13])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_uDMAChannelControlSet                                             \
        ((void (*)(unsigned long ulChannelStructIndex,                        \
                   unsigned long ulControl))ROM_UDMATABLE[14])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_uDMAChannelSizeGet                                                \
        ((unsigned long (*)(unsigned long ulChannelStructIndex))ROM_UDMATABLE[15])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_uDMAChannelModeGet                                                \
        ((unsigned long (*)(unsigned long ulChannelStructIndex))ROM_UDMATABLE[16])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_uDMAIntStatus                                                     \
        ((unsigned long (*)(void))ROM_UDMATABLE[17])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_uDMAIntClear                                                      \
        ((void (*)(unsigned long ulChanMask))ROM_UDMATABLE[18])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_uDMAControlAlternateBaseGet                                       \
        ((void * (*)(void))ROM_UDMATABLE[19])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_uDMAChannelScatterGatherSet                                       \
        ((void (*)(unsigned long ulChannelNum,                                \
                   unsigned ulTaskCount,                                      \
                   void *pvTaskList,                                          \
                   unsigned long ulIsPeriphSG))ROM_UDMATABLE[20])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_uDMAChannelAssign                                                 \
        ((void (*)(unsigned long ulMapping))ROM_UDMATABLE[21])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_uDMAIntRegister                                                   \
        ((void (*)(unsigned long ulIntChannel,                                \
                   void (*pfnHandler)(void)))ROM_UDMATABLE[22])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_uDMAIntUnregister                                                 \
        ((void (*)(unsigned long ulIntChannel))ROM_UDMATABLE[23])
#endif

//*****************************************************************************
//
// Macros for calling ROM functions in the Watchdog API.
//
//*****************************************************************************
#if defined(TARGET_IS_CC3200)
#define ROM_WatchdogIntClear                                                  \
        ((void (*)(unsigned long ulBase))ROM_WATCHDOGTABLE[0])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_WatchdogRunning                                                   \
        ((tBoolean (*)(unsigned long ulBase))ROM_WATCHDOGTABLE[1])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_WatchdogEnable                                                    \
        ((void (*)(unsigned long ulBase))ROM_WATCHDOGTABLE[2])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_WatchdogLock                                                      \
        ((void (*)(unsigned long ulBase))ROM_WATCHDOGTABLE[3])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_WatchdogUnlock                                                    \
        ((void (*)(unsigned long ulBase))ROM_WATCHDOGTABLE[4])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_WatchdogLockState                                                 \
        ((tBoolean (*)(unsigned long ulBase))ROM_WATCHDOGTABLE[5])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_WatchdogReloadSet                                                 \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulLoadVal))ROM_WATCHDOGTABLE[6])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_WatchdogReloadGet                                                 \
        ((unsigned long (*)(unsigned long ulBase))ROM_WATCHDOGTABLE[7])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_WatchdogValueGet                                                  \
        ((unsigned long (*)(unsigned long ulBase))ROM_WATCHDOGTABLE[8])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_WatchdogIntStatus                                                 \
        ((unsigned long (*)(unsigned long ulBase,                             \
                            tBoolean bMasked))ROM_WATCHDOGTABLE[10])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_WatchdogStallEnable                                               \
        ((void (*)(unsigned long ulBase))ROM_WATCHDOGTABLE[11])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_WatchdogStallDisable                                              \
        ((void (*)(unsigned long ulBase))ROM_WATCHDOGTABLE[12])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_WatchdogIntRegister                                               \
        ((void (*)(unsigned long ulBase,                                      \
                   void(*pfnHandler)(void)))ROM_WATCHDOGTABLE[13])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_WatchdogIntUnregister                                             \
        ((void (*)(unsigned long ulBase))ROM_WATCHDOGTABLE[14])
#endif

//*****************************************************************************
//
// Macros for calling ROM functions in the I2C API.
//
//*****************************************************************************
#if defined(TARGET_IS_CC3200)
#define ROM_I2CIntRegister                                                    \
        ((void (*)(uint32_t ui32Base,                                         \
                   void(pfnHandler)(void)))ROM_I2CTABLE[0])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CIntUnregister                                                  \
        ((void (*)(uint32_t ui32Base))ROM_I2CTABLE[1])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CTxFIFOConfigSet                                                \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint32_t ui32Config))ROM_I2CTABLE[2])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CTxFIFOFlush                                                    \
        ((void (*)(uint32_t ui32Base))ROM_I2CTABLE[3])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CRxFIFOConfigSet                                                \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint32_t ui32Config))ROM_I2CTABLE[4])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CRxFIFOFlush                                                    \
        ((void (*)(uint32_t ui32Base))ROM_I2CTABLE[5])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CFIFOStatus                                                     \
        ((uint32_t (*)(uint32_t ui32Base))ROM_I2CTABLE[6])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CFIFODataPut                                                    \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint8_t ui8Data))ROM_I2CTABLE[7])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CFIFODataPutNonBlocking                                         \
        ((uint32_t (*)(uint32_t ui32Base,                                     \
                       uint8_t ui8Data))ROM_I2CTABLE[8])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CFIFODataGet                                                    \
        ((uint32_t (*)(uint32_t ui32Base))ROM_I2CTABLE[9])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CFIFODataGetNonBlocking                                         \
        ((uint32_t (*)(uint32_t ui32Base,                                     \
                       uint8_t *pui8Data))ROM_I2CTABLE[10])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CMasterBurstLengthSet                                           \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint8_t ui8Length))ROM_I2CTABLE[11])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CMasterBurstCountGet                                            \
        ((uint32_t (*)(uint32_t ui32Base))ROM_I2CTABLE[12])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CMasterGlitchFilterConfigSet                                    \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint32_t ui32Config))ROM_I2CTABLE[13])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CSlaveFIFOEnable                                                \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint32_t ui32Config))ROM_I2CTABLE[14])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CSlaveFIFODisable                                               \
        ((void (*)(uint32_t ui32Base))ROM_I2CTABLE[15])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CMasterBusBusy                                                  \
        ((bool (*)(uint32_t ui32Base))ROM_I2CTABLE[16])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CMasterBusy                                                     \
        ((bool (*)(uint32_t ui32Base))ROM_I2CTABLE[17])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CMasterControl                                                  \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint32_t ui32Cmd))ROM_I2CTABLE[18])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CMasterDataGet                                                  \
        ((uint32_t (*)(uint32_t ui32Base))ROM_I2CTABLE[19])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CMasterDataPut                                                  \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint8_t ui8Data))ROM_I2CTABLE[20])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CMasterDisable                                                  \
        ((void (*)(uint32_t ui32Base))ROM_I2CTABLE[21])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CMasterEnable                                                   \
        ((void (*)(uint32_t ui32Base))ROM_I2CTABLE[22])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CMasterErr                                                      \
        ((uint32_t (*)(uint32_t ui32Base))ROM_I2CTABLE[23])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CMasterIntClear                                                 \
        ((void (*)(uint32_t ui32Base))ROM_I2CTABLE[24])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CMasterIntDisable                                               \
        ((void (*)(uint32_t ui32Base))ROM_I2CTABLE[25])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CMasterIntEnable                                                \
        ((void (*)(uint32_t ui32Base))ROM_I2CTABLE[26])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CMasterIntStatus                                                \
        ((bool (*)(uint32_t ui32Base,                                         \
                   bool bMasked))ROM_I2CTABLE[27])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CMasterIntEnableEx                                              \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint32_t ui32IntFlags))ROM_I2CTABLE[28])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CMasterIntDisableEx                                             \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint32_t ui32IntFlags))ROM_I2CTABLE[29])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CMasterIntStatusEx                                              \
        ((uint32_t (*)(uint32_t ui32Base,                                     \
                       bool bMasked))ROM_I2CTABLE[30])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CMasterIntClearEx                                               \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint32_t ui32IntFlags))ROM_I2CTABLE[31])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CMasterTimeoutSet                                               \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint32_t ui32Value))ROM_I2CTABLE[32])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CSlaveACKOverride                                               \
        ((void (*)(uint32_t ui32Base,                                         \
                   bool bEnable))ROM_I2CTABLE[33])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CSlaveACKValueSet                                               \
        ((void (*)(uint32_t ui32Base,                                         \
                   bool bACK))ROM_I2CTABLE[34])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CMasterLineStateGet                                             \
        ((uint32_t (*)(uint32_t ui32Base))ROM_I2CTABLE[35])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CMasterSlaveAddrSet                                             \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint8_t ui8SlaveAddr,                                      \
                   bool bReceive))ROM_I2CTABLE[36])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CSlaveDataGet                                                   \
        ((uint32_t (*)(uint32_t ui32Base))ROM_I2CTABLE[37])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CSlaveDataPut                                                   \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint8_t ui8Data))ROM_I2CTABLE[38])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CSlaveDisable                                                   \
        ((void (*)(uint32_t ui32Base))ROM_I2CTABLE[39])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CSlaveEnable                                                    \
        ((void (*)(uint32_t ui32Base))ROM_I2CTABLE[40])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CSlaveInit                                                      \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint8_t ui8SlaveAddr))ROM_I2CTABLE[41])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CSlaveAddressSet                                                \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint8_t ui8AddrNum,                                        \
                   uint8_t ui8SlaveAddr))ROM_I2CTABLE[42])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CSlaveIntClear                                                  \
        ((void (*)(uint32_t ui32Base))ROM_I2CTABLE[43])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CSlaveIntDisable                                                \
        ((void (*)(uint32_t ui32Base))ROM_I2CTABLE[44])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CSlaveIntEnable                                                 \
        ((void (*)(uint32_t ui32Base))ROM_I2CTABLE[45])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CSlaveIntClearEx                                                \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint32_t ui32IntFlags))ROM_I2CTABLE[46])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CSlaveIntDisableEx                                              \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint32_t ui32IntFlags))ROM_I2CTABLE[47])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CSlaveIntEnableEx                                               \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint32_t ui32IntFlags))ROM_I2CTABLE[48])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CSlaveIntStatus                                                 \
        ((bool (*)(uint32_t ui32Base,                                         \
                   bool bMasked))ROM_I2CTABLE[49])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CSlaveIntStatusEx                                               \
        ((uint32_t (*)(uint32_t ui32Base,                                     \
                       bool bMasked))ROM_I2CTABLE[50])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CSlaveStatus                                                    \
        ((uint32_t (*)(uint32_t ui32Base))ROM_I2CTABLE[51])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2CMasterInitExpClk                                               \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint32_t ui32I2CClk,                                       \
                   bool bFast))ROM_I2CTABLE[52])
#endif

//*****************************************************************************
//
// Macros for calling ROM functions in the SPI API.
//
//*****************************************************************************
#if defined(TARGET_IS_CC3200)
#define ROM_SPIEnable                                                         \
        ((void (*)(unsigned long ulBase))ROM_SPITABLE[0])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SPIDisable                                                        \
        ((void (*)(unsigned long ulBase))ROM_SPITABLE[1])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SPIReset                                                          \
        ((void (*)(unsigned long ulBase))ROM_SPITABLE[2])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SPIConfigSetExpClk                                                \
        ((void (*)(unsigned long ulBase,                                      \
                  unsigned long ulSPIClk,                                     \
                   unsigned long ulBitRate,                                   \
                   unsigned long ulMode,                                      \
                   unsigned long ulSubMode,                                   \
                   unsigned long ulConfig))ROM_SPITABLE[3])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SPIDataGetNonBlocking                                             \
        ((long (*)(unsigned long ulBase,                                      \
                   unsigned long * pulData))ROM_SPITABLE[4])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SPIDataGet                                                        \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long *pulData))ROM_SPITABLE[5])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SPIDataPutNonBlocking                                             \
        ((long (*)(unsigned long ulBase,                                      \
                   unsigned long ulData))ROM_SPITABLE[6])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SPIDataPut                                                        \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulData))ROM_SPITABLE[7])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SPIFIFOEnable                                                     \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulFlags))ROM_SPITABLE[8])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SPIFIFODisable                                                    \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulFlags))ROM_SPITABLE[9])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SPIFIFOLevelSet                                                   \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulTxLevel,                                   \
                   unsigned long ulRxLevel))ROM_SPITABLE[10])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SPIFIFOLevelGet                                                   \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long *pulTxLevel,                                 \
                   unsigned long *pulRxLevel))ROM_SPITABLE[11])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SPIWordCountSet                                                   \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulWordCount))ROM_SPITABLE[12])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SPIIntRegister                                                    \
        ((void (*)(unsigned long ulBase,                                      \
                   void(*pfnHandler)(void)))ROM_SPITABLE[13])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SPIIntUnregister                                                  \
        ((void (*)(unsigned long ulBase))ROM_SPITABLE[14])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SPIIntEnable                                                      \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulIntFlags))ROM_SPITABLE[15])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SPIIntDisable                                                     \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulIntFlags))ROM_SPITABLE[16])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SPIIntStatus                                                      \
        ((unsigned long (*)(unsigned long ulBase,                             \
                            tBoolean bMasked))ROM_SPITABLE[17])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SPIIntClear                                                       \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulIntFlags))ROM_SPITABLE[18])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SPIDmaEnable                                                      \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulFlags))ROM_SPITABLE[19])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SPIDmaDisable                                                     \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulFlags))ROM_SPITABLE[20])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SPICSEnable                                                       \
        ((void (*)(unsigned long ulBase))ROM_SPITABLE[21])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SPICSDisable                                                      \
        ((void (*)(unsigned long ulBase))ROM_SPITABLE[22])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SPITransfer                                                       \
        ((long (*)(unsigned long ulBase,                                      \
                   unsigned char *ucDout,                                     \
                   unsigned char *ucDin,                                      \
                   unsigned long ulSize,                                      \
                   unsigned long ulFlags))ROM_SPITABLE[23])
#endif

//*****************************************************************************
//
// Macros for calling ROM functions in the CAM API.
//
//*****************************************************************************
#if defined(TARGET_IS_CC3200)
#define ROM_CameraReset                                                       \
        ((void (*)(unsigned long ulBase))ROM_CAMERATABLE[0])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_CameraParamsConfig                                                \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulHSPol,                                     \
                   unsigned long ulVSPol,                                     \
                   unsigned long ulFlags))ROM_CAMERATABLE[1])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_CameraXClkConfig                                                  \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulCamClkIn,                                  \
                   unsigned long ulXClk))ROM_CAMERATABLE[2])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_CameraXClkSet                                                     \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned char bXClkFlags))ROM_CAMERATABLE[3])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_CameraDMAEnable                                                   \
        ((void (*)(unsigned long ulBase))ROM_CAMERATABLE[4])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_CameraDMADisable                                                  \
        ((void (*)(unsigned long ulBase))ROM_CAMERATABLE[5])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_CameraThresholdSet                                                \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulThreshold))ROM_CAMERATABLE[6])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_CameraIntRegister                                                 \
        ((void (*)(unsigned long ulBase,                                      \
                   void (*pfnHandler)(void)))ROM_CAMERATABLE[7])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_CameraIntUnregister                                               \
        ((void (*)(unsigned long ulBase))ROM_CAMERATABLE[8])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_CameraIntEnable                                                   \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulIntFlags))ROM_CAMERATABLE[9])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_CameraIntDisable                                                  \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulIntFlags))ROM_CAMERATABLE[10])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_CameraIntStatus                                                   \
        ((unsigned long (*)(unsigned long ulBase))ROM_CAMERATABLE[11])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_CameraIntClear                                                    \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulIntFlags))ROM_CAMERATABLE[12])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_CameraCaptureStop                                                 \
        ((void (*)(unsigned long ulBase,                                      \
                   tBoolean bImmediate))ROM_CAMERATABLE[13])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_CameraCaptureStart                                                \
        ((void (*)(unsigned long ulBase))ROM_CAMERATABLE[14])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_CameraBufferRead                                                  \
        ((void (*)(unsigned long ulBase,                                      \
                  unsigned long *pBuffer,                                     \
                   unsigned char ucSize))ROM_CAMERATABLE[15])
#endif

//*****************************************************************************
//
// Macros for calling ROM functions in the FLASH API.
//
//*****************************************************************************
#if defined(TARGET_IS_CC3200)
#define ROM_FlashDisable                                                      \
        ((void (*)(void))ROM_FLASHTABLE[0])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_FlashErase                                                        \
        ((long (*)(unsigned long ulAddress))ROM_FLASHTABLE[1])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_FlashMassErase                                                    \
        ((long (*)(void))ROM_FLASHTABLE[2])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_FlashMassEraseNonBlocking                                         \
        ((void (*)(void))ROM_FLASHTABLE[3])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_FlashEraseNonBlocking                                             \
        ((void (*)(unsigned long ulAddress))ROM_FLASHTABLE[4])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_FlashProgram                                                      \
        ((long (*)(unsigned long *pulData,                                    \
                   unsigned long ulAddress,                                   \
                   unsigned long ulCount))ROM_FLASHTABLE[5])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_FlashProgramNonBlocking                                           \
        ((long (*)(unsigned long *pulData,                                    \
                   unsigned long ulAddress,                                   \
                   unsigned long ulCount))ROM_FLASHTABLE[6])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_FlashIntRegister                                                  \
        ((void (*)(void (*pfnHandler)(void)))ROM_FLASHTABLE[7])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_FlashIntUnregister                                                \
        ((void (*)(void))ROM_FLASHTABLE[8])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_FlashIntEnable                                                    \
        ((void (*)(unsigned long ulIntFlags))ROM_FLASHTABLE[9])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_FlashIntDisable                                                   \
        ((void (*)(unsigned long ulIntFlags))ROM_FLASHTABLE[10])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_FlashIntStatus                                                    \
        ((unsigned long (*)(tBoolean bMasked))ROM_FLASHTABLE[11])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_FlashIntClear                                                     \
        ((void (*)(unsigned long ulIntFlags))ROM_FLASHTABLE[12])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_FlashProtectGet                                                   \
        ((tFlashProtection (*)(unsigned long ulAddress))ROM_FLASHTABLE[13])
#endif

//*****************************************************************************
//
// Macros for calling ROM functions in the Pin API.
//
//*****************************************************************************
#if defined(TARGET_IS_CC3200)
#define ROM_PinModeSet                                                        \
        ((void (*)(unsigned long ulPin,                                       \
                   unsigned long ulPinMode))ROM_PINTABLE[0])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PinDirModeSet                                                     \
        ((void (*)(unsigned long ulPin,                                       \
                   unsigned long ulPinIO))ROM_PINTABLE[1])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PinDirModeGet                                                     \
        ((unsigned long (*)(unsigned long ulPin))ROM_PINTABLE[2])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PinModeGet                                                        \
        ((unsigned long (*)(unsigned long ulPin))ROM_PINTABLE[3])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PinConfigGet                                                      \
        ((void (*)(unsigned long ulPin,                                       \
                  unsigned long  *pulPinStrength,                             \
                   unsigned long *pulPinType))ROM_PINTABLE[4])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PinConfigSet                                                      \
        ((void (*)(unsigned long ulPin,                                       \
                  unsigned long  ulPinStrength,                               \
                   unsigned long ulPinType))ROM_PINTABLE[5])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PinTypeUART                                                       \
        ((void (*)(unsigned long ulPin,                                       \
                  unsigned long ulPinMode))ROM_PINTABLE[6])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PinTypeI2C                                                        \
        ((void (*)(unsigned long ulPin,                                       \
                  unsigned long ulPinMode))ROM_PINTABLE[7])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PinTypeSPI                                                        \
        ((void (*)(unsigned long ulPin,                                       \
                  unsigned long ulPinMode))ROM_PINTABLE[8])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PinTypeI2S                                                        \
        ((void (*)(unsigned long ulPin,                                       \
                  unsigned long ulPinMode))ROM_PINTABLE[9])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PinTypeTimer                                                      \
        ((void (*)(unsigned long ulPin,                                       \
                  unsigned long ulPinMode))ROM_PINTABLE[10])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PinTypeCamera                                                     \
        ((void (*)(unsigned long ulPin,                                       \
                  unsigned long ulPinMode))ROM_PINTABLE[11])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PinTypeGPIO                                                       \
        ((void (*)(unsigned long ulPin,                                       \
                  unsigned long ulPinMode,                                    \
                   tBoolean bOpenDrain))ROM_PINTABLE[12])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PinTypeADC                                                        \
        ((void (*)(unsigned long ulPin,                                       \
                  unsigned long ulPinMode))ROM_PINTABLE[13])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PinTypeSDHost                                                     \
        ((void (*)(unsigned long ulPin,                                       \
                  unsigned long ulPinMode))ROM_PINTABLE[14])
#endif

//*****************************************************************************
//
// Macros for calling ROM functions in the SYSTICK API.
//
//*****************************************************************************
#if defined(TARGET_IS_CC3200)
#define ROM_SysTickEnable                                                     \
        ((void (*)(void))ROM_SYSTICKTABLE[0])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SysTickDisable                                                    \
        ((void (*)(void))ROM_SYSTICKTABLE[1])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SysTickIntRegister                                                \
        ((void (*)(void (*pfnHandler)(void)))ROM_SYSTICKTABLE[2])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SysTickIntUnregister                                              \
        ((void (*)(void))ROM_SYSTICKTABLE[3])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SysTickIntEnable                                                  \
        ((void (*)(void))ROM_SYSTICKTABLE[4])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SysTickIntDisable                                                 \
        ((void (*)(void))ROM_SYSTICKTABLE[5])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SysTickPeriodSet                                                  \
        ((void (*)(unsigned long ulPeriod))ROM_SYSTICKTABLE[6])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SysTickPeriodGet                                                  \
        ((unsigned long (*)(void))ROM_SYSTICKTABLE[7])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SysTickValueGet                                                   \
        ((unsigned long (*)(void))ROM_SYSTICKTABLE[8])
#endif

//*****************************************************************************
//
// Macros for calling ROM functions in the UTILS API.
//
//*****************************************************************************
#if defined(TARGET_IS_CC3200)
#define ROM_UtilsDelay                                                        \
        ((void (*)(unsigned long ulCount))ROM_UTILSTABLE[0])
#endif

//*****************************************************************************
//
// Macros for calling ROM functions in the I2S API.
//
//*****************************************************************************
#if defined(TARGET_IS_CC3200)
#define ROM_I2SEnable                                                         \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulMode))ROM_I2STABLE[0])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2SDisable                                                        \
        ((void (*)(unsigned long ulBase))ROM_I2STABLE[1])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2SDataPut                                                        \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulDataLine,                                  \
                   unsigned long ulData))ROM_I2STABLE[2])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2SDataPutNonBlocking                                             \
        ((long (*)(unsigned long ulBase,                                      \
                   unsigned long ulDataLine,                                  \
                   unsigned long ulData))ROM_I2STABLE[3])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2SDataGet                                                        \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulDataLine,                                  \
                   unsigned long *pulData))ROM_I2STABLE[4])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2SDataGetNonBlocking                                             \
        ((long (*)(unsigned long ulBase,                                      \
                   unsigned long ulDataLine,                                  \
                   unsigned long *pulData))ROM_I2STABLE[5])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2SConfigSetExpClk                                                \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulI2SClk,                                    \
                   unsigned long ulBitClk,                                    \
                   unsigned long ulConfig))ROM_I2STABLE[6])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2STxFIFOEnable                                                   \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulTxLevel,                                   \
                   unsigned long ulWordsPerTransfer))ROM_I2STABLE[7])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2STxFIFODisable                                                  \
        ((void (*)(unsigned long ulBase))ROM_I2STABLE[8])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2SRxFIFOEnable                                                   \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulRxLevel,                                   \
                   unsigned long ulWordsPerTransfer))ROM_I2STABLE[9])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2SRxFIFODisable                                                  \
        ((void (*)(unsigned long ulBase))ROM_I2STABLE[10])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2STxFIFOStatusGet                                                \
        ((unsigned long (*)(unsigned long ulBase))ROM_I2STABLE[11])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2SRxFIFOStatusGet                                                \
        ((unsigned long (*)(unsigned long ulBase))ROM_I2STABLE[12])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2SSerializerConfig                                               \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulDataLine,                                  \
                   unsigned long ulSerMode,                                   \
                   unsigned long ulInActState))ROM_I2STABLE[13])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2SIntEnable                                                      \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulIntFlags))ROM_I2STABLE[14])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2SIntDisable                                                     \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulIntFlags))ROM_I2STABLE[15])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2SIntStatus                                                      \
        ((unsigned long (*)(unsigned long ulBase))ROM_I2STABLE[16])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2SIntClear                                                       \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulIntFlags))ROM_I2STABLE[17])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2SIntRegister                                                    \
        ((void (*)(unsigned long ulBase,                                      \
                   void (*pfnHandler)(void)))ROM_I2STABLE[18])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_I2SIntUnregister                                                  \
        ((void (*)(unsigned long ulBase))ROM_I2STABLE[19])
#endif

//*****************************************************************************
//
// Macros for calling ROM functions in the GPIO API.
//
//*****************************************************************************
#if defined(TARGET_IS_CC3200)
#define ROM_GPIODirModeSet                                                    \
        ((void (*)(unsigned long ulPort,                                      \
                   unsigned char ucPins,                                      \
                   unsigned long ulPinIO))ROM_GPIOTABLE[0])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_GPIODirModeGet                                                    \
        ((unsigned long (*)(unsigned long ulPort,                             \
                            unsigned char ucPin))ROM_GPIOTABLE[1])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_GPIOIntTypeSet                                                    \
        ((void (*)(unsigned long ulPort,                                      \
                   unsigned char ucPins,                                      \
                   unsigned long ulIntType))ROM_GPIOTABLE[2])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_GPIODMATriggerEnable                                              \
        ((void (*)(unsigned long ulPort))ROM_GPIOTABLE[3])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_GPIODMATriggerDisable                                             \
        ((void (*)(unsigned long ulPort))ROM_GPIOTABLE[4])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_GPIOIntTypeGet                                                    \
        ((unsigned long (*)(unsigned long ulPort,                             \
                            unsigned char ucPin))ROM_GPIOTABLE[5])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_GPIOIntEnable                                                     \
        ((void (*)(unsigned long ulPort,                                      \
                   unsigned long ulIntFlags))ROM_GPIOTABLE[6])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_GPIOIntDisable                                                    \
        ((void (*)(unsigned long ulPort,                                      \
                   unsigned long ulIntFlags))ROM_GPIOTABLE[7])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_GPIOIntStatus                                                     \
        ((long (*)(unsigned long ulPort,                                      \
                   tBoolean bMasked))ROM_GPIOTABLE[8])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_GPIOIntClear                                                      \
        ((void (*)(unsigned long ulPort,                                      \
                   unsigned long ulIntFlags))ROM_GPIOTABLE[9])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_GPIOIntRegister                                                   \
        ((void (*)(unsigned long ulPort,                                      \
                   void (*pfnIntHandler)(void)))ROM_GPIOTABLE[10])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_GPIOIntUnregister                                                 \
        ((void (*)(unsigned long ulPort))ROM_GPIOTABLE[11])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_GPIOPinRead                                                       \
        ((long (*)(unsigned long ulPort,                                      \
                   unsigned char ucPins))ROM_GPIOTABLE[12])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_GPIOPinWrite                                                      \
        ((void (*)(unsigned long ulPort,                                      \
                   unsigned char ucPins,                                      \
                   unsigned char ucVal))ROM_GPIOTABLE[13])
#endif

//*****************************************************************************
//
// Macros for calling ROM functions in the AES API.
//
//*****************************************************************************
#if defined(TARGET_IS_CC3200)
#define ROM_AESConfigSet                                                      \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint32_t ui32Config))ROM_AESTABLE[0])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_AESKey1Set                                                        \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint8_t *pui8Key,                                          \
                   uint32_t ui32Keysize))ROM_AESTABLE[1])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_AESKey2Set                                                        \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint8_t *pui8Key,                                          \
                   uint32_t ui32Keysize))ROM_AESTABLE[2])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_AESKey3Set                                                        \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint8_t *pui8Key))ROM_AESTABLE[3])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_AESIVSet                                                          \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint8_t *pui8IVdata))ROM_AESTABLE[4])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_AESTagRead                                                        \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint8_t *pui8TagData))ROM_AESTABLE[5])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_AESDataLengthSet                                                  \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint64_t ui64Length))ROM_AESTABLE[6])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_AESAuthDataLengthSet                                              \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint32_t ui32Length))ROM_AESTABLE[7])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_AESDataReadNonBlocking                                            \
        ((bool (*)(uint32_t ui32Base,                                         \
                   uint8_t *pui8Dest,                                         \
                   uint8_t ui8Length))ROM_AESTABLE[8])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_AESDataRead                                                       \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint8_t *pui8Dest,                                         \
                   uint8_t ui8Length))ROM_AESTABLE[9])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_AESDataWriteNonBlocking                                           \
        ((bool (*)(uint32_t ui32Base,                                         \
                   uint8_t *pui8Src,                                          \
                   uint8_t ui8Length))ROM_AESTABLE[10])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_AESDataWrite                                                      \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint8_t *pui8Src,                                          \
                   uint8_t ui8Length))ROM_AESTABLE[11])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_AESDataProcess                                                    \
        ((bool (*)(uint32_t ui32Base,                                         \
                   uint8_t *pui8Src,                                          \
                   uint8_t *pui8Dest,                                         \
                   uint32_t ui32Length))ROM_AESTABLE[12])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_AESDataMAC                                                        \
        ((bool (*)(uint32_t ui32Base,                                         \
                   uint8_t *pui8Src,                                          \
                   uint32_t ui32Length,                                       \
                   uint8_t *pui8Tag))ROM_AESTABLE[13])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_AESDataProcessAE                                                  \
        ((bool (*)(uint32_t ui32Base,                                         \
                   uint8_t *pui8Src,                                          \
                   uint8_t *pui8Dest,                                         \
                   uint32_t ui32Length,                                       \
                   uint8_t *pui8AuthSrc,                                      \
                   uint32_t ui32AuthLength,                                   \
                   uint8_t *pui8Tag))ROM_AESTABLE[14])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_AESIntStatus                                                      \
        ((uint32_t (*)(uint32_t ui32Base,                                     \
                       bool bMasked))ROM_AESTABLE[15])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_AESIntEnable                                                      \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint32_t ui32IntFlags))ROM_AESTABLE[16])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_AESIntDisable                                                     \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint32_t ui32IntFlags))ROM_AESTABLE[17])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_AESIntClear                                                       \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint32_t ui32IntFlags))ROM_AESTABLE[18])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_AESIntRegister                                                    \
        ((void (*)(uint32_t ui32Base,                                         \
                   void(*pfnHandler)(void)))ROM_AESTABLE[19])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_AESIntUnregister                                                  \
        ((void (*)(uint32_t ui32Base))ROM_AESTABLE[20])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_AESDMAEnable                                                      \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint32_t ui32Flags))ROM_AESTABLE[21])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_AESDMADisable                                                     \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint32_t ui32Flags))ROM_AESTABLE[22])
#endif

//*****************************************************************************
//
// Macros for calling ROM functions in the DES API.
//
//*****************************************************************************
#if defined(TARGET_IS_CC3200)
#define ROM_DESConfigSet                                                      \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint32_t ui32Config))ROM_DESTABLE[0])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_DESDataRead                                                       \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint8_t *pui8Dest,                                         \
                   uint8_t ui8Length))ROM_DESTABLE[1])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_DESDataReadNonBlocking                                            \
        ((bool (*)(uint32_t ui32Base,                                         \
                   uint8_t *pui8Dest,                                         \
                   uint8_t ui8Length))ROM_DESTABLE[2])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_DESDataProcess                                                    \
        ((bool (*)(uint32_t ui32Base,                                         \
                   uint8_t *pui8Src,                                          \
                   uint8_t *pui8Dest,                                         \
                   uint32_t ui32Length))ROM_DESTABLE[3])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_DESDataWrite                                                      \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint8_t *pui8Src,                                          \
                   uint8_t ui8Length))ROM_DESTABLE[4])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_DESDataWriteNonBlocking                                           \
        ((bool (*)(uint32_t ui32Base,                                         \
                   uint8_t *pui8Src,                                          \
                   uint8_t ui8Length))ROM_DESTABLE[5])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_DESDMADisable                                                     \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint32_t ui32Flags))ROM_DESTABLE[6])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_DESDMAEnable                                                      \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint32_t ui32Flags))ROM_DESTABLE[7])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_DESIntClear                                                       \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint32_t ui32IntFlags))ROM_DESTABLE[8])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_DESIntDisable                                                     \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint32_t ui32IntFlags))ROM_DESTABLE[9])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_DESIntEnable                                                      \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint32_t ui32IntFlags))ROM_DESTABLE[10])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_DESIntRegister                                                    \
        ((void (*)(uint32_t ui32Base,                                         \
                   void(*pfnHandler)(void)))ROM_DESTABLE[11])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_DESIntStatus                                                      \
        ((uint32_t (*)(uint32_t ui32Base,                                     \
                       bool bMasked))ROM_DESTABLE[12])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_DESIntUnregister                                                  \
        ((void (*)(uint32_t ui32Base))ROM_DESTABLE[13])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_DESIVSet                                                          \
        ((bool (*)(uint32_t ui32Base,                                         \
                   uint8_t *pui8IVdata))ROM_DESTABLE[14])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_DESKeySet                                                         \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint8_t *pui8Key))ROM_DESTABLE[15])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_DESDataLengthSet                                                  \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint32_t ui32Length))ROM_DESTABLE[16])
#endif

//*****************************************************************************
//
// Macros for calling ROM functions in the SHAMD5 API.
//
//*****************************************************************************
#if defined(TARGET_IS_CC3200)
#define ROM_SHAMD5ConfigSet                                                   \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint32_t ui32Mode))ROM_SHAMD5TABLE[0])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SHAMD5DataProcess                                                 \
        ((bool (*)(uint32_t ui32Base,                                         \
                   uint8_t *pui8DataSrc,                                      \
                   uint32_t ui32DataLength,                                   \
                   uint8_t *pui8HashResult))ROM_SHAMD5TABLE[1])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SHAMD5DataWrite                                                   \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint8_t *pui8Src))ROM_SHAMD5TABLE[2])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SHAMD5DataWriteNonBlocking                                        \
        ((bool (*)(uint32_t ui32Base,                                         \
                   uint8_t *pui8Src))ROM_SHAMD5TABLE[3])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SHAMD5DMADisable                                                  \
        ((void (*)(uint32_t ui32Base))ROM_SHAMD5TABLE[4])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SHAMD5DMAEnable                                                   \
        ((void (*)(uint32_t ui32Base))ROM_SHAMD5TABLE[5])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SHAMD5DataLengthSet                                               \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint32_t ui32Length))ROM_SHAMD5TABLE[6])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SHAMD5HMACKeySet                                                  \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint8_t *pui8Src))ROM_SHAMD5TABLE[7])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SHAMD5HMACPPKeyGenerate                                           \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint8_t *pui8Key,                                          \
                   uint8_t *pui8PPKey))ROM_SHAMD5TABLE[8])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SHAMD5HMACPPKeySet                                                \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint8_t *pui8Src))ROM_SHAMD5TABLE[9])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SHAMD5HMACProcess                                                 \
        ((bool (*)(uint32_t ui32Base,                                         \
                   uint8_t *pui8DataSrc,                                      \
                   uint32_t ui32DataLength,                                   \
                   uint8_t *pui8HashResult))ROM_SHAMD5TABLE[10])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SHAMD5IntClear                                                    \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint32_t ui32IntFlags))ROM_SHAMD5TABLE[11])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SHAMD5IntDisable                                                  \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint32_t ui32IntFlags))ROM_SHAMD5TABLE[12])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SHAMD5IntEnable                                                   \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint32_t ui32IntFlags))ROM_SHAMD5TABLE[13])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SHAMD5IntRegister                                                 \
        ((void (*)(uint32_t ui32Base,                                         \
                   void(*pfnHandler)(void)))ROM_SHAMD5TABLE[14])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SHAMD5IntStatus                                                   \
        ((uint32_t (*)(uint32_t ui32Base,                                     \
                       bool bMasked))ROM_SHAMD5TABLE[15])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SHAMD5IntUnregister                                               \
        ((void (*)(uint32_t ui32Base))ROM_SHAMD5TABLE[16])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SHAMD5ResultRead                                                  \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint8_t *pui8Dest))ROM_SHAMD5TABLE[17])
#endif

//*****************************************************************************
//
// Macros for calling ROM functions in the CRC API.
//
//*****************************************************************************
#if defined(TARGET_IS_CC3200)
#define ROM_CRCConfigSet                                                      \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint32_t ui32CRCConfig))ROM_CRCTABLE[0])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_CRCDataProcess                                                    \
        ((uint32_t (*)(uint32_t ui32Base,                                     \
                       void *puiDataIn,                                       \
                       uint32_t ui32DataLength,                               \
                       uint32_t ui32Config))ROM_CRCTABLE[1])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_CRCDataWrite                                                      \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint32_t ui32Data))ROM_CRCTABLE[2])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_CRCResultRead                                                     \
        ((uint32_t (*)(uint32_t ui32Base))ROM_CRCTABLE[3])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_CRCSeedSet                                                        \
        ((void (*)(uint32_t ui32Base,                                         \
                   uint32_t ui32Seed))ROM_CRCTABLE[4])
#endif

//*****************************************************************************
//
// Macros for calling ROM functions in the SDHOST API.
//
//*****************************************************************************
#if defined(TARGET_IS_CC3200)
#define ROM_SDHostCmdReset                                                    \
        ((void (*)(unsigned long ulBase))ROM_SDHOSTTABLE[0])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SDHostInit                                                        \
        ((void (*)(unsigned long ulBase))ROM_SDHOSTTABLE[1])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SDHostCmdSend                                                     \
        ((long (*)(unsigned long ulBase,                                      \
                  unsigned long ulCmd,                                        \
                   unsigned ulArg))ROM_SDHOSTTABLE[2])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SDHostIntRegister                                                 \
        ((void (*)(unsigned long ulBase,                                      \
                   void (*pfnHandler)(void)))ROM_SDHOSTTABLE[3])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SDHostIntUnregister                                               \
        ((void (*)(unsigned long ulBase))ROM_SDHOSTTABLE[4])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SDHostIntEnable                                                   \
        ((void (*)(unsigned long ulBase,                                      \
                  unsigned long ulIntFlags))ROM_SDHOSTTABLE[5])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SDHostIntDisable                                                  \
        ((void (*)(unsigned long ulBase,                                      \
                  unsigned long ulIntFlags))ROM_SDHOSTTABLE[6])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SDHostIntStatus                                                   \
        ((unsigned long (*)(unsigned long ulBase))ROM_SDHOSTTABLE[7])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SDHostIntClear                                                    \
        ((void (*)(unsigned long ulBase,                                      \
                  unsigned long ulIntFlags))ROM_SDHOSTTABLE[8])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SDHostRespStatus                                                  \
        ((unsigned long (*)(unsigned long ulBase))ROM_SDHOSTTABLE[9])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SDHostRespGet                                                     \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulRespnse[4]))ROM_SDHOSTTABLE[10])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SDHostBlockSizeSet                                                \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned short ulBlkSize))ROM_SDHOSTTABLE[11])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SDHostBlockCountSet                                               \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned short ulBlkCount))ROM_SDHOSTTABLE[12])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SDHostDataNonBlockingWrite                                        \
        ((tBoolean (*)(unsigned long ulBase,                                  \
                       unsigned long ulData))ROM_SDHOSTTABLE[13])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SDHostDataNonBlockingRead                                         \
        ((tBoolean (*)(unsigned long ulBase,                                  \
                       unsigned long *pulData))ROM_SDHOSTTABLE[14])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SDHostDataWrite                                                   \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulData))ROM_SDHOSTTABLE[15])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SDHostDataRead                                                    \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long *ulData))ROM_SDHOSTTABLE[16])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_SDHostSetExpClk                                                   \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulSDHostClk,                                 \
                   unsigned long ulCardClk))ROM_SDHOSTTABLE[17])
#endif

//*****************************************************************************
//
// Macros for calling ROM functions in the PRCM API.
//
//*****************************************************************************
#if defined(TARGET_IS_CC3200)
#define ROM_PRCMMCUReset                                                      \
        ((void (*)(tBoolean bIncludeSubsystem))ROM_PRCMTABLE[1])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PRCMSysResetCauseGet                                              \
        ((unsigned long (*)(void))ROM_PRCMTABLE[2])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PRCMPeripheralClkEnable                                           \
        ((void (*)(unsigned long ulPeripheral,                                \
                   unsigned long ulClkFlags))ROM_PRCMTABLE[3])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PRCMPeripheralClkDisable                                          \
        ((void (*)(unsigned long ulPeripheral,                                \
                   unsigned long ulClkFlags))ROM_PRCMTABLE[4])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PRCMPeripheralReset                                               \
        ((void (*)(unsigned long ulPeripheral))ROM_PRCMTABLE[5])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PRCMPeripheralStatusGet                                           \
        ((tBoolean (*)(unsigned long ulPeripheral))ROM_PRCMTABLE[6])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PRCMI2SClockFreqSet                                               \
        ((void (*)(unsigned long ulI2CClkFreq))ROM_PRCMTABLE[7])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PRCMPeripheralClockGet                                            \
        ((unsigned long (*)(unsigned long ulPeripheral))ROM_PRCMTABLE[8])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PRCMSleepEnter                                                    \
        ((void (*)(void))ROM_PRCMTABLE[9])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PRCMDeepSleepEnter                                                \
        ((void (*)(void))ROM_PRCMTABLE[10])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PRCMSRAMRetentionEnable                                           \
        ((void (*)(unsigned long ulSramColSel,                                \
                   unsigned long ulFlags))ROM_PRCMTABLE[11])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PRCMSRAMRetentionDisable                                          \
        ((void (*)(unsigned long ulSramColSel,                                \
                   unsigned long ulFlags))ROM_PRCMTABLE[12])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PRCMLPDSEnter                                                     \
        ((void (*)(void))ROM_PRCMTABLE[13])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PRCMLPDSIntervalSet                                               \
        ((void (*)(unsigned long ulTicks))ROM_PRCMTABLE[14])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PRCMLPDSWakeupSourceEnable                                        \
        ((void (*)(unsigned long ulLpdsWakeupSrc))ROM_PRCMTABLE[15])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PRCMLPDSWakeupCauseGet                                            \
        ((unsigned long (*)(void))ROM_PRCMTABLE[16])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PRCMLPDSWakeUpGPIOSelect                                          \
        ((void (*)(unsigned long ulGPIOPin,                                   \
                   unsigned long ulType))ROM_PRCMTABLE[17])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PRCMLPDSWakeupSourceDisable                                       \
        ((void (*)(unsigned long ulLpdsWakeupSrc))ROM_PRCMTABLE[18])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PRCMHibernateEnter                                                \
        ((void (*)(void))ROM_PRCMTABLE[19])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PRCMHibernateWakeupSourceEnable                                   \
        ((void (*)(unsigned long ulHIBWakupSrc))ROM_PRCMTABLE[20])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PRCMHibernateWakeupCauseGet                                       \
        ((unsigned long (*)(void))ROM_PRCMTABLE[21])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PRCMHibernateWakeUpGPIOSelect                                     \
        ((void (*)(unsigned long ulMultiGPIOBitMap,                           \
                   unsigned long ulType))ROM_PRCMTABLE[22])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PRCMHibernateWakeupSourceDisable                                  \
        ((void (*)(unsigned long ulHIBWakupSrc))ROM_PRCMTABLE[23])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PRCMHibernateIntervalSet                                          \
        ((void (*)(unsigned long long ullTicks))ROM_PRCMTABLE[24])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PRCMSlowClkCtrGet                                                 \
        ((unsigned long long (*)(void))ROM_PRCMTABLE[25])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PRCMSlowClkCtrMatchSet                                            \
        ((void (*)(unsigned long long ullTicks))ROM_PRCMTABLE[26])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PRCMSlowClkCtrMatchGet                                            \
        ((unsigned long long (*)(void))ROM_PRCMTABLE[27])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PRCMOCRRegisterWrite                                              \
        ((void (*)(unsigned char ucIndex,                                     \
                   unsigned long ulRegValue))ROM_PRCMTABLE[28])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PRCMOCRRegisterRead                                               \
        ((unsigned long (*)(unsigned char ucIndex))ROM_PRCMTABLE[29])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PRCMIntRegister                                                   \
        ((void (*)(void (*pfnHandler)(void)))ROM_PRCMTABLE[30])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PRCMIntUnregister                                                 \
        ((void (*)(void))ROM_PRCMTABLE[31])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PRCMIntEnable                                                     \
        ((void (*)(unsigned long ulIntFlags))ROM_PRCMTABLE[32])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PRCMIntDisable                                                    \
        ((void (*)(unsigned long ulIntFlags))ROM_PRCMTABLE[33])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PRCMIntStatus                                                     \
        ((unsigned long (*)(void))ROM_PRCMTABLE[34])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PRCMRTCInUseSet                                                   \
        ((void (*)(void))ROM_PRCMTABLE[35])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PRCMRTCInUseGet                                                   \
        ((tBoolean (*)(void))ROM_PRCMTABLE[36])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PRCMRTCSet                                                        \
        ((void (*)(unsigned long ulSecs,                                      \
                   unsigned short usMsec))ROM_PRCMTABLE[37])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PRCMRTCGet                                                        \
        ((void (*)(unsigned long *ulSecs,                                     \
                   unsigned short *usMsec))ROM_PRCMTABLE[38])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PRCMRTCMatchSet                                                   \
        ((void (*)(unsigned long ulSecs,                                      \
                   unsigned short usMsec))ROM_PRCMTABLE[39])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PRCMRTCMatchGet                                                   \
        ((void (*)(unsigned long *ulSecs,                                     \
                   unsigned short *usMsec))ROM_PRCMTABLE[40])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_PRCMLPDSRestoreInfoSet                                            \
        ((void (*)(unsigned long ulRestoreSP,                                 \
                   unsigned long ulRestorePC))ROM_PRCMTABLE[41])
#endif

//*****************************************************************************
//
// Macros for calling ROM functions in the HWSPINLOCK API.
//
//*****************************************************************************
#if defined(TARGET_IS_CC3200)
#define ROM_HwSpinLockAcquire                                                 \
        ((void (*)(uint32_t ui32LockID))ROM_HWSPINLOCKTABLE[0])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_HwSpinLockTryAcquire                                              \
        ((int32_t (*)(uint32_t ui32LockID,                                    \
                      uint32_t ui32Retry))ROM_HWSPINLOCKTABLE[1])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_HwSpinLockRelease                                                 \
        ((void (*)(uint32_t ui32LockID))ROM_HWSPINLOCKTABLE[2])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_HwSpinLockTest                                                    \
        ((uint32_t (*)(uint32_t ui32LockID,                                   \
                       bool bCurrentStatus))ROM_HWSPINLOCKTABLE[3])
#endif

//*****************************************************************************
//
// Macros for calling ROM functions in the ADC API.
//
//*****************************************************************************
#if defined(TARGET_IS_CC3200)
#define ROM_ADCEnable                                                         \
        ((void (*)(unsigned long ulBase))ROM_ADCTABLE[0])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_ADCDisable                                                        \
        ((void (*)(unsigned long ulBase))ROM_ADCTABLE[1])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_ADCChannelEnable                                                  \
        ((void (*)(unsigned long ulBase,                                      \
                  unsigned long ulChannel))ROM_ADCTABLE[2])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_ADCChannelDisable                                                 \
        ((void (*)(unsigned long ulBase,                                      \
                  unsigned long ulChannel))ROM_ADCTABLE[3])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_ADCIntRegister                                                    \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulChannel,                                   \
                   void (*pfnHandler)(void)))ROM_ADCTABLE[4])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_ADCIntUnregister                                                  \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulChannel))ROM_ADCTABLE[5])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_ADCIntEnable                                                      \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulChannel,                                   \
                   unsigned long ulIntFlags))ROM_ADCTABLE[6])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_ADCIntDisable                                                     \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulChannel,                                   \
                   unsigned long ulIntFlags))ROM_ADCTABLE[7])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_ADCIntStatus                                                      \
        ((unsigned long (*)(unsigned long ulBase,                             \
                           unsigned long ulChannel))ROM_ADCTABLE[8])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_ADCIntClear                                                       \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulChannel,                                   \
                   unsigned long ulIntFlags))ROM_ADCTABLE[9])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_ADCDMAEnable                                                      \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulChannel))ROM_ADCTABLE[10])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_ADCDMADisable                                                     \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulChannel))ROM_ADCTABLE[11])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_ADCChannelGainSet                                                 \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulChannel,                                   \
                   unsigned char ucGain))ROM_ADCTABLE[12])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_ADCChannleGainGet                                                 \
        ((unsigned char (*)(unsigned long ulBase,                             \
                            unsigned long ulChannel))ROM_ADCTABLE[13])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_ADCTimerConfig                                                    \
        ((void (*)(unsigned long ulBase,                                      \
                   unsigned long ulValue))ROM_ADCTABLE[14])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_ADCTimerEnable                                                    \
        ((void (*)(unsigned long ulBase))ROM_ADCTABLE[15])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_ADCTimerDisable                                                   \
        ((void (*)(unsigned long ulBase))ROM_ADCTABLE[16])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_ADCTimerReset                                                     \
        ((void (*)(unsigned long ulBase))ROM_ADCTABLE[17])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_ADCTimerValueGet                                                  \
        ((unsigned long (*)(unsigned long ulBase))ROM_ADCTABLE[18])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_ADCFIFOLvlGet                                                     \
        ((unsigned char (*)(unsigned long ulBase,                             \
                            unsigned long ulChannel))ROM_ADCTABLE[19])
#endif
#if defined(TARGET_IS_CC3200)
#define ROM_ADCFIFORead                                                       \
        ((unsigned long (*)(unsigned long ulBase,                             \
                            unsigned long ulChannel))ROM_ADCTABLE[20])
#endif

#endif // __ROM_H__
