/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Philipp Ebensberger
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#if MICROPY_HW_SDRAM_AVAIL

#include "modmachine.h"
#include "fsl_semc.h"
#include "fsl_iomuxc.h"

// Linker symbols
extern uint8_t __sdram_start;


void mimxrt_sdram_init(void) {
    // Set Clocks
    CLOCK_InitSysPfd(kCLOCK_Pfd2, 29);   // '29' PLL2 PFD2 frequency = 528MHz * 18 / 29 = 327.72MHz (with 528MHz = PLL2 frequency)
    CLOCK_SetMux(kCLOCK_SemcAltMux, 0);  // '0'  PLL2 PFD2 will be selected as alternative clock for SEMC root clock
    CLOCK_SetMux(kCLOCK_SemcMux, 1);     // '1'  SEMC alternative clock will be used as SEMC clock root
    CLOCK_SetDiv(kCLOCK_SemcDiv, 1);     // '1'  divide by 2 -> SEMC clock = 163.86 MHz

    // Set Pins

    // Data Pins
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DATA00, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DATA00, 0xE1UL);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DATA01, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DATA01, 0xE1UL);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DATA02, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DATA02, 0xE1UL);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DATA03, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DATA03, 0xE1UL);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DATA04, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DATA04, 0xE1UL);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DATA05, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DATA05, 0xE1UL);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DATA06, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DATA06, 0xE1UL);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DATA07, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DATA07, 0xE1UL);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DATA08, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DATA08, 0xE1UL);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DATA09, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DATA09, 0xE1UL);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DATA10, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DATA10, 0xE1UL);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DATA11, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DATA11, 0xE1UL);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DATA12, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DATA12, 0xE1UL);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DATA13, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DATA13, 0xE1UL);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DATA14, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DATA14, 0xE1UL);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DATA15, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DATA15, 0xE1UL);

    // Address Pins
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_ADDR00, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_ADDR00, 0xE1UL);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_ADDR01, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_ADDR01, 0xE1UL);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_ADDR02, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_ADDR02, 0xE1UL);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_ADDR03, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_ADDR03, 0xE1UL);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_ADDR04, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_ADDR04, 0xE1UL);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_ADDR05, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_ADDR05, 0xE1UL);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_ADDR06, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_ADDR06, 0xE1UL);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_ADDR07, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_ADDR07, 0xE1UL);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_ADDR08, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_ADDR08, 0xE1UL);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_ADDR09, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_ADDR09, 0xE1UL);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_ADDR10, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_ADDR10, 0xE1UL);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_ADDR11, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_ADDR11, 0xE1UL);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_ADDR12, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_ADDR12, 0xE1UL);

    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DM00, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DM00, 0xE1UL);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_BA0, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_BA0, 0xE1UL);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_BA1, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_BA1, 0xE1UL);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_CAS, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_CAS, 0xE1UL);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_RAS, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_RAS, 0xE1UL);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_CLK, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_CLK, 0xE1UL);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_CKE, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_CKE, 0xE1UL);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_WE, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_WE, 0xE1UL);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DM01, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DM01, 0xE1UL);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DQS, 1UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DQS, 0xE1UL);

    // Chip Select Pins
    #ifndef MIMXRT_IOMUXC_SEMC_CS0
    #error No SEMC CS0 defined!
    #endif
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_CS0, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_CS0, 0xE1UL);

    #ifdef MIMXRT_IOMUXC_SEMC_CS1
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_CS1, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_CS1, 0xE1UL);
    #endif

    #ifdef MIMXRT_IOMUXC_SEMC_CS2
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_CS2, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_CS2, 0xE1UL);
    #endif

    #ifdef MIMXRT_IOMUXC_SEMC_CS3
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_CS3, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_CS3, 0xE1UL);
    #endif

    // Configure SEMC
    semc_config_t semc_cfg;
    SEMC_GetDefaultConfig(&semc_cfg);

    semc_cfg.dqsMode = kSEMC_Loopbackdqspad;  // For more accurate timing.
    SEMC_Init(SEMC, &semc_cfg);

    uint32_t clock_freq = CLOCK_GetFreq(kCLOCK_SemcClk);
    semc_sdram_config_t sdram_cfg = {
        .csxPinMux = kSEMC_MUXCSX0,
        .address = ((uint32_t)&__sdram_start),
        .memsize_kbytes = (MICROPY_HW_SDRAM_SIZE >> 10),  // Right shift by 10 == division by 1024
        .portSize = kSEMC_PortSize16Bit,
        .burstLen = kSEMC_Sdram_BurstLen1,
        .columnAddrBitNum = kSEMC_SdramColunm_9bit,
        .casLatency = kSEMC_LatencyThree,
        .tPrecharge2Act_Ns = 18,  // Trp 18ns
        .tAct2ReadWrite_Ns = 18,  // Trcd 18ns
        .tRefreshRecovery_Ns = (60 + 67),
        .tWriteRecovery_Ns = 12,  // 12ns
        .tCkeOff_Ns = 42,  // The minimum cycle of SDRAM CLK off state. CKE is off in self refresh at a minimum period tRAS.
        .tAct2Prechage_Ns = 42,  // Tras 42ns
        .tSelfRefRecovery_Ns = 67,
        .tRefresh2Refresh_Ns = 60,
        .tAct2Act_Ns = 60,
        .tPrescalePeriod_Ns = 160 * (1000000000 / clock_freq),
        .tIdleTimeout_Ns = 0UL,
        .refreshPeriod_nsPerRow = 64 * 1000000 / 8192,  // 64ms/8192
        .refreshUrgThreshold = 64 * 1000000 / 8192,  // 64ms/8192
        .refreshBurstLen = 1
    };

    (status_t)SEMC_ConfigureSDRAM(SEMC, kSEMC_SDRAM_CS0, &sdram_cfg, clock_freq);
}

#endif
