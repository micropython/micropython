// modusbmsc.c
#include "py/runtime.h"
#include "py/obj.h"
#include "py/mphal.h"

#include "usb/usb_host.h"
#include "usb/msc_host.h"

#define USBMSC_TAG "modusbmsc"

typedef struct _usbmsc_obj_t {
    mp_obj_base_t base;
    msc_host_device_handle_t dev;
    uint32_t block_size;
    uint32_t block_count;
    bool ready;
} usbmsc_obj_t;

STATIC usbmsc_obj_t usbmsc_singleton;
static mp_obj_t usbmsc_py_callback = mp_const_none;


STATIC mp_obj_t usbmsc_set_callback(mp_obj_t cb) {
    usbmsc_py_callback = cb;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(usbmsc_set_callback_obj, usbmsc_set_callback);


// --- Low-level helpers (C side) -----------------------------------------

// You should call this from your USB MSC host event code when a device is ready.
// For now we assume a single device and fill the singleton.
void usbmsc_set_device(msc_host_device_handle_t dev,
                       uint32_t block_size,
                       uint32_t block_count)
{
    usbmsc_singleton.dev = dev;
    usbmsc_singleton.block_size = block_size;
    usbmsc_singleton.block_count = block_count;
    usbmsc_singleton.ready = true;
}

// Optional: call when device is removed.
void usbmsc_clear_device(void)
{
    usbmsc_singleton.dev = NULL;
    usbmsc_singleton.block_size = 0;
    usbmsc_singleton.block_count = 0;
    usbmsc_singleton.ready = false;
}

// --- Block device methods -----------------------------------------------

STATIC mp_obj_t usbmsc_readblocks(mp_obj_t self_in, mp_obj_t blocknum_in, mp_obj_t buf_in) {
    usbmsc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (!self->ready || self->dev == NULL) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("USB MSC not ready"));
    }

    uint32_t blocknum = mp_obj_get_int(blocknum_in);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_WRITE);

    if (self->block_size == 0) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("invalid block size"));
    }

    size_t nblocks = bufinfo.len / self->block_size;
    if (nblocks == 0 || (nblocks * self->block_size) != bufinfo.len) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("buffer size not multiple of block size"));
    }

    esp_err_t err = msc_host_read_sector(self->dev, blocknum, bufinfo.buf, nblocks);
    if (err != ESP_OK) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("USB MSC read failed"));
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(usbmsc_readblocks_obj, usbmsc_readblocks);

STATIC mp_obj_t usbmsc_writeblocks(mp_obj_t self_in, mp_obj_t blocknum_in, mp_obj_t buf_in) {
    usbmsc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (!self->ready || self->dev == NULL) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("USB MSC not ready"));
    }

    uint32_t blocknum = mp_obj_get_int(blocknum_in);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);

    if (self->block_size == 0) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("invalid block size"));
    }

    size_t nblocks = bufinfo.len / self->block_size;
    if (nblocks == 0 || (nblocks * self->block_size) != bufinfo.len) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("buffer size not multiple of block size"));
    }

    esp_err_t err = msc_host_write_sector(self->dev, blocknum, bufinfo.buf, nblocks);
    if (err != ESP_OK) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("USB MSC write failed"));
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(usbmsc_writeblocks_obj, usbmsc_writeblocks);

STATIC mp_obj_t usbmsc_ioctl(mp_obj_t self_in, mp_obj_t op_in, mp_obj_t arg_in) {
    usbmsc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t op = mp_obj_get_int(op_in);

    switch (op) {
        case MP_BLOCKDEV_IOCTL_INIT:
            // Already initialized by USB MSC host
            return MP_OBJ_NEW_SMALL_INT(0);

        case MP_BLOCKDEV_IOCTL_DEINIT:
            // Let ESP-IDF side handle actual teardown if needed
            return MP_OBJ_NEW_SMALL_INT(0);

        case MP_BLOCKDEV_IOCTL_SYNC:
            // No explicit flush needed for MSC host
            return MP_OBJ_NEW_SMALL_INT(0);

        case MP_BLOCKDEV_IOCTL_BLOCK_COUNT:
            if (!self->ready) {
                return MP_OBJ_NEW_SMALL_INT(0);
            }
            return MP_OBJ_NEW_SMALL_INT(self->block_count);

        case MP_BLOCKDEV_IOCTL_BLOCK_SIZE:
            if (!self->ready) {
                return MP_OBJ_NEW_SMALL_INT(0);
            }
            return MP_OBJ_NEW_SMALL_INT(self->block_size);

        default:
            return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(usbmsc_ioctl_obj, usbmsc_ioctl);

// --- Type definition -----------------------------------------------------

STATIC const mp_rom_map_elem_t usbmsc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_readblocks), MP_ROM_PTR(&usbmsc_readblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&usbmsc_writeblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl), MP_ROM_PTR(&usbmsc_ioctl_obj) },
};
STATIC MP_DEFINE_CONST_DICT(usbmsc_locals_dict, usbmsc_locals_dict_table);

STATIC const mp_obj_type_t usbmsc_type = {
    { &mp_type_type },
    .name = MP_QSTR_USBMSC,
    .locals_dict = (mp_obj_dict_t *)&usbmsc_locals_dict,
};

// --- Module-level constructor -------------------------------------------

STATIC mp_obj_t usbmsc_device(void) {
    // Return a reference to the singleton block device
    usbmsc_singleton.base.type = &usbmsc_type;
    return MP_OBJ_FROM_PTR(&usbmsc_singleton);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(usbmsc_device_obj, usbmsc_device);

// Optional: simple "ready" helper for Python
STATIC mp_obj_t usbmsc_is_ready(void) {
    return mp_obj_new_bool(usbmsc_singleton.ready && usbmsc_singleton.dev != NULL);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(usbmsc_is_ready_obj, usbmsc_is_ready);

// --- Module globals ------------------------------------------------------

STATIC const mp_rom_map_elem_t mp_module_usbmsc_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_usbmsc) },
    { MP_ROM_QSTR(MP_QSTR_device), MP_ROM_PTR(&usbmsc_device_obj) },
    { MP_ROM_QSTR(MP_QSTR_is_ready), MP_ROM_PTR(&usbmsc_is_ready_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_callback), MP_ROM_PTR(&usbmsc_set_callback_obj) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_usbmsc_globals, mp_module_usbmsc_globals_table);

const mp_obj_module_t mp_module_usbmsc = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_usbmsc_globals,
};

MP_REGISTER_MODULE(MP_QSTR_usbmsc, mp_module_usbmsc);
