/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Damien P. George
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
#include "py/mperrno.h"
#include "extmod/vfs.h"
#include "extmod/machine_qspi.h"
#include "extmod/machine_spi.h"
#include "extmod/machine_spiflash.h"

#if MICROPY_PY_MACHINE_SPIFLASH

/******************************************************************************/
// MicroPython bindings for machine.SPIFlash providing block protocol

mp_obj_t mp_machine_spiflash_make_bdev(mp_spiflash_t *spiflash, uint32_t block_start, uint32_t block_count, uint32_t block_size) {
    // setup and initialize a machine_spiflash object wrapping an existing spiflash driver
    mp_machine_spiflash_obj_t *self = m_new_obj(mp_machine_spiflash_obj_t);
    self->base.type = &mp_machine_spiflash_type;
    self->spiflash = spiflash;
   
    self->block_size = block_size;
    self->block_count = block_count;
    self->block_start = block_start;
    self->flash_tick_counter_last_write = 0;
    return self;
}

STATIC mp_obj_t machine_spiflash_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_spi, ARG_length, ARG_start, ARG_block_size, ARG_cached };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_spi,         MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_length,      MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_start,       MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_block_size,  MP_ARG_INT, {.u_int = MP_SPIFLASH_DEFAULT_BLOCK_SIZE} },
        { MP_QSTR_cached,      MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // setup and initialize the object
    mp_machine_spiflash_obj_t *self = m_new_obj(mp_machine_spiflash_obj_t);
    mp_spiflash_t *spiflash = m_new_obj(mp_spiflash_t);
    self->base.type = &mp_machine_spiflash_type;
    self->spiflash = spiflash;

    mp_obj_type_t *spi = MP_OBJ_TO_PTR(args[ARG_spi].u_obj);
    if (spi == MP_OBJ_NULL) {
        mp_raise_ValueError("Must provide spi driver");
    }

    self->spiflash->data = NULL;
    
    if (mp_obj_is_subclass_fast(MP_OBJ_FROM_PTR(spi->base.type), MP_OBJ_FROM_PTR(&mp_machine_spi_type))) {
        if (((mp_machine_spi_obj_t *)spi)->proto == NULL) {
            mp_raise_ValueError("Incompatible spi driver - proto not configured");
        }
        self->spiflash->spi_proto = ((mp_machine_spi_obj_t *)spi)->proto;
    #if MICROPY_PY_MACHINE_QSPI
    } else if (mp_obj_is_subclass_fast(MP_OBJ_FROM_PTR(spi->base.type), MP_OBJ_FROM_PTR(&mp_machine_qspi_type))) {
        if (((mp_machine_qspi_obj_t *)spi)->proto == NULL) {
            mp_raise_ValueError("Incompatible qspi driver - proto not configured");
        }
        self->spiflash->qspi_proto = ((mp_machine_qspi_obj_t *)spi)->proto;
    #endif
    } else {
        mp_raise_ValueError("Incompatible spi driver");
    }
    
    self->block_size = args[ARG_block_size].u_int;
    
    uint32_t length = args[ARG_length].u_int;
    if (length % self->block_size) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "length must be multple of block size (%d)", self->block_size));
    }
    self->block_count = length / self->block_size;
    
    uint32_t start = args[ARG_start].u_int;
    if (start % self->block_size) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "start must be multple of block size (%d)", self->block_size));
    }
    self->block_start = start / self->block_size;

    if (args[ARG_cached].u_bool) {
        mp_spiflash_cache_t *spi_bdev_cache = m_new_obj(mp_spiflash_cache_t);
        self->spiflash->cache = spi_bdev_cache;
    } else {
        self->spiflash->cache = NULL;
    }
    
    self->flash_tick_counter_last_write = 0;
    return self;
}

