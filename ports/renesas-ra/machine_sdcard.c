/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Vekatech Ltd.
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

#include "py/mpconfig.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "modmachine.h"
#include "extmod/vfs.h"
#include "hal_data.h"
#include "led.h"

#if MICROPY_HW_HAS_SDHI_CARD

#define CARD_HOLDER    1000  // 500 ms
#define CARD_OPERATION  200  // 100 ms

static volatile uint8_t OP_complete = 0;

typedef struct _machine_sdcard_obj_t {
    mp_obj_base_t base;
    sdmmc_status_t status;
    uint16_t block_len;
    uint32_t block_count;
} machine_sdcard_obj_t;

static machine_sdcard_obj_t machine_sdcard_objs[] = {
    {{&machine_sdcard_type}, {false, false, false}, 0, 0}
};

void sdhi_ISR(sdmmc_callback_args_t *p_args) {
    switch (p_args->event)
    {
        case SDMMC_EVENT_CARD_INSERTED:
            machine_sdcard_objs[0].status.card_inserted = true;
            OP_complete = 0;
            break;

        case SDMMC_EVENT_CARD_REMOVED:
            machine_sdcard_objs[0].status.card_inserted = false;
            machine_sdcard_objs[0].status.initialized = false;
            machine_sdcard_objs[0].block_count = 0;
            break;

        case SDMMC_EVENT_ERASE_COMPLETE:
        case SDMMC_EVENT_TRANSFER_COMPLETE:
            OP_complete = 0;
            break;

        // case SDMMC_EVENT_ERASE_BUSY:
        // case SDMMC_EVENT_TRANSFER_ERROR:

        default:
            break;
    }
}

static mp_obj_t machine_sdcard_init(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    fsp_err_t err = FSP_SUCCESS;
    machine_sdcard_obj_t *self = mp_const_none;

    if (g_sdmmc0_ctrl.open) {
    check:
        if ((err = R_SDHI_StatusGet(&g_sdmmc0_ctrl, &self->status)) == FSP_SUCCESS) {
            self = &machine_sdcard_objs[0];
            if (!self->status.initialized && self->status.card_inserted) {
                if ((err = R_SDHI_MediaInit(&g_sdmmc0_ctrl, NULL)) == FSP_SUCCESS) {
                    self->status.initialized = true;
                    self->block_len = g_sdmmc0_ctrl.device.sector_size_bytes;
                    self->block_count = g_sdmmc0_ctrl.device.sector_count;
                } else {
                    mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("can't init SDHI card, %d"), err);
                }
            }
        } else {
            mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("can't get SDHI Status, %d"), err);
        }
    } else {
        if ((err = R_SDHI_Open(&g_sdmmc0_ctrl, &g_sdmmc0_cfg)) == FSP_SUCCESS) {
            uint32_t Istamp = mp_hal_ticks_ms();
            OP_complete = 1;
            while (OP_complete) {
                if ((mp_hal_ticks_ms() - Istamp) > CARD_HOLDER) {
                    OP_complete = 0;
                    Istamp = 0;
                }
            }

            goto check;
        } else {
            mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("can't open SDHI controller, %d"), err);
        }
    }

    return MP_OBJ_FROM_PTR(self);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_sdcard_init_obj, 1, machine_sdcard_init);

static mp_obj_t sdcard_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    mp_map_t kw_args;

    mp_arg_check_num(n_args, n_kw, 0, 0, true);
    mp_map_init_fixed_table(&kw_args, n_kw, all_args + n_args);
    return machine_sdcard_init(n_args, all_args, &kw_args);
}

// deinit()
static mp_obj_t machine_sdcard_deinit(mp_obj_t self_in) {
    machine_sdcard_obj_t *self = MP_OBJ_TO_PTR(self_in);
    R_SDHI_Close(&g_sdmmc0_ctrl);
    self->block_count = self->block_len = 0;
    self->status.card_inserted = self->status.initialized = self->status.transfer_in_progress = false;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_sdcard_deinit_obj, machine_sdcard_deinit);

