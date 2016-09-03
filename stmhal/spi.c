/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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
#include <string.h>

#include "py/nlr.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "extmod/machine_spi.h"
#include "irq.h"
#include "pin.h"
#include "genhdr/pins.h"
#include "bufhelper.h"
#include "dma.h"
#include "spi.h"

/// \moduleref pyb
/// \class SPI - a master-driven serial protocol
///
/// SPI is a serial protocol that is driven by a master.  At the physical level
/// there are 3 lines: SCK, MOSI, MISO.
///
/// See usage model of I2C; SPI is very similar.  Main difference is
/// parameters to init the SPI bus:
///
///     from pyb import SPI
///     spi = SPI(1, SPI.MASTER, baudrate=600000, polarity=1, phase=0, crc=0x7)
///
/// Only required parameter is mode, SPI.MASTER or SPI.SLAVE.  Polarity can be
/// 0 or 1, and is the level the idle clock line sits at.  Phase can be 0 or 1
/// to sample data on the first or second clock edge respectively.  Crc can be
/// None for no CRC, or a polynomial specifier.
///
/// Additional method for SPI:
///
///     data = spi.send_recv(b'1234')        # send 4 bytes and receive 4 bytes
///     buf = bytearray(4)
///     spi.send_recv(b'1234', buf)          # send 4 bytes and receive 4 into buf
///     spi.send_recv(buf, buf)              # send/recv 4 bytes from/to buf

// Possible DMA configurations for SPI busses:
// SPI1_TX: DMA2_Stream3.CHANNEL_3 or DMA2_Stream5.CHANNEL_3
// SPI1_RX: DMA2_Stream0.CHANNEL_3 or DMA2_Stream2.CHANNEL_3
// SPI2_TX: DMA1_Stream4.CHANNEL_0
// SPI2_RX: DMA1_Stream3.CHANNEL_0
// SPI3_TX: DMA1_Stream5.CHANNEL_0 or DMA1_Stream7.CHANNEL_0
// SPI3_RX: DMA1_Stream0.CHANNEL_0 or DMA1_Stream2.CHANNEL_0
// SPI4_TX: DMA2_Stream4.CHANNEL_5 or DMA2_Stream1.CHANNEL_4
// SPI4_RX: DMA2_Stream3.CHANNEL_5 or DMA2_Stream0.CHANNEL_4
// SPI5_TX: DMA2_Stream4.CHANNEL_2 or DMA2_Stream6.CHANNEL_7
// SPI5_RX: DMA2_Stream3.CHANNEL_2 or DMA2_Stream5.CHANNEL_7
// SPI6_TX: DMA2_Stream5.CHANNEL_1
// SPI6_RX: DMA2_Stream6.CHANNEL_1

typedef struct _pyb_spi_obj_t {
    mp_obj_base_t base;
    SPI_HandleTypeDef *spi;
    const dma_descr_t *tx_dma_descr;
    const dma_descr_t *rx_dma_descr;
} pyb_spi_obj_t;

#if defined(MICROPY_HW_SPI1_SCK)
SPI_HandleTypeDef SPIHandle1 = {.Instance = NULL};
#endif
#if defined(MICROPY_HW_SPI2_SCK)
SPI_HandleTypeDef SPIHandle2 = {.Instance = NULL};
#endif
#if defined(MICROPY_HW_SPI3_SCK)
SPI_HandleTypeDef SPIHandle3 = {.Instance = NULL};
#endif
#if defined(MICROPY_HW_SPI4_SCK)
SPI_HandleTypeDef SPIHandle4 = {.Instance = NULL};
#endif
#if defined(MICROPY_HW_SPI5_SCK)
SPI_HandleTypeDef SPIHandle5 = {.Instance = NULL};
#endif
#if defined(MICROPY_HW_SPI6_SCK)
SPI_HandleTypeDef SPIHandle6 = {.Instance = NULL};
#endif

