/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
 * Copyright (c) 2021 Robert Hammelrath
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

#include "py/runtime.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "extmod/machine_spi.h"
#include "modmachine.h"
#include "dma_channel.h"

// #include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "fsl_dmamux.h"
#include "fsl_cache.h"

#include "fsl_lpspi.h"
#include "fsl_lpspi_edma.h"

#define DEFAULT_SPI_BAUDRATE    (1000000)
#define DEFAULT_SPI_POLARITY    (0)
#define DEFAULT_SPI_PHASE       (0)
#define DEFAULT_SPI_BITS        (8)
#define DEFAULT_SPI_FIRSTBIT    (kLPSPI_MsbFirst)

#define CLOCK_DIVIDER           (3)

typedef struct _machine_spi_obj_t {
    mp_obj_base_t base;
    uint8_t spi_id;
    uint8_t spi_hw_id;
    LPSPI_Type *spi_inst;
    lpspi_master_config_t config;
    lpspi_master_handle_t handle;
    bool transfer_busy;
} machine_spi_obj_t;

STATIC const uint8_t spi_index_table[] = MICROPY_HW_SPI_INDEX;
STATIC LPSPI_Type *spi_base_ptr_table[] = LPSPI_BASE_PTRS;
extern bool lpspi_set_iomux(int8_t spi);

static const char *firstbit_str[] = {"MSB", "LSB"};


STATIC void machine_spi_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "SPI(%u, baudrate=%u, polarity=%u, phase=%u, bits=%u, firstbit=%s)",
        self->spi_id, self->config.baudRate, self->config.cpol, self->config.cpha, self->config.bitsPerFrame,
        firstbit_str[self->config.direction]);
}

mp_obj_t machine_spi_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_id, ARG_baudrate, ARG_polarity, ARG_phase, ARG_bits, ARG_firstbit, ARG_gap };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id,       MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = DEFAULT_SPI_BAUDRATE} },
        { MP_QSTR_polarity, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_SPI_POLARITY} },
        { MP_QSTR_phase,    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_SPI_PHASE} },
        { MP_QSTR_bits,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_SPI_BITS} },
        { MP_QSTR_firstbit, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_SPI_FIRSTBIT} },
        { MP_QSTR_gap,      MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    };

    static bool clk_init = true;

    // Parse the arguments.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Get the SPI bus id.
    int spi_id = mp_obj_get_int(args[ARG_id].u_obj);
    if (spi_id < 0 || spi_id >= MP_ARRAY_SIZE(spi_index_table)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("SPI(%d) doesn't exist"), spi_id);
    }

    // Get peripheral object.
    uint8_t spi_hw_id = spi_index_table[spi_id];  // the hw spi number 1..n
    machine_spi_obj_t *self = m_new_obj(machine_spi_obj_t);
    self->base.type = &machine_spi_type;
    self->spi_id = spi_id;
    self->spi_inst = spi_base_ptr_table[spi_hw_id];
    self->spi_hw_id = spi_hw_id;
    LPSPI_MasterGetDefaultConfig(&self->config);

    if (clk_init) {
        clk_init = false;
        /*Set clock source for LPSPI*/
        CLOCK_SetMux(kCLOCK_LpspiMux, 1);  // Clock source is kCLOCK_Usb1PllPfd0Clk
        CLOCK_SetDiv(kCLOCK_LpspiDiv, CLOCK_DIVIDER);
    }

    // Initialise the SPI peripheral.
    self->config.baudRate = args[ARG_baudrate].u_int;
    self->config.betweenTransferDelayInNanoSec = 1000000000 / self->config.baudRate * 2;
    self->config.cpol = args[ARG_polarity].u_int;
    self->config.cpha = args[ARG_phase].u_int;
    self->config.bitsPerFrame = args[ARG_bits].u_int;
    self->config.direction = args[ARG_firstbit].u_int;
    if (args[ARG_gap].u_int != -1) {
        self->config.betweenTransferDelayInNanoSec = args[ARG_gap].u_int;
    }

    lpspi_set_iomux(spi_index_table[spi_id]);
    LPSPI_Reset(self->spi_inst);
    LPSPI_Enable(self->spi_inst, false);  // Disable first before new settings are applies
    LPSPI_MasterInit(self->spi_inst, &self->config, CLOCK_GetFreq(kCLOCK_Usb1PllPfd0Clk) / (CLOCK_DIVIDER + 1));

    return MP_OBJ_FROM_PTR(self);
}

