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

#include "py/nlr.h"
#include "py/runtime.h"

#if MICROPY_HW_ENABLE_I2S2 || MICROPY_HW_ENABLE_I2S3

#include "py/objstr.h"
#include "py/objlist.h"
#include "py/stream.h"
#include "file.h" // for stream methods?
#include "irq.h"
#include "pin.h"
#include "led.h" // For led_toggle(n) debugging
#include "genhdr/pins.h"
#include "dma.h"
#include "bufhelper.h"
#include "i2s.h"
#include MICROPY_HAL_H

/// \moduleref pyb
/// \class I2S - Inter-IC-Sound, a protocol to transfer isochronous audio data
///
/// I2S is a serial protocol for sending and receiving audio. At the physical
/// level there are 3 or 4 lines: Bit Clock, Word Select, and 1 or 2 data lines
/// for simplex or duplex operation. In addition, I2S can optionally provide a
/// Master Clock (MCLK) signal to drive an external codec, or accept an I2S
/// clock input signal (I2S_CLKIN) from an external source to drive I2SPLL.
///
/// TODO: Provide detailed synopsis of I2S
/// - Usage example
/// - Methods list

// Possible DMA configurations for I2S busses:
// SPI2 RX:     DMA1_Stream3.CHANNEL_0
// SPI2 TX:     DMA1_Stream4.CHANNEL_0
// I2S2_EXT RX: DMA1_Stream3.CHANNEL_3
// I2S2_EXT TX: DMA1_Stream4.CHANNEL_2
// SPI3 RX:     DMA1_Stream0.CHANNEL_0 or DMA1_Stream2.CHANNEL_0
// SPI3 TX:     DMA1_Stream5.CHANNEL_0 or DMA1_Stream7.CHANNEL_0
// I2S3_EXT RX: DMA1_Stream0.CHANNEL_3 or DMA1_Stream2.CHANNEL_2
// I2S3_EXT TX: DMA1_Stream5.CHANNEL_2

#define AUDIOBUFFER_BYTES 8192

typedef struct _pyb_i2s_obj_t {
    mp_obj_base_t base;
    I2S_HandleTypeDef i2s;
    DMA_Stream_TypeDef *tx_dma_stream;
    uint32_t tx_dma_channel;
    DMA_Stream_TypeDef *rx_dma_stream;
    uint32_t rx_dma_channel;
    mp_obj_base_t *dstream_tx;
    mp_obj_base_t *dstream_rx;
    const pin_obj_t *pins[5];
    // audio buffers placed before smaller struct members to keep word-aligned
    uint16_t audiobuf_tx[2][AUDIOBUFFER_BYTES / 8];
    uint16_t audiobuf_rx[2][AUDIOBUFFER_BYTES / 8];
    mp_int_t i2s_id : 8;
    bool is_enabled : 1;
    bool is_master : 1;
    bool is_duplex : 1;
    bool base_is_tx : 1; // base instance SPIx is either tx or rx
    bool pp_ptr : 1; // ping-pong pointer for double buffers
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
    //.FIFOMode            = DMA_FIFOMODE_DISABLE,
    .FIFOMode            = DMA_FIFOMODE_ENABLE,
    //.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL,
    .FIFOThreshold       = DMA_FIFO_THRESHOLD_HALFFULL,
    .MemBurst            = DMA_MBURST_SINGLE,
    .PeriphBurst         = DMA_PBURST_SINGLE
};

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

    // New GPIO initialization section, supercedes the separate I2S2/3 sections
    // Seems to work! Check with logic analyzer and some more pin combinations
    // TODO - needs better description

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

    // Configure and enable I2SPLL:
    // TODO: This may not be the correct method to initialize and activate
    // the I2SPLL, needs more testing
    __HAL_RCC_PLLI2S_DISABLE();
    if (i2s_obj->i2s.Init.Mode == I2S_MODE_MASTER_TX ||
        i2s_obj->i2s.Init.Mode == I2S_MODE_MASTER_RX ) {
        // TODO - calculate values based on available parameters
        __HAL_RCC_PLLI2S_CONFIG(384, 5);
        __HAL_RCC_PLLI2S_ENABLE();
    }

    if (HAL_I2S_Init(&i2s_obj->i2s) != HAL_OK) {
        // This message is redundant, exception will be raised by return value
        printf("OSError: HAL_I2S_Init failed\n");
        return false;
    } else {
        dma_invalidate_channel(i2s_obj->tx_dma_stream, i2s_obj->tx_dma_channel);
        dma_invalidate_channel(i2s_obj->rx_dma_stream, i2s_obj->rx_dma_channel);
        i2s_obj->is_enabled = true;
        return true;
    }
}