STATIC const pyb_spi_obj_t pyb_spi_obj[] = {
    #if defined(MICROPY_HW_SPI1_SCK)
    {{&pyb_spi_type}, &SPIHandle1, &dma_SPI_1_TX, &dma_SPI_1_RX},
    #else
    {{&pyb_spi_type}, NULL, NULL, NULL},
    #endif
    #if defined(MICROPY_HW_SPI2_SCK)
    {{&pyb_spi_type}, &SPIHandle2, &dma_SPI_2_TX, &dma_SPI_2_RX},
    #else
    {{&pyb_spi_type}, NULL, NULL, NULL},
    #endif
    #if defined(MICROPY_HW_SPI3_SCK)
    {{&pyb_spi_type}, &SPIHandle3, &dma_SPI_3_TX, &dma_SPI_3_RX},
    #else
    {{&pyb_spi_type}, NULL, NULL, NULL},
    #endif
    #if defined(MICROPY_HW_SPI4_SCK)
    {{&pyb_spi_type}, &SPIHandle4, &dma_SPI_4_TX, &dma_SPI_4_RX},
    #else
    {{&pyb_spi_type}, NULL, NULL, NULL},
    #endif
    #if defined(MICROPY_HW_SPI5_SCK)
    {{&pyb_spi_type}, &SPIHandle5, &dma_SPI_5_TX, &dma_SPI_5_RX},
    #else
    {{&pyb_spi_type}, NULL, NULL, NULL},
    #endif
    #if defined(MICROPY_HW_SPI6_SCK)
    {{&pyb_spi_type}, &SPIHandle6, &dma_SPI_6_TX, &dma_SPI_6_RX},
    #else
    {{&pyb_spi_type}, NULL, NULL, NULL},
    #endif
};

void spi_init0(void) {
    // reset the SPI handles
    #if defined(MICROPY_HW_SPI1_SCK)
    memset(&SPIHandle1, 0, sizeof(SPI_HandleTypeDef));
    SPIHandle1.Instance = SPI1;
    #endif
    #if defined(MICROPY_HW_SPI2_SCK)
    memset(&SPIHandle2, 0, sizeof(SPI_HandleTypeDef));
    SPIHandle2.Instance = SPI2;
    #endif
    #if defined(MICROPY_HW_SPI3_SCK)
    memset(&SPIHandle3, 0, sizeof(SPI_HandleTypeDef));
    SPIHandle3.Instance = SPI3;
    #endif
    #if defined(MICROPY_HW_SPI4_SCK)
    memset(&SPIHandle4, 0, sizeof(SPI_HandleTypeDef));
    SPIHandle4.Instance = SPI4;
    #endif
    #if defined(MICROPY_HW_SPI5_SCK)
    memset(&SPIHandle5, 0, sizeof(SPI_HandleTypeDef));
    SPIHandle5.Instance = SPI5;
    #endif
    #if defined(MICROPY_HW_SPI6_SCK)
    memset(&SPIHandle6, 0, sizeof(SPI_HandleTypeDef));
    SPIHandle6.Instance = SPI6;
    #endif
}

