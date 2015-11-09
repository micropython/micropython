/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2015 Bryan Morrissey
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
#include <errno.h> // used by mp_is_nonblocking_error
#include "py/nlr.h"
#include "py/runtime.h"
#include "py/gc.h"

#if MICROPY_HW_ENABLE_I2S2 || MICROPY_HW_ENABLE_I2S3

#include "py/objstr.h"
#include "py/objlist.h"
#include "py/stream.h"
#include "py/mphal.h"
#include "file.h"
#include "irq.h"
#include "pin.h"
#include "led.h" // For debugging using led_toggle(n)
#include "genhdr/pins.h"
#include "dma.h"
#include "bufhelper.h"
#include "i2s.h"

/// \moduleref pyb
/// \class I2S - Inter-IC-Sound, a protocol to transfer isochronous audio data
///
/// I2S is a serial protocol for sending and receiving audio. At the physical
/// level there are 3 or 4 lines: Bit Clock, Word Select, and 1 or 2 data lines
/// for simplex or duplex operation. In addition, I2S can optionally provide a
/// Master Clock (MCLK) signal to drive an external codec, or accept an I2S
/// clock input signal (I2S_CLKIN) from an external source to drive I2SPLL.
///

// Possible DMA configurations for I2S busses:
// SPI2 RX:     DMA1_Stream3.CHANNEL_0
// SPI2 TX:     DMA1_Stream4.CHANNEL_0
// I2S2_EXT RX: DMA1_Stream3.CHANNEL_3
// I2S2_EXT TX: DMA1_Stream4.CHANNEL_2
// SPI3 RX:     DMA1_Stream0.CHANNEL_0 or DMA1_Stream2.CHANNEL_0
// SPI3 TX:     DMA1_Stream5.CHANNEL_0 or DMA1_Stream7.CHANNEL_0
// I2S3_EXT RX: DMA1_Stream0.CHANNEL_3 or DMA1_Stream2.CHANNEL_2
// I2S3_EXT TX: DMA1_Stream5.CHANNEL_2

// Duplex streaming seems to require at least this much; there are hiccups at 8192
// It may require some testing to determine the ideal value
#define AUDIOBUFFER_BYTES 12288

typedef enum {
    INACTIVE       = 0x00,
    BUFFER_WR      = 0x01,
    BUFFER_RD      = 0x02,
    BUFFER_RD_WR   = 0x03,
    BUF_STR_DIV    = 0x04, // (xfer_state >= BUF_STR_DIV) indicates streaming
    STREAM_OUT     = 0x04,
    STREAM_IN      = 0x08,
    STREAM_IN_OUT  = 0x0C,
} xfer_state_t;

// For buffer transfers, we want to be able to use the buffer methods write,
// read, and write_readinto as the transfer initiation function as well as the
// callback function.
// When a transfer is initiated, the state will be INACTIVE, so the full
// initialization occurs. After that, when the functions are incorporated into
// callbacks, the mode will be active, and the initialization can be skipped.
// The mode provides information as to the last function called, so if
// the callback is changed on the fly, (to go from a pure write to start
// recording as well) the functions can respond accordingly.

// Also, each function (read, write, write_readinto, and stream_out and stream_in)
// will check to make sure that any ongoing transfer is of the same type (buffer
// or stream) and will throw an exception without interrupting the ongoing transfer
// if called.

// If the stop() function is passed an argument, it can set flags to stop part of
// a transfer (read or write) while allowing the other part to continue uninterrupted

typedef enum {
    I2S_SIG_NONE,
    I2S_SIG_STOP_RX,
    I2S_SIG_STOP_TX,
    I2S_SIG_STOP_ALL,
    I2S_SIG_STOP_EOF_DIV,
    I2S_SIG_EOF_RX,
    I2S_SIG_EOF_TX,
    I2S_SIG_EOF_ALL,
} xfer_signal_t;

typedef struct _pyb_i2s_obj_t {
    mp_obj_base_t base;
    I2S_HandleTypeDef i2s;
    DMA_Stream_TypeDef *tx_dma_stream;
    uint32_t tx_dma_channel;
    DMA_Stream_TypeDef *rx_dma_stream;
    uint32_t rx_dma_channel;
    DMA_HandleTypeDef tx_dma;
    DMA_HandleTypeDef rx_dma;
    mp_obj_base_t *dstream_tx;
    mp_obj_base_t *dstream_rx;
    mp_int_t stream_tx_len;
    mp_int_t stream_rx_len;
    mp_uint_t last_stream_read_sz;
    mp_uint_t last_stream_write_sz;
    mp_obj_t callback;
    xfer_state_t xfer_state;
    xfer_signal_t xfer_signal;
    const pin_obj_t *pins[5];
    // audio buffers placed before smaller struct members to keep word-alignment:
    uint16_t audiobuf_tx[2][AUDIOBUFFER_BYTES / 8];
    uint16_t audiobuf_rx[2][AUDIOBUFFER_BYTES / 8];
    mp_int_t i2s_id : 8;
    bool is_enabled : 1;
    bool is_duplex : 1;
    bool base_is_tx : 1; // base instance SPIx is either tx or rx
    bool pp_ptr : 1;     // ping-pong pointer for double buffers
} pyb_i2s_obj_t;

// pins are Bit Clock, Word Select, TX Data, RX Data, and Master Clock Out
typedef enum { BCK, WS, TX, RX, MCK, NUM_PINS } i2s_pin_t;

// Note: This is where spi.c defines SPI_HandleTypeDef SPIHandle[1,2,3]
// as well as initializing the static pyb_spi_obj[] array -
// probably not needed here?

// TODO: more testing to determine optimal DMA configuration
const DMA_InitTypeDef dma_init_struct_i2s = {
    .Channel             = 0,
    .Direction           = 0,
    .PeriphInc           = DMA_PINC_DISABLE,
    .MemInc              = DMA_MINC_ENABLE,
    .PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD,
    .MemDataAlignment    = DMA_MDATAALIGN_HALFWORD,
    .Mode                = DMA_NORMAL,
    .Priority            = DMA_PRIORITY_VERY_HIGH,
    .FIFOMode            = DMA_FIFOMODE_DISABLE,
    //.FIFOMode            = DMA_FIFOMODE_ENABLE,
    .FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL,
    //.FIFOThreshold       = DMA_FIFO_THRESHOLD_HALFFULL,
    .MemBurst            = DMA_MBURST_SINGLE,
    .PeriphBurst         = DMA_PBURST_SINGLE
};

static inline bool i2s_has_tx(pyb_i2s_obj_t *i2s_obj) {
    return (i2s_obj->is_duplex || i2s_obj->base_is_tx);
}

static inline bool i2s_has_rx(pyb_i2s_obj_t *i2s_obj) {
    return (i2s_obj->is_duplex || !(i2s_obj->base_is_tx));
}

static inline bool i2s_is_master(pyb_i2s_obj_t *i2s_obj) {
    return (i2s_obj->i2s.Init.Mode == I2S_MODE_MASTER_TX ||
            i2s_obj->i2s.Init.Mode == I2S_MODE_MASTER_RX);
}

// i2s_init0 is direct crib from stmhal/can.c - can_init0()
void i2s_init0(void) {
    for (int i = 0; i < MP_ARRAY_SIZE(MP_STATE_PORT(pyb_i2s_obj_all)); i++) {
        MP_STATE_PORT(pyb_i2s_obj_all)[i] = NULL;
    }
}

