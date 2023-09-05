/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Ibrahim Abdelkader <iabdalkader@openmv.io>
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

#include <stdio.h>
#include "py/mperrno.h"
#include "py/mphal.h"
#include "pin.h"
#include "pendsv.h"

#if MICROPY_PY_NETWORK_CYW43

#include "fsl_usdhc.h"
#include "fsl_iomuxc.h"

#if MICROPY_HW_SDIO_SDMMC == 1
#define SDMMC                       USDHC1
#define SDMMC_IRQn                  USDHC1_IRQn
#ifdef MIMXRT117x_SERIES
#define SDMMC_CLOCK_MUX             kCLOCK_USDHC1_ClockRoot_MuxSysPll2Pfd2
#define SDMMC_CLOCK_ROOT            kCLOCK_Root_Usdhc1
#else
#define SDMMC_CLOCK_DIV             kCLOCK_Usdhc1Div
#define SDMMC_CLOCK_MUX             kCLOCK_Usdhc1Mux
#define SDMMC_CLOCK_ROOT            kCLOCK_Usdhc1ClkRoot
#endif
#ifndef MICROPY_HW_SDIO_CLK_ALT
#define MICROPY_HW_SDIO_CMD_ALT     (0)
#define MICROPY_HW_SDIO_CLK_ALT     (0)
#define MICROPY_HW_SDIO_D0_ALT      (0)
#define MICROPY_HW_SDIO_D1_ALT      (0)
#define MICROPY_HW_SDIO_D2_ALT      (0)
#define MICROPY_HW_SDIO_D3_ALT      (0)
#endif
#else
#define SDMMC                       USDHC2
#define SDMMC_IRQn                  USDHC2_IRQn
#ifdef MIMXRT117x_SERIES
#define SDMMC_CLOCK_MUX             kCLOCK_USDHC2_ClockRoot_MuxSysPll2Pfd2
#define SDMMC_CLOCK_ROOT            kCLOCK_Root_Usdhc2
#else
#define SDMMC_CLOCK_DIV             kCLOCK_Usdhc2Div
#define SDMMC_CLOCK_MUX             kCLOCK_Usdhc2Mux
#define SDMMC_CLOCK_ROOT            kCLOCK_Usdhc2ClkRoot
#endif
#ifndef MICROPY_HW_SDIO_CLK_ALT
#define MICROPY_HW_SDIO_CMD_ALT     (6)
#define MICROPY_HW_SDIO_CLK_ALT     (6)
#define MICROPY_HW_SDIO_D0_ALT      (6)
#define MICROPY_HW_SDIO_D1_ALT      (6)
#define MICROPY_HW_SDIO_D2_ALT      (6)
#define MICROPY_HW_SDIO_D3_ALT      (6)
#endif
#endif

#define SDMMC_CLOCK_400KHZ      (400000U)
#define SDMMC_CLOCK_25MHZ       (25000000U)
#define SDMMC_CLOCK_50MHZ       (50000000U)

#if SDIO_DEBUG
#define debug_printf(...) mp_printf(&mp_plat_print, __VA_ARGS__)
#else
#define debug_printf(...)
#endif

#define DMA_DESCRIPTOR_BUFFER_SIZE (32U)
AT_NONCACHEABLE_SECTION_ALIGN(
    static uint32_t sdio_adma_descriptor_table[DMA_DESCRIPTOR_BUFFER_SIZE], USDHC_ADMA2_ADDRESS_ALIGN);

typedef struct _mimxrt_sdmmc_t {
    USDHC_Type *inst;
    usdhc_handle_t handle;
    volatile uint32_t xfer_flags;
    volatile uint32_t xfer_error;
} mimxrt_sdmmc_t;

static mimxrt_sdmmc_t sdmmc = {
    .inst = SDMMC,
};

typedef enum {
    SDIO_TRANSFER_DATA_COMPLETE = (1 << 0),
    SDIO_TRANSFER_CMD_COMPLETE  = (1 << 1),
    SDIO_TRANSFER_ERROR         = (1 << 2),
} sdio_xfer_flags_t;

static uint32_t sdio_base_clk(void) {
    #ifdef MIMXRT117x_SERIES
    return CLOCK_GetRootClockFreq(SDMMC_CLOCK_ROOT);
    #else
    return CLOCK_GetClockRootFreq(SDMMC_CLOCK_ROOT);
    #endif
}

static uint32_t sdio_response_type(uint32_t cmd) {
    switch (cmd) {
        case 3:
            return kCARD_ResponseTypeR6;
        case 5:
            return kCARD_ResponseTypeR4;
        case 7:
            return kCARD_ResponseTypeR1;
        case 52:
            return kCARD_ResponseTypeR5;
        default:
            return kCARD_ResponseTypeNone;
    }
}

static void sdio_transfer_callback(USDHC_Type *base,
    usdhc_handle_t *handle, status_t status, void *userData) {
    if (status == kStatus_USDHC_TransferDataComplete) {
        sdmmc.xfer_flags |= SDIO_TRANSFER_DATA_COMPLETE;
    } else if (status == kStatus_USDHC_SendCommandSuccess) {
        sdmmc.xfer_flags |= SDIO_TRANSFER_CMD_COMPLETE;
    } else if (status != kStatus_USDHC_BusyTransferring) {
        sdmmc.xfer_error = status;
        sdmmc.xfer_flags |= SDIO_TRANSFER_ERROR;
    }
}