// TODO allow to take a list of pins to use
void spi_init(SPI_HandleTypeDef *spi, bool enable_nss_pin) {
    // init the GPIO lines
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
    GPIO_InitStructure.Pull = spi->Init.CLKPolarity == SPI_POLARITY_LOW ? GPIO_PULLDOWN : GPIO_PULLUP;

    const pyb_spi_obj_t *self;
    const pin_obj_t *pins[4];
    pins[0] = NULL;

    if (0) {
    #if defined(MICROPY_HW_SPI1_SCK)
    } else if (spi->Instance == SPI1) {
        self = &pyb_spi_obj[0];
        #if defined(MICROPY_HW_SPI1_NSS)
        pins[0] = &MICROPY_HW_SPI1_NSS;
        #endif
        pins[1] = &MICROPY_HW_SPI1_SCK;
        pins[2] = &MICROPY_HW_SPI1_MISO;
        pins[3] = &MICROPY_HW_SPI1_MOSI;
        // enable the SPI clock
        __SPI1_CLK_ENABLE();
    #endif
    #if defined(MICROPY_HW_SPI2_SCK)
    } else if (spi->Instance == SPI2) {
        self = &pyb_spi_obj[1];
        #if defined(MICROPY_HW_SPI2_NSS)
        pins[0] = &MICROPY_HW_SPI2_NSS;
        #endif
        pins[1] = &MICROPY_HW_SPI2_SCK;
        pins[2] = &MICROPY_HW_SPI2_MISO;
        pins[3] = &MICROPY_HW_SPI2_MOSI;
        // enable the SPI clock
        __SPI2_CLK_ENABLE();
    #endif
    #if defined(MICROPY_HW_SPI3_SCK)
    } else if (spi->Instance == SPI3) {
        self = &pyb_spi_obj[2];
        #if defined(MICROPY_HW_SPI3_NSS)
        pins[0] = &MICROPY_HW_SPI3_NSS;
        #endif
        pins[1] = &MICROPY_HW_SPI3_SCK;
        pins[2] = &MICROPY_HW_SPI3_MISO;
        pins[3] = &MICROPY_HW_SPI3_MOSI;
        // enable the SPI clock
        __SPI3_CLK_ENABLE();
    #endif
    #if defined(MICROPY_HW_SPI4_SCK)
    } else if (spi->Instance == SPI4) {
        self = &pyb_spi_obj[3];
        #if defined(MICROPY_HW_SPI4_NSS)
        pins[0] = &MICROPY_HW_SPI4_NSS;
        #endif
        pins[1] = &MICROPY_HW_SPI4_SCK;
        pins[2] = &MICROPY_HW_SPI4_MISO;
        pins[3] = &MICROPY_HW_SPI4_MOSI;
        // enable the SPI clock
        __SPI4_CLK_ENABLE();
    #endif
    #if defined(MICROPY_HW_SPI5_SCK)
    } else if (spi->Instance == SPI5) {
        self = &pyb_spi_obj[4];
        #if defined(MICROPY_HW_SPI5_NSS)
        pins[0] = &MICROPY_HW_SPI5_NSS;
        #endif
        pins[1] = &MICROPY_HW_SPI5_SCK;
        pins[2] = &MICROPY_HW_SPI5_MISO;
        pins[3] = &MICROPY_HW_SPI5_MOSI;
        // enable the SPI clock
        __SPI5_CLK_ENABLE();
    #endif
    #if defined(MICROPY_HW_SPI6_SCK)
    } else if (spi->Instance == SPI6) {
        self = &pyb_spi_obj[5];
        #if defined(MICROPY_HW_SPI6_NSS)
        pins[0] = &MICROPY_HW_SPI6_NSS;
        #endif
        pins[1] = &MICROPY_HW_SPI6_SCK;
        pins[2] = &MICROPY_HW_SPI6_MISO;
        pins[3] = &MICROPY_HW_SPI6_MOSI;
        // enable the SPI clock
        __SPI6_CLK_ENABLE();
    #endif
    } else {
        // SPI does not exist for this board (shouldn't get here, should be checked by caller)
        return;
    }

    for (uint i = (enable_nss_pin && pins[0] ? 0 : 1); i < 4; i++) {
        mp_hal_gpio_set_af(pins[i], &GPIO_InitStructure, AF_FN_SPI, (self - &pyb_spi_obj[0]) + 1);
    }

    // init the SPI device
    if (HAL_SPI_Init(spi) != HAL_OK) {
        // init error
        // TODO should raise an exception, but this function is not necessarily going to be
        // called via Python, so may not be properly wrapped in an NLR handler
        printf("OSError: HAL_SPI_Init failed\n");
        return;
    }

    // After calling HAL_SPI_Init() it seems that the DMA gets disconnected if
    // it was previously configured.  So we invalidate the DMA channel to force
    // an initialisation the next time we use it.
    dma_invalidate_channel(self->tx_dma_descr);
    dma_invalidate_channel(self->rx_dma_descr);
}

void spi_deinit(SPI_HandleTypeDef *spi) {
    HAL_SPI_DeInit(spi);
    if (0) {
    #if defined(MICROPY_HW_SPI1_SCK)
    } else if (spi->Instance == SPI1) {
        __SPI1_FORCE_RESET();
        __SPI1_RELEASE_RESET();
        __SPI1_CLK_DISABLE();
    #endif
    #if defined(MICROPY_HW_SPI2_SCK)
    } else if (spi->Instance == SPI2) {
        __SPI2_FORCE_RESET();
        __SPI2_RELEASE_RESET();
        __SPI2_CLK_DISABLE();
    #endif
    #if defined(MICROPY_HW_SPI3_SCK)
    } else if (spi->Instance == SPI3) {
        __SPI3_FORCE_RESET();
        __SPI3_RELEASE_RESET();
        __SPI3_CLK_DISABLE();
    #endif
    #if defined(MICROPY_HW_SPI4_SCK)
    } else if (spi->Instance == SPI4) {
        __SPI4_FORCE_RESET();
        __SPI4_RELEASE_RESET();
        __SPI4_CLK_DISABLE();
    #endif
    #if defined(MICROPY_HW_SPI5_SCK)
    } else if (spi->Instance == SPI5) {
        __SPI5_FORCE_RESET();
        __SPI5_RELEASE_RESET();
        __SPI5_CLK_DISABLE();
    #endif
    #if defined(MICROPY_HW_SPI6_SCK)
    } else if (spi->Instance == SPI6) {
        __SPI6_FORCE_RESET();
        __SPI6_RELEASE_RESET();
        __SPI6_CLK_DISABLE();
    #endif
    }
}