// assumes init parameters are set up correctly
STATIC bool i2s_init(pyb_i2s_obj_t *i2s_obj) {
    // init the GPIO lines
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
    GPIO_InitStructure.Pull = GPIO_PULLUP; // see spi.c for dealing with CPOL

    // TODO:
    // - Use STM32F4DISC to make sure that I2S3 works with that board's codec -
    // (I2S3 conflicts with SDIO and LED's on pyboard and therefore is disabled)
    // - Provide configuration to enable I2S_CKIN if it available (only if SDIO
    // is not configured)

    // If I2Sx_SD (SPIx) and I2SxEXT_SD pins are both given, then
    // init full duplex mode.
    // - If only I2Sx_SD is given, init simplex mode
    // - SD pin is configured as either Rx or Tx in pyb_i2s_init_helper
    // based on postion in list; if ext_SD pin is present we set full-duplex
    // option, and HAL_I2S_Init automatically sets up the ext I2Sxext
    // instance as opposite direction (Rx or Tx) to I2Sx

    if (0) {
#if MICROPY_HW_ENABLE_I2S2
    } else if (i2s_obj->i2s_id == 2) {
        i2s_obj->i2s.Instance = SPI2;
        __SPI2_CLK_ENABLE();
        // configure DMA streams - see RM0090 section 10.3.3, Tables 42 & 43
        if (i2s_obj->is_duplex) {
            if (i2s_obj->base_is_tx) {
                // SPI2 instance is Tx
                i2s_obj->tx_dma_stream = DMA1_Stream4;
                i2s_obj->tx_dma_channel = DMA_CHANNEL_0;
                // I2S2_ext instance is Rx
                i2s_obj->rx_dma_stream = DMA1_Stream3;
                i2s_obj->rx_dma_channel = DMA_CHANNEL_3;
            } else {
                // I2S2_ext instance is Rx
                i2s_obj->tx_dma_stream = DMA1_Stream4;
                i2s_obj->tx_dma_channel = DMA_CHANNEL_2;
                // SPI2 instance is Rx
                i2s_obj->rx_dma_stream = DMA1_Stream3;
                i2s_obj->rx_dma_channel = DMA_CHANNEL_0;
            }
        } else {
            // simplex: set up both DMA streams for SPI2, only one will be used
            i2s_obj->tx_dma_stream = DMA1_Stream4;
            i2s_obj->tx_dma_channel = DMA_CHANNEL_0;
            i2s_obj->rx_dma_stream = DMA1_Stream3;
            i2s_obj->rx_dma_channel = DMA_CHANNEL_0;
        }
#endif
#if MICROPY_HW_ENABLE_I2S3
    } else if (i2s_obj->i2s_id == 3) {
        i2s_obj->i2s.Instance = SPI3;
        __SPI3_CLK_ENABLE();
        // configure DMA streams - see RM0090 section 10.3.3, Tables 42 & 43
        // I2S3 has alternate stream configurations, listed here in comments
        if (i2s_obj->is_duplex) {
            if (i2s_obj->base_is_tx) {
                // SPI3 instance is TX
                i2s_obj->tx_dma_stream = DMA1_Stream5;
                //i2s_obj->tx_dma_stream = DMA1_Stream7;
                i2s_obj->tx_dma_channel = DMA_CHANNEL_0;
                // I2S3_ext instance is RX (DMA1_Stream0.CHANNEL_3 or DMA1_Stream2.CHANNEL_2)
                i2s_obj->rx_dma_stream = DMA1_Stream0;
                i2s_obj->rx_dma_channel = DMA_CHANNEL_3;
                //i2s_obj->rx_dma_stream = DMA1_Stream2;
                //i2s_obj->rx_dma_channel = DMA_CHANNEL_2;
            } else {
                // I2S3_ext instance is TX
                i2s_obj->tx_dma_stream = DMA1_Stream5;
                i2s_obj->tx_dma_channel = DMA_CHANNEL_2;
                // SPI3 instance is RX
                i2s_obj->rx_dma_stream = DMA1_Stream0;
                //i2s_obj->rx_dma_stream = DMA1_Stream2;
                i2s_obj->rx_dma_channel = DMA_CHANNEL_0;
            }
        } else {
            // Simplex: set up both DMA streams for SPI3, only one will be used
            i2s_obj->tx_dma_stream = DMA1_Stream5;
            //i2s_obj->tx_dma_stream = DMA1_Stream7;
            i2s_obj->tx_dma_channel = DMA_CHANNEL_0;
            i2s_obj->rx_dma_stream = DMA1_Stream0;
            //i2s_obj->rx_dma_stream = DMA1_Stream2;
            i2s_obj->rx_dma_channel = DMA_CHANNEL_0;
        }
#endif
    } else {
        // invalid i2s_id number; shouldn't get here as i2s object should not
        // have been created without setting a valid i2s instance number
        return false;
    }

    // GPIO Pin initialization
    // If master clock is not enabled, its pin will not get initialized
    int num_pins = (i2s_obj->i2s.Init.MCLKOutput ==
                    I2S_MCLKOUTPUT_ENABLE) ? NUM_PINS : NUM_PINS - 1;

    for (int i = 0; i < num_pins; i++) {
        if (i2s_obj->pins[i] != MP_OBJ_NULL) {
            GPIO_InitStructure.Pin = i2s_obj->pins[i]->pin_mask;
            const pin_af_obj_t *af = pin_find_af(i2s_obj->pins[i], AF_FN_I2S, i2s_obj->i2s_id);
            assert(af != NULL);
            // Alt function is set using af->idx instead of GPIO_AFx_I2Sx macros
            GPIO_InitStructure.Alternate = (uint8_t)af->idx;
            HAL_GPIO_Init(i2s_obj->pins[i]->gpio, &GPIO_InitStructure);
        }
    }

    // Configure and enable I2SPLL - I2S_MASTER modes only:
    // References: see table 126 of RM0090, also lines 457-494 of
    // STM32Cube_FW_F4_V1.5.0/Drivers/BSP/STM32F4-Discovery/stm32f4_discovery_audio.c
    // 48kHz family is accurate for 8, 16, 24, and 48kHz but not 32 or 96
    // 44.1kHz family is accurate for 11.025, 22.05 and 44.1kHz but not 88.2

    // TODO: support more of the commonly-used frequencies and account for 16/32 bit frames
    // Also: Refactor to use macros as provided by stm32f4xx_hal_rcc_ex.h
    __HAL_RCC_PLLI2S_DISABLE();
    if (i2s_is_master(i2s_obj)) {
        if (i2s_obj->i2s.Init.MCLKOutput == I2S_MCLKOUTPUT_ENABLE) {
            if ((i2s_obj->i2s.Init.AudioFreq & 0x7) == 0) {
                __HAL_RCC_PLLI2S_CONFIG(258, 3); // 8, 16, 24, 48kHz with mclkout; not 32 or 96
            } else {
                __HAL_RCC_PLLI2S_CONFIG(271, 2); // 11.025, 22.05, 44.1kHz with mclkout; not 88.2
            }
        } else {
            if ((i2s_obj->i2s.Init.AudioFreq & 0x7) == 0) {
                __HAL_RCC_PLLI2S_CONFIG(384, 5);
            } else {
                __HAL_RCC_PLLI2S_CONFIG(429, 4);
            }
        }
        __HAL_RCC_PLLI2S_ENABLE();
    }

    if (HAL_I2S_Init(&i2s_obj->i2s) == HAL_OK) {
        // Reset and initialize Tx and Rx DMA channels
        // TODO: Currently both DMA's are initialized regardless of whether I2S
        // is instantiated as simplex or duplex - should we check instead?

        // Reset and initialize tx DMA
        dma_invalidate_channel(i2s_obj->tx_dma_stream, i2s_obj->tx_dma_channel);
        dma_init(&i2s_obj->tx_dma, i2s_obj->tx_dma_stream, &dma_init_struct_i2s,
                 i2s_obj->tx_dma_channel, DMA_MEMORY_TO_PERIPH, &i2s_obj->i2s);
        i2s_obj->i2s.hdmatx = &i2s_obj->tx_dma;

        // Reset and initialize rx DMA
        dma_invalidate_channel(i2s_obj->rx_dma_stream, i2s_obj->rx_dma_channel);
        dma_init(&i2s_obj->rx_dma, i2s_obj->rx_dma_stream, &dma_init_struct_i2s,
                 i2s_obj->rx_dma_channel, DMA_PERIPH_TO_MEMORY, &i2s_obj->i2s);
        i2s_obj->i2s.hdmarx = &i2s_obj->rx_dma;

        i2s_obj->xfer_state = INACTIVE;
        i2s_obj->is_enabled = true;
    } else {
        i2s_obj->is_enabled = false;
    }
    return i2s_obj->is_enabled;
}

