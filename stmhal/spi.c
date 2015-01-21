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
#include "irq.h"
#include "pin.h"
#include "genhdr/pins.h"
#include "bufhelper.h"
#include "spi.h"
#include MICROPY_HAL_H

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

#if MICROPY_HW_ENABLE_SPI1
SPI_HandleTypeDef SPIHandle1 = {.Instance = NULL};
#endif
#if MICROPY_HW_ENABLE_SPI2
SPI_HandleTypeDef SPIHandle2 = {.Instance = NULL};
#endif
#if MICROPY_HW_ENABLE_SPI3
SPI_HandleTypeDef SPIHandle3 = {.Instance = NULL};
#endif

// Possible DMA configurations for SPI busses:
// SPI1_RX: DMA2_Stream0.CHANNEL_3 or DMA2_Stream2.CHANNEL_3
// SPI1_TX: DMA2_Stream3.CHANNEL_3 or DMA2_Stream5.CHANNEL_3
// SPI2_RX: DMA1_Stream3.CHANNEL_0
// SPI2_TX: DMA1_Stream4.CHANNEL_0
// SPI3_RX: DMA1_Stream0.CHANNEL_0 or DMA1_Stream2.CHANNEL_0
// SPI3_TX: DMA1_Stream5.CHANNEL_0 or DMA1_Stream7.CHANNEL_0

#define SPI1_DMA_CLK_ENABLE __DMA2_CLK_ENABLE
#define SPI1_RX_DMA_STREAM (DMA2_Stream2)
#define SPI1_TX_DMA_STREAM (DMA2_Stream5)
#define SPI1_DMA_CHANNEL (DMA_CHANNEL_3)
#define SPI1_RX_DMA_IRQN (DMA2_Stream2_IRQn)
#define SPI1_TX_DMA_IRQN (DMA2_Stream5_IRQn)
#define SPI1_RX_DMA_IRQ_HANDLER DMA2_Stream2_IRQHandler
#define SPI1_TX_DMA_IRQ_HANDLER DMA2_Stream5_IRQHandler

#define SPI2_DMA_CLK_ENABLE __DMA1_CLK_ENABLE
#define SPI2_RX_DMA_STREAM (DMA1_Stream3)
#define SPI2_TX_DMA_STREAM (DMA1_Stream4)
#define SPI2_DMA_CHANNEL (DMA_CHANNEL_0)
#define SPI2_RX_DMA_IRQN (DMA1_Stream3_IRQn)
#define SPI2_TX_DMA_IRQN (DMA1_Stream4_IRQn)
#define SPI2_RX_DMA_IRQ_HANDLER DMA1_Stream3_IRQHandler
#define SPI2_TX_DMA_IRQ_HANDLER DMA1_Stream4_IRQHandler

#define SPI3_DMA_CLK_ENABLE __DMA1_CLK_ENABLE
#define SPI3_RX_DMA_STREAM (DMA1_Stream2)
#define SPI3_TX_DMA_STREAM (DMA1_Stream7)
#define SPI3_DMA_CHANNEL (DMA_CHANNEL_0)
#define SPI3_RX_DMA_IRQN (DMA1_Stream2_IRQn)
#define SPI3_TX_DMA_IRQN (DMA1_Stream7_IRQn)
#define SPI3_RX_DMA_IRQ_HANDLER DMA1_Stream2_IRQHandler
#define SPI3_TX_DMA_IRQ_HANDLER DMA1_Stream7_IRQHandler

#if MICROPY_HW_ENABLE_SPI1
STATIC DMA_HandleTypeDef spi1_rx_dma_handle;
STATIC DMA_HandleTypeDef spi1_tx_dma_handle;
void SPI1_RX_DMA_IRQ_HANDLER(void) { HAL_DMA_IRQHandler(&spi1_rx_dma_handle); }
void SPI1_TX_DMA_IRQ_HANDLER(void) { HAL_DMA_IRQHandler(&spi1_tx_dma_handle); }
#endif

#if MICROPY_HW_ENABLE_SPI2
STATIC DMA_HandleTypeDef spi2_rx_dma_handle;
STATIC DMA_HandleTypeDef spi2_tx_dma_handle;
void SPI2_RX_DMA_IRQ_HANDLER(void) { HAL_DMA_IRQHandler(&spi2_rx_dma_handle); }
void SPI2_TX_DMA_IRQ_HANDLER(void) { HAL_DMA_IRQHandler(&spi2_tx_dma_handle); }
#endif

#if MICROPY_HW_ENABLE_SPI3
STATIC DMA_HandleTypeDef spi3_rx_dma_handle;
STATIC DMA_HandleTypeDef spi3_tx_dma_handle;
void SPI3_RX_DMA_IRQ_HANDLER(void) { HAL_DMA_IRQHandler(&spi3_rx_dma_handle); }
void SPI3_TX_DMA_IRQ_HANDLER(void) { HAL_DMA_IRQHandler(&spi3_tx_dma_handle); }
#endif