STATIC HAL_StatusTypeDef spi_wait_dma_finished(SPI_HandleTypeDef *spi, uint32_t timeout) {
    // Note: we can't use WFI to idle in this loop because the DMA completion
    // interrupt may occur before the WFI.  Hence we miss it and have to wait
    // until the next sys-tick (up to 1ms).
    uint32_t start = HAL_GetTick();
    while (HAL_SPI_GetState(spi) != HAL_SPI_STATE_READY) {
        if (HAL_GetTick() - start >= timeout) {
            return HAL_TIMEOUT;
        }
    }
    return HAL_OK;
}

STATIC void spi_transfer(mp_obj_base_t *self_in, size_t src_len, const uint8_t *src_buf, size_t dest_len, uint8_t *dest_buf, uint32_t timeout) {
    // Note: there seems to be a problem sending 1 byte using DMA the first
    // time directly after the SPI/DMA is initialised.  The cause of this is
    // unknown but we sidestep the issue by using polling for 1 byte transfer.

    pyb_spi_obj_t *self = (pyb_spi_obj_t*)self_in;
    HAL_StatusTypeDef status;

    if (dest_len == 0) {
        // send only
        if (src_len == 1 || query_irq() == IRQ_STATE_DISABLED) {
            status = HAL_SPI_Transmit(self->spi, (uint8_t*)src_buf, src_len, timeout);
        } else {
            DMA_HandleTypeDef tx_dma;
            dma_init(&tx_dma, self->tx_dma_descr, self->spi);
            self->spi->hdmatx = &tx_dma;
            self->spi->hdmarx = NULL;
            status = HAL_SPI_Transmit_DMA(self->spi, (uint8_t*)src_buf, src_len);
            if (status == HAL_OK) {
                status = spi_wait_dma_finished(self->spi, timeout);
            }
            dma_deinit(self->tx_dma_descr);
        }
    } else if (src_len == 0) {
        // receive only
        if (dest_len == 1 || query_irq() == IRQ_STATE_DISABLED) {
            status = HAL_SPI_Receive(self->spi, dest_buf, dest_len, timeout);
        } else {
            DMA_HandleTypeDef tx_dma, rx_dma;
            if (self->spi->Init.Mode == SPI_MODE_MASTER) {
                // in master mode the HAL actually does a TransmitReceive call
                dma_init(&tx_dma, self->tx_dma_descr, self->spi);
                self->spi->hdmatx = &tx_dma;
            } else {
                self->spi->hdmatx = NULL;
            }
            dma_init(&rx_dma, self->rx_dma_descr, self->spi);
            self->spi->hdmarx = &rx_dma;

            status = HAL_SPI_Receive_DMA(self->spi, dest_buf, dest_len);
            if (status == HAL_OK) {
                status = spi_wait_dma_finished(self->spi, timeout);
            }
            if (self->spi->hdmatx != NULL) {
                dma_deinit(self->tx_dma_descr);
            }
            dma_deinit(self->rx_dma_descr);
        }
    } else {
        // send and receive
        // requires src_len==dest_len
        if (src_len == 1 || query_irq() == IRQ_STATE_DISABLED) {
            status = HAL_SPI_TransmitReceive(self->spi, (uint8_t*)src_buf, dest_buf, src_len, timeout);
        } else {
            DMA_HandleTypeDef tx_dma, rx_dma;
            dma_init(&tx_dma, self->tx_dma_descr, self->spi);
            self->spi->hdmatx = &tx_dma;
            dma_init(&rx_dma, self->rx_dma_descr, self->spi);
            self->spi->hdmarx = &rx_dma;
            status = HAL_SPI_TransmitReceive_DMA(self->spi, (uint8_t*)src_buf, dest_buf, src_len);
            if (status == HAL_OK) {
                status = spi_wait_dma_finished(self->spi, timeout);
            }
            dma_deinit(self->tx_dma_descr);
            dma_deinit(self->rx_dma_descr);
        }
    }

    if (status != HAL_OK) {
        mp_hal_raise(status);
    }
}