STATIC mp_obj_t machine_spiflash_ioctl(mp_obj_t self_in, mp_obj_t op_in, mp_obj_t arg_in) {
    mp_machine_spiflash_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t op = mp_obj_get_int(op_in);

    switch (op) {
        case BP_IOCTL_INIT:
            mp_spiflash_init(self->spiflash);
            self->flash_tick_counter_last_write = 0;
            return MP_OBJ_NEW_SMALL_INT(0);

        case BP_IOCTL_IRQ_HANDLER:
            if ((self->spiflash->flags & 1) && HAL_GetTick() - self->flash_tick_counter_last_write >= 1000) {
                mp_spiflash_cache_flush(self->spiflash);
                // led_state(PYB_LED_RED, 0); // indicate a clean cache with LED off
            }
            return MP_OBJ_NEW_SMALL_INT(0);

        case BP_IOCTL_SYNC:
            if (self->spiflash->flags & 1) {
                uint32_t basepri = raise_irq_pri(IRQ_PRI_FLASH); // prevent cache flushing and USB access
                mp_spiflash_cache_flush(self->spiflash);
                // led_state(PYB_LED_RED, 0); // indicate a clean cache with LED off
                restore_irq_pri(basepri);
            }
            return MP_OBJ_NEW_SMALL_INT(0);
        
        case BP_IOCTL_SEC_COUNT:
            return MP_OBJ_NEW_SMALL_INT(self->block_count);
        
        case BP_IOCTL_SEC_SIZE:
            return MP_OBJ_NEW_SMALL_INT(self->block_size);
    }
    return MP_OBJ_NEW_SMALL_INT(-MP_EINVAL);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(machine_spiflash_ioctl_obj, machine_spiflash_ioctl);


STATIC mp_obj_t machine_spiflash_readblocks(mp_obj_t self_in, mp_obj_t block_num_in, mp_obj_t buf) {
    mp_machine_spiflash_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_buffer_info_t bufinfo;
    uint32_t block_num = mp_obj_get_int(block_num_in) + self->block_start;

    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_WRITE);

    uint32_t basepri = raise_irq_pri(IRQ_PRI_FLASH); // prevent cache flushing and USB access
    mp_spiflash_cached_read(self->spiflash, block_num * self->block_size, bufinfo.len, bufinfo.buf);
    restore_irq_pri(basepri);

    return mp_obj_new_bool(1);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(machine_spiflash_readblocks_obj, machine_spiflash_readblocks);

STATIC mp_obj_t machine_spiflash_writeblocks(mp_obj_t self_in, mp_obj_t block_num_in, mp_obj_t buf) {
    mp_machine_spiflash_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_buffer_info_t bufinfo;
    uint32_t block_num = mp_obj_get_int(block_num_in) + self->block_start;

    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_READ);
    
    uint32_t basepri = raise_irq_pri(IRQ_PRI_FLASH); // prevent cache flushing and USB access
    int ret = mp_spiflash_cached_write(self->spiflash, block_num * self->block_size, bufinfo.len, bufinfo.buf);
    if (self->spiflash->flags & 1) {
        // led_state(PYB_LED_RED, 1); // indicate a dirty cache with LED on
        self->flash_tick_counter_last_write = HAL_GetTick();
    }
    restore_irq_pri(basepri);

    return mp_obj_new_bool(ret == 0);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(machine_spiflash_writeblocks_obj, machine_spiflash_writeblocks);

STATIC const mp_rom_map_elem_t machine_spiflash_locals_dict_table[] = {
    // block device protocol
    { MP_ROM_QSTR(MP_QSTR_readblocks), MP_ROM_PTR(&machine_spiflash_readblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&machine_spiflash_writeblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl), MP_ROM_PTR(&machine_spiflash_ioctl_obj) },
};

STATIC MP_DEFINE_CONST_DICT(machine_spiflash_locals_dict, machine_spiflash_locals_dict_table);

const mp_obj_type_t mp_machine_spiflash_type = {
    { &mp_type_type },
    .name = MP_QSTR_SPIFlash,
    .make_new = machine_spiflash_make_new,
    .locals_dict = (mp_obj_t)&machine_spiflash_locals_dict,
};

#endif // MICROPY_PY_MACHINE_SPIFLASH