// this is needed because i2s_deinit invokes pyb_i2s_deinit - should it be
// other way around?
STATIC mp_obj_t pyb_i2s_deinit(mp_obj_t self_in);

// i2s_deinit is direct crib from stmhal/can.c - can_deinit()
// unregister all interrupt sources
void i2s_deinit(void) {
    for (int i = 0; i < MP_ARRAY_SIZE(MP_STATE_PORT(pyb_i2s_obj_all)); i++) {
        pyb_i2s_obj_t *i2s_obj = MP_STATE_PORT(pyb_i2s_obj_all)[i];
        if (i2s_obj != NULL) {
            pyb_i2s_deinit(i2s_obj);
        }
    }
}

// TODO: This is currently enabled, but eventually I2S should be entirely
// non-blocking, using DMA with callbacks
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


STATIC void stream_out_buffering(pyb_i2s_obj_t *self) {
    int buf_sz = AUDIOBUFFER_BYTES / 4;
    int error;
    HAL_StatusTypeDef status;
    status = HAL_I2S_Transmit_DMA(&self->i2s, self->audiobuf_tx[self->pp_ptr], buf_sz / 2);

    if (status != HAL_OK) {
        mp_hal_raise(status);
    }

    self->pp_ptr = !(self->pp_ptr);

    mp_uint_t out_sz = self->dstream_tx->type->stream_p->read(self->dstream_tx, &self->audiobuf_tx[self->pp_ptr], buf_sz, &error);

    if (out_sz == MP_STREAM_ERROR) {
        nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(error)));
    }
}

// Debug placeholders for DMA callback methods
void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s) {
    //printf("I2S-TxHalfCplt\n");
    //led_toggle(2);
}

void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s) {
    led_state(1, 1);
    int rp_index = 0;
    // If both I2S instances are enabled, set root pointer index to 1 if I2S
    // instance is SPI3/I2S3; otherwise index is 0
#if MICROPY_HW_ENABLE_I2S2 && MICROPY_HW_ENABLE_I2S3
    if (hi2s->Instance == SPI3) {
        rp_index = 1;
    }
#endif

    pyb_i2s_obj_t *self = MP_STATE_PORT(pyb_i2s_obj_all)[rp_index];
    stream_out_buffering(self);
    
    //printf("I2S-TxCplt\n");
    led_state(1, 0);
}

void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s) {
    //printf("I2S-RxHalfCplt\n");
}

void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s) {
    //printf("I2S-RxCplt\n");
}

void HAL_I2S_ErrorCallback(I2S_HandleTypeDef *hi2s) {
    //printf("I2S-Error\n");
    //led_toggle(4);
}

/******************************************************************************/
/* Micro Python bindings                                                      */

