//*****************************************************************************
//
//  prcm.h
//
//  Prototypes for the PRCM control driver.
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

#ifndef __PRCM_H__
#define __PRCM_H__

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// Peripheral clock and reset control registers
//
//*****************************************************************************
typedef struct _PRCM_PeripheralRegs_
{

unsigned char ulClkReg;
unsigned char ulRstReg;

}PRCM_PeriphRegs_t;

//*****************************************************************************
// Values that can be passed to PRCMPeripheralEnable() and
// PRCMPeripheralDisable()
//*****************************************************************************
#define PRCM_RUN_MODE_CLK         0x00000001
#define PRCM_SLP_MODE_CLK         0x00000100
#define PRCM_DSLP_MODE_CLK        0x00010000

//*****************************************************************************
// Values that can be passed to PRCMSRAMRetentionEnable() and
// PRCMSRAMRetentionDisable() as ulSramColSel.
//*****************************************************************************
#define PRCM_SRAM_COL_1           0x00000001
#define PRCM_SRAM_COL_2           0x00000002
#define PRCM_SRAM_COL_3           0x00000004
#define PRCM_SRAM_COL_4           0x00000008

//*****************************************************************************
// Values that can be passed to PRCMSRAMRetentionEnable() and
// PRCMSRAMRetentionDisable() as ulModeFlags.
//*****************************************************************************
#define PRCM_SRAM_DSLP_RET        0x00000001
#define PRCM_SRAM_LPDS_RET        0x00000002

//*****************************************************************************
// Values that can be passed to PRCMLPDSWakeupSourceEnable(),
// PRCMLPDSWakeupCauseGet() and PRCMLPDSWakeupSourceDisable().
//*****************************************************************************
#define PRCM_LPDS_HOST_IRQ        0x00000080
#define PRCM_LPDS_GPIO            0x00000010
#define PRCM_LPDS_TIMER           0x00000001

//*****************************************************************************
// Values that can be passed to PRCMLPDSWakeUpGPIOSelect() as Type
//*****************************************************************************
#define PRCM_LPDS_LOW_LEVEL       0x00000002
#define PRCM_LPDS_HIGH_LEVEL      0x00000000
#define PRCM_LPDS_FALL_EDGE       0x00000001
#define PRCM_LPDS_RISE_EDGE       0x00000003

//*****************************************************************************
// Values that can be passed to PRCMLPDSWakeUpGPIOSelect()
//*****************************************************************************
#define PRCM_LPDS_GPIO2           0x00000000
#define PRCM_LPDS_GPIO4           0x00000001
#define PRCM_LPDS_GPIO13          0x00000002
#define PRCM_LPDS_GPIO17          0x00000003
#define PRCM_LPDS_GPIO11          0x00000004
#define PRCM_LPDS_GPIO24          0x00000005
#define PRCM_LPDS_GPIO26          0x00000006

//*****************************************************************************
// Values that can be passed to PRCMHibernateWakeupSourceEnable(),
// PRCMHibernateWakeupSourceDisable().
//*****************************************************************************
#define PRCM_HIB_SLOW_CLK_CTR     0x00000001

//*****************************************************************************
// Values that can be passed to PRCMHibernateWakeUpGPIOSelect() as ulType
//*****************************************************************************
#define PRCM_HIB_LOW_LEVEL        0x00000000
#define PRCM_HIB_HIGH_LEVEL       0x00000001
#define PRCM_HIB_FALL_EDGE        0x00000002
#define PRCM_HIB_RISE_EDGE        0x00000003

//*****************************************************************************
// Values that can be passed to PRCMHibernateWakeupSourceEnable(),
// PRCMHibernateWakeupSourceDisable(), PRCMHibernateWakeUpGPIOSelect()
//*****************************************************************************
#define PRCM_HIB_GPIO2            0x00010000
#define PRCM_HIB_GPIO4            0x00020000
#define PRCM_HIB_GPIO13           0x00040000
#define PRCM_HIB_GPIO17           0x00080000
#define PRCM_HIB_GPIO11           0x00100000
#define PRCM_HIB_GPIO24           0x00200000
#define PRCM_HIB_GPIO26           0x00400000

//*****************************************************************************
// Values that will be returned from PRCMSysResetCauseGet().
//*****************************************************************************
#define PRCM_POWER_ON             0x00000000
#define PRCM_LPDS_EXIT            0x00000001
#define PRCM_CORE_RESET           0x00000003
#define PRCM_MCU_RESET            0x00000004
#define PRCM_WDT_RESET            0x00000005
#define PRCM_SOC_RESET            0x00000006
#define PRCM_HIB_EXIT             0x00000007

//*****************************************************************************
// Values that can be passed to PRCMHibernateWakeupCauseGet().
//*****************************************************************************
#define PRCM_HIB_WAKEUP_CAUSE_SLOW_CLOCK  0x00000002
#define PRCM_HIB_WAKEUP_CAUSE_GPIO        0x00000004

//*****************************************************************************
// Values that can be passed to PRCMIntEnable
//*****************************************************************************
#define PRCM_INT_SLOW_CLK_CTR     0x00004000