static void sdio_interrupt_callback(USDHC_Type *base, void *userData) {
    extern void (*cyw43_poll)(void);

    USDHC_DisableInterruptSignal(base, kUSDHC_CardInterruptFlag);
    USDHC_ClearInterruptStatusFlags(base, kUSDHC_CardInterruptFlag);

    if (cyw43_poll) {
        pendsv_schedule_dispatch(PENDSV_DISPATCH_CYW43, cyw43_poll);
    }
}

void sdio_init(uint32_t irq_pri) {
    machine_pin_config(MICROPY_HW_SDIO_CMD, PIN_MODE_ALT, PIN_PULL_UP_100K, PIN_DRIVE_6, 0, MICROPY_HW_SDIO_CMD_ALT);
    machine_pin_config(MICROPY_HW_SDIO_CLK, PIN_MODE_ALT, PIN_PULL_DISABLED, PIN_DRIVE_6, 0, MICROPY_HW_SDIO_CLK_ALT);
    machine_pin_config(MICROPY_HW_SDIO_D0, PIN_MODE_ALT, PIN_PULL_UP_100K, PIN_DRIVE_6, 0, MICROPY_HW_SDIO_D0_ALT);
    machine_pin_config(MICROPY_HW_SDIO_D1, PIN_MODE_ALT, PIN_PULL_UP_100K, PIN_DRIVE_6, 0, MICROPY_HW_SDIO_D1_ALT);
    machine_pin_config(MICROPY_HW_SDIO_D2, PIN_MODE_ALT, PIN_PULL_UP_100K, PIN_DRIVE_6, 0, MICROPY_HW_SDIO_D2_ALT);
    machine_pin_config(MICROPY_HW_SDIO_D3, PIN_MODE_ALT, PIN_PULL_UP_100K, PIN_DRIVE_6, 0, MICROPY_HW_SDIO_D3_ALT);

    #ifdef MIMXRT117x_SERIES
    CLOCK_InitPfd(kCLOCK_PllSys2, kCLOCK_Pfd2, 24);

    clock_root_config_t rootCfg = { 0 };
    rootCfg.mux = SDMMC_CLOCK_MUX;
    rootCfg.div = 2;
    CLOCK_SetRootClock(SDMMC_CLOCK_ROOT, &rootCfg);
    #else
    // Configure PFD0 of PLL2 (system PLL) fractional divider to 24 resulting in:
    //  with PFD0_clk = PLL2_clk * 18 / N
    //       PFD0_clk = 528MHz   * 18 / 24 = 396MHz
    CLOCK_InitSysPfd(kCLOCK_Pfd0, 24U);
    CLOCK_SetDiv(SDMMC_CLOCK_DIV, 1U);  // USDHC_input_clk = PFD0_clk / 2
    CLOCK_SetMux(SDMMC_CLOCK_MUX, 1U);  // Select PFD0 as clock input for USDHC
    #endif

    // Initialize USDHC
    const usdhc_config_t config = {
        .endianMode = kUSDHC_EndianModeLittle,
        .dataTimeout = 0xFU,
        #ifndef MIMXRT117x_SERIES
        .readBurstLen = 0,
        .writeBurstLen = 0,
        #endif
        .readWatermarkLevel = 128U,
        .writeWatermarkLevel = 128U,
    };

    USDHC_Init(sdmmc.inst, &config);
    USDHC_Reset(SDMMC, kUSDHC_ResetAll, 1000U);
    USDHC_DisableInterruptSignal(SDMMC, kUSDHC_AllInterruptFlags);
    USDHC_SetSdClock(sdmmc.inst, sdio_base_clk(), SDMMC_CLOCK_25MHZ);
    USDHC_SetDataBusWidth(sdmmc.inst, kUSDHC_DataBusWidth1Bit);

    mp_hal_delay_ms(10);

    NVIC_SetPriority(SDMMC_IRQn, irq_pri);
    EnableIRQ(SDMMC_IRQn);

    usdhc_transfer_callback_t callbacks = {
        .SdioInterrupt = sdio_interrupt_callback,
        .TransferComplete = sdio_transfer_callback,
    };
    USDHC_TransferCreateHandle(sdmmc.inst, &sdmmc.handle, &callbacks, NULL);
}

void sdio_deinit(void) {
}

void sdio_reenable(void) {
}

void sdio_enable_irq(bool enable) {
    if (enable) {
        USDHC_ClearInterruptStatusFlags(sdmmc.inst, kUSDHC_CardInterruptFlag);
        USDHC_EnableInterruptStatus(sdmmc.inst, kUSDHC_CardInterruptFlag);
        USDHC_EnableInterruptSignal(sdmmc.inst, kUSDHC_CardInterruptFlag);
    } else {
        USDHC_DisableInterruptStatus(sdmmc.inst, kUSDHC_CardInterruptFlag);
        USDHC_ClearInterruptStatusFlags(sdmmc.inst, kUSDHC_CardInterruptFlag);
        USDHC_DisableInterruptSignal(sdmmc.inst, kUSDHC_CardInterruptFlag);
    }
}

