/*
** ###################################################################
**     Processors:          MCXN947VDF_cm33_core0
**                          MCXN947VNL_cm33_core0
**
**     Compilers:           GNU C Compiler
**                          IAR ANSI C/C++ Compiler for ARM
**                          Keil ARM C/C++ Compiler
**                          MCUXpresso Compiler
**
**     Reference manual:    MCXNx4x Reference Manual
**     Version:             rev. 2.0, 2023-02-01
**     Build:               b231120
**
**     Abstract:
**         Provides a system configuration function and a global variable that
**         contains the system frequency. It configures the device and initializes
**         the oscillator (PLL) that is part of the microcontroller device.
**
**     Copyright 2016 Freescale Semiconductor, Inc.
**     Copyright 2016-2023 NXP
**     SPDX-License-Identifier: BSD-3-Clause
**
**     http:                 www.nxp.com
**     mail:                 support@nxp.com
**
**     Revisions:
**     - rev. 1.0 (2022-10-01)
**         Initial version
**     - rev. 2.0 (2023-02-01)
**         Initial version based on Rev. 2 Draft B
**
** ###################################################################
*/

/*!
 * @file MCXN947_cm33_core0
 * @version 2.0
 * @date 2023-02-01
 * @brief Device specific configuration file for MCXN947_cm33_core0
 *        (implementation file)
 *
 * Provides a system configuration function and a global variable that contains
 * the system frequency. It configures the device and initializes the oscillator
 * (PLL) that is part of the microcontroller device.
 */

#include <stdint.h>
#include "fsl_device_registers.h"






/* ----------------------------------------------------------------------------
   -- Core clock
   ---------------------------------------------------------------------------- */

uint32_t SystemCoreClock = DEFAULT_SYSTEM_CLOCK;

/* ----------------------------------------------------------------------------
   -- SystemInit()
   ---------------------------------------------------------------------------- */

__attribute__ ((weak)) void SystemInit(void) {
    #if ((__FPU_PRESENT == 1) && (__FPU_USED == 1))
    SCB->CPACR |= ((3UL << 10 * 2) | (3UL << 11 * 2)); /* set CP10, CP11 Full Access in Secure mode */
    #if defined(__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
    SCB_NS->CPACR |= ((3UL << 10 * 2) | (3UL << 11 * 2)); /* set CP10, CP11 Full Access in Non-secure mode */
    #endif /* (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U) */
    #endif /* ((__FPU_PRESENT == 1) && (__FPU_USED == 1)) */


    SCB->CPACR |= ((3UL << 0 * 2) | (3UL << 1 * 2)); /* set CP0, CP1 Full Access in Secure mode (enable PowerQuad) */
    #if defined(__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
    SCB_NS->CPACR |= ((3UL << 0 * 2) | (3UL << 1 * 2)); /* set CP0, CP1 Full Access in Normal mode (enable PowerQuad) */
    #endif /* (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U) */

    SCB->NSACR |= ((3UL << 0) | (3UL << 10)); /* enable CP0, CP1, CP10, CP11 Non-secure Access */

    SYSCON->ECC_ENABLE_CTRL = 0; /* disable RAM ECC to get max RAM size */

    // SYSCON->NVM_CTRL &= ~SYSCON_NVM_CTRL_DIS_MBECC_ERR_DATA_MASK; /* enables bus error on multi-bit ECC error for data */

    #if defined(__MCUXPRESSO)
    extern void(*const g_pfnVectors[]) (void);
    SCB->VTOR = (uint32_t)&g_pfnVectors;
    #else
    extern void *__Vectors;
    SCB->VTOR = (uint32_t)&__Vectors;
    #endif
    /* enable the flash cache LPCAC */
    SYSCON->LPCAC_CTRL &= ~SYSCON_LPCAC_CTRL_DIS_LPCAC_MASK;

    /* Disable aGDET trigger the CHIP_RESET */
    ITRC0->OUT_SEL[4][0] = (ITRC0->OUT_SEL[4][0] & ~ITRC_OUTX_SEL_OUTX_SELY_OUT_SEL_IN9_SELn_MASK) | (ITRC_OUTX_SEL_OUTX_SELY_OUT_SEL_IN9_SELn(0x2));
    ITRC0->OUT_SEL[4][1] = (ITRC0->OUT_SEL[4][1] & ~ITRC_OUTX_SEL_OUTX_SELY_OUT_SEL_IN9_SELn_MASK) | (ITRC_OUTX_SEL_OUTX_SELY_OUT_SEL_IN9_SELn(0x2));
    /* Disable aGDET interrupt and reset */
    SPC0->ACTIVE_CFG |= SPC_ACTIVE_CFG_GLITCH_DETECT_DISABLE_MASK;
    SPC0->VDD_CORE_GLITCH_DETECT_SC &= ~SPC_VDD_CORE_GLITCH_DETECT_SC_LOCK_MASK;
    SPC0->VDD_CORE_GLITCH_DETECT_SC = 0x3C;

    /* Disable dGDET trigger the CHIP_RESET */
    ITRC0->OUT_SEL[4][0] = (ITRC0->OUT_SEL[4][0] & ~ITRC_OUTX_SEL_OUTX_SELY_OUT_SEL_IN0_SELn_MASK) | (ITRC_OUTX_SEL_OUTX_SELY_OUT_SEL_IN0_SELn(0x2));
    ITRC0->OUT_SEL[4][1] = (ITRC0->OUT_SEL[4][1] & ~ITRC_OUTX_SEL_OUTX_SELY_OUT_SEL_IN0_SELn_MASK) | (ITRC_OUTX_SEL_OUTX_SELY_OUT_SEL_IN0_SELn(0x2));
    GDET0->GDET_ENABLE1 = 0;
    GDET1->GDET_ENABLE1 = 0;

    SystemInitHook();
}

/* ----------------------------------------------------------------------------
   -- SystemCoreClockUpdate()
   ---------------------------------------------------------------------------- */

void SystemCoreClockUpdate(void) {



}

/* ----------------------------------------------------------------------------
   -- SystemInitHook()
   ---------------------------------------------------------------------------- */

__attribute__ ((weak)) void SystemInitHook(void) {
    /* Void implementation of the weak function. */
}