void spi_init0(void) {
    // reset the SPI handles
#if MICROPY_HW_ENABLE_SPI1
    memset(&SPIHandle1, 0, sizeof(SPI_HandleTypeDef));
    SPIHandle1.Instance = SPI1;
#endif
#if MICROPY_HW_ENABLE_SPI2
    memset(&SPIHandle2, 0, sizeof(SPI_HandleTypeDef));
    SPIHandle2.Instance = SPI2;
#endif
#if MICROPY_HW_ENABLE_SPI3
    memset(&SPIHandle3, 0, sizeof(SPI_HandleTypeDef));
    SPIHandle3.Instance = SPI3;
#endif
}

// TODO allow to take a list of pins to use
void spi_init(SPI_HandleTypeDef *spi, bool enable_nss_pin) {
    // init the GPIO lines
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
    GPIO_InitStructure.Pull = spi->Init.CLKPolarity == SPI_POLARITY_LOW ? GPIO_PULLDOWN : GPIO_PULLUP;

    DMA_HandleTypeDef *rx_dma, *tx_dma;
    IRQn_Type rx_dma_irqn, tx_dma_irqn;

    const pin_obj_t *pins[4];
    if (0) {
#if MICROPY_HW_ENABLE_SPI1
    } else if (spi->Instance == SPI1) {
        // X-skin: X5=PA4=SPI1_NSS, X6=PA5=SPI1_SCK, X7=PA6=SPI1_MISO, X8=PA7=SPI1_MOSI
        pins[0] = &pin_A4;
        pins[1] = &pin_A5;
        pins[2] = &pin_A6;
        pins[3] = &pin_A7;
        GPIO_InitStructure.Alternate = GPIO_AF5_SPI1;
        // enable the SPI clock
        __SPI1_CLK_ENABLE();
        // configure DMA
        SPI1_DMA_CLK_ENABLE();
        spi1_rx_dma_handle.Instance = SPI1_RX_DMA_STREAM;
        spi1_rx_dma_handle.Init.Channel = SPI1_DMA_CHANNEL;
        spi1_tx_dma_handle.Instance = SPI1_TX_DMA_STREAM;
        rx_dma = &spi1_rx_dma_handle;
        tx_dma = &spi1_tx_dma_handle;
        rx_dma_irqn = SPI1_RX_DMA_IRQN;
        tx_dma_irqn = SPI1_TX_DMA_IRQN;
#endif
#if MICROPY_HW_ENABLE_SPI2
    } else if (spi->Instance == SPI2) {
        // Y-skin: Y5=PB12=SPI2_NSS, Y6=PB13=SPI2_SCK, Y7=PB14=SPI2_MISO, Y8=PB15=SPI2_MOSI
        pins[0] = &pin_B12;
        pins[1] = &pin_B13;
        pins[2] = &pin_B14;
        pins[3] = &pin_B15;
        GPIO_InitStructure.Alternate = GPIO_AF5_SPI2;
        // enable the SPI clock
        __SPI2_CLK_ENABLE();
        // configure DMA
        SPI2_DMA_CLK_ENABLE();
        spi2_rx_dma_handle.Instance = SPI2_RX_DMA_STREAM;
        spi2_rx_dma_handle.Init.Channel = SPI2_DMA_CHANNEL;
        spi2_tx_dma_handle.Instance = SPI2_TX_DMA_STREAM;
        rx_dma = &spi2_rx_dma_handle;
        tx_dma = &spi2_tx_dma_handle;
        rx_dma_irqn = SPI2_RX_DMA_IRQN;
        tx_dma_irqn = SPI2_TX_DMA_IRQN;
#endif
#if MICROPY_HW_ENABLE_SPI3
    } else if (spi->Instance == SPI3) {
        pins[0] = &pin_A4;
        pins[1] = &pin_B3;
        pins[2] = &pin_B4;
        pins[3] = &pin_B5;
        GPIO_InitStructure.Alternate = GPIO_AF6_SPI3;
        // enable the SPI clock
        __SPI3_CLK_ENABLE();
        // configure DMA
        SPI3_DMA_CLK_ENABLE();
        spi3_rx_dma_handle.Instance = SPI3_RX_DMA_STREAM;
        spi3_rx_dma_handle.Init.Channel = SPI3_DMA_CHANNEL;
        spi3_tx_dma_handle.Instance = SPI3_TX_DMA_STREAM;
        rx_dma = &spi3_rx_dma_handle;
        tx_dma = &spi3_tx_dma_handle;
        rx_dma_irqn = SPI3_RX_DMA_IRQN;
        tx_dma_irqn = SPI3_TX_DMA_IRQN;
#endif
    } else {
        // SPI does not exist for this board (shouldn't get here, should be checked by caller)
        return;
    }

    for (uint i = (enable_nss_pin ? 0 : 1); i < 4; i++) {
        GPIO_InitStructure.Pin = pins[i]->pin_mask;
        HAL_GPIO_Init(pins[i]->gpio, &GPIO_InitStructure);
    }

    // init the SPI device
    if (HAL_SPI_Init(spi) != HAL_OK) {
        // init error
        // TODO should raise an exception, but this function is not necessarily going to be
        // called via Python, so may not be properly wrapped in an NLR handler
        printf("OSError: HAL_SPI_Init failed\n");
        return;
    }

    // configure DMA

    rx_dma->Init.Direction           = DMA_PERIPH_TO_MEMORY;
    rx_dma->Init.PeriphInc           = DMA_PINC_DISABLE;
    rx_dma->Init.MemInc              = DMA_MINC_ENABLE;
    rx_dma->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    rx_dma->Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    rx_dma->Init.Mode                = DMA_NORMAL;
    rx_dma->Init.Priority            = DMA_PRIORITY_LOW;
    rx_dma->Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
    rx_dma->Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
    rx_dma->Init.MemBurst            = DMA_MBURST_INC4;
    rx_dma->Init.PeriphBurst         = DMA_PBURST_INC4;

    tx_dma->Init = rx_dma->Init; // copy rx settings
    tx_dma->Init.Direction = DMA_MEMORY_TO_PERIPH;

    __HAL_LINKDMA(spi, hdmarx, *rx_dma);
    HAL_DMA_DeInit(rx_dma);
    HAL_DMA_Init(rx_dma);

    __HAL_LINKDMA(spi, hdmatx, *tx_dma);
    HAL_DMA_DeInit(tx_dma);
    HAL_DMA_Init(tx_dma);

    // Enable the relevant IRQs.
    HAL_NVIC_SetPriority(rx_dma_irqn, 6, 0);
    HAL_NVIC_EnableIRQ(rx_dma_irqn);
    HAL_NVIC_SetPriority(tx_dma_irqn, 6, 0);
    HAL_NVIC_EnableIRQ(tx_dma_irqn);
}

