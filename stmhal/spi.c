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

#include "stm32f4xx_hal.h"

#include "mpconfig.h"
#include "nlr.h"
#include "misc.h"
#include "qstr.h"
#include "obj.h"
#include "runtime.h"
#include "pin.h"
#include "genhdr/pins.h"
#include "bufhelper.h"
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
///     spi = SPI(1, SPI.MASTER, baudrate=600000, polarity=1, phase=1, crc=0x7)
///
/// Only required parameter is mode, SPI.MASTER or SPI.SLAVE.  Polarity can be
/// 0 or 1, and is the level the idle clock line sits at.  Phase can be 1 or 2
/// for number of edges.  Crc can be None for no CRC, or a polynomial specifier.
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
SPI_HandleTypeDef SPIHandle2 = {.Instance = NULL};
#if MICROPY_HW_ENABLE_SPI3
SPI_HandleTypeDef SPIHandle3 = {.Instance = NULL};
#endif

void spi_init0(void) {
    // reset the SPI handles
#if MICROPY_HW_ENABLE_SPI1
    memset(&SPIHandle1, 0, sizeof(SPI_HandleTypeDef));
    SPIHandle1.Instance = SPI1;
#endif
    memset(&SPIHandle2, 0, sizeof(SPI_HandleTypeDef));
    SPIHandle2.Instance = SPI2;
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
#endif
    } else if (spi->Instance == SPI2) {
        // Y-skin: Y5=PB12=SPI2_NSS, Y6=PB13=SPI2_SCK, Y7=PB14=SPI2_MISO, Y8=PB15=SPI2_MOSI
        pins[0] = &pin_B12;
        pins[1] = &pin_B13;
        pins[2] = &pin_B14;
        pins[3] = &pin_B15;
        GPIO_InitStructure.Alternate = GPIO_AF5_SPI2;
        // enable the SPI clock
        __SPI2_CLK_ENABLE();
#if MICROPY_HW_ENABLE_SPI3
    } else if (spi->Instance == SPI3) {
        pins[0] = &pin_A4;
        pins[1] = &pin_B3;
        pins[2] = &pin_B4;
        pins[3] = &pin_B5;
        GPIO_InitStructure.Alternate = GPIO_AF6_SPI3;
        // enable the SPI clock
        __SPI3_CLK_ENABLE();
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
        printf("HardwareError: HAL_SPI_Init failed\n");
        return;
    }
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
    } else if (spi->Instance == SPI2) {
        __SPI2_FORCE_RESET();
        __SPI2_RELEASE_RESET();
        __SPI2_CLK_DISABLE();
#if MICROPY_HW_ENABLE_SPI3
    } else if (spi->Instance == SPI3) {
        __SPI3_FORCE_RESET();
        __SPI3_RELEASE_RESET();
        __SPI3_CLK_DISABLE();
#endif
    }
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
    {{&pyb_spi_type}, &SPIHandle2},
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
            uint baudrate = spi_clock >> ((self->spi->Init.BaudRatePrescaler >> 3) + 1);
            print(env, "SPI(%u, SPI.MASTER, baudrate=%u", spi_num, baudrate);
        } else {
            print(env, "SPI(%u, SPI.SLAVE", spi_num);
        }
        print(env, ", polarity=%u, phase=%u, bits=%u", self->spi->Init.CLKPolarity == SPI_POLARITY_LOW ? 0 : 1, self->spi->Init.CLKPhase == SPI_PHASE_1EDGE ? 1 : 2, self->spi->Init.DataSize == SPI_DATASIZE_8BIT ? 8 : 16);
        if (self->spi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLED) {
            print(env, ", crc=0x%x", self->spi->Init.CRCPolynomial);
        }
        print(env, ")");
    }
}