void sdio_enable_high_speed_4bit(void) {
    USDHC_SetSdClock(sdmmc.inst, sdio_base_clk(), SDMMC_CLOCK_50MHZ);
    USDHC_SetDataBusWidth(sdmmc.inst, kUSDHC_DataBusWidth4Bit);
}

static status_t sdio_transfer_dma(USDHC_Type *base,
    usdhc_handle_t *handle, usdhc_transfer_t *transfer, uint32_t timeout_ms) {
    status_t status;
    usdhc_adma_config_t dma_config = {
        .dmaMode = kUSDHC_DmaModeAdma2,
        #if !FSL_FEATURE_USDHC_HAS_NO_RW_BURST_LEN
        .burstLen = kUSDHC_EnBurstLenForINCR,
        #endif
        .admaTable = sdio_adma_descriptor_table,
        .admaTableWords = DMA_DESCRIPTOR_BUFFER_SIZE,
    };

    sdmmc.xfer_flags = 0;
    sdmmc.xfer_error = 0;

    uint32_t xfer_flags = SDIO_TRANSFER_CMD_COMPLETE;
    if (transfer->data != NULL) {
        xfer_flags |= SDIO_TRANSFER_DATA_COMPLETE;
    }

    status = USDHC_TransferNonBlocking(base, handle, &dma_config, transfer);
    if (status != kStatus_Success) {
        debug_printf("sdio_transfer_dma failed to start transfer error: %lu\n", status);
        return status;
    }

    uint32_t start = mp_hal_ticks_ms();
    while ((sdmmc.xfer_flags != xfer_flags) &&
           !(sdmmc.xfer_flags & SDIO_TRANSFER_ERROR) &&
           (mp_hal_ticks_ms() - start) < timeout_ms) {
        MICROPY_EVENT_POLL_HOOK;
    }

    if (sdmmc.xfer_flags == 0) {
        debug_printf("sdio_transfer_dma transfer timeout.\n");
        return kStatus_Timeout;
    } else if (sdmmc.xfer_flags != xfer_flags) {
        debug_printf("sdio_transfer_dma transfer failed: %lu\n", sdmmc.xfer_error);
        USDHC_Reset(base, kUSDHC_ResetCommand, 100);
        if (xfer_flags & SDIO_TRANSFER_DATA_COMPLETE) {
            USDHC_Reset(base, kUSDHC_ResetData, 100);
        }
        return sdmmc.xfer_error;
    }

    return kStatus_Success;
}

int sdio_transfer(uint32_t cmd, uint32_t arg, uint32_t *resp) {
    status_t status;
    usdhc_command_t command = {
        .index = cmd,
        .argument = arg,
        .type = kCARD_CommandTypeNormal,
        .responseType = sdio_response_type(cmd),
        .responseErrorFlags = 0
    };

    usdhc_transfer_t transfer = {
        .data = NULL,
        .command = &command,
    };

    status = sdio_transfer_dma(sdmmc.inst, &sdmmc.handle, &transfer, 5000);

    if (status != kStatus_Success) {
        debug_printf("sdio_transfer failed!\n");
        return -MP_EIO;
    }

    if (resp != NULL) {
        *resp = command.response[0];
    }

    return 0;
}

int sdio_transfer_cmd53(bool write, uint32_t block_size, uint32_t arg, size_t len, uint8_t *buf) {
    usdhc_data_t data = {
        .enableAutoCommand12 = false,
        .enableAutoCommand23 = false,
        .enableIgnoreError = false,
        .dataType = kUSDHC_TransferDataNormal,
    };

    usdhc_command_t command = {
        .index = 53,
        .argument = arg,
        .type = kCARD_CommandTypeNormal,
        .responseType = kCARD_ResponseTypeR5,
        .responseErrorFlags = 0
    };

    usdhc_transfer_t transfer = {
        .data = &data,
        .command = &command,
    };

    if (write) {
        data.rxData = NULL;
        data.txData = (uint32_t *)buf;
    } else {
        data.txData = NULL;
        data.rxData = (uint32_t *)buf;
    }

    if (arg & (1 << 27)) {
        // SDIO_BLOCK_MODE
        data.blockSize = block_size;
        data.blockCount = len / block_size;
    } else {
        // SDIO_BYTE_MODE
        data.blockSize = block_size;
        data.blockCount = 1;
    }

    debug_printf("cmd53 rw: %d addr 0x%p blocksize %u blockcount %lu total %lu len %d\n",
        write, buf, data.blockSize, data.blockCount, data.blockSize * data.blockCount, len);

    status_t status = sdio_transfer_dma(sdmmc.inst, &sdmmc.handle, &transfer, 5000);

    if (status != kStatus_Success) {
        debug_printf("sdio_transfer_cmd53 failed!\n");
        return -1;
    }

    return 0;
}

#endif