//*****************************************************************************
// Values that can be passed to PRCMPeripheralClkEnable(),
// PRCMPeripheralClkDisable(), PRCMPeripheralReset()
//*****************************************************************************
#define PRCM_CAMERA               0x00000000
#define PRCM_I2S                  0x00000001
#define PRCM_SDHOST               0x00000002
#define PRCM_GSPI                 0x00000003
#define PRCM_LSPI                 0x00000004
#define PRCM_UDMA                 0x00000005
#define PRCM_GPIOA0               0x00000006
#define PRCM_GPIOA1               0x00000007
#define PRCM_GPIOA2               0x00000008
#define PRCM_GPIOA3               0x00000009
#define PRCM_GPIOA4               0x0000000A
#define PRCM_WDT                  0x0000000B
#define PRCM_UARTA0               0x0000000C
#define PRCM_UARTA1               0x0000000D
#define PRCM_TIMERA0              0x0000000E
#define PRCM_TIMERA1              0x0000000F
#define PRCM_TIMERA2              0x00000010
#define PRCM_TIMERA3              0x00000011
#define PRCM_DTHE                 0x00000012
#define PRCM_SSPI                 0x00000013
#define PRCM_I2CA0                0x00000014
// Note : PRCM_ADC is a dummy define for pinmux utility code generation
// PRCM_ADC should never be used in any user code.
#define PRCM_ADC                  0x000000FF

//*****************************************************************************
// User bits in the PRCM persistent registers
//*****************************************************************************
#define PRCM_SAFE_BOOT_BIT              30
#define PRCM_WDT_RESET_BIT              29
#define PRCM_FIRST_BOOT_BIT             28

//*****************************************************************************
//
// API Function prototypes
//
//*****************************************************************************
extern void PRCMSetSpecialBit(unsigned char bit);
extern void PRCMClearSpecialBit(unsigned char bit);
extern tBoolean PRCMGetSpecialBit(unsigned char bit);
extern void PRCMSOCReset(void);
extern void PRCMMCUReset(tBoolean bIncludeSubsystem);
extern unsigned long PRCMSysResetCauseGet(void);

extern void PRCMPeripheralClkEnable(unsigned long ulPeripheral,
                                    unsigned long ulClkFlags);
extern void PRCMPeripheralClkDisable(unsigned long ulPeripheral,
                                     unsigned long ulClkFlags);
extern void PRCMPeripheralReset(unsigned long ulPeripheral);
extern tBoolean PRCMPeripheralStatusGet(unsigned long ulPeripheral);

extern void PRCMI2SClockFreqSet(unsigned long ulI2CClkFreq);
extern unsigned long PRCMPeripheralClockGet(unsigned long ulPeripheral);

extern void PRCMSleepEnter(void);
extern void PRCMDeepSleepEnter(void);

extern void PRCMSRAMRetentionEnable(unsigned long ulSramColSel,
                                    unsigned long ulFlags);
extern void PRCMSRAMRetentionDisable(unsigned long ulSramColSel,
                                     unsigned long ulFlags);
extern void PRCMLPDSRestoreInfoSet(unsigned long ulRestoreSP,
                                   unsigned long ulRestorePC);
extern void PRCMLPDSEnter(void);
extern void PRCMLPDSIntervalSet(unsigned long ulTicks);
extern void PRCMLPDSWakeupSourceEnable(unsigned long ulLpdsWakeupSrc);
extern unsigned long PRCMLPDSWakeupCauseGet(void);
extern void PRCMLPDSWakeUpGPIOSelect(unsigned long ulGPIOPin,
                                     unsigned long ulType);
extern void PRCMLPDSWakeupSourceDisable(unsigned long ulLpdsWakeupSrc);

extern void PRCMHibernateEnter(void);
extern void PRCMHibernateWakeupSourceEnable(unsigned long ulHIBWakupSrc);
extern unsigned long PRCMHibernateWakeupCauseGet(void);
extern void PRCMHibernateWakeUpGPIOSelect(unsigned long ulMultiGPIOBitMap,
                                          unsigned long ulType);
extern void PRCMHibernateWakeupSourceDisable(unsigned long ulHIBWakupSrc);
extern void PRCMHibernateIntervalSet(unsigned long long ullTicks);

extern unsigned long long PRCMSlowClkCtrGet(void);
extern unsigned long long PRCMSlowClkCtrFastGet(void);
extern void PRCMSlowClkCtrMatchSet(unsigned long long ullTicks);
extern unsigned long long PRCMSlowClkCtrMatchGet(void);

extern void PRCMOCRRegisterWrite(unsigned char ucIndex,
                                 unsigned long ulRegValue);
extern unsigned long PRCMOCRRegisterRead(unsigned char ucIndex);

extern void PRCMIntRegister(void (*pfnHandler)(void));
extern void PRCMIntUnregister(void);
extern void PRCMIntEnable(unsigned long ulIntFlags);
extern void PRCMIntDisable(unsigned long ulIntFlags);
extern unsigned long PRCMIntStatus(void);
extern void PRCMRTCInUseSet(void);
extern void PRCMRTCInUseClear(void);
extern tBoolean PRCMRTCInUseGet(void);
extern void PRCMRTCSet(unsigned long ulSecs, unsigned short usMsec);
extern void PRCMRTCGet(unsigned long *ulSecs, unsigned short *usMsec);
extern void PRCMRTCMatchSet(unsigned long ulSecs, unsigned short usMsec);
extern void PRCMRTCMatchGet(unsigned long *ulSecs, unsigned short *usMsec);
extern void PRCMCC3200MCUInit(void);
extern unsigned long PRCMHIBRegRead(unsigned long ulRegAddr);
extern void PRCMHIBRegWrite(unsigned long ulRegAddr, unsigned long ulValue);
extern unsigned long PRCMCameraFreqSet(unsigned char ulDivider, unsigned char ulWidth);


//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif //  __PRCM_H__