STATIC void spi_transfer_machine(mp_obj_base_t *self_in, size_t src_len, const uint8_t *src_buf, size_t dest_len, uint8_t *dest_buf) {
    if (src_len == 1 && dest_len > 1) {
        // this catches read and readinto
        // copy the single output byte to the dest buffer and use that as source
        memset(dest_buf, src_buf[0], dest_len);
        src_len = dest_len;
        src_buf = dest_buf;
    }
    spi_transfer(self_in, src_len, src_buf, dest_len, dest_buf, 100);
}

/******************************************************************************/
/* Micro Python bindings                                                      */

SPI_HandleTypeDef *spi_get_handle(mp_obj_t o) {
    if (!MP_OBJ_IS_TYPE(o, &pyb_spi_type)) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "expecting an SPI object"));
    }
    pyb_spi_obj_t *self = o;
    return self->spi;
}

STATIC void pyb_spi_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_spi_obj_t *self = self_in;

    uint spi_num = 1; // default to SPI1
    if (self->spi->Instance == SPI2) { spi_num = 2; }
    else if (self->spi->Instance == SPI3) { spi_num = 3; }
    #if defined(SPI4)
    else if (self->spi->Instance == SPI4) { spi_num = 4; }
    #endif
    #if defined(SPI5)
    else if (self->spi->Instance == SPI5) { spi_num = 5; }
    #endif
    #if defined(SPI6)
    else if (self->spi->Instance == SPI6) { spi_num = 6; }
    #endif

    if (self->spi->State == HAL_SPI_STATE_RESET) {
        mp_printf(print, "SPI(%u)", spi_num);
    } else {
        if (self->spi->Init.Mode == SPI_MODE_MASTER) {
            // compute baudrate
            uint spi_clock;
            if (self->spi->Instance == SPI2 || self->spi->Instance == SPI3) {
                // SPI2 and SPI3 are on APB1
                spi_clock = HAL_RCC_GetPCLK1Freq();
            } else {
                // SPI1, SPI4, SPI5 and SPI6 are on APB2
                spi_clock = HAL_RCC_GetPCLK2Freq();
            }
            uint log_prescaler = (self->spi->Init.BaudRatePrescaler >> 3) + 1;
            uint baudrate = spi_clock >> log_prescaler;
            mp_printf(print, "SPI(%u, SPI.MASTER, baudrate=%u, prescaler=%u", spi_num, baudrate, 1 << log_prescaler);
        } else {
            mp_printf(print, "SPI(%u, SPI.SLAVE", spi_num);
        }
        mp_printf(print, ", polarity=%u, phase=%u, bits=%u", self->spi->Init.CLKPolarity == SPI_POLARITY_LOW ? 0 : 1, self->spi->Init.CLKPhase == SPI_PHASE_1EDGE ? 0 : 1, self->spi->Init.DataSize == SPI_DATASIZE_8BIT ? 8 : 16);
        if (self->spi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLED) {
            mp_printf(print, ", crc=0x%x", self->spi->Init.CRCPolynomial);
        }
        mp_print_str(print, ")");
    }
}

