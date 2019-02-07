/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2018 Damien P. George
 * Copyright (c) 2019 Analog Devices, Inc.
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

#include "py/runtime.h"
#include "py/mphal.h"
#include "modmachine.h"
#include "spi.h"
#include "bm_spi.h"

#if defined(MICROPY_HW_SPI0_SCK)
BM_SPI spi_handle_0;
#endif
#if defined(MICROPY_HW_SPI1_SCK)
BM_SPI spi_handle_1;
#endif
#if defined(MICROPY_HW_SPI2_SCK)
BM_SPI spi_handle_2;
#endif

const spi_t spi_obj[3] = {
    #if defined(MICROPY_HW_SPI0_SCK)
    {&spi_handle_0},
    #else
    {NULL},
    #endif
    #if defined(MICROPY_HW_SPI1_SCK)
    {&spi_handle_1},
    #else
    {NULL},
    #endif
    #if defined(MICROPY_HW_SPI2_SCK)
    {&spi_handle_2},
    #else
    {NULL},
    #endif
};

void spi_init0(void) {
    // Initialise the SPI handles.
#if defined(MICROPY_HW_SPI0_SCK)
    spi_handle_0.device_num = BM_SPI0;
#endif
#if defined(MICROPY_HW_SPI1_SCK)
    spi_handle_1.device_num = BM_SPI1;
#endif
#if defined(MICROPY_HW_SPI2_SCK)
    spi_handle_2.device_num = BM_SPI2;
#endif
}

int spi_find_index(mp_obj_t id) {
    if (MP_OBJ_IS_STR(id)) {
        // given a string id
        const char *port = mp_obj_str_get_str(id);
        if (0) {
        #ifdef MICROPY_HW_SPI0_NAME
        } else if (strcmp(port, MICROPY_HW_SPI0_NAME) == 0) {
            return 0;
        #endif
        #ifdef MICROPY_HW_SPI1_NAME
        } else if (strcmp(port, MICROPY_HW_SPI1_NAME) == 0) {
            return 1;
        #endif
        #ifdef MICROPY_HW_SPI2_NAME
        } else if (strcmp(port, MICROPY_HW_SPI2_NAME) == 0) {
            return 2;
        #endif
        }
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
            "SPI(%s) doesn't exist", port));
    } else {
        // given an integer id
        int spi_id = mp_obj_get_int(id);
        if (spi_id >= 1 && spi_id <= MP_ARRAY_SIZE(spi_obj)
            && spi_obj[spi_id].spi != NULL) {
            return spi_id;
        }
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
            "SPI(%d) doesn't exist", spi_id));
    }
}

// sets the parameters in the SPI_InitTypeDef struct
// if an argument is -1 then the corresponding parameter is not changed
void spi_set_params(const spi_t *spi_obj, int32_t baudrate,
    int32_t polarity, int32_t phase, int32_t bits) {
    BM_SPI *spi = spi_obj->spi;

    if (baudrate != -1) {
        spi -> clockFreq = baudrate;
    }

    if (polarity != -1) {
        if (polarity) {
            if (spi -> spi_mode == SPI_MODE_0) spi->spi_mode = SPI_MODE_2;
            else if (spi -> spi_mode == SPI_MODE_1) spi->spi_mode = SPI_MODE_3;
        }
        else {
            if (spi -> spi_mode == SPI_MODE_2) spi->spi_mode = SPI_MODE_0;
            else if (spi -> spi_mode == SPI_MODE_3) spi->spi_mode = SPI_MODE_1;
        }
    }

    if (phase != -1) {
        if (phase) {
            if (spi -> spi_mode == SPI_MODE_0) spi->spi_mode = SPI_MODE_1;
            else if (spi -> spi_mode == SPI_MODE_2) spi->spi_mode = SPI_MODE_3;
        }
        else {
            if (spi -> spi_mode == SPI_MODE_1) spi->spi_mode = SPI_MODE_0;
            else if (spi -> spi_mode == SPI_MODE_3) spi->spi_mode = SPI_MODE_2;
        }
    }

    if (bits != -1) {
        spi -> transmit_len = (bits == 8) ? (SPI_WORDLEN_8BIT) : (
                (bits == 16) ? (SPI_WORDLEN_16BIT) : (SPI_WORDLEN_32BIT));
    }
}