void spi_deinit(SPI_HandleTypeDef *spi) {
    HAL_SPI_DeInit(spi);
    if (0) {
#if MICROPY_HW_ENABLE_SPI1
    } else if (spi->Instance == SPI1) {
        __SPI1_FORCE_RESET();
        __SPI1_RELEASE_RESET();
        __SPI1_CLK_DISABLE();
#endif
#if MICROPY_HW_ENABLE_SPI2
    } else if (spi->Instance == SPI2) {
        __SPI2_FORCE_RESET();
        __SPI2_RELEASE_RESET();
        __SPI2_CLK_DISABLE();
#endif
#if MICROPY_HW_ENABLE_SPI3
    } else if (spi->Instance == SPI3) {
        __SPI3_FORCE_RESET();
        __SPI3_RELEASE_RESET();
        __SPI3_CLK_DISABLE();
#endif
    }
}

STATIC HAL_StatusTypeDef spi_wait_dma_finished(SPI_HandleTypeDef *spi, uint32_t timeout) {
    uint32_t start = HAL_GetTick();
    while (HAL_SPI_GetState(spi) != HAL_SPI_STATE_READY) {
        if (HAL_GetTick() - start >= timeout) {
            return HAL_TIMEOUT;
        }
        __WFI();
    }
    return HAL_OK;
}

/******************************************************************************/
/* Micro Python bindings                                                      */

typedef struct _pyb_spi_obj_t {
    mp_obj_base_t base;
    SPI_HandleTypeDef *spi;
} pyb_spi_obj_t;

STATIC const pyb_spi_obj_t pyb_spi_obj[] = {
#if MICROPY_HW_ENABLE_SPI1
    {{&pyb_spi_type}, &SPIHandle1},
#else
    {{&pyb_spi_type}, NULL},
#endif
#if MICROPY_HW_ENABLE_SPI2
    {{&pyb_spi_type}, &SPIHandle2},
#else
    {{&pyb_spi_type}, NULL},
#endif
#if MICROPY_HW_ENABLE_SPI3
    {{&pyb_spi_type}, &SPIHandle3},
#else
    {{&pyb_spi_type}, NULL},
#endif
};
#define PYB_NUM_SPI MP_ARRAY_SIZE(pyb_spi_obj)