/// \method init(mode, baudrate=328125, *, polarity=1, phase=0, bits=8, firstbit=SPI.MSB, ti=False, crc=None)
///
/// Initialise the SPI bus with the given parameters:
///
///   - `mode` must be either `SPI.MASTER` or `SPI.SLAVE`.
///   - `baudrate` is the SCK clock rate (only sensible for a master).
STATIC mp_obj_t pyb_spi_init_helper(const pyb_spi_obj_t *self, mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode,     MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = 328125} },
        { MP_QSTR_prescaler, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0xffffffff} },
        { MP_QSTR_polarity, MP_ARG_KW_ONLY | MP_ARG_INT,  {.u_int = 1} },
        { MP_QSTR_phase,    MP_ARG_KW_ONLY | MP_ARG_INT,  {.u_int = 0} },
        { MP_QSTR_dir,      MP_ARG_KW_ONLY | MP_ARG_INT,  {.u_int = SPI_DIRECTION_2LINES} },
        { MP_QSTR_bits,     MP_ARG_KW_ONLY | MP_ARG_INT,  {.u_int = 8} },
        { MP_QSTR_nss,      MP_ARG_KW_ONLY | MP_ARG_INT,  {.u_int = SPI_NSS_SOFT} },
        { MP_QSTR_firstbit, MP_ARG_KW_ONLY | MP_ARG_INT,  {.u_int = SPI_FIRSTBIT_MSB} },
        { MP_QSTR_ti,       MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_crc,      MP_ARG_KW_ONLY | MP_ARG_OBJ,  {.u_obj = mp_const_none} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // set the SPI configuration values
    SPI_InitTypeDef *init = &self->spi->Init;
    init->Mode = args[0].u_int;

    // configure the prescaler
    mp_uint_t br_prescale = args[2].u_int;
    if (br_prescale == 0xffffffff) {
        // prescaler not given, so select one that yields at most the requested baudrate
        mp_uint_t spi_clock;
        if (self->spi->Instance == SPI2 || self->spi->Instance == SPI3) {
            // SPI2 and SPI3 are on APB1
            spi_clock = HAL_RCC_GetPCLK1Freq();
        } else {
            // SPI1, SPI4, SPI5 and SPI6 are on APB2
            spi_clock = HAL_RCC_GetPCLK2Freq();
        }
        br_prescale = spi_clock / args[1].u_int;
    }
    if (br_prescale <= 2) { init->BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2; }
    else if (br_prescale <= 4) { init->BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4; }
    else if (br_prescale <= 8) { init->BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8; }
    else if (br_prescale <= 16) { init->BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16; }
    else if (br_prescale <= 32) { init->BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32; }
    else if (br_prescale <= 64) { init->BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64; }
    else if (br_prescale <= 128) { init->BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128; }
    else { init->BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256; }

    init->CLKPolarity = args[3].u_int == 0 ? SPI_POLARITY_LOW : SPI_POLARITY_HIGH;
    init->CLKPhase = args[4].u_int == 0 ? SPI_PHASE_1EDGE : SPI_PHASE_2EDGE;
    init->Direction = args[5].u_int;
    init->DataSize = (args[6].u_int == 16) ? SPI_DATASIZE_16BIT : SPI_DATASIZE_8BIT;
    init->NSS = args[7].u_int;
    init->FirstBit = args[8].u_int;
    init->TIMode = args[9].u_bool ? SPI_TIMODE_ENABLED : SPI_TIMODE_DISABLED;
    if (args[10].u_obj == mp_const_none) {
        init->CRCCalculation = SPI_CRCCALCULATION_DISABLED;
        init->CRCPolynomial = 0;
    } else {
        init->CRCCalculation = SPI_CRCCALCULATION_ENABLED;
        init->CRCPolynomial = mp_obj_get_int(args[10].u_obj);
    }

    // init the SPI bus
    spi_init(self->spi, init->NSS != SPI_NSS_SOFT);

    return mp_const_none;
}

/// \classmethod \constructor(bus, ...)
///
/// Construct an SPI object on the given bus.  `bus` can be 1 or 2.
/// With no additional parameters, the SPI object is created but not
/// initialised (it has the settings from the last initialisation of
/// the bus, if any).  If extra arguments are given, the bus is initialised.
/// See `init` for parameters of initialisation.
///
/// The physical pins of the SPI busses are:
///
///   - `SPI(1)` is on the X position: `(NSS, SCK, MISO, MOSI) = (X5, X6, X7, X8) = (PA4, PA5, PA6, PA7)`
///   - `SPI(2)` is on the Y position: `(NSS, SCK, MISO, MOSI) = (Y5, Y6, Y7, Y8) = (PB12, PB13, PB14, PB15)`
///
/// At the moment, the NSS pin is not used by the SPI driver and is free
/// for other use.
STATIC mp_obj_t pyb_spi_make_new(const mp_obj_type_t *type, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // work out SPI bus
    int spi_id = 0;
    if (MP_OBJ_IS_STR(args[0])) {
        const char *port = mp_obj_str_get_str(args[0]);
        if (0) {
        #ifdef MICROPY_HW_SPI1_NAME
        } else if (strcmp(port, MICROPY_HW_SPI1_NAME) == 0) {
            spi_id = 1;
        #endif
        #ifdef MICROPY_HW_SPI2_NAME
        } else if (strcmp(port, MICROPY_HW_SPI2_NAME) == 0) {
            spi_id = 2;
        #endif
        #ifdef MICROPY_HW_SPI3_NAME
        } else if (strcmp(port, MICROPY_HW_SPI3_NAME) == 0) {
            spi_id = 3;
        #endif
        } else {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                "SPI(%s) does not exist", port));
        }
    } else {
        spi_id = mp_obj_get_int(args[0]);
        if (spi_id < 1 || spi_id > MP_ARRAY_SIZE(pyb_spi_obj)
            || pyb_spi_obj[spi_id - 1].spi == NULL) {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                "SPI(%d) does not exist", spi_id));
        }
    }

    // get SPI object
    const pyb_spi_obj_t *spi_obj = &pyb_spi_obj[spi_id - 1];

    if (n_args > 1 || n_kw > 0) {
        // start the peripheral
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        pyb_spi_init_helper(spi_obj, n_args - 1, args + 1, &kw_args);
    }

    return (mp_obj_t)spi_obj;
}