// present()
static mp_obj_t machine_sdcard_present(mp_obj_t self_in) {
    fsp_err_t err = FSP_SUCCESS;
    machine_sdcard_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if ((err = R_SDHI_StatusGet(&g_sdmmc0_ctrl, &self->status)) == FSP_SUCCESS) {
        return mp_obj_new_bool(self->status.card_inserted);
    } else {
        mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("can't get SDHI Status, %d"), err);
    }
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_sdcard_present_obj, machine_sdcard_present);

static mp_obj_t machine_sdcard_info(mp_obj_t self_in) {
    machine_sdcard_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->status.card_inserted && self->status.initialized) {
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
static MP_DEFINE_CONST_FUN_OBJ_1(machine_sdcard_info_obj, machine_sdcard_info);

// readblocks(block_num, buf)
static mp_obj_t machine_sdcard_readblocks(mp_obj_t self_in, mp_obj_t block_num, mp_obj_t buf) {
    mp_buffer_info_t bufinfo;
    machine_sdcard_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_WRITE);

    if ((self->status.initialized) && (FSP_SUCCESS == R_SDHI_Read(&g_sdmmc0_ctrl, bufinfo.buf, mp_obj_get_int(block_num), bufinfo.len / g_sdmmc0_cfg.block_size))) {
        uint32_t Rstamp = mp_hal_ticks_ms();
        OP_complete = 1;
        while (OP_complete) {
            if ((mp_hal_ticks_ms() - Rstamp) > CARD_OPERATION) {
                OP_complete = 0;
                Rstamp = 0;
            }
        }

        return Rstamp? MP_OBJ_NEW_SMALL_INT(0) : MP_OBJ_NEW_SMALL_INT(-MP_EIO);
    } else {
        return MP_OBJ_NEW_SMALL_INT(-MP_EIO);
    }
}
static MP_DEFINE_CONST_FUN_OBJ_3(machine_sdcard_readblocks_obj, machine_sdcard_readblocks);

// writeblocks(block_num, buf)
static mp_obj_t machine_sdcard_writeblocks(mp_obj_t self_in, mp_obj_t block_num, mp_obj_t buf) {
    mp_buffer_info_t bufinfo;
    machine_sdcard_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_WRITE);

    if ((self->status.initialized) && (FSP_SUCCESS == R_SDHI_Write(&g_sdmmc0_ctrl, bufinfo.buf, mp_obj_get_int(block_num), bufinfo.len / g_sdmmc0_cfg.block_size))) {
        uint32_t Wstamp = mp_hal_ticks_ms();
        OP_complete = 1;
        while (OP_complete) {
            if ((mp_hal_ticks_ms() - Wstamp) > CARD_OPERATION) {
                OP_complete = 0;
                Wstamp = 0;
            }
        }

        return Wstamp? MP_OBJ_NEW_SMALL_INT(0) : MP_OBJ_NEW_SMALL_INT(-MP_EIO);
    } else {
        return MP_OBJ_NEW_SMALL_INT(-MP_EIO);
    }
}
static MP_DEFINE_CONST_FUN_OBJ_3(machine_sdcard_writeblocks_obj, machine_sdcard_writeblocks);

