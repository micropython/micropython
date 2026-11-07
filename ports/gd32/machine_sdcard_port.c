// machine_sdcard_port.c - machine.SDCard for GD32F4xx (SDIO)
// The port defines the type; it is registered through
// MICROPY_PY_MACHINE_EXTRA_GLOBALS.

#include <string.h>

#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/objarray.h"
#include "py/mphal.h"
#include "extmod/vfs.h"

#include "gd32f4xx.h"

// implemented in sdcard.c
int sdcard_init(void);
bool sdcard_ready(void);
bool sdcard_is_present(void);
int sdcard_read(uint32_t block, uint8_t *buf, uint32_t count);
int sdcard_write(uint32_t block, const uint8_t *buf, uint32_t count);
uint32_t sdcard_block_count(void);
bool sdcard_is_high_capacity(void);
const char *sdcard_error(void);

typedef struct _machine_sdcard_obj_t {
    mp_obj_base_t base;
} machine_sdcard_obj_t;

// forward declaration (type defined at the bottom)
extern const mp_obj_type_t machine_sdcard_type;

static machine_sdcard_obj_t machine_sdcard_obj = {
    .base = { &machine_sdcard_type },
};

// ---- block device protocol ----
static mp_obj_t machine_sdcard_readblocks(size_t n_args, const mp_obj_t *args) {
    uint32_t block_num = mp_obj_get_int(args[1]);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_WRITE);

    size_t offset = 0;
    size_t len = bufinfo.len;
    if (n_args > 3) {
        offset = mp_obj_get_int(args[3]);
        if (n_args > 3 && offset + len > 512) {
            len = 512 - offset;
        }
    }

    static uint8_t bounce[512];
    uint32_t blocks = (len + 511) / 512;
    if (blocks == 0) {
        blocks = 1;
    }
    if (sdcard_read(block_num, bounce, blocks) != 0) {
        mp_raise_OSError(MP_EIO);
    }
    memcpy(bufinfo.buf, bounce + offset, len);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_sdcard_readblocks_obj, 3, 4, machine_sdcard_readblocks);

static mp_obj_t machine_sdcard_writeblocks(size_t n_args, const mp_obj_t *args) {
    uint32_t block_num = mp_obj_get_int(args[1]);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_READ);

    size_t offset = 0;
    size_t len = bufinfo.len;
    if (n_args > 3) {
        offset = mp_obj_get_int(args[3]);
        if (offset + len > 512) {
            len = 512 - offset;
        }
    }

    static uint8_t bounce[512];
    memset(bounce, 0xff, sizeof(bounce));
    memcpy(bounce + offset, bufinfo.buf, len);

    if (sdcard_write(block_num, bounce, 1) != 0) {
        mp_raise_OSError(MP_EIO);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_sdcard_writeblocks_obj, 3, 4, machine_sdcard_writeblocks);

static mp_obj_t machine_sdcard_ioctl(mp_obj_t self_in, mp_obj_t cmd_in, mp_obj_t arg_in) {
    (void)self_in;
    mp_int_t cmd = mp_obj_get_int(cmd_in);
    switch (cmd) {
        case MP_BLOCKDEV_IOCTL_INIT:
            return MP_OBJ_NEW_SMALL_INT(sdcard_init() == 0 ? 0 : -1);
        case MP_BLOCKDEV_IOCTL_DEINIT:
            return MP_OBJ_NEW_SMALL_INT(0);
        case MP_BLOCKDEV_IOCTL_SYNC:
            return MP_OBJ_NEW_SMALL_INT(0);
        case MP_BLOCKDEV_IOCTL_BLOCK_COUNT:
            return MP_OBJ_NEW_SMALL_INT(sdcard_block_count());
        case MP_BLOCKDEV_IOCTL_BLOCK_SIZE:
            return MP_OBJ_NEW_SMALL_INT(512);
        case MP_BLOCKDEV_IOCTL_BLOCK_ERASE:
            return MP_OBJ_NEW_SMALL_INT(0);
        default:
            return MP_OBJ_NEW_SMALL_INT(-1);
    }
}
static MP_DEFINE_CONST_FUN_OBJ_3(machine_sdcard_ioctl_obj, machine_sdcard_ioctl);

// extra methods: present() / info() / mount helpers
static mp_obj_t machine_sdcard_present(mp_obj_t self_in) {
    (void)self_in;
    return mp_obj_new_bool(sdcard_is_present());
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_sdcard_present_obj, machine_sdcard_present);

static mp_obj_t machine_sdcard_info(mp_obj_t self_in) {
    (void)self_in;
    mp_obj_t items[4] = {
        MP_OBJ_NEW_SMALL_INT(sdcard_block_count()),
        MP_OBJ_NEW_SMALL_INT(512),
        mp_obj_new_bool(sdcard_is_high_capacity()),
        mp_obj_new_bool(sdcard_ready()),
    };
    return mp_obj_new_tuple(4, items);
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_sdcard_info_obj, machine_sdcard_info);

static const mp_rom_map_elem_t machine_sdcard_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_readblocks),  MP_ROM_PTR(&machine_sdcard_readblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&machine_sdcard_writeblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl),       MP_ROM_PTR(&machine_sdcard_ioctl_obj) },
    { MP_ROM_QSTR(MP_QSTR_present),     MP_ROM_PTR(&machine_sdcard_present_obj) },
    { MP_ROM_QSTR(MP_QSTR_info),        MP_ROM_PTR(&machine_sdcard_info_obj) },
};
static MP_DEFINE_CONST_DICT(machine_sdcard_locals_dict, machine_sdcard_locals_dict_table);

// ---- type ----
static void machine_sdcard_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    mp_printf(print, "SDCard(present=%s, ready=%s)",
        sdcard_is_present() ? "yes" : "no",
        sdcard_ready() ? "yes" : "no");
}

static mp_obj_t machine_sdcard_make_new(const mp_obj_type_t *type, size_t n_args,
                                        size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, MP_OBJ_FUN_ARGS_MAX, true);

    if (!sdcard_is_present()) {
        mp_raise_OSError(MP_ENODEV);
    }
    if (sdcard_init() != 0) {
        mp_raise_OSError(MP_EIO);
    }
    (void)type;
    machine_sdcard_obj.base.type = &machine_sdcard_type;
    return MP_OBJ_FROM_PTR(&machine_sdcard_obj);
}

MP_DEFINE_CONST_OBJ_TYPE(
    machine_sdcard_type,
    MP_QSTR_SDCard,
    MP_TYPE_FLAG_NONE,
    make_new, machine_sdcard_make_new,
    print, machine_sdcard_print,
    locals_dict, &machine_sdcard_locals_dict
    );
