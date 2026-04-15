#include "py/runtime.h"
#include "py/obj.h"
#include "py/mperrno.h"
#include "extmod/vfs_fat.h"

#include "esp_partition.h"
#include "wear_levelling.h"
#include "esp_log.h"

#define TAG "wl_blockdev"

typedef struct _wl_blockdev_t {
    mp_obj_base_t base;
    wl_handle_t handle;
    const esp_partition_t *part;
    uint32_t sector_size;
} wl_blockdev_t;

const mp_obj_type_t machine_flash_wl_type;

static mp_obj_t machine_flash_wl_make_new(void) {
    wl_blockdev_t *self = mp_obj_malloc_with_finaliser(wl_blockdev_t, &machine_flash_wl_type);
    self->base.type = &machine_flash_wl_type;

    const esp_partition_t *part =
        esp_partition_find_first(ESP_PARTITION_TYPE_DATA,
                                 ESP_PARTITION_SUBTYPE_ANY,
                                 "locfd");

    if (!part) {
        mp_raise_OSError(MP_ENOENT);
    }

    esp_err_t err = wl_mount(part, &self->handle);
    if (err != ESP_OK) {
        mp_raise_OSError(err);
    }
    self->part = part;
    self->sector_size = wl_sector_size(self->handle);
    ESP_LOGI(TAG, "locfd partition at 0x%08x, size 0x%08x, sector size %d",
        part->address, part->size, self->sector_size);
    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t machine_flash_wl_readblocks(mp_obj_t self_in, mp_obj_t block_num, mp_obj_t buf_in) {
    wl_blockdev_t *self = MP_OBJ_TO_PTR(self_in);
    uint32_t sector = mp_obj_get_int(block_num);

    mp_buffer_info_t buf;
    mp_get_buffer_raise(buf_in, &buf, MP_BUFFER_WRITE);

    wl_read(self->handle, sector * self->sector_size, buf.buf, buf.len);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_3(machine_flash_wl_readblocks_obj, machine_flash_wl_readblocks);

static mp_obj_t machine_flash_wl_writeblocks(mp_obj_t self_in, mp_obj_t block_num, mp_obj_t buf_in) {
    wl_blockdev_t *self = MP_OBJ_TO_PTR(self_in);
    uint32_t sector = mp_obj_get_int(block_num);

    mp_buffer_info_t buf;
    mp_get_buffer_raise(buf_in, &buf, MP_BUFFER_READ);

    wl_write(self->handle, sector * self->sector_size, buf.buf, buf.len);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_3(machine_flash_wl_writeblocks_obj, machine_flash_wl_writeblocks);

// ---------------------------------------------------------------------------
// ioctl(self, op, arg)
// ---------------------------------------------------------------------------

static mp_obj_t machine_flash_wl_ioctl(mp_obj_t self_in, mp_obj_t op_in, mp_obj_t arg_in) {
    wl_blockdev_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t op = mp_obj_get_int(op_in);

    switch (op) {
        case MP_BLOCKDEV_IOCTL_INIT:
        case MP_BLOCKDEV_IOCTL_DEINIT:
        case MP_BLOCKDEV_IOCTL_SYNC:
            return MP_OBJ_NEW_SMALL_INT(0);

        case MP_BLOCKDEV_IOCTL_BLOCK_COUNT:
            return mp_obj_new_int(self->part->size / self->sector_size);

        case MP_BLOCKDEV_IOCTL_BLOCK_SIZE:
            return mp_obj_new_int(self->sector_size);
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_3(machine_flash_wl_ioctl_obj, machine_flash_wl_ioctl);

static const mp_rom_map_elem_t machine_flash_wl_locals_dict_table[] = {
    // block device protocol
    { MP_ROM_QSTR(MP_QSTR_readblocks), MP_ROM_PTR(&machine_flash_wl_readblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&machine_flash_wl_writeblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl), MP_ROM_PTR(&machine_flash_wl_ioctl_obj) },
};

static MP_DEFINE_CONST_DICT(machine_flash_wl_locals_dict, machine_flash_wl_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_flash_wl_type,
    MP_QSTR_FlashWL,
    MP_TYPE_FLAG_NONE,
    make_new, machine_flash_wl_make_new,
    locals_dict, &machine_flash_wl_locals_dict
);