STATIC mp_obj_t pyb_i2s_deinit(mp_obj_t self_in);
void i2s_deinit(void) {
    for (int i = 0; i < MP_ARRAY_SIZE(MP_STATE_PORT(pyb_i2s_obj_all)); i++) {
        pyb_i2s_obj_t *i2s_obj = MP_STATE_PORT(pyb_i2s_obj_all)[i];
        if (i2s_obj != NULL) {
            pyb_i2s_deinit(i2s_obj);
        }
    }
}

// TODO: This is currently enabled to support the buffer-oriented methods send,
// recv, and send_recv. Eventually I2S should be entirely  non-blocking, using
// DMA with callbacks
STATIC HAL_StatusTypeDef i2s_wait_dma_finished(I2S_HandleTypeDef *i2s, uint32_t timeout) {
    uint32_t start = HAL_GetTick();
    while (HAL_I2S_GetState(i2s) != HAL_I2S_STATE_READY) {
        if (HAL_GetTick() - start >= timeout) {
            return HAL_TIMEOUT;
        }
        __WFI();
    }
    return HAL_OK;
}

STATIC HAL_StatusTypeDef i2s_bus_sync(pyb_i2s_obj_t *self, uint32_t polarity, uint32_t timeout) {
    // In I2S Slave mode, force a busy-wait for at least one full word-select
    // cycle to sync with WS clock before initiating a transfer.
    bool pol = polarity & 1;
    uint32_t start = HAL_GetTick();
    if (!i2s_is_master(self)) {
        while(GPIO_read_pin(self->pins[WS]->gpio, self->pins[WS]->pin) == pol) {
            if (HAL_GetTick() - start >= timeout) { return HAL_TIMEOUT; }
        }
        while(GPIO_read_pin(self->pins[WS]->gpio, self->pins[WS]->pin) == !pol) {
            if (HAL_GetTick() - start >= timeout) { return HAL_TIMEOUT; }
        }
        while(GPIO_read_pin(self->pins[WS]->gpio, self->pins[WS]->pin) == pol) {
            if (HAL_GetTick() - start >= timeout) { return HAL_TIMEOUT; }
        }
    }
    return HAL_OK;
}

STATIC void i2s_stream_handler(pyb_i2s_obj_t *self);
STATIC mp_obj_t pyb_i2s_callback(mp_obj_t self_in, mp_obj_t callback);

STATIC void i2s_handle_mp_callback(pyb_i2s_obj_t *self) {
    if (self->callback != mp_const_none) {
        gc_lock();
        nlr_buf_t nlr;
        if (nlr_push(&nlr) == 0) {
            mp_call_function_1(self->callback, self);
            nlr_pop();
        } else {
            // Uncaught exception; disable the callback so it doesn't run again.
            self->callback = mp_const_none;
            // DMA_HandleTypeDef dma = self->tx_dma;
            // __HAL_DMA_DISABLE(&dma);
            printf("uncaught exception in I2S(%u) DMA interrupt handler\n", self->i2s_id);
            mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
        }
        gc_unlock();
    }
}

void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s) {
    led_state(1, 1); //DEBUG
    // I2S root pointer index is 1 if both I2S instances are enabled and I2S
    // instance is SPI3/I2S3; otherwise index is 0:
    pyb_i2s_obj_t *self;
    if (0) {
#if MICROPY_HW_ENABLE_I2S2 && MICROPY_HW_ENABLE_I2S3
    } else if (hi2s->Instance == SPI3) {
        self = MP_STATE_PORT(pyb_i2s_obj_all)[1];
#endif
    } else {
        self = MP_STATE_PORT(pyb_i2s_obj_all)[0];
    }

    // The HAL_I2S_TxCpltCallback and RxCpltCallback should only get called after
    // a transfer is completed and the I2S bus is in the HAL_I2S_STATE_READY state.
    // However, the first time that HAL_I2S_TransmitReceiveDMA() gets called after
    // initializing the I2S bus, the callback gets triggered even though the I2S
    // bus is not in a ready state. This will cause a hard crash the next time
    // HAL_I2S_TransmitReceiveDMA() gets called.
    // Checking the state and calling HAL_I2S_DMAStop() before calling the
    // function again seems to resolve the problem; it also seems a bit hacky -
    // would be nice to figure out why this happens and how to prevent it.
    if (HAL_I2S_GetState(&self->i2s) != HAL_I2S_STATE_READY) {
        HAL_I2S_DMAStop(&self->i2s);
        // printf("I2S transfer stop and restart\n"); // DEBUG
    }

    if (self->xfer_state >= BUF_STR_DIV) {
        i2s_stream_handler(self);
    } else if (self->xfer_state != INACTIVE) {
        // buffer transfer, call user-defined callback
        i2s_handle_mp_callback(self);
    }
    led_state(1, 0); //DEBUG
}

void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s) {
    led_state(2, 1); // DEBUG
    // I2S root pointer index is 1 if both I2S instances are enabled and I2S
    // instance is SPI3/I2S3; otherwise index is 0:
    pyb_i2s_obj_t *self;
    if (0) {
#if MICROPY_HW_ENABLE_I2S2 && MICROPY_HW_ENABLE_I2S3
    } else if (hi2s->Instance == SPI3) {
        self = MP_STATE_PORT(pyb_i2s_obj_all)[1];
#endif
    } else {
        self = MP_STATE_PORT(pyb_i2s_obj_all)[0];
    }

    // In duplex mode, both HAL_I2S_TxCpltCallback and HAL_I2S_RxCpltCallback will
    // be triggered. This callback is defined to handle Rx-only transfers; it is
    // skipped for duplex transfers so callback code is only processed once:
    if (!self->is_duplex) {
        if (self->xfer_state == STREAM_IN) {
            i2s_stream_handler(self);
        } else if (self->xfer_state > INACTIVE && self->xfer_state < BUF_STR_DIV) {
            i2s_handle_mp_callback(self);
        }
    }
    led_state(2, 0); //DEBUG
}

// I2S HalfCplt and Error callback stubs, currently unused
void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s) {}
void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s) {}
void HAL_I2S_ErrorCallback(I2S_HandleTypeDef *hi2s) {
    printf("I2S Error\n");
}

/******************************************************************************/
/* Micro Python bindings                                                      */

// spi.c provides *spi_get_handle, do we need to provide *i2s_get_handle ?

STATIC void pyb_i2s_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_i2s_obj_t *self = self_in;

    mp_printf(print, "I2S(%u, pins=[", self->i2s_id);
    for (int i = 0; i < 4; i++) {
        if (self->pins[i] != MP_OBJ_NULL) {
            mp_printf(print, "%q ", self->pins[i]->name);
        } else {
            mp_print_str(print, "None ");
        }
    }
    mp_print_str(print, "\b]");
    if (self->is_enabled) {
        mp_print_str(print, ", I2S.");
        if (i2s_is_master(self)) {
            mp_printf(print, "%q, %q ", MP_QSTR_MASTER, MP_QSTR_mclkout);
            if (self->i2s.Init.MCLKOutput == I2S_MCLKOUTPUT_ENABLE) {
                mp_printf(print, "on %q", self->pins[MCK]->name);
            } else {
                mp_print_str(print, "off");
            }
            mp_printf(print, ", %q=%u", MP_QSTR_audiofreq, self->i2s.Init.AudioFreq);
        } else {
            mp_printf(print, "%q", MP_QSTR_SLAVE);
        }

        qstr standard = 0;
        if (self->i2s.Init.Standard == I2S_STANDARD_PHILIPS) { standard = MP_QSTR_PHILIPS; }
        else if (self->i2s.Init.Standard == I2S_STANDARD_MSB) { standard = MP_QSTR_MSB; }
        else if (self->i2s.Init.Standard == I2S_STANDARD_LSB) { standard = MP_QSTR_LSB; }
        else if (self->i2s.Init.Standard == I2S_STANDARD_PCM_SHORT) { standard = MP_QSTR_PCM_SHORT; }
        else if (self->i2s.Init.Standard == I2S_STANDARD_PCM_LONG) { standard = MP_QSTR_PCM_LONG; }
        else { /* shouldn't get here */ }
        mp_printf(print, ", %q=%u, %q=%q, ",
                  MP_QSTR_polarity, self->i2s.Init.CPOL, MP_QSTR_standard, standard);
        int n_data_bits = ((self->i2s.Init.DataFormat >> 1) + 2) * 8;
        mp_printf(print, "%q=%uB%s", MP_QSTR_dataformat, n_data_bits,
                  self->i2s.Init.DataFormat == I2S_DATAFORMAT_16B_EXTENDED ? "_EXT" : "");
    }
    mp_print_str(print, ")");
}

