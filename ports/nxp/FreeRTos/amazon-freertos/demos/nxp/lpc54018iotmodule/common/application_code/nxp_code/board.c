/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "board.h"
#include <stdint.h>
#include "clock_config.h"
#include "fsl_common.h"
#include "fsl_debug_console.h"
#include "fsl_emc.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* The SDRAM timing. */
#define SDRAM_REFRESHPERIOD_NS (64 * 1000000 / 4096) /* 4096 rows/ 64ms */
#define SDRAM_TRP_NS (18u)
#define SDRAM_TRAS_NS (42u)
#define SDRAM_TSREX_NS (67u)
#define SDRAM_TAPR_NS (18u)
#define SDRAM_TWRDELT_NS (6u)
#define SDRAM_TRC_NS (60u)
#define SDRAM_RFC_NS (60u)
#define SDRAM_XSR_NS (67u)
#define SDRAM_RRD_NS (12u)
#define SDRAM_MRD_NCLK (2u)
#define SDRAM_RAS_NCLK (2u)
#define SDRAM_MODEREG_VALUE (0x23u)
#define SDRAM_DEV_MEMORYMAP (0x09u) /* 128Mbits (8M*16, 4banks, 12 rows, 9 columns)*/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/* Clock rate on the CLKIN pin */
const uint32_t ExtClockIn = BOARD_EXTCLKINRATE;

/*******************************************************************************
 * Code
 ******************************************************************************/
/* Initialize debug console. */
status_t BOARD_InitDebugConsole(void)
{
    status_t result;
    RESET_PeripheralReset(BOARD_DEBUG_UART_RST);
    result = DbgConsole_Init(BOARD_DEBUG_UART_BASEADDR, BOARD_DEBUG_UART_BAUDRATE, BOARD_DEBUG_UART_TYPE,
                             BOARD_DEBUG_UART_CLK_FREQ);
    assert(kStatus_Success == result);
    return result;
}

/* Initialize the external memory. */
void BOARD_InitSDRAM(void)
{
    emc_basic_config_t basicConfig;
    emc_dynamic_timing_config_t dynTiming;
    emc_dynamic_chip_config_t dynChipConfig;

    /* Basic configuration. */
    basicConfig.endian = kEMC_LittleEndian;
    basicConfig.fbClkSrc = kEMC_IntloopbackEmcclk;
    /* EMC Clock = CPU FREQ/2 here can fit CPU freq from 12M ~ 180M.
     * If you change the divide to 0 and EMC clock is larger than 100M
     * please take refer to emc.dox to adjust EMC clock delay.
     */
    basicConfig.emcClkDiv = 1;
    /* Dynamic memory timing configuration. */
    dynTiming.readConfig = kEMC_Cmddelay;
    dynTiming.refreshPeriod_Nanosec = SDRAM_REFRESHPERIOD_NS;
    dynTiming.tRp_Ns = SDRAM_TRP_NS;
    dynTiming.tRas_Ns = SDRAM_TRAS_NS;
    dynTiming.tSrex_Ns = SDRAM_TSREX_NS;
    dynTiming.tApr_Ns = SDRAM_TAPR_NS;
    dynTiming.tWr_Ns = (1000000000 / CLOCK_GetFreq(kCLOCK_EMC) + SDRAM_TWRDELT_NS); /* one clk + 6ns */
    dynTiming.tDal_Ns = dynTiming.tWr_Ns + dynTiming.tRp_Ns;
    dynTiming.tRc_Ns = SDRAM_TRC_NS;
    dynTiming.tRfc_Ns = SDRAM_RFC_NS;
    dynTiming.tXsr_Ns = SDRAM_XSR_NS;
    dynTiming.tRrd_Ns = SDRAM_RRD_NS;
    dynTiming.tMrd_Nclk = SDRAM_MRD_NCLK;
    /* Dynamic memory chip specific configuration: Chip 0 - MTL48LC8M16A2B4-6A */
    dynChipConfig.chipIndex = 0;
    dynChipConfig.dynamicDevice = kEMC_Sdram;
    dynChipConfig.rAS_Nclk = SDRAM_RAS_NCLK;
    dynChipConfig.sdramModeReg = SDRAM_MODEREG_VALUE;
    dynChipConfig.sdramExtModeReg = 0; /* it has no use for normal sdram */
    dynChipConfig.devAddrMap = SDRAM_DEV_MEMORYMAP;
    /* EMC Basic configuration. */
    EMC_Init(EMC, &basicConfig);
    /* EMC Dynamc memory configuration. */
    EMC_DynamicMemInit(EMC, &dynTiming, &dynChipConfig, 1);
}