// spi.c provides *spi_get_handle, probably don't need to provide *i2s_get_handle

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
    mp_print_str(print, "\b], I2S.");
    if (self->is_enabled) {
        if (self->i2s.Init.Mode == I2S_MODE_MASTER_TX ||
            self->i2s.Init.Mode == I2S_MODE_MASTER_RX) {
            mp_printf(print, "%q, %q ", MP_QSTR_MASTER, MP_QSTR_mclkout);
            if (self->i2s.Init.MCLKOutput == I2S_MCLKOUTPUT_ENABLE) {
                mp_printf(print, "on %q", self->pins[4]->name);
            } else {
                mp_print_str(print, "off");
            }
            mp_printf(print, ", %q=%u", MP_QSTR_audiofreq, self->i2s.Init.AudioFreq);
        } else if (self->i2s.Init.Mode == I2S_MODE_SLAVE_TX ||
                   self->i2s.Init.Mode == I2S_MODE_SLAVE_RX) {
            mp_printf(print, "%q", MP_QSTR_SLAVE);
        } else {
            // Shouldn't get here if self->is_enabled=true
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

/// \method init(mode, standard=I2S.PHILIPS, dataformat=0,
///              polarity=0, audiofreq=48000,
///              clksrc=I2S.PLL, mclkout=0)
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
        // Include option for setting I2SPLL parameters directly?
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


//////////////////////////////////////

STATIC mp_obj_t pyb_i2s_init(mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return pyb_i2s_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_i2s_init_obj, 1, pyb_i2s_init);

// TODO: init and deinit follow model from stmhal/can.c; should is follow logic
// closer to spi_init and spi_deinit instead?

/// \method deinit()
/// Turn off the I2S bus.
STATIC mp_obj_t pyb_i2s_deinit(mp_obj_t self_in) {
    pyb_i2s_obj_t *self = self_in;
    self->is_enabled = false;
    HAL_I2S_DeInit(&self->i2s);
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

/// \method send(send, *, timeout=5000)
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
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 5000} },
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
    // interfaces when available instead of raising error?
    if (!self->base_is_tx) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                                                "I2S(%d) base instance not in transmit mode",
                                                self->i2s_id));
    }

    // get the buffer to send from
    mp_buffer_info_t bufinfo;
    uint8_t data[1];
    pyb_buf_get_for_send(args[0].u_obj, &bufinfo, data);

    // TODO - implement 24-bit and 32-bit data width cases for all methods
    // send the data
    HAL_StatusTypeDef status;
    if (query_irq() == IRQ_STATE_DISABLED) {
        status = HAL_I2S_Transmit(&self->i2s, bufinfo.buf, bufinfo.len / 2, args[1].u_int);
    } else {
        DMA_HandleTypeDef tx_dma;
        dma_init(&tx_dma, self->tx_dma_stream, &dma_init_struct_i2s,
                 self->tx_dma_channel, DMA_MEMORY_TO_PERIPH, &self->i2s);
        self->i2s.hdmatx = &tx_dma;
        self->i2s.hdmarx = NULL;
        status = HAL_I2S_Transmit_DMA(&self->i2s, bufinfo.buf, bufinfo.len / 2);
        if (status == HAL_OK) {
            //led_toggle(1);
            status = i2s_wait_dma_finished(&self->i2s, args[1].u_int);
        }
        dma_deinit(&tx_dma);
    }

    if (status != HAL_OK) {
        mp_hal_raise(status);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_i2s_send_obj, 1, pyb_i2s_send);