/// \method init(mode, dataformat=0, standard=I2S.PHILIPS,
///              polarity=0, audiofreq=48000, clksrc=I2S.PLL,
///              mclkout=0, callback=none)
///
/// Initialise the I2S bus with the given parameters:
///
///   - `mode` must be either `I2S.MASTER` or `I2S.SLAVE`.
///   - `standard` can be `PHILIPS`, `MSB`, `LSB`, `PCM_SHORT`, or `PCM_LONG`.
///   - `dataformat` can be 0 (16B_EXTENDED), 16, 24, or 32 (bits).
///   - `polarity` can be 0 for clock steady state low, or 1 for steady state high.
///   - Options only relevant to master mode:
///   - `audiofreq` can be any common audio sampling frequency, default is 48000.
///   - `clksrc` can be `PLL` or `EXTERNAL`.
///   - `mclkout` can be 0 to disable or 1 to enable.
STATIC mp_obj_t pyb_i2s_init_helper(pyb_i2s_obj_t *self, mp_uint_t n_args,
                                    const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode,      MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0 /* MASTER */ } },
        { MP_QSTR_dataformat, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0 /* 16B EXTENDED */ } },
        { MP_QSTR_standard,   MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = I2S_STANDARD_PHILIPS} },
        { MP_QSTR_polarity,   MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_audiofreq,  MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = I2S_AUDIOFREQ_48K} },
        { MP_QSTR_clksrc,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = I2S_CLOCK_PLL} },
        { MP_QSTR_mclkout,    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_callback,   MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        // TODO: Include option for setting I2SPLL parameters directly?
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args,
                     MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // set the I2S configuration values
    memset(&self->i2s, 0, sizeof(self->i2s));
    I2S_InitTypeDef *init = &self->i2s.Init;

    // init->Mode can be MASTER_TX, MASTER_RX, SLAVE_TX, or SLAVE_RX;
    if (args[0].u_int == 0) {
        init->Mode = self->base_is_tx ? I2S_MODE_MASTER_TX : I2S_MODE_MASTER_RX;
    } else {
        init->Mode = self->base_is_tx ? I2S_MODE_SLAVE_TX : I2S_MODE_SLAVE_RX;
    }

    if (args[1].u_int == 0)  { init->DataFormat = I2S_DATAFORMAT_16B_EXTENDED; }
    else if (args[1].u_int == 16) { init->DataFormat = I2S_DATAFORMAT_16B; }
    else if (args[1].u_int == 24) { init->DataFormat = I2S_DATAFORMAT_24B; }
    else if (args[1].u_int == 32) { init->DataFormat = I2S_DATAFORMAT_32B; }
    else { nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                                                   "%d not a valid data format", args[2].u_int)); }

    init->Standard   = args[2].u_int;
    init->CPOL       = args[3].u_int ? I2S_CPOL_HIGH : I2S_CPOL_LOW;
    init->AudioFreq  = args[4].u_int;
    init->MCLKOutput = args[6].u_int ? I2S_MCLKOUTPUT_ENABLE : I2S_MCLKOUTPUT_DISABLE;
    init->FullDuplexMode = self->is_duplex ?
        I2S_FULLDUPLEXMODE_ENABLE : I2S_FULLDUPLEXMODE_DISABLE;

    // If an I2S object had previously registered callback, the default is to
    // keep that callback. Is this correct, or should the default be to set the
    // callback to None on init?
    if (args[7].u_obj != mp_const_none) {
        pyb_i2s_callback(self, args[7].u_obj);
    }

    // -------------Possible bug in HAL-------------------
    // According to the datasheet (RM0090, Sec 28.4.6 - I2S Slave Mode) in Slave
    // mode there is no need to enable I2SPLL to generate a clock for the I2S
    // interface; the CLK and WS pins receive clock signals from another master
    // to drive the data transfer.
    // HOWEVER, HAL_I2S_Init returns an error if the I2SPLL is not enabled and
    // ClockSource != I2S_CLOCK_EXTERNAL
    // This appears to be incorrect behavior; the parameter I2S_CLOCK_EXTERNAL
    // is intended to set pin PC9 as I2S_CKIN, an external source for I2SPLL.
    // So I don't think that this work around is really correct here:
    if (args[0].u_int) {
        // Slave mode
        init->ClockSource = I2S_CLOCK_EXTERNAL;
    } else {
        init->ClockSource = args[5].u_int;
    }

    // init the I2S bus
    if (!i2s_init(self)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                                                "I2S port %d init failed", self->i2s_id));
    }

    return mp_const_none;
}

/// \classmethod \constructor(bus, ...)
///
/// Construct an I2S object with the given list of pins.
/// I2S requires a clock pin (CK), a word select pin (WS) and either
/// one or two data pins.
/// The pins can be passed as either a list or a tuple with the format
/// [Clock, Word Select, TX Data, RX Data]
/// If two data pins are provided, one must be an SD pin and the other must
/// be a corresponding SDext pin; the I2S port will be initialized in duplex
/// (TX + RX) mode.
/// If only one data pin is provided, it must be an SD (not SDext) pin. The
/// I2S port will be initialized in simplex mode, with the direction determined
/// by the position of the data pin in the list.
/// Examples:
/// pin_list = ['B10','B12','C3','C2'] - Duplex mode, C3 as Tx, C2 as Rx
/// pin_list = ['B10','B12',0,'C2'] - Simplex mode, C2 as RX
/// pin_list = ['Y6','Y5','Y7','Y8'] - Use board name strings
/// pin_list = ('Y6','Y5','Y7','Y8') - Tuple works as well as list
/// Mixed list of strings and objects:
/// pin_list = [pyb.Pin.cpu.B10,'B12','Y7','pyb.Pin.board.Y8']
///
/// Valid pins for I2S2 on the pyboard:
/// CK -    PB13 / Y6,  PB10 / Y9     (SPI2 SCK)
/// WS -    PB12 / Y5,  PB9  / Y4     (SPI2 NSS)
/// SD -    PB15 / Y8,  PC3  / X22    (SPI2 MOSI)
/// SDext - PB14 / Y7,  PC2  / X21    (SPI2 MISO)
/// MCK -   PC6 / Y1
///
/// The I2S3 port is disabled by default on the pyboard, as its pins would
/// conflict with the SD Card and other pyboard functions. It can be enabled
/// for the STM32F4DISCOVERY port, which has an I2S codec on I2S3.
///
/// CK -    PC10 / NA (SD_D2),  PB3  / X17 (USR SW)       (SPI3 SCK)
/// WS -    PA4  / X5,          PA15 / P3  (YELLOW LED)   (SPI3 NSS)
/// SD -    PC12 / NA (SD_CK),  PB5  / NA  (MMA_AVDD)     (SPI3 MOSI)
/// SDext - PC11 / NA (SD_D3),  PB4  / P2  (BLUE LED)     (SPI3 MISO)
/// MCK -   PC7 (Y2)
///
/// With no additional parameters, the I2S object is created but not
/// initialised (it has the settings from the last initialisation of
/// the bus, if any).  If extra arguments are given, the bus is initialised.
/// See `init` for parameters of initialisation.