STATIC void machine_spi_init(mp_obj_base_t *self_in, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_baudrate, ARG_polarity, ARG_phase, ARG_bits, ARG_firstbit, ARG_gap };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_baudrate, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_polarity, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_phase,    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_bits,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_firstbit, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_gap,      MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    };

    // Parse the arguments.
    machine_spi_obj_t *self = (machine_spi_obj_t *)self_in;
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    bool set_format = false;

    // Reconfigure the baudrate if requested.
    if (args[ARG_baudrate].u_int != -1) {
        self->config.baudRate = args[ARG_baudrate].u_int;
        self->config.betweenTransferDelayInNanoSec = 1000000000 / self->config.baudRate * 2;
        set_format = true;
    }

    // Reconfigure the format if requested.
    if (args[ARG_polarity].u_int != -1) {
        self->config.cpol = args[ARG_polarity].u_int;
        set_format = true;
    }
    if (args[ARG_phase].u_int != -1) {
        self->config.cpha = args[ARG_phase].u_int;
        set_format = true;
    }
    if (args[ARG_bits].u_int != -1) {
        self->config.bitsPerFrame = args[ARG_bits].u_int;
        set_format = true;
    }
    if (args[ARG_firstbit].u_int != -1) {
        self->config.direction = args[ARG_firstbit].u_int;
    }
    if (args[ARG_gap].u_int != -1) {
        self->config.betweenTransferDelayInNanoSec = args[ARG_gap].u_int;
    }
    if (set_format) {
        LPSPI_Enable(self->spi_inst, false);  // Disable first before new settings are applies
        LPSPI_MasterInit(self->spi_inst, &self->config, CLOCK_GetFreq(kCLOCK_Usb1PllPfd0Clk) / (CLOCK_DIVIDER + 1));
    }
}

static uint16_t dma_req_src_rx[] = { 0, kDmaRequestMuxLPSPI1Rx, kDmaRequestMuxLPSPI2Rx,
                                        kDmaRequestMuxLPSPI3Rx, kDmaRequestMuxLPSPI4Rx };
static uint16_t dma_req_src_tx[] = { 0, kDmaRequestMuxLPSPI1Tx, kDmaRequestMuxLPSPI2Tx,
                                        kDmaRequestMuxLPSPI3Tx, kDmaRequestMuxLPSPI4Tx };


void LPSPI_Callback(LPSPI_Type *base, lpspi_master_edma_handle_t *handle, status_t status, void *self_in) {
    machine_spi_obj_t *self = (machine_spi_obj_t *)self_in;

    self->transfer_busy = false;
}

