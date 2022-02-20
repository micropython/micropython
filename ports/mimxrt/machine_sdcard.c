/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Philipp Ebensberger
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

#if MICROPY_PY_MACHINE_SDCARD

#include "py/runtime.h"
#include "py/mperrno.h"
#include "extmod/vfs.h"
#include "ticks.h"
#include "fsl_cache.h"

#include "sdcard.h"


enum { SDCARD_INIT_ARG_ID };


STATIC const mp_arg_t sdcard_init_allowed_args[] = {
    [SDCARD_INIT_ARG_ID] = { MP_QSTR_id, MP_ARG_INT, {.u_int = 1} },
};


STATIC void machine_sdcard_init_helper(mimxrt_sdcard_obj_t *self) {
    sdcard_init(self, 198000000UL);  // Initialize SDCard Host with 198MHz base clock
    sdcard_init_pins(self);

    ticks_delay_us64(2ULL * 1000ULL);  // Wait 2ms to allow USDHC signals to settle/debounce
}

STATIC mp_obj_t sdcard_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, all_args + n_args);

    // Parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(sdcard_init_allowed_args)];
    mp_arg_parse_all(n_args, all_args, &kw_args, MP_ARRAY_SIZE(sdcard_init_allowed_args), sdcard_init_allowed_args, args);

    // Extract arguments
    mp_int_t sdcard_id = args[SDCARD_INIT_ARG_ID].u_int;

    if (!(1 <= sdcard_id && sdcard_id <= MP_ARRAY_SIZE(mimxrt_sdcard_objs))) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "SDCard(%d) doesn't exist", sdcard_id));
    }

    mimxrt_sdcard_obj_t *self = &mimxrt_sdcard_objs[(sdcard_id - 1)];

    // Initialize SDCard Host
    if (!sdcard_state_initialized(self)) {
        machine_sdcard_init_helper(self);
    }
    return MP_OBJ_FROM_PTR(self);
}