/// \method init(mode, baudrate=328125, *, polarity=1, phase=1, bits=8, firstbit=SPI.MSB, ti=False, crc=None)
///
/// Initialise the SPI bus with the given parameters:
///
///   - `mode` must be either `SPI.MASTER` or `SPI.SLAVE`.
///   - `baudrate` is the SCK clock rate (only sensible for a master).
STATIC const mp_arg_t pyb_spi_init_args[] = {
    { MP_QSTR_mode,     MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = 328125} },
    { MP_QSTR_polarity, MP_ARG_KW_ONLY | MP_ARG_INT,  {.u_int = 1} },
    { MP_QSTR_phase,    MP_ARG_KW_ONLY | MP_ARG_INT,  {.u_int = 1} },
    { MP_QSTR_dir,      MP_ARG_KW_ONLY | MP_ARG_INT,  {.u_int = SPI_DIRECTION_2LINES} },
    { MP_QSTR_bits,     MP_ARG_KW_ONLY | MP_ARG_INT,  {.u_int = 8} },
    { MP_QSTR_nss,      MP_ARG_KW_ONLY | MP_ARG_INT,  {.u_int = SPI_NSS_SOFT} },
    { MP_QSTR_firstbit, MP_ARG_KW_ONLY | MP_ARG_INT,  {.u_int = SPI_FIRSTBIT_MSB} },
    { MP_QSTR_ti,       MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    { MP_QSTR_crc,      MP_ARG_KW_ONLY | MP_ARG_OBJ,  {.u_obj = mp_const_none} },
};
#define PYB_SPI_INIT_NUM_ARGS MP_ARRAY_SIZE(pyb_spi_init_args)

STATIC mp_obj_t pyb_spi_init_helper(const pyb_spi_obj_t *self, mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    // parse args
    mp_arg_val_t vals[PYB_SPI_INIT_NUM_ARGS];
    mp_arg_parse_all(n_args, args, kw_args, PYB_SPI_INIT_NUM_ARGS, pyb_spi_init_args, vals);

    // set the SPI configuration values
    SPI_InitTypeDef *init = &self->spi->Init;
    init->Mode = vals[0].u_int;

    // compute the baudrate prescaler from the requested baudrate
    // select a prescaler that yields at most the requested baudrate
    uint spi_clock;
    if (self->spi->Instance == SPI1) {
        // SPI1 is on APB2
        spi_clock = HAL_RCC_GetPCLK2Freq();
    } else {
        // SPI2 and SPI3 are on APB1
        spi_clock = HAL_RCC_GetPCLK1Freq();
    }
    uint br_prescale = spi_clock / vals[1].u_int;
    if (br_prescale <= 2) { init->BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2; }
    else if (br_prescale <= 4) { init->BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4; }
    else if (br_prescale <= 8) { init->BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8; }
    else if (br_prescale <= 16) { init->BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16; }
    else if (br_prescale <= 32) { init->BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32; }
    else if (br_prescale <= 64) { init->BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64; }
    else if (br_prescale <= 128) { init->BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128; }
    else { init->BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256; }

    init->CLKPolarity = vals[2].u_int == 0 ? SPI_POLARITY_LOW : SPI_POLARITY_HIGH;
    init->CLKPhase = vals[3].u_int == 1 ? SPI_PHASE_1EDGE : SPI_PHASE_2EDGE;
    init->Direction = vals[4].u_int;
    init->DataSize = (vals[5].u_int == 16) ? SPI_DATASIZE_16BIT : SPI_DATASIZE_8BIT;
    init->NSS = vals[6].u_int;
    init->FirstBit = vals[7].u_int;
    init->TIMode = vals[8].u_bool ? SPI_TIMODE_ENABLED : SPI_TIMODE_DISABLED;
    if (vals[9].u_obj == mp_const_none) {
        init->CRCCalculation = SPI_CRCCALCULATION_DISABLED;
        init->CRCPolynomial = 0;
    } else {
        init->CRCCalculation = SPI_CRCCALCULATION_ENABLED;
        init->CRCPolynomial = mp_obj_get_int(vals[9].u_obj);
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
STATIC const mp_arg_t pyb_spi_send_args[] = {
    { MP_QSTR_send,    MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 5000} },
};
#define PYB_SPI_SEND_NUM_ARGS MP_ARRAY_SIZE(pyb_spi_send_args)

STATIC mp_obj_t pyb_spi_send(mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    // TODO assumes transmission size is 8-bits wide

    pyb_spi_obj_t *self = args[0];

    // parse args
    mp_arg_val_t vals[PYB_SPI_SEND_NUM_ARGS];
    mp_arg_parse_all(n_args - 1, args + 1, kw_args, PYB_SPI_SEND_NUM_ARGS, pyb_spi_send_args, vals);

    // get the buffer to send from
    mp_buffer_info_t bufinfo;
    uint8_t data[1];
    pyb_buf_get_for_send(vals[0].u_obj, &bufinfo, data);

    // send the data
    HAL_StatusTypeDef status = HAL_SPI_Transmit(self->spi, bufinfo.buf, bufinfo.len, vals[1].u_int);

    if (status != HAL_OK) {
        // TODO really need a HardwareError object, or something
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_Exception, "HAL_SPI_Transmit failed with code %d", status));
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
STATIC const mp_arg_t pyb_spi_recv_args[] = {
    { MP_QSTR_recv,    MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 5000} },
};
#define PYB_SPI_RECV_NUM_ARGS MP_ARRAY_SIZE(pyb_spi_recv_args)