STATIC mp_obj_t pyb_i2s_make_new(mp_obj_t type_in, mp_uint_t n_args,
                                 mp_uint_t n_kw, const mp_obj_t *args) {

    // If n_args == 0 and I2S2 is enabled, set default pin list to the SPI2
    // pins on the pyboard (Y6, Y5, Y7, Y8):
#ifdef MICROPY_HW_ENABLE_I2S2
    mp_arg_check_num(n_args, n_kw, 0, MP_OBJ_FUN_ARGS_MAX, true);
#else
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);
#endif

    // get array of pin identifiers, could be name strings or pin objects
    mp_uint_t array_len = 0;
    mp_obj_t *pin_names;
    if (n_args == 0) {
#ifdef MICROPY_HW_ENABLE_I2S2
        static const pin_obj_t *default_pins[4] = {&pin_B13, &pin_B12, &pin_B15, &pin_B14};
        pin_names = (mp_obj_t)default_pins;
        array_len = 4;
#endif
    } else {
        mp_obj_get_array(args[0], &array_len, &pin_names);
    }
    if (array_len != 4) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                                                "Pin list requires 4 items, %d given",
                                                array_len));
    }

    // Get array of pin objects: False / empty values are valid for pins[2:3],
    // empty values get set to MP_OBJ_NULL
    // pins[0:3] set from user-provided array; pins[4] is MCK which is fixed to be
    // C6 for I2S2 and C7 for I2S3:
    const pin_obj_t *pins[5];
    for (int i = 0; i < 4; i++) {
        if (mp_obj_is_true(pin_names[i])) {
            pins[i] = pin_find(pin_names[i]);
        } else {
            pins[i] = MP_OBJ_NULL;
        }

        // DEBUG - print list of pin objects. This information can also
        // be accessed from within uPy with the command pyb.Pin.debug(True)
        if (0) {
            mp_obj_print((mp_obj_t)pins[i], PRINT_STR);
            printf("\n");
        }
    }

    // Check if pins represents a valid I2S port configuration:
    // Each entry of pins[] array (0=BCK, 1=WS, 2=TX, 3=RX) is checked to
    // determine if it is valid for its designated function.
    // Of the two entries for data pins (2-TX and 3-RX) exactly one of them
    // must be a valid base SD pin for the same I2S port as BCK and WS; the
    // other can be either a valid SDext pin to select duplex mode, or empty
    // to select simplex mode.

    const pin_af_obj_t *pin_af[5];
    mp_uint_t i2s_id = 0;
    bool is_duplex = false;
    bool base_is_tx = false;
    qstr pin_err = 0;

    // Check pins[BCK] (first entry in array) to determine which I2S instance
    // it belongs to. The pin will have an I2S af for at most one of the
    // available ports. Each I2S instance has one pin that can be used for MCK,
    // this gets added to the array here as the I2S instance is selected.
#if MICROPY_HW_ENABLE_I2S2
    pin_af[BCK] = pin_find_af(pins[BCK], AF_FN_I2S, 2);
    if (pin_af[BCK]->type == AF_PIN_TYPE_I2S_CK) {
        i2s_id = 2;
        pins[MCK] = &pin_C6;
    }
#endif
#if MICROPY_HW_ENABLE_I2S3
    pin_af[BCK] = pin_find_af(pins[BCK], AF_FN_I2S, 3);
    if (pin_af[BCK]->type == AF_PIN_TYPE_I2S_CK) {
        i2s_id = 3;
        pins[MCK] = &pin_C7;
    }
#endif

    if (i2s_id == 0) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                                                "%q not an I2S BCK pin", pins[BCK]->name));
    } else {
        for (int i = 0; i < NUM_PINS /* = 5 */; i++) {
            pin_af[i] = pin_find_af(pins[i], AF_FN_I2S, i2s_id);
        }
    }

#if (0)
    for (int i = 0; i < NUM_PINS; i++) {
        if (pins[i]) {
            printf("pin = %s, af = %s\n", qstr_str(pins[i]->name), qstr_str(pin_af[i]->name));
        } else {
            printf("pin = %s, af = %s\n", "no pin", "no af");
        }
    }
#endif

    if (pin_af[WS]->type == AF_PIN_TYPE_I2S_WS) {
        // Clock and Word Select pins are valid
        if (pin_af[TX]->type == AF_PIN_TYPE_I2S_SD) {
            base_is_tx = true;
            if (pin_af[RX]->type == AF_PIN_TYPE_I2S_EXTSD) {
                is_duplex = true;
            } else if (pins[RX] == MP_OBJ_NULL) {
                is_duplex = false;
            } else {
                pin_err = pins[RX]->name;
            }
        } else if (pin_af[RX]->type == AF_PIN_TYPE_I2S_SD) {
            base_is_tx = false;
            if (pin_af[TX]->type == AF_PIN_TYPE_I2S_EXTSD) {
                is_duplex = true;
            } else if (pins[TX] == MP_OBJ_NULL) {
                is_duplex = false;
            } else {
                pin_err = pins[TX]->name;
            }
        } else {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                                                    "no valid SD pin for I2S%d", i2s_id));
        }
    } else {
        pin_err = pins[WS]->name;
    }

    if (pin_err) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                                                "%q not valid for I2S%d",
                                                pin_err, i2s_id));
    }


#if (MICROPY_HW_ENABLE_I2S3) && !(MICROPY_HW_ENABLE_I2S2)
#define I2S_OBJECT_OFFSET (3)
#else
#define I2S_OBJECT_OFFSET (2)
#endif
    // get I2S object and set initialization flags
    pyb_i2s_obj_t *i2s_obj;
    if (MP_STATE_PORT(pyb_i2s_obj_all)[i2s_id - I2S_OBJECT_OFFSET] == NULL) {
        // create new I2S object
        i2s_obj = m_new_obj(pyb_i2s_obj_t);
        i2s_obj->base.type = &pyb_i2s_type;
        i2s_obj->i2s_id = i2s_id;
        i2s_obj->is_enabled = false;
        MP_STATE_PORT(pyb_i2s_obj_all)[i2s_id - I2S_OBJECT_OFFSET] = i2s_obj;
    } else {
        i2s_obj = MP_STATE_PORT(pyb_i2s_obj_all)[i2s_id - I2S_OBJECT_OFFSET];
    }

    i2s_obj->callback = mp_const_none;
    i2s_obj->is_duplex = is_duplex;
    i2s_obj->base_is_tx = base_is_tx;
    for (int i = 0; i < NUM_PINS; i++) {
        i2s_obj->pins[i] = pins[i];
    }

    if (n_args > 1 || n_kw > 0) {
        // start the peripheral
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        pyb_i2s_init_helper(i2s_obj, n_args - 1, args + 1, &kw_args);
    }

    return (mp_obj_t)i2s_obj;
}

STATIC mp_obj_t pyb_i2s_init(mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return pyb_i2s_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_i2s_init_obj, 1, pyb_i2s_init);

/// \method deinit()
/// Turn off the I2S bus.
STATIC mp_obj_t pyb_i2s_deinit(mp_obj_t self_in) {
    pyb_i2s_callback(self_in, mp_const_none);
    pyb_i2s_obj_t *self = self_in;
    if (self->is_enabled) {
        dma_deinit(&self->tx_dma);
        dma_deinit(&self->rx_dma);
        HAL_I2S_DeInit(&self->i2s);
        self->is_enabled = false;
    }
    if (0) {
#if MICROPY_HW_ENABLE_I2S2
    } else if (self->i2s.Instance == SPI2) {
        __SPI2_FORCE_RESET();
        __SPI2_RELEASE_RESET();
        __SPI2_CLK_DISABLE();
#endif
#if MICROPY_HW_ENABLE_I2S3
    } else if (self->i2s.Instance == SPI3) {
        __SPI3_FORCE_RESET();
        __SPI3_RELEASE_RESET();
        __SPI3_CLK_DISABLE();
#endif
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_i2s_deinit_obj, pyb_i2s_deinit);

// TODO - adapt docstrings copied from spi.c for i2s

/// \method send(send, *, timeout=100)
/// Send data on the bus:
///
///   - `send` is the data to send (an integer to send, or a buffer object).
///   - `timeout` is the timeout in milliseconds to wait for the send.
///
/// Return value: `None`.
STATIC mp_obj_t pyb_i2s_send(mp_uint_t n_args, const mp_obj_t *pos_args,
                             mp_map_t *kw_args) {
    // skeleton copied from spi.c
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_send,    MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 100} },
    };

    // parse args
    pyb_i2s_obj_t *self = pos_args[0];
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args,
                     MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // 'send' can be used in either simplex or duplex mode, but only if the
    // base i2s instance is in TX mode - in duplex mode, the extended instance
    // must transmit using send_recv

    // TODO - invoke HAL_I2SEx_TransmitReceive_DMA to handle send on I2Sx_EXT
    // interfaces when available instead of raising error
    if (!self->base_is_tx) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                                                "I2S(%d) base instance not in transmit mode",
                                                self->i2s_id));
    }

    if (self->xfer_state > BUF_STR_DIV) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError,
                                           "Buffer op not allowed while streaming"));
    }
    self->xfer_state |= BUFFER_WR;
    // get the buffer to send from
    mp_buffer_info_t bufinfo;
    uint8_t data[1];
    pyb_buf_get_for_send(args[0].u_obj, &bufinfo, data);

    // send the data
    HAL_StatusTypeDef status;
    status = i2s_bus_sync(self, 0 /*polarity*/, 100);
    if (status != HAL_OK) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "No I2S bus clock"));
    }
    // TODO - implement 24-bit and 32-bit data width cases for all methods
    if (query_irq() == IRQ_STATE_DISABLED) {
        status = HAL_I2S_Transmit(&self->i2s, bufinfo.buf, bufinfo.len / 2, args[1].u_int);
    } else {
        status = HAL_I2S_Transmit_DMA(&self->i2s, bufinfo.buf, bufinfo.len / 2);
        if (status == HAL_OK) {
            status = i2s_wait_dma_finished(&self->i2s, args[1].u_int);
        }
    }

    if (status != HAL_OK) {
        mp_hal_raise(status);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_i2s_send_obj, 1, pyb_i2s_send);