// TODO allow to take a list of pins to use
void spi_init(const spi_t *self, bool enable_ss_pin) {
    BM_SPI *spi = self->spi;
    const pin_obj_t *pins[4] = { NULL, NULL, NULL, NULL };

    if (0) {
    #if defined(MICROPY_HW_SPI0_SCK)
    } else if (spi->device_num == BM_SPI0) {
        #if defined(MICROPY_HW_SPI0_SS)
        pins[0] = MICROPY_HW_SPI0_SS;
        #endif
        pins[1] = MICROPY_HW_SPI0_SCK;
        #if defined(MICROPY_HW_SPI0_MISO)
        pins[2] = MICROPY_HW_SPI0_MISO;
        #endif
        pins[3] = MICROPY_HW_SPI0_MOSI;
    #endif
    #if defined(MICROPY_HW_SPI1_SCK)
    } else if (spi->device_num == BM_SPI1) {
        #if defined(MICROPY_HW_SPI1_SS)
        pins[0] = MICROPY_HW_SPI1_SS;
        #endif
        pins[1] = MICROPY_HW_SPI1_SCK;
        #if defined(MICROPY_HW_SPI1_MISO)
        pins[2] = MICROPY_HW_SPI1_MISO;
        #endif
        pins[3] = MICROPY_HW_SPI1_MOSI;
    #endif
    #if defined(MICROPY_HW_SPI2_SCK)
    } else if (spi->device_num == BM_SPI2) {
        #if defined(MICROPY_HW_SPI2_SS)
        pins[0] = MICROPY_HW_SPI2_SS;
        #endif
        pins[1] = MICROPY_HW_SPI2_SCK;
        #if defined(MICROPY_HW_SPI2_MISO)
        pins[2] = MICROPY_HW_SPI2_MISO;
        #endif
        pins[3] = MICROPY_HW_SPI2_MOSI;
    #endif
    } else {
        // SPI does not exist for this board (shouldn't get here, should be checked by caller)
        return;
    }

    // init the GPIO lines
    uint32_t mode = GPIO_MODE_AF_PP;
    for (uint i = (enable_ss_pin ? 0 : 1); i < 4; i++) {
        if (pins[i] == NULL) {
            continue;
        }
        mp_hal_pin_config_alt(pins[i], mode, 0, AF_FN_SPI, (self - &spi_obj[0]));
    }

    // init the SPI device
    spi_initialize(spi, spi -> spi_mode, SPI_SSEL_MANUAL, spi -> transmit_len, machine_get_fsclk0(), spi -> device_num);
    // set the SPI clock
    spi_setClock(spi, spi -> clockFreq);
}

void spi_deinit(const spi_t *spi_obj) {
    BM_SPI *spi = spi_obj->spi;
    spi_end(spi);
}

void spi_transfer(const spi_t *self, size_t len, const uint8_t *src, uint8_t *dest, uint32_t timeout) {
    if (self -> spi -> transmit_len == SPI_WORDLEN_8BIT) {
        for (int i = 0; i < len; i++) {
            if (dest != NULL)
                *dest++ = spi_transfer_single(self->spi, *src++);
            else
                spi_transfer_single(self->spi, *src++);
        }
    }
    else if (self -> spi -> transmit_len == SPI_WORDLEN_16BIT) {
        for (int i = 0; i < len / 2; i++) {
            if (dest != NULL)
                *(uint16_t *)dest++ = spi_transfer_single(self->spi, *(uint16_t *)src++);
            else
                spi_transfer_single(self->spi, *(uint16_t *)src++);
        }
    }
    else if (self -> spi -> transmit_len == SPI_WORDLEN_32BIT) {
        for (int i = 0; i < len / 4; i++) {
            if (dest != NULL)
                *(uint32_t *)dest++ = spi_transfer_single(self->spi, *(uint32_t *)src++);
            else
                spi_transfer_single(self->spi, *(uint32_t *)src++);
        }
    }
}

void spi_print(const mp_print_t *print, const spi_t *spi_obj, bool legacy) {
    BM_SPI *spi = spi_obj->spi;

    uint spi_num = 0; // default to SPI0
    if (0) { }
    #if defined(MICROPY_HW_SPI1_SCK)
    else if (spi->device_num == BM_SPI1) { spi_num = 1; }
    #endif
    #if defined(MICROPY_HW_SPI2_SCK)
    else if (spi->device_num == BM_SPI2) { spi_num = 2; }
    #endif

    mp_printf(print, "SPI(%u", spi_num);
    // compute baudrate
    uint baudrate = spi->clockFreq;
    if (legacy) {
        mp_printf(print, ", SPI.MASTER");
    }
    mp_printf(print, ", baudrate=%u", baudrate);
    mp_printf(print, ", polarity=%u, phase=%u, bits=%u",
            ((spi->spi_mode == SPI_MODE_0) || (spi->spi_mode == SPI_MODE_1)) ? 0 : 1,
            ((spi->spi_mode == SPI_MODE_0) || (spi->spi_mode == SPI_MODE_2)) ? 0 : 1,
            (spi->transmit_len == SPI_WORDLEN_8BIT) ? 8 : ((spi->transmit_len == SPI_WORDLEN_16BIT) ? 16 : (32)));
    mp_print_str(print, ")");
}

const spi_t *spi_from_mp_obj(mp_obj_t o) {
    if (MP_OBJ_IS_TYPE(o, &pyb_spi_type)) {
        machine_hard_spi_obj_t *self = MP_OBJ_TO_PTR(o);
        return self->spi;
    } else if (MP_OBJ_IS_TYPE(o, &machine_hard_spi_type)) {
        machine_hard_spi_obj_t *self = MP_OBJ_TO_PTR(o);
        return self->spi;
    } else {
        mp_raise_TypeError("expecting an SPI object");
    }
}

/******************************************************************************/
// Implementation of low-level SPI C protocol

STATIC int spi_proto_ioctl(void *self_in, uint32_t cmd) {
    spi_proto_cfg_t *self = (spi_proto_cfg_t*)self_in;

    switch (cmd) {
        case MP_SPI_IOCTL_INIT:
            spi_set_params(self->spi, self->baudrate,
                self->polarity, self->phase, self->bits);
            spi_init(self->spi, false);
            break;

        case MP_SPI_IOCTL_DEINIT:
            spi_deinit(self->spi);
            break;
    }

    return 0;
}

STATIC void spi_proto_transfer(void *self_in, size_t len, const uint8_t *src, uint8_t *dest) {
    spi_proto_cfg_t *self = (spi_proto_cfg_t*)self_in;
    spi_transfer(self->spi, len, src, dest, SPI_TRANSFER_TIMEOUT(len));
}

const mp_spi_proto_t spi_proto = {
    .ioctl = spi_proto_ioctl,
    .transfer = spi_proto_transfer,
};