// ioctl(op, arg)
static mp_obj_t machine_sdcard_ioctl(mp_obj_t self_in, mp_obj_t cmd_in, mp_obj_t arg_in) {
    machine_sdcard_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t cmd = mp_obj_get_int(cmd_in);

    switch (cmd) {
        case MP_BLOCKDEV_IOCTL_INIT: {
            if (!g_sdmmc0_ctrl.open) {
                if (FSP_SUCCESS == R_SDHI_Open(&g_sdmmc0_ctrl, &g_sdmmc0_cfg)) {
                    uint32_t Istamp = mp_hal_ticks_ms();
                    OP_complete = 1;
                    while (OP_complete) {
                        if ((mp_hal_ticks_ms() - Istamp) > CARD_OPERATION) {
                            OP_complete = 0;
                            Istamp = 0;
                        }
                    }
                } else {
                    return MP_OBJ_NEW_SMALL_INT(-MP_EIO);
                }
            }

            R_SDHI_StatusGet(&g_sdmmc0_ctrl, &self->status);

            if (self->status.card_inserted) {
                if (!self->status.initialized) {
                    if (FSP_SUCCESS == R_SDHI_MediaInit(&g_sdmmc0_ctrl, NULL)) {
                        self->status.initialized = true;
                        self->block_len = g_sdmmc0_ctrl.device.sector_size_bytes;
                        self->block_count = g_sdmmc0_ctrl.device.sector_count;
                        return MP_OBJ_NEW_SMALL_INT(0);
                    } else {
                        return MP_OBJ_NEW_SMALL_INT(-MP_EIO); // Initialization failed
                    }
                } else {
                    return MP_OBJ_NEW_SMALL_INT(0);
                }
            } else {
                return MP_OBJ_NEW_SMALL_INT(-MP_EIO);  // Initialization failed
            }
        }
        break;

        case MP_BLOCKDEV_IOCTL_DEINIT: {
            if (g_sdmmc0_ctrl.open) {
                R_SDHI_Close(&g_sdmmc0_ctrl);
                self->block_count = self->block_len = 0;
                self->status.card_inserted = self->status.initialized = self->status.transfer_in_progress = false;
                return MP_OBJ_NEW_SMALL_INT(0);
            } else {
                return MP_OBJ_NEW_SMALL_INT(-MP_EIO);  // Deinitialization failed
            }
        }
        break;

        case MP_BLOCKDEV_IOCTL_SYNC: {
            return MP_OBJ_NEW_SMALL_INT(0);
        }
        break;

        case MP_BLOCKDEV_IOCTL_BLOCK_COUNT: {
            if (self->status.initialized) {
                return MP_OBJ_NEW_SMALL_INT(self->block_count);
            } else {
                return MP_OBJ_NEW_SMALL_INT(-MP_EIO);  // Card not initialized
            }
        }
        break;

        case MP_BLOCKDEV_IOCTL_BLOCK_SIZE: {
            if (self->status.initialized) {
                return MP_OBJ_NEW_SMALL_INT(self->block_len);
            } else {
                return MP_OBJ_NEW_SMALL_INT(-MP_EIO);  // Card not initialized
            }
        }
        break;

        case MP_BLOCKDEV_IOCTL_BLOCK_ERASE: {
            if (self->status.initialized) {
                uint32_t Estamp = mp_hal_ticks_ms();
                OP_complete = 1;
                while (OP_complete) {
                    if ((mp_hal_ticks_ms() - Estamp) > CARD_OPERATION) {
                        OP_complete = 0;
                        Estamp = 0;
                    }
                }

                return Estamp? MP_OBJ_NEW_SMALL_INT(0) : MP_OBJ_NEW_SMALL_INT(-MP_EIO);
            } else {
                return MP_OBJ_NEW_SMALL_INT(-MP_EIO);  // Card not initialized
            }
        }

        default: // unknown command
            return mp_const_none;
            break;
    }
}
static MP_DEFINE_CONST_FUN_OBJ_3(machine_sdcard_ioctl_obj, machine_sdcard_ioctl);

static const mp_rom_map_elem_t sdcard_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init),        MP_ROM_PTR(&machine_sdcard_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit),      MP_ROM_PTR(&machine_sdcard_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_present),     MP_ROM_PTR(&machine_sdcard_present_obj) },
    { MP_ROM_QSTR(MP_QSTR_info),        MP_ROM_PTR(&machine_sdcard_info_obj) },
    // block device protocol
    { MP_ROM_QSTR(MP_QSTR_readblocks),  MP_ROM_PTR(&machine_sdcard_readblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&machine_sdcard_writeblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl),       MP_ROM_PTR(&machine_sdcard_ioctl_obj) },
};
static MP_DEFINE_CONST_DICT(sdcard_locals_dict, sdcard_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_sdcard_type,
    MP_QSTR_SDCard,
    MP_TYPE_FLAG_NONE,
    make_new, sdcard_obj_make_new,
    locals_dict, &sdcard_locals_dict
    );

#endif // MICROPY_PY_MACHINE_SDCARD