STATIC mp_obj_t pyb_spi_init(mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return pyb_spi_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_spi_init_obj, 1, pyb_spi_init);

/// \method deinit()
/// Turn off the SPI bus.
STATIC mp_obj_t pyb_spi_deinit(mp_obj_t self_in) {
    pyb_spi_obj_t *self = self_in;
    spi_deinit(self->spi);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_spi_deinit_obj, pyb_spi_deinit);

/// \method send(send, *, timeout=5000)
/// Send data on the bus:
///
///   - `send` is the data to send (an integer to send, or a buffer object).
///   - `timeout` is the timeout in milliseconds to wait for the send.
///
/// Return value: `None`.
STATIC mp_obj_t pyb_spi_send(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    // TODO assumes transmission size is 8-bits wide

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_send,    MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 5000} },
    };

    // parse args
    pyb_spi_obj_t *self = pos_args[0];
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // get the buffer to send from
    mp_buffer_info_t bufinfo;
    uint8_t data[1];
    pyb_buf_get_for_send(args[0].u_obj, &bufinfo, data);

    // send the data
    spi_transfer((mp_obj_base_t*)self, bufinfo.len, bufinfo.buf, 0, NULL, args[1].u_int);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_spi_send_obj, 1, pyb_spi_send);

/// \method recv(recv, *, timeout=5000)
///
/// Receive data on the bus:
///
///   - `recv` can be an integer, which is the number of bytes to receive,
///     or a mutable buffer, which will be filled with received bytes.
///   - `timeout` is the timeout in milliseconds to wait for the receive.
///
/// Return value: if `recv` is an integer then a new buffer of the bytes received,
/// otherwise the same buffer that was passed in to `recv`.
STATIC mp_obj_t pyb_spi_recv(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    // TODO assumes transmission size is 8-bits wide

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_recv,    MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 5000} },
    };

    // parse args
    pyb_spi_obj_t *self = pos_args[0];
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // get the buffer to receive into
    vstr_t vstr;
    mp_obj_t o_ret = pyb_buf_get_for_recv(args[0].u_obj, &vstr);

    // receive the data
    spi_transfer((mp_obj_base_t*)self, 0, NULL, vstr.len, (uint8_t*)vstr.buf, args[1].u_int);

    // return the received data
    if (o_ret != MP_OBJ_NULL) {
        return o_ret;
    } else {
        return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_spi_recv_obj, 1, pyb_spi_recv);