STATIC void machine_spi_transfer(mp_obj_base_t *self_in, size_t len, const uint8_t *src, uint8_t *dest) {
    machine_spi_obj_t *self = (machine_spi_obj_t *)self_in;
    // Use DMA for large transfers if channels are available
    const size_t dma_min_size_threshold = 32;
    uint64_t t;

    int chan_tx = -1;
    int chan_rx = -1;
    if (len >= dma_min_size_threshold) {
        // Use two DMA channels to service the two FIFOs
        chan_rx = allocate_DMA_channel();
        chan_tx = allocate_DMA_channel();
    }
    bool use_dma = chan_rx >= 0 && chan_tx >= 0;
    lpspi_transfer_t masterXfer;

    if (use_dma) {
        edma_config_t userConfig;
        lpspi_master_edma_handle_t g_m_edma_handle;
        edma_handle_t lpspiEdmaMasterRxRegToRxDataHandle;
        edma_handle_t lpspiEdmaMasterTxDataToTxRegHandle;

        static bool init_dma = true;

        if (init_dma) {
            init_dma = false;
            /* DMA MUX init*/
            DMAMUX_Init(DMAMUX);

            DMAMUX_SetSource(DMAMUX, chan_rx, dma_req_src_rx[self->spi_hw_id]); // ## SPIn source
            DMAMUX_EnableChannel(DMAMUX, chan_rx);

            DMAMUX_SetSource(DMAMUX, chan_tx, dma_req_src_tx[self->spi_hw_id]);
            DMAMUX_EnableChannel(DMAMUX, chan_tx);

            EDMA_GetDefaultConfig(&userConfig);
            EDMA_Init(DMA0, &userConfig);
        }

        // Reconfigure the TXR, required after switch between DMA vs. non-DMA
        LPSPI_Enable(self->spi_inst, false);  // Disable first before new settings are applied
        self->spi_inst->TCR = LPSPI_TCR_CPOL(self->config.cpol) | LPSPI_TCR_CPHA(self->config.cpha) |
                    LPSPI_TCR_LSBF(self->config.direction) | LPSPI_TCR_FRAMESZ(self->config.bitsPerFrame - 1) |
                    (self->spi_inst->TCR & LPSPI_TCR_PRESCALE_MASK) | LPSPI_TCR_PCS(self->config.whichPcs);
        LPSPI_Enable(self->spi_inst, true);

        /*Set up lpspi master*/
        L1CACHE_DisableDCache();

        memset(&(g_m_edma_handle), 0, sizeof(g_m_edma_handle));
        memset(&(lpspiEdmaMasterRxRegToRxDataHandle), 0, sizeof(lpspiEdmaMasterRxRegToRxDataHandle));
        memset(&(lpspiEdmaMasterTxDataToTxRegHandle), 0, sizeof(lpspiEdmaMasterTxDataToTxRegHandle));

        EDMA_CreateHandle(&(lpspiEdmaMasterRxRegToRxDataHandle), DMA0, chan_rx);
        EDMA_CreateHandle(&(lpspiEdmaMasterTxDataToTxRegHandle), DMA0, chan_tx);

        LPSPI_MasterTransferCreateHandleEDMA(self->spi_inst, &g_m_edma_handle, LPSPI_Callback, self,
                                            &lpspiEdmaMasterRxRegToRxDataHandle,
                                            &lpspiEdmaMasterTxDataToTxRegHandle);
        /*Start master transfer*/
        masterXfer.txData = (uint8_t *)src;
        masterXfer.rxData = (uint8_t *)dest;
        masterXfer.dataSize = len;
        masterXfer.configFlags = kLPSPI_MasterPcs0 | kLPSPI_MasterPcsContinuous | kLPSPI_MasterByteSwap;

        self->transfer_busy = true;
        LPSPI_MasterTransferEDMA(self->spi_inst, &g_m_edma_handle, &masterXfer);

        // Wait until transfer completed. Use the timer as backup for timeout
        t = ticks_us64() + 15000000 * len / self->config.baudRate + 1000000;

        while (self->transfer_busy) {
            if (ticks_us64() > t) {  // Timeout
                // Abort the transfer
                LPSPI_MasterTransferAbortEDMA(self->spi_inst, &g_m_edma_handle);
                break;
            }
            MICROPY_EVENT_POLL_HOOK
        }
        L1CACHE_EnableDCache();
    }
    // Release DMA channels, even if never allocated.
    if (chan_rx >= 0) {
        free_DMA_channel(chan_rx);
    }
    if (chan_tx >= 0) {
        free_DMA_channel(chan_tx);
    }

    if (!use_dma) {
        // Reconfigure the TXR, required after switch between DMA vs. non-DMA
        LPSPI_Enable(self->spi_inst, false);  // Disable first before new settings are applied
        self->spi_inst->TCR = LPSPI_TCR_CPOL(self->config.cpol) | LPSPI_TCR_CPHA(self->config.cpha) |
                    LPSPI_TCR_LSBF(self->config.direction) | LPSPI_TCR_FRAMESZ(self->config.bitsPerFrame - 1) |
                    (self->spi_inst->TCR & LPSPI_TCR_PRESCALE_MASK) | LPSPI_TCR_PCS(self->config.whichPcs);
        LPSPI_Enable(self->spi_inst, true);

        masterXfer.txData = (uint8_t *)src;
        masterXfer.rxData = (uint8_t *)dest;
        masterXfer.dataSize = len;
        masterXfer.configFlags = kLPSPI_MasterPcs0 | kLPSPI_MasterPcsContinuous | kLPSPI_MasterByteSwap;

        LPSPI_MasterTransferBlocking(self->spi_inst, &masterXfer);
    }
}

STATIC const mp_machine_spi_p_t machine_spi_p = {
    .init = machine_spi_init,
    .transfer = machine_spi_transfer,
};

const mp_obj_type_t machine_spi_type = {
    { &mp_type_type },
    .name = MP_QSTR_SPI,
    .print = machine_spi_print,
    .make_new = machine_spi_make_new,
    .protocol = &machine_spi_p,
    .locals_dict = (mp_obj_dict_t *)&mp_machine_spi_locals_dict,
};