STATIC mp_obj_t pyb_spi_recv(mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    // TODO assumes transmission size is 8-bits wide

    pyb_spi_obj_t *self = args[0];

    // parse args
    mp_arg_val_t vals[PYB_SPI_RECV_NUM_ARGS];
    mp_arg_parse_all(n_args - 1, args + 1, kw_args, PYB_SPI_RECV_NUM_ARGS, pyb_spi_recv_args, vals);

    // get the buffer to receive into
    mp_buffer_info_t bufinfo;
    mp_obj_t o_ret = pyb_buf_get_for_recv(vals[0].u_obj, &bufinfo);

    // receive the data
    HAL_StatusTypeDef status = HAL_SPI_Receive(self->spi, bufinfo.buf, bufinfo.len, vals[1].u_int);

    if (status != HAL_OK) {
        // TODO really need a HardwareError object, or something
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_Exception, "HAL_SPI_Receive failed with code %d", status));
    }

    // return the received data
    if (o_ret == MP_OBJ_NULL) {
        return vals[0].u_obj;
    } else {
        return mp_obj_str_builder_end(o_ret);
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
STATIC const mp_arg_t pyb_spi_send_recv_args[] = {
    { MP_QSTR_send,    MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    { MP_QSTR_recv,    MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 5000} },
};
#define PYB_SPI_SEND_RECV_NUM_ARGS MP_ARRAY_SIZE(pyb_spi_send_recv_args)

STATIC mp_obj_t pyb_spi_send_recv(mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    // TODO assumes transmission size is 8-bits wide

    pyb_spi_obj_t *self = args[0];

    // parse args
    mp_arg_val_t vals[PYB_SPI_SEND_RECV_NUM_ARGS];
    mp_arg_parse_all(n_args - 1, args + 1, kw_args, PYB_SPI_SEND_RECV_NUM_ARGS, pyb_spi_send_recv_args, vals);

    // get buffers to send from/receive to
    mp_buffer_info_t bufinfo_send;
    uint8_t data_send[1];
    mp_buffer_info_t bufinfo_recv;
    mp_obj_t o_ret;

    if (vals[0].u_obj == vals[1].u_obj) {
        // same object for send and receive, it must be a r/w buffer
        mp_get_buffer_raise(vals[0].u_obj, &bufinfo_send, MP_BUFFER_RW);
        bufinfo_recv = bufinfo_send;
        o_ret = MP_OBJ_NULL;
    } else {
        // get the buffer to send from
        pyb_buf_get_for_send(vals[0].u_obj, &bufinfo_send, data_send);

        // get the buffer to receive into
        if (vals[1].u_obj == MP_OBJ_NULL) {
            // only send argument given, so create a fresh buffer of the send length
            bufinfo_recv.len = bufinfo_send.len;
            bufinfo_recv.typecode = 'B';
            o_ret = mp_obj_str_builder_start(&mp_type_bytes, bufinfo_recv.len, (byte**)&bufinfo_recv.buf);
        } else {
            // recv argument given
            mp_get_buffer_raise(vals[1].u_obj, &bufinfo_recv, MP_BUFFER_WRITE);
            if (bufinfo_recv.len != bufinfo_send.len) {
                nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "recv must be same length as send"));
            }
            o_ret = MP_OBJ_NULL;
        }
    }

    // send and receive the data
    HAL_StatusTypeDef status = HAL_SPI_TransmitReceive(self->spi, bufinfo_send.buf, bufinfo_recv.buf, bufinfo_send.len, vals[2].u_int);

    if (status != HAL_OK) {
        // TODO really need a HardwareError object, or something
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_Exception, "HAL_SPI_TransmitReceive failed with code %d", status));
    }

    // return the received data
    if (o_ret == MP_OBJ_NULL) {
        return vals[1].u_obj;
    } else {
        return mp_obj_str_builder_end(o_ret);
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