/// \method send_recv(send, recv=None, *, timeout=5000)
///
/// Send and receive data on the bus at the same time:
///
///   - `send` is the data to send (an integer to send, or a buffer object).
///   - `recv` is a mutable buffer which will be filled with received bytes.
///   It can be the same as `send`, or omitted.  If omitted, a new buffer will
///   be created.
///   - `timeout` is the timeout in milliseconds to wait for the receive.
///
/// Return value: the buffer with the received bytes.
STATIC mp_obj_t pyb_spi_send_recv(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    // TODO assumes transmission size is 8-bits wide

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_send,    MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_recv,    MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 5000} },
    };

    // parse args
    pyb_spi_obj_t *self = pos_args[0];
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // get buffers to send from/receive to
    mp_buffer_info_t bufinfo_send;
    uint8_t data_send[1];
    mp_buffer_info_t bufinfo_recv;
    vstr_t vstr_recv;
    mp_obj_t o_ret;

    if (args[0].u_obj == args[1].u_obj) {
        // same object for send and receive, it must be a r/w buffer
        mp_get_buffer_raise(args[0].u_obj, &bufinfo_send, MP_BUFFER_RW);
        bufinfo_recv = bufinfo_send;
        o_ret = args[0].u_obj;
    } else {
        // get the buffer to send from
        pyb_buf_get_for_send(args[0].u_obj, &bufinfo_send, data_send);

        // get the buffer to receive into
        if (args[1].u_obj == MP_OBJ_NULL) {
            // only send argument given, so create a fresh buffer of the send length
            vstr_init_len(&vstr_recv, bufinfo_send.len);
            bufinfo_recv.len = vstr_recv.len;
            bufinfo_recv.buf = vstr_recv.buf;
            o_ret = MP_OBJ_NULL;
        } else {
            // recv argument given
            mp_get_buffer_raise(args[1].u_obj, &bufinfo_recv, MP_BUFFER_WRITE);
            if (bufinfo_recv.len != bufinfo_send.len) {
                nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "recv must be same length as send"));
            }
            o_ret = args[1].u_obj;
        }
    }

    // do the transfer
    spi_transfer((mp_obj_base_t*)self, bufinfo_send.len, bufinfo_send.buf, bufinfo_recv.len, bufinfo_recv.buf, args[2].u_int);

    // return the received data
    if (o_ret != MP_OBJ_NULL) {
        return o_ret;
    } else {
        return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr_recv);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_spi_send_recv_obj, 1, pyb_spi_send_recv);

STATIC const mp_map_elem_t pyb_spi_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_init), (mp_obj_t)&pyb_spi_init_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_deinit), (mp_obj_t)&pyb_spi_deinit_obj },

    { MP_OBJ_NEW_QSTR(MP_QSTR_read), (mp_obj_t)&mp_machine_spi_read_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_readinto), (mp_obj_t)&mp_machine_spi_readinto_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_write), (mp_obj_t)&mp_machine_spi_write_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_write_readinto), (mp_obj_t)&mp_machine_spi_write_readinto_obj },

    // legacy methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_send), (mp_obj_t)&pyb_spi_send_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_recv), (mp_obj_t)&pyb_spi_recv_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_send_recv), (mp_obj_t)&pyb_spi_send_recv_obj },

    // class constants
    /// \constant MASTER - for initialising the bus to master mode
    /// \constant SLAVE - for initialising the bus to slave mode
    /// \constant MSB - set the first bit to MSB
    /// \constant LSB - set the first bit to LSB
    { MP_OBJ_NEW_QSTR(MP_QSTR_MASTER), MP_OBJ_NEW_SMALL_INT(SPI_MODE_MASTER) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SLAVE),  MP_OBJ_NEW_SMALL_INT(SPI_MODE_SLAVE) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MSB),    MP_OBJ_NEW_SMALL_INT(SPI_FIRSTBIT_MSB) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_LSB),    MP_OBJ_NEW_SMALL_INT(SPI_FIRSTBIT_LSB) },
    /* TODO
    { MP_OBJ_NEW_QSTR(MP_QSTR_DIRECTION_2LINES             ((uint32_t)0x00000000)
    { MP_OBJ_NEW_QSTR(MP_QSTR_DIRECTION_2LINES_RXONLY      SPI_CR1_RXONLY
    { MP_OBJ_NEW_QSTR(MP_QSTR_DIRECTION_1LINE              SPI_CR1_BIDIMODE
    { MP_OBJ_NEW_QSTR(MP_QSTR_NSS_SOFT                    SPI_CR1_SSM
    { MP_OBJ_NEW_QSTR(MP_QSTR_NSS_HARD_INPUT              ((uint32_t)0x00000000)
    { MP_OBJ_NEW_QSTR(MP_QSTR_NSS_HARD_OUTPUT             ((uint32_t)0x00040000)
    */
};

STATIC MP_DEFINE_CONST_DICT(pyb_spi_locals_dict, pyb_spi_locals_dict_table);

STATIC const mp_machine_spi_p_t pyb_spi_p = {
    .transfer = spi_transfer_machine,
};

const mp_obj_type_t pyb_spi_type = {
    { &mp_type_type },
    .name = MP_QSTR_SPI,
    .print = pyb_spi_print,
    .make_new = pyb_spi_make_new,
    .protocol = &pyb_spi_p,
    .locals_dict = (mp_obj_t)&pyb_spi_locals_dict,
};