// TODO - adapt docstrings copied from spi.c for i2s

/// \method recv(recv, *, timeout=100)
///
/// Receive data on the bus:
///
///   - `recv` can be an integer, which is the number of bytes to receive,
///     or a mutable buffer, which will be filled with received bytes.
///   - `timeout` is the timeout in milliseconds to wait for the receive.
///
/// Return value: if `recv` is an integer then a new buffer of the bytes received,
/// otherwise the same buffer that was passed in to `recv`.
STATIC mp_obj_t pyb_i2s_recv(mp_uint_t n_args, const mp_obj_t *pos_args,
                             mp_map_t *kw_args) {
    // TODO check transmission size for I2S

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_recv,    MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 100} },
    };

    // parse args
    pyb_i2s_obj_t *self = pos_args[0];
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args,
                     MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // 'recv' can be used in either simplex or duplex mode, but only if the
    // base i2s instance is in RX mode - in duplex mode, the extended instance
    // must receive using send_recv

    // TODO - invoke HAL_I2SEx_TransmitReceive_DMA to handle recv on I2Sx_EXT
    // interfaces when available instead of raising error?
    if (self->base_is_tx) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                                                "I2S(%d) base instance not in receive mode",
                                                self->i2s_id));
    }

    if (self->xfer_state > BUF_STR_DIV) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError,
                                           "Buffer op not allowed while streaming"));
    }
    self->xfer_state |= BUFFER_RD;
    // get the buffer to receive into
    vstr_t vstr;
    mp_obj_t o_ret = pyb_buf_get_for_recv(args[0].u_obj, &vstr);

    // receive the data
    HAL_StatusTypeDef status;
    status = i2s_bus_sync(self, 0 /*polarity*/, 100);
    if (status != HAL_OK) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "No I2S bus clock"));
    }
    // TODO - implement 24-bit and 32-bit data transfers for all methods
    if (query_irq() == IRQ_STATE_DISABLED) {
        status = HAL_I2S_Receive(&self->i2s, (uint16_t*)vstr.buf,
                                 vstr.len / 2, args[1].u_int);
    } else {
        status = HAL_I2S_Receive_DMA(&self->i2s, (uint16_t*)vstr.buf, vstr.len / 2);
        if (status == HAL_OK) {
            status = i2s_wait_dma_finished(&self->i2s, args[1].u_int);
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
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_i2s_recv_obj, 1, pyb_i2s_recv);

// TODO - adapt docstrings copied from spi.c for i2s

/// \method send_recv(send, recv=None, *, timeout=100)
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
STATIC mp_obj_t pyb_i2s_send_recv(mp_uint_t n_args, const mp_obj_t *pos_args,
                                  mp_map_t *kw_args) {

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_send,    MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_recv,    MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    };

    // parse args
    pyb_i2s_obj_t *self = pos_args[0];
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args,
                     MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // TODO - invoke HAL_I2SEx_TransmitReceive_DMA to handle recv on I2Sx_EXT
    // interfaces when available instead of raising error?
    // TODO - Could this test be wrapped in a simple inline function to replace
    // the pyb_i2s_obj->is_duplex flag?
    if (self->i2s.Init.FullDuplexMode != I2S_FULLDUPLEXMODE_ENABLE) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                                                "I2S(%d) not in duplex mode", self->i2s_id));
    }

    if (self->xfer_state >= BUF_STR_DIV) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError,
                                           "Buffer op not allowed while streaming"));
    }
    self->xfer_state = BUFFER_RD_WR;
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
                nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError,
                                                   "recv must be same length as send"));
            }
            o_ret = args[1].u_obj;
        }
    }

    // send and receive the data
    HAL_StatusTypeDef status;
    status = i2s_bus_sync(self, 0 /*polarity*/, 100);
    if (status != HAL_OK) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "No I2S bus clock"));
    }
    // TODO - implement 24-bit and 32-bit data width cases for all methods
    if (query_irq() == IRQ_STATE_DISABLED) {
        status = HAL_I2SEx_TransmitReceive(&self->i2s, bufinfo_send.buf, bufinfo_recv.buf,
                                           bufinfo_send.len / 2, args[2].u_int);
    } else {
        status = HAL_I2SEx_TransmitReceive_DMA(&self->i2s, bufinfo_send.buf,
                                               bufinfo_recv.buf, bufinfo_send.len / 2);
        if (status == HAL_OK) {
            status = i2s_wait_dma_finished(&self->i2s, args[2].u_int);
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
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_i2s_send_recv_obj, 1, pyb_i2s_send_recv);


/**** I2S streaming methods: ****/
// Taken from stream.c; could use to make sure stream is opened in binary mode?
#define STREAM_CONTENT_TYPE(stream) (((stream)->is_text) ? &mp_type_str : &mp_type_bytes)

// helper functions for stream operations; to be removed if similar functions get
// incorporated into stream.[ch]
typedef enum {
    MP_STREAM_OP_READ,
    MP_STREAM_OP_WRITE,
    MP_STREAM_OP_IOCTL,
} mp_stream_op_t;

mp_obj_t mp_stream_op_supported(mp_obj_t self_in, mp_stream_op_t op) {
    struct _mp_obj_base_t *o = (struct _mp_obj_base_t *)self_in;
    if (o->type->stream_p == NULL) {
        // CPython: io.UnsupportedOperation, OSError subclass
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError,
                                           "stream object required"));
    } else if (op == MP_STREAM_OP_READ && o->type->stream_p->read == NULL) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError,
                                           "object with stream.read required"));
    } else if (op == MP_STREAM_OP_WRITE && o->type->stream_p->write == NULL) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError,
                                           "object with stream.write required"));
    } else if (op == MP_STREAM_OP_IOCTL && o->type->stream_p->ioctl == NULL) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError,
                                           "object with stream.ioctl required"));
    }
    return o;
}

mp_obj_t mp_stream_read(mp_obj_t self_in, void *buf, mp_uint_t len) {
    // Supported op check included here for protability and to be equivalent to
    // mp_stream_write; some stream read methods will now have redundant checks:
    struct _mp_obj_base_t *o = mp_stream_op_supported(self_in, MP_STREAM_OP_READ);
    int error;
    mp_uint_t out_sz = o->type->stream_p->read(self_in, buf, len, &error);
    if (out_sz == MP_STREAM_ERROR) {
        if (mp_is_nonblocking_error(error)) {
            return mp_const_none;
        }
        nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(error)));
    } else {
        return MP_OBJ_NEW_SMALL_INT(out_sz);
    }
}