// TODO - adapt docstrings copied from spi.c for i2s

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
STATIC mp_obj_t pyb_i2s_recv(mp_uint_t n_args, const mp_obj_t *pos_args,
                             mp_map_t *kw_args) {
    // TODO check transmission size for I2S

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_recv,    MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 5000} },
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

    // get the buffer to receive into
    vstr_t vstr;
    mp_obj_t o_ret = pyb_buf_get_for_recv(args[0].u_obj, &vstr);

    // Only 16-bit data transfers are implemented
    // TODO - implement 24-bit and 32-bit data transfers for all methods
    // receive the data
    HAL_StatusTypeDef status;
    if (query_irq() == IRQ_STATE_DISABLED) {
        status = HAL_I2S_Receive(&self->i2s, (uint16_t*)vstr.buf,
                                 vstr.len / 2, args[1].u_int);
    } else {
        DMA_HandleTypeDef rx_dma;
        dma_init(&rx_dma, self->rx_dma_stream, &dma_init_struct_i2s,
                 self->rx_dma_channel, DMA_PERIPH_TO_MEMORY, &self->i2s);
        self->i2s.hdmarx = &rx_dma;
        self->i2s.hdmatx = NULL;
        status = HAL_I2S_Receive_DMA(&self->i2s, (uint16_t*)vstr.buf, vstr.len / 2);
        if (status == HAL_OK) {
            status = i2s_wait_dma_finished(&self->i2s, args[1].u_int);
        }
        dma_deinit(&rx_dma);
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
STATIC mp_obj_t pyb_i2s_send_recv(mp_uint_t n_args, const mp_obj_t *pos_args,
                                  mp_map_t *kw_args) {

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_send,    MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_recv,    MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 5000} },
    };

    // parse args
    pyb_i2s_obj_t *self = pos_args[0];
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args,
                     MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // TODO - invoke HAL_I2SEx_TransmitReceive_DMA to handle recv on I2Sx_EXT
    // interfaces when available instead of raising error?
    //if (!self->is_duplex) {
    if (self->i2s.Init.FullDuplexMode != I2S_FULLDUPLEXMODE_ENABLE) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                                                "I2S(%d) not in duplex mode", self->i2s_id));
    }

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

    // TODO - implement 24-bit and 32-bit data width cases for all methods
    // send and receive the data
    HAL_StatusTypeDef status;
    if (query_irq() == IRQ_STATE_DISABLED) {
        status = HAL_I2SEx_TransmitReceive(&self->i2s, bufinfo_send.buf, bufinfo_recv.buf,
                                           bufinfo_send.len / 2, args[2].u_int);
    } else {
        DMA_HandleTypeDef tx_dma, rx_dma;
        dma_init(&tx_dma, self->tx_dma_stream, &dma_init_struct_i2s,
                 self->tx_dma_channel, DMA_MEMORY_TO_PERIPH, &self->i2s);
        self->i2s.hdmatx = &tx_dma;
        dma_init(&rx_dma, self->rx_dma_stream, &dma_init_struct_i2s,
                 self->rx_dma_channel, DMA_PERIPH_TO_MEMORY, &self->i2s);
        self->i2s.hdmarx = &rx_dma;
        status = HAL_I2SEx_TransmitReceive_DMA(&self->i2s, bufinfo_send.buf,
                                               bufinfo_recv.buf, bufinfo_send.len / 2);
        if (status == HAL_OK) {
            status = i2s_wait_dma_finished(&self->i2s, args[2].u_int);
        }
        dma_deinit(&tx_dma);
        dma_deinit(&rx_dma);
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
/////////////

///// Streaming methods:
// Taken from stream.c:
#if MICROPY_STREAMS_NON_BLOCK
// TODO: This is POSIX-specific (but then POSIX is the only real thing,
// and anything else just emulates it, right?)
#include <errno.h>
#define is_nonblocking_error(errno) ((errno) == EAGAIN || (errno) == EWOULDBLOCK)
#else
#define is_nonblocking_error(errno) (0)
#endif

#define STREAM_CONTENT_TYPE(stream) (((stream)->is_text) ? &mp_type_str : &mp_type_bytes)


STATIC mp_obj_t pyb_i2s_stream_out(mp_uint_t n_args, const mp_obj_t *pos_args,
                                   mp_map_t *kw_args) {

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_stream_out, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 5000} },
    };

    // parse args
    pyb_i2s_obj_t *self = pos_args[0];
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args,
                     MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Re-work this to use the mp_obj_get_type idiom (more clarity):
    struct _mp_obj_base_t *stream = (struct _mp_obj_base_t *)args[0].u_obj;
    //mp_obj_t stream = args[0].u_obj;
    //mp_obj_type_t *type = mp_obj_get_type(stream);
    // Check that 'stream' provides an mp_stream_p_t and a read
    // Note that 'read' will be present even if the stream opened in write-mode
    //if (type->stream_p == NULL || type->stream_p->read == NULL) {
    if (stream->type->stream_p == NULL || stream->type->stream_p->read == NULL) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                                                "Object type %s not a readable stream",
                                                mp_obj_get_type_str(stream)));
    }

    self->dstream_tx = stream;



    int buf_sz = AUDIOBUFFER_BYTES / 4;
    int error;
    self->pp_ptr = 0;

    //mp_uint_t out_sz = stream->type->stream_p->read(stream, tx_buf, sz, &error);
    mp_uint_t out_sz = self->dstream_tx->type->stream_p->read(self->dstream_tx, &self->audiobuf_tx[self->pp_ptr], buf_sz, &error);

    if (out_sz == MP_STREAM_ERROR) {
        if (is_nonblocking_error(error)) {
            // https://docs.python.org/3.4/library/io.html#io.RawIOBase.read
            // "If the object is in non-blocking mode and no bytes are available,
            // None is returned."
            // This is actually very weird, as naive truth check will treat
            // this as EOF.
            return mp_const_none;
        }
        nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(error)));
    }

    DMA_HandleTypeDef tx_dma;
    dma_init(&tx_dma, self->tx_dma_stream, &dma_init_struct_i2s,
             self->tx_dma_channel, DMA_MEMORY_TO_PERIPH, &self->i2s);
    self->i2s.hdmatx = &tx_dma;
    self->i2s.hdmarx = NULL;

    // A hack to make sure that slave-mode transfers don't start on the wrong clock:
    if (!(self->is_master)) {
        while(GPIO_read_pin(self->pins[WS]->gpio, self->pins[WS]->pin) == 1) {;}
        while(GPIO_read_pin(self->pins[WS]->gpio, self->pins[WS]->pin) == 0) {;}
    }

    stream_out_buffering(self);
    /////////////////////
    /* HAL_StatusTypeDef status; */
    /* status = HAL_I2S_Transmit_DMA(&self->i2s, self->audiobuf_tx[self->pp_ptr], buf_sz / 2); */
    /* if (status != HAL_OK) { */
    /*  mp_hal_raise(status); */
    /* } */

    /* self->pp_ptr = !(self->pp_ptr); */
    /* out_sz = self->dstream_tx->type->stream_p->read(self->dstream_tx, &self->audiobuf_tx[self->pp_ptr], buf_sz, &error); */
    /* if (out_sz == MP_STREAM_ERROR) { */
    /*  nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(error))); */
    /* } */
    //////////////////
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_i2s_stream_out_obj, 1, pyb_i2s_stream_out);


STATIC mp_obj_t pyb_i2s_stream_in (mp_uint_t n_args, const mp_obj_t *pos_args,
                                   mp_map_t *kw_args) {

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_stream_in, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 5000} },
    };

    // parse args
    // pyb_i2s_obj_t *self = pos_args[0];
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args,
                     MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t stream = args[0].u_obj;
    // Got the file; the following check doesn't do anything.
    // need to check if the handle has a 'write' method
    if (stream == NULL /* || stream->type->stream_p->write == NULL */) {
        // CPython: io.UnsupportedOperation, OSError subclass
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "Operation not supported"));
    }

    return stream;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_i2s_stream_in_obj, 1, pyb_i2s_stream_in);


STATIC const mp_map_elem_t pyb_i2s_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_init), (mp_obj_t)&pyb_i2s_init_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_deinit), (mp_obj_t)&pyb_i2s_deinit_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_send), (mp_obj_t)&pyb_i2s_send_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_recv), (mp_obj_t)&pyb_i2s_recv_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_send_recv), (mp_obj_t)&pyb_i2s_send_recv_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_stream_out), (mp_obj_t)&pyb_i2s_stream_out_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_stream_in), (mp_obj_t)&pyb_i2s_stream_in_obj },

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
