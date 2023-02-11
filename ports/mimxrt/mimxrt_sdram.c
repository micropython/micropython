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
#include "fsl_clock.h"

// Linker symbols
extern uint8_t __sdram_start;

#if defined(MIMXRT117x_SERIES)
// Pull Up, High drive strength
#define SDRAM_PIN_CONFIG  (0x07UL)
#else
// Pull up 22K, high slew rate
#define SDRAM_PIN_CONFIG  (0xE1UL)
#endif

void mimxrt_sdram_init(void) {

    #if !defined(MIMXRT117x_SERIES)
    // Set Clocks
    CLOCK_InitSysPfd(kCLOCK_Pfd2, 29);   // '29' PLL2 PFD2 frequency = 528MHz * 18 / 29 = 327.72MHz (with 528MHz = PLL2 frequency)
    CLOCK_SetMux(kCLOCK_SemcAltMux, 0);  // '0'  PLL2 PFD2 will be selected as alternative clock for SEMC root clock
    CLOCK_SetMux(kCLOCK_SemcMux, 1);     // '1'  SEMC alternative clock will be used as SEMC clock root
    CLOCK_SetDiv(kCLOCK_SemcDiv, 1);     // '1'  divide by 2 -> SEMC clock = 163.86 MHz
    #endif

    // Set Pins

    // Data Pins 0..15
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DATA00, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DATA00, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DATA01, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DATA01, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DATA02, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DATA02, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DATA03, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DATA03, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DATA04, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DATA04, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DATA05, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DATA05, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DATA06, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DATA06, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DATA07, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DATA07, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DATA08, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DATA08, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DATA09, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DATA09, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DATA10, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DATA10, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DATA11, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DATA11, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DATA12, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DATA12, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DATA13, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DATA13, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DATA14, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DATA14, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DATA15, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DATA15, SDRAM_PIN_CONFIG);

    // Address Pins
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_ADDR00, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_ADDR00, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_ADDR01, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_ADDR01, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_ADDR02, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_ADDR02, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_ADDR03, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_ADDR03, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_ADDR04, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_ADDR04, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_ADDR05, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_ADDR05, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_ADDR06, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_ADDR06, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_ADDR07, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_ADDR07, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_ADDR08, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_ADDR08, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_ADDR09, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_ADDR09, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_ADDR10, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_ADDR10, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_ADDR11, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_ADDR11, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_ADDR12, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_ADDR12, SDRAM_PIN_CONFIG);

    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DM00, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DM00, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_BA0, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_BA0, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_BA1, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_BA1, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_CAS, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_CAS, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_RAS, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_RAS, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_CLK, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_CLK, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_CKE, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_CKE, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_WE, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_WE, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DM01, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DM01, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DQS, 1UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DQS, SDRAM_PIN_CONFIG);

    #if defined(MIMXRT117x_SERIES)
    // Data Pins 16..31
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DATA16, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DATA16, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DATA17, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DATA17, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DATA18, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DATA18, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DATA19, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DATA19, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DATA20, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DATA20, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DATA21, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DATA21, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DATA22, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DATA22, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DATA23, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DATA23, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DATA24, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DATA24, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DATA25, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DATA25, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DATA26, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DATA26, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DATA27, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DATA27, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DATA28, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DATA28, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DATA29, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DATA29, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DATA30, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DATA30, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DATA31, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DATA31, SDRAM_PIN_CONFIG);

    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DM02, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DM02, SDRAM_PIN_CONFIG);
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_DM03, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_DM03, SDRAM_PIN_CONFIG);

    #endif

    // Chip Select Pins
    #ifndef MIMXRT_IOMUXC_SEMC_CS0
    #error No SEMC CS0 defined!
    #endif
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_CS0, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_CS0, SDRAM_PIN_CONFIG);

    #ifdef MIMXRT_IOMUXC_SEMC_CS1
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_CS1, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_CS1, SDRAM_PIN_CONFIG);
    #endif

    #ifdef MIMXRT_IOMUXC_SEMC_CS2
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_CS2, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_CS2, SDRAM_PIN_CONFIG);
    #endif

    #ifdef MIMXRT_IOMUXC_SEMC_CS3
    IOMUXC_SetPinMux(MIMXRT_IOMUXC_SEMC_CS3, 0UL);
    IOMUXC_SetPinConfig(MIMXRT_IOMUXC_SEMC_CS3, SDRAM_PIN_CONFIG);
    #endif

    // Configure SEMC
    semc_config_t semc_cfg;
    SEMC_GetDefaultConfig(&semc_cfg);

    semc_cfg.dqsMode = kSEMC_Loopbackdqspad;  // For more accurate timing.
    SEMC_Init(SEMC, &semc_cfg);

    #if defined(MIMXRT117x_SERIES)

    uint32_t clock_freq = CLOCK_GetRootClockFreq(kCLOCK_Root_Semc);

    semc_sdram_config_t sdram_cfg = {
        .csxPinMux = kSEMC_MUXCSX0,
        .address = 0x80000000,
        .memsize_kbytes = (MICROPY_HW_SDRAM_SIZE >> 10),
        .portSize = kSEMC_PortSize32Bit, // two 16-bit SDRAMs make up 32-bit portsize
        .burstLen = kSEMC_Sdram_BurstLen8,
        .columnAddrBitNum = kSEMC_SdramColunm_9bit,
        .casLatency = kSEMC_LatencyThree,
        .tPrecharge2Act_Ns = 15, // tRP 15ns
        .tAct2ReadWrite_Ns = 15, // tRCD 15ns
        .tRefreshRecovery_Ns = 70, // Use the maximum of the (Trfc , Txsr).
        .tWriteRecovery_Ns = 2,  // tWR 2ns
        .tCkeOff_Ns = 42, // The minimum cycle of SDRAM CLK off state. CKE is off in self refresh at a minimum period tRAS.
        .tAct2Prechage_Ns = 40, // tRAS 40ns
        .tSelfRefRecovery_Ns = 70,
        .tRefresh2Refresh_Ns = 60,
        .tAct2Act_Ns = 2, // tRC/tRDD 2ns
        .tPrescalePeriod_Ns = 160 * (1000000000 / clock_freq),
        .refreshPeriod_nsPerRow = 64 * 1000000 / 8192, // 64ms/8192
        .refreshUrgThreshold = sdram_cfg.refreshPeriod_nsPerRow,
        .refreshBurstLen = 1,
        .delayChain = 2,
    };

    #else

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

    #endif

    (status_t)SEMC_ConfigureSDRAM(SEMC, kSEMC_SDRAM_CS0, &sdram_cfg, clock_freq);
}

#endif