STATIC mp_obj_t pyb_i2s_stream_out(mp_uint_t n_args, const mp_obj_t *pos_args,
                                   mp_map_t *kw_args) {

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_stream_out, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 100} },
        { MP_QSTR_len, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        // TODO: Allow length to have units other than n_samples
    };

    // parse args
    pyb_i2s_obj_t *self = pos_args[0];
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args,
                     MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t stream = mp_stream_op_supported(args[0].u_obj, MP_STREAM_OP_READ);

    // make sure that I2S bus is configured for transmit:
    if (!i2s_has_tx(self)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                                                "I2S(%d) not configured for tx",
                                                self->i2s_id));
    }

    // If I2S state is not 'ready', raise an error _unless_ the current transaction is a
    // simplex stream_in(); in that case set signals and stream handle to begin duplex
    // stream_in + stream_out with next callback:
    if (HAL_I2S_GetState(&self->i2s) != HAL_I2S_STATE_READY) {
        if (self->xfer_state != STREAM_IN) {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                                                    "I2S(%d) bus busy",
                                                    self->i2s_id));
        } else {
            // attach stream handle to i2s object and update state for
            // stream_handler to add stream_out to ongoing stream transaction 
            self->xfer_signal = I2S_SIG_NONE;
            self->xfer_state |= STREAM_OUT;
            self->stream_tx_len = args[2].u_int;
            self->dstream_tx = stream;
        }
    } else {
        // set up for simplex stream_out
        self->xfer_signal = I2S_SIG_NONE;
        self->xfer_state = STREAM_OUT;
        self->stream_tx_len = args[2].u_int;
        int buf_sz = AUDIOBUFFER_BYTES / 4;
        self->dstream_tx = stream;
        self->pp_ptr = 0;

        // If stream length is specified:
        if ((self->stream_tx_len > -1) && (buf_sz / 4 > self->stream_tx_len)) {
            buf_sz = self->stream_tx_len * 4;
        }
        mp_obj_t ret = mp_stream_read(self->dstream_tx, &self->audiobuf_tx[self->pp_ptr], buf_sz);
        if (mp_obj_is_integer(ret)) {
            self->last_stream_read_sz = mp_obj_get_int(ret);
            // decrement length if it was specified
            if (self->stream_tx_len > -1) { 
                if (self->stream_tx_len >= self->last_stream_read_sz / 4) {
                    self->stream_tx_len -= self->last_stream_read_sz / 4;
                } else {
                    self->stream_tx_len = 0;
                }
            }
            if (self->last_stream_read_sz == 0) {
                self->xfer_signal = I2S_SIG_EOF_TX;
            }
        } else {
            // If ret isn't an integer it indicates a stream read error, stop tx:
            self->xfer_signal = I2S_SIG_EOF_TX;
        }

        // Sync to I2S bus and start sending data:
        HAL_StatusTypeDef status = i2s_bus_sync(self, 0 /*polarity*/, 100);
        if (status != HAL_OK) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "No I2S bus clock"));
        }

        i2s_stream_handler(self);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_i2s_stream_out_obj, 1, pyb_i2s_stream_out);

STATIC mp_obj_t pyb_i2s_stream_in (mp_uint_t n_args, const mp_obj_t *pos_args,
                                   mp_map_t *kw_args) {

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_stream_in, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 100} },
        { MP_QSTR_len, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        // TODO: Allow length to have units other than n_samples
    };

    // parse args
    pyb_i2s_obj_t *self = pos_args[0];
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args,
                     MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t stream = mp_stream_op_supported(args[0].u_obj, MP_STREAM_OP_WRITE);

    // make sure that I2S bus is configured for receive:
    if (!i2s_has_rx(self)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                                                "I2S(%d) not configured for rx",
                                                self->i2s_id));
    }
    
    // If I2S state is not 'ready', raise an error _unless_ the current transaction is a
    // simplex stream_out(); in that case set signals and stream handle to begin duplex
    // stream_in + stream_out with next callback:
    if (HAL_I2S_GetState(&self->i2s) != HAL_I2S_STATE_READY) {
        if (self->xfer_state != STREAM_OUT) {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                                                    "I2S(%d) bus busy",
                                                    self->i2s_id));
        } else {
            // attach stream handle to i2s object and update state for
            // stream_handler to add stream_in to ongoing stream transaction 
            self->xfer_signal = I2S_SIG_NONE;
            self->xfer_state |= STREAM_IN;
            self->stream_rx_len = args[2].u_int;
            self->dstream_rx = stream;
        }
    } else {
        // set up for simplex stream_in
        self->xfer_signal = I2S_SIG_NONE;
        self->xfer_state = STREAM_IN;
        self->stream_rx_len = args[2].u_int;
        int buf_sz = AUDIOBUFFER_BYTES / 4;
        self->dstream_rx = stream;
        self->pp_ptr = 0;

        // NOTE: stream_in is not the simple switching of rx/tx and read/write methods
        // from stream_out - the order of operations also must be reversed; the receive
        // must fill the buffer _before_ the buffer is written out a stream, and data
        // must be written out after the last receive. stream_in initiates the streaming
        // process by calling HAL_I2S_Receive_DMA or HAL_I2SEx_TransmitReceive_DMA, and
        // any data received is written to dstream_rx by stream_handler when it is
        // invoked via HAL I2S callback

        // Sync to I2S bus and start receiving data:
        HAL_StatusTypeDef status = i2s_bus_sync(self, 0 /*polarity*/, 100);
        if (status != HAL_OK) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "No I2S bus clock"));
        }

        if (self->is_duplex) {
            // clear transmit buffers to use duplex function to begin simplex stream_in:
            memset(&self->audiobuf_tx[0], 0, buf_sz);
            memset(&self->audiobuf_tx[1], 0, buf_sz);
            status = HAL_I2SEx_TransmitReceive_DMA(&self->i2s,
                                                   self->audiobuf_tx[self->pp_ptr],
                                                   self->audiobuf_rx[self->pp_ptr],
                                                   buf_sz / 2);
        } else {
            status = HAL_I2S_Receive_DMA(&self->i2s,
                                         self->audiobuf_rx[self->pp_ptr],
                                         buf_sz / 2);
        }

        if (status != HAL_OK) {
            mp_hal_raise(status);
        }
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_i2s_stream_in_obj, 1, pyb_i2s_stream_in);