SPI_HandleTypeDef *spi_get_handle(mp_obj_t o) {
    if (!MP_OBJ_IS_TYPE(o, &pyb_spi_type)) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "expecting an SPI object"));
    }
    pyb_spi_obj_t *self = o;
    return self->spi;
}

STATIC void pyb_spi_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_spi_obj_t *self = self_in;

    uint spi_num;
    if (self->spi->Instance == SPI1) { spi_num = 1; }
    else if (self->spi->Instance == SPI2) { spi_num = 2; }
    else { spi_num = 3; }

    if (self->spi->State == HAL_SPI_STATE_RESET) {
        print(env, "SPI(%u)", spi_num);
    } else {
        if (self->spi->Init.Mode == SPI_MODE_MASTER) {
            // compute baudrate
            uint spi_clock;
            if (self->spi->Instance == SPI1) {
                // SPI1 is on APB2
                spi_clock = HAL_RCC_GetPCLK2Freq();
            } else {
                // SPI2 and SPI3 are on APB1
                spi_clock = HAL_RCC_GetPCLK1Freq();
            }
            uint log_prescaler = (self->spi->Init.BaudRatePrescaler >> 3) + 1;
            uint baudrate = spi_clock >> log_prescaler;
            print(env, "SPI(%u, SPI.MASTER, baudrate=%u, prescaler=%u", spi_num, baudrate, 1 << log_prescaler);
        } else {
            print(env, "SPI(%u, SPI.SLAVE", spi_num);
        }
        print(env, ", polarity=%u, phase=%u, bits=%u", self->spi->Init.CLKPolarity == SPI_POLARITY_LOW ? 0 : 1, self->spi->Init.CLKPhase == SPI_PHASE_1EDGE ? 0 : 1, self->spi->Init.DataSize == SPI_DATASIZE_8BIT ? 8 : 16);
        if (self->spi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLED) {
            print(env, ", crc=0x%x", self->spi->Init.CRCPolynomial);
        }
        print(env, ")");
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
        if (self->spi->Instance == SPI1) {
            // SPI1 is on APB2
            spi_clock = HAL_RCC_GetPCLK2Freq();
        } else {
            // SPI2 and SPI3 are on APB1
            spi_clock = HAL_RCC_GetPCLK1Freq();
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
STATIC mp_obj_t pyb_spi_make_new(mp_obj_t type_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // get SPI number
    mp_int_t spi_id = mp_obj_get_int(args[0]) - 1;

    // check SPI number
    if (!(0 <= spi_id && spi_id < PYB_NUM_SPI && pyb_spi_obj[spi_id].spi != NULL)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "SPI bus %d does not exist", spi_id + 1));
    }

    // get SPI object
    const pyb_spi_obj_t *spi_obj = &pyb_spi_obj[spi_id];

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
    HAL_StatusTypeDef status;
    if (query_irq() == IRQ_STATE_DISABLED) {
        status = HAL_SPI_Transmit(self->spi, bufinfo.buf, bufinfo.len, args[1].u_int);
    } else {
        status = HAL_SPI_Transmit_DMA(self->spi, bufinfo.buf, bufinfo.len);
        if (status == HAL_OK) {
            status = spi_wait_dma_finished(self->spi, args[1].u_int);
        }
    }

    if (status != HAL_OK) {
        mp_hal_raise(status);
    }

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
    HAL_StatusTypeDef status;
    if (query_irq() == IRQ_STATE_DISABLED) {
        status = HAL_SPI_Receive(self->spi, (uint8_t*)vstr.buf, vstr.len, args[1].u_int);
    } else {
        status = HAL_SPI_Receive_DMA(self->spi, (uint8_t*)vstr.buf, vstr.len);
        if (status == HAL_OK) {
            status = spi_wait_dma_finished(self->spi, args[1].u_int);
        }
    }

    if (status != HAL_OK) {
        mp_hal_raise(status);
    }

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

    // send and receive the data
    HAL_StatusTypeDef status;
    if (query_irq() == IRQ_STATE_DISABLED) {
        status = HAL_SPI_TransmitReceive(self->spi, bufinfo_send.buf, bufinfo_recv.buf, bufinfo_send.len, args[2].u_int);
    } else {
        status = HAL_SPI_TransmitReceive_DMA(self->spi, bufinfo_send.buf, bufinfo_recv.buf, bufinfo_send.len);
        if (status == HAL_OK) {
            status = spi_wait_dma_finished(self->spi, args[2].u_int);
        }
    }

    if (status != HAL_OK) {
        mp_hal_raise(status);
    }

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

const mp_obj_type_t pyb_spi_type = {
    { &mp_type_type },
    .name = MP_QSTR_SPI,
    .print = pyb_spi_print,
    .make_new = pyb_spi_make_new,
    .locals_dict = (mp_obj_t)&pyb_spi_locals_dict,
};
