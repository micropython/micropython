/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 SummerGift <zhangyuan@rt-thread.com>
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
#include "extmod/machine_spi.h"

#ifdef MICROPYTHON_USING_MACHINE_SPI

#ifndef RT_USING_SPI
#error "Please define the RT_USING_SPI on 'rtconfig.h'"
#endif

STATIC const mp_obj_type_t machine_hard_spi_type;

typedef struct _machine_hard_spi_obj_t {
    mp_obj_base_t base;
    struct rt_spi_device *spi_device;
} machine_hard_spi_obj_t;

STATIC void machine_hard_spi_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_hard_spi_obj_t *self = (machine_hard_spi_obj_t*)self_in;
    mp_printf(print,"SPI(device port : %s)",self->spi_device->parent.parent.name);
}

mp_obj_t machine_hard_spi_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    char spi_dev_name[RT_NAME_MAX];

    snprintf(spi_dev_name, sizeof(spi_dev_name), "spi%d", mp_obj_get_int(all_args[0]));

    struct rt_spi_device *rt_spi_device = (struct rt_spi_device *) rt_device_find(spi_dev_name);
    if (rt_spi_device == RT_NULL || rt_spi_device->parent.type != RT_Device_Class_SPIDevice) {
        mp_printf(&mp_plat_print, "ERROR: SPI device %s not found!\n", spi_dev_name);
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "SPI(%s) doesn't exist", spi_dev_name));
    }

    // create new hard SPI object
    machine_hard_spi_obj_t *self = m_new_obj(machine_hard_spi_obj_t);
    self->base.type = &machine_hard_spi_type;
    self->spi_device = rt_spi_device;
    return (mp_obj_t) self;
}

//SPI.init( baudrate=100000, polarity=0, phase=0, bits=8, firstbit=SPI.MSB/LSB )
STATIC void machine_hard_spi_init(mp_obj_base_t *self_in, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    machine_hard_spi_obj_t *self = (machine_hard_spi_obj_t*)self_in;
    rt_uint8_t mode = 0;
    int baudrate = mp_obj_get_int(pos_args[0]);
    int polarity = mp_obj_get_int(pos_args[1]);
    int phase = mp_obj_get_int(pos_args[2]);
    int bits = mp_obj_get_int(pos_args[3]);
    int firstbit = mp_obj_get_int(pos_args[4]);

    if(!polarity && !phase)
    {
        mode = RT_SPI_MODE_0;
    }

    if(!polarity && phase)
    {
        mode = RT_SPI_MODE_1;
    }

    if(polarity && !phase)
    {
        mode = RT_SPI_MODE_2;
    }

    if(polarity && phase)
    {
        mode = RT_SPI_MODE_3;
    }

    if(firstbit)
    {
        mode |= RT_SPI_MSB;
    } else {
        mode |= RT_SPI_LSB;
    }

    /* config spi */
    {
        struct rt_spi_configuration cfg;
        cfg.data_width = bits;
        cfg.mode = mode;
        cfg.max_hz = baudrate;
        rt_spi_configure(self->spi_device, &cfg);
    }
}

STATIC void machine_hard_spi_transfer(mp_obj_base_t *self_in, size_t len, const uint8_t *src, uint8_t *dest) {
    machine_hard_spi_obj_t *self = (machine_hard_spi_obj_t*)self_in;

    if (src && dest) {
        rt_spi_send_then_recv(self->spi_device, src, len, dest, len);
    } else if (src) {
        rt_spi_send(self->spi_device, src, len);
    } else {
        rt_spi_recv(self->spi_device, dest, len);
    }
}

STATIC const mp_machine_spi_p_t machine_hard_spi_p = {
    .init = machine_hard_spi_init,
    .deinit = NULL,
    .transfer = machine_hard_spi_transfer,
};

STATIC const mp_obj_type_t machine_hard_spi_type = {
    { &mp_type_type },
    .name = MP_QSTR_SPI,
    .print = machine_hard_spi_print,
    .make_new = machine_hard_spi_make_new,
    .protocol = &machine_hard_spi_p,
    .locals_dict = (mp_obj_t)&mp_machine_spi_locals_dict,
};

#endif // MICROPYTHON_USING_MACHINE_SPI