// init()
STATIC mp_obj_t machine_sdcard_init(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    mimxrt_sdcard_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    if (!sdcard_state_initialized(self)) {
        machine_sdcard_init_helper(self);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_sdcard_init_obj, 1, machine_sdcard_init);

// deinit()
STATIC mp_obj_t machine_sdcard_deinit(mp_obj_t self_in) {
    mimxrt_sdcard_obj_t *self = MP_OBJ_TO_PTR(self_in);
    sdcard_deinit(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_sdcard_deinit_obj, machine_sdcard_deinit);

// readblocks(block_num, buf)
STATIC mp_obj_t machine_sdcard_readblocks(mp_obj_t self_in, mp_obj_t block_num, mp_obj_t buf) {
    mp_buffer_info_t bufinfo;
    mimxrt_sdcard_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_WRITE);

    if (sdcard_read(self, bufinfo.buf, mp_obj_get_int(block_num), bufinfo.len / SDCARD_DEFAULT_BLOCK_SIZE)) {
        return MP_OBJ_NEW_SMALL_INT(0);
    } else {
        return MP_OBJ_NEW_SMALL_INT(-MP_EIO);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(machine_sdcard_readblocks_obj, machine_sdcard_readblocks);

// present()
STATIC mp_obj_t machine_sdcard_present(mp_obj_t self_in) {
    mimxrt_sdcard_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(sdcard_detect(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_sdcard_present_obj, machine_sdcard_present);

STATIC mp_obj_t machine_sdcard_info(mp_obj_t self_in) {
    mimxrt_sdcard_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (sdcard_detect(self) && sdcard_state_initialized(self)) {
        uint32_t log_block_nbr = self->block_count;
        uint32_t log_block_size = self->block_len;

        mp_obj_t tuple[2] = {
            mp_obj_new_int_from_ull((uint64_t)log_block_nbr * (uint64_t)log_block_size),
            mp_obj_new_int_from_uint(log_block_size),
        };
        return mp_obj_new_tuple(2, tuple);
    } else {
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_sdcard_info_obj, machine_sdcard_info);

// writeblocks(block_num, buf)
STATIC mp_obj_t machine_sdcard_writeblocks(mp_obj_t self_in, mp_obj_t block_num, mp_obj_t buf) {
    mp_buffer_info_t bufinfo;
    mimxrt_sdcard_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_WRITE);

    if (sdcard_write(self, bufinfo.buf, mp_obj_get_int(block_num), bufinfo.len / SDCARD_DEFAULT_BLOCK_SIZE)) {
        return MP_OBJ_NEW_SMALL_INT(0);
    } else {
        return MP_OBJ_NEW_SMALL_INT(-MP_EIO);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(machine_sdcard_writeblocks_obj, machine_sdcard_writeblocks);

// ioctl(op, arg)
STATIC mp_obj_t machine_sdcard_ioctl(mp_obj_t self_in, mp_obj_t cmd_in, mp_obj_t arg_in) {
    mimxrt_sdcard_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t cmd = mp_obj_get_int(cmd_in);

    switch (cmd) {
        case MP_BLOCKDEV_IOCTL_INIT: {
            if (sdcard_detect(self)) {
                if (sdcard_power_on(self)) {
                    return MP_OBJ_NEW_SMALL_INT(0);
                } else {
                    sdcard_power_off(self);
                    return MP_OBJ_NEW_SMALL_INT(-MP_EIO);  // Initialization failed
                }
            } else {
                return MP_OBJ_NEW_SMALL_INT(-MP_EIO);  // Initialization failed
            }
        }
        case MP_BLOCKDEV_IOCTL_DEINIT: {
            if (sdcard_power_off(self)) {
                return MP_OBJ_NEW_SMALL_INT(0);
            } else {
                return MP_OBJ_NEW_SMALL_INT(-MP_EIO);  // Deinitialization failed
            }
        }
        case MP_BLOCKDEV_IOCTL_SYNC: {
            return MP_OBJ_NEW_SMALL_INT(0);
        }
        case MP_BLOCKDEV_IOCTL_BLOCK_COUNT: {
            if (sdcard_state_initialized(self)) {
                return MP_OBJ_NEW_SMALL_INT(self->block_count);
            } else {
                return MP_OBJ_NEW_SMALL_INT(-MP_EIO);  // Card not initialized
            }
        }
        case MP_BLOCKDEV_IOCTL_BLOCK_SIZE: {
            if (sdcard_state_initialized(self)) {
                return MP_OBJ_NEW_SMALL_INT(self->block_len);
            } else {
                return MP_OBJ_NEW_SMALL_INT(-MP_EIO);  // Card not initialized
            }
        }
        default: // unknown command
        {
            return mp_const_none;
        }
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(machine_sdcard_ioctl_obj, machine_sdcard_ioctl);

STATIC const mp_rom_map_elem_t sdcard_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init),        MP_ROM_PTR(&machine_sdcard_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit),      MP_ROM_PTR(&machine_sdcard_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_present),     MP_ROM_PTR(&machine_sdcard_present_obj) },
    { MP_ROM_QSTR(MP_QSTR_info),        MP_ROM_PTR(&machine_sdcard_info_obj) },
    // block device protocol
    { MP_ROM_QSTR(MP_QSTR_readblocks),  MP_ROM_PTR(&machine_sdcard_readblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&machine_sdcard_writeblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl),       MP_ROM_PTR(&machine_sdcard_ioctl_obj) },
};
STATIC MP_DEFINE_CONST_DICT(sdcard_locals_dict, sdcard_locals_dict_table);

const mp_obj_type_t machine_sdcard_type = {
    { &mp_type_type },
    .name = MP_QSTR_SDCard,
    .make_new = sdcard_obj_make_new,
    .locals_dict = (mp_obj_dict_t *)&sdcard_locals_dict,
};

void machine_sdcard_init0(void) {
    return;
}

#endif // MICROPY_PY_MACHINE_SDCARD