STATIC void i2s_stream_handler(pyb_i2s_obj_t *self) {
    // i2s_stream_handler manages HAL_I2S_*_DMA functions and data transfers
    // between streams and buffers:
    int buf_sz = AUDIOBUFFER_BYTES / 4;
    HAL_StatusTypeDef status;

    // Set xfer_state as indicated by xfer_signal:
    int sig = self->xfer_signal;
    int state = self->xfer_state;
    if (sig == I2S_SIG_STOP_ALL || sig == I2S_SIG_EOF_ALL) {
        self->xfer_state = INACTIVE;
        self->xfer_signal = I2S_SIG_NONE;
    } else if (sig == I2S_SIG_STOP_RX || sig == I2S_SIG_EOF_RX) {
        if (state == STREAM_OUT || state == STREAM_IN_OUT) {
            self->xfer_state = STREAM_OUT;
        } else {
            self->xfer_state = INACTIVE;
        }
        self->xfer_signal = I2S_SIG_NONE;
    } else if (sig == I2S_SIG_STOP_TX || sig == I2S_SIG_EOF_TX) {
        if (state == STREAM_IN || state == STREAM_IN_OUT) {
            self->xfer_state = STREAM_IN;
        } else {
            self->xfer_state = INACTIVE;
        }
        self->xfer_signal = I2S_SIG_NONE;
    }
    // If sig == I2S_SIG_EOF_* then invoke user-defined callback
    if (sig > I2S_SIG_STOP_EOF_DIV) {
        i2s_handle_mp_callback(self);
    }

    // DMA transfer:
    if (self->xfer_state >= BUF_STR_DIV) {
        // xfer_state indicates active stream transfer
        self->pp_ptr = !(self->pp_ptr);
        if (self->is_duplex) {
            int sz;
            if (self->xfer_state == STREAM_IN) {
                sz = buf_sz / 2;
            } else {
                sz = self->last_stream_read_sz / 2;
            }
            status = HAL_I2SEx_TransmitReceive_DMA(&self->i2s,
                                                   self->audiobuf_tx[self->pp_ptr],
                                                   self->audiobuf_rx[self->pp_ptr],
                                                   sz);

        } else if (self->xfer_state == STREAM_OUT) {
            status = HAL_I2S_Transmit_DMA(&self->i2s,
                                          self->audiobuf_tx[self->pp_ptr],
                                          self->last_stream_read_sz / 2);

        } else if (self->xfer_state == STREAM_IN) {
            // TODO - Simplex receive should not use last read or write size!
            status = HAL_I2S_Receive_DMA(&self->i2s,
                                         self->audiobuf_rx[self->pp_ptr],
                                         buf_sz / 2);

        } else {
            // TODO: clean up and raise an error?
            // shouldn't get here in normal operation
            status = HAL_ERROR;
        }
    } else {
        // If xfer_state indicates active buffer transfer (it shouldn't get here
        // in that case) or INACTIVE, all stream transfers are stopped.
        status = HAL_I2S_DMAStop(&self->i2s);
    }

    if (status != HAL_OK) {
        mp_hal_raise(status);
    }

    bool buf_ptr = !(self->pp_ptr);

    if ((self->xfer_state & STREAM_OUT) != 0) {
        if ((self->stream_tx_len > -1) && (buf_sz / 4 > self->stream_tx_len)) {
            buf_sz = self->stream_tx_len * 4;
        }
        mp_obj_t ret = mp_stream_read(self->dstream_tx, &self->audiobuf_tx[buf_ptr], buf_sz);
        if (mp_obj_is_integer(ret)) {
            self->last_stream_read_sz = mp_obj_get_int(ret);
            // decrement tx length if it was specified:
            if (self->stream_tx_len > -1) {
                if (self->stream_tx_len >= self->last_stream_read_sz / 4) {
                    self->stream_tx_len -= self->last_stream_read_sz / 4;
                } else {
                    self->stream_tx_len = 0;
                }
            }
            if (self->last_stream_read_sz == 0) {
                // A read of 0 length happens when at the end of the file stream or because
                // self->stream_tx_len == 0, either way set TX signal to EOF:
                self->xfer_signal = I2S_SIG_EOF_TX;
            }
        } else {
            // If ret isn't an integer it indicates a stream read error, stop tx:
            self->xfer_signal = I2S_SIG_EOF_TX;
        }
    }
    if ((self->xfer_state & STREAM_IN) != 0) {
        buf_sz = AUDIOBUFFER_BYTES / 4;
        if ((self->stream_rx_len > -1) && (buf_sz / 4 > self->stream_rx_len)) {
            buf_sz = self->stream_rx_len * 4;
        }
        mp_obj_t ret = mp_stream_write(self->dstream_rx, &self->audiobuf_rx[buf_ptr], buf_sz);
        if (mp_obj_is_integer(ret)) {
            self->last_stream_write_sz = mp_obj_get_int(ret);
            // decrement rx length if it was specified:
            if (self->stream_rx_len > -1) {
                if (self->stream_rx_len >= self->last_stream_write_sz / 4) {
                    self->stream_rx_len -= self->last_stream_write_sz / 4;
                } else {
                    self->stream_rx_len = 0;
                }
            }
            if (self->last_stream_write_sz == 0) {
                // A write of 0 length happens when the filesystem is full or when
                // self->stream_rx_len == 0, either way set RX signal to EOF:
                self->xfer_signal = I2S_SIG_EOF_RX;
            }
        } else {
            // If ret isn't an integer it indicates a stream write error, stop rx:
            self->xfer_signal = I2S_SIG_EOF_RX;
        }
    }
}

STATIC mp_obj_t pyb_i2s_pause(mp_obj_t self_in){
    pyb_i2s_obj_t *self = self_in;
    HAL_StatusTypeDef status;
    status = HAL_I2S_DMAPause(&self->i2s);
    if (status != HAL_OK) {
        mp_hal_raise(status);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_i2s_pause_obj, pyb_i2s_pause);

STATIC mp_obj_t pyb_i2s_resume(mp_obj_t self_in){
    pyb_i2s_obj_t *self = self_in;
    HAL_StatusTypeDef status;
    status = HAL_I2S_DMAResume(&self->i2s);
    if (status != HAL_OK) {
        mp_hal_raise(status);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_i2s_resume_obj, pyb_i2s_resume);

STATIC mp_obj_t pyb_i2s_stop(mp_uint_t n_args, const mp_obj_t *args){
    pyb_i2s_obj_t *self = args[0];
    if (n_args > 1) {
        const char *p = mp_obj_str_get_str(args[1]);
        qstr signal = qstr_from_str(p); 
        if (signal == MP_QSTR_stream_in) {
            self->xfer_signal |= I2S_SIG_STOP_RX;
        } else if (signal == MP_QSTR_stream_out) {
            self->xfer_signal |= I2S_SIG_STOP_TX;
        } else {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                                                    "%s not a valid stop argument", p));
        }
    } else {
        self->xfer_signal = I2S_SIG_STOP_ALL;
    }
    //printf("stp %d,%d", self->xfer_signal, self->xfer_state);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_i2s_stop_obj, 1, 2, pyb_i2s_stop);

STATIC mp_obj_t pyb_i2s_callback(mp_obj_t self_in, mp_obj_t callback) {
    // Stub method to set a Python callback
    // TODO: determine whether interrupts need to be disabled; maybe not,
    // since I anticipate that I2S callbacks may be changed on the fly to
    // manage the behavior of long buffer transfers
    pyb_i2s_obj_t *self = self_in;
    if (callback == mp_const_none) {
        self->callback = mp_const_none;
    } else if (mp_obj_is_callable(callback)) {
        self->callback = callback;
    } else {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "callback must be None or a callable object"));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_i2s_callback_obj, pyb_i2s_callback);


STATIC const mp_map_elem_t pyb_i2s_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_init), (mp_obj_t)&pyb_i2s_init_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_deinit), (mp_obj_t)&pyb_i2s_deinit_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_send), (mp_obj_t)&pyb_i2s_send_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_recv), (mp_obj_t)&pyb_i2s_recv_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_send_recv), (mp_obj_t)&pyb_i2s_send_recv_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_stream_out), (mp_obj_t)&pyb_i2s_stream_out_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_stream_in), (mp_obj_t)&pyb_i2s_stream_in_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_pause), (mp_obj_t)&pyb_i2s_pause_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_resume), (mp_obj_t)&pyb_i2s_resume_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_stop), (mp_obj_t)&pyb_i2s_stop_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_callback), (mp_obj_t)&pyb_i2s_callback_obj },

    // class constants
    /// \constant MASTER - for initialising the bus to master mode
    /// \constant SLAVE - for initialising the bus to slave mode
    { MP_OBJ_NEW_QSTR(MP_QSTR_MASTER), MP_OBJ_NEW_SMALL_INT(0) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SLAVE),  MP_OBJ_NEW_SMALL_INT(1) },

    // set format standard for I2S data
    { MP_OBJ_NEW_QSTR(MP_QSTR_PHILIPS),   MP_OBJ_NEW_SMALL_INT(I2S_STANDARD_PHILIPS) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MSB),       MP_OBJ_NEW_SMALL_INT(I2S_STANDARD_MSB) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_LSB),       MP_OBJ_NEW_SMALL_INT(I2S_STANDARD_LSB) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PCM_SHORT), MP_OBJ_NEW_SMALL_INT(I2S_STANDARD_PCM_SHORT) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PCM_LONG),  MP_OBJ_NEW_SMALL_INT(I2S_STANDARD_PCM_LONG) },

    // set I2S clock source
    { MP_OBJ_NEW_QSTR(MP_QSTR_PLL),      MP_OBJ_NEW_SMALL_INT(I2S_CLOCK_PLL) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_EXTERNAL), MP_OBJ_NEW_SMALL_INT(I2S_CLOCK_EXTERNAL) },
};

STATIC MP_DEFINE_CONST_DICT(pyb_i2s_locals_dict, pyb_i2s_locals_dict_table);

const mp_obj_type_t pyb_i2s_type = {
    { &mp_type_type },
    .name = MP_QSTR_I2S,
    .print = pyb_i2s_print,
    .make_new = pyb_i2s_make_new,
    .locals_dict = (mp_obj_t)&pyb_i2s_locals_dict,
};

#endif // MICROPY_HW_ENABLE_I2S2 || MICROPY_HW_ENABLE_I2S3
