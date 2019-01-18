#include <string.h>
#include "py/nlr.h"
#include "py/obj.h"
#include "py/runtime.h"

#include "esp_flash_partitions.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_partition.h"

const mp_obj_type_t mp_esp_ota_type;
static const char *TAG = "ota_update";

typedef struct mp_esp_ota_obj_t {
    mp_obj_base_t base;
    esp_ota_handle_t update_handle;
    const esp_partition_t *update_partition;
} mp_esp_ota_obj_t;

STATIC mp_obj_t mp_esp_ota_make_new(const mp_obj_type_t *type_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {

    // update handle : set by esp_ota_begin(), must be freed via esp_ota_end()
    mp_esp_ota_obj_t *self = m_new_obj(mp_esp_ota_obj_t);
    self->base.type = &mp_esp_ota_type;
    self->update_handle = 0;

    esp_err_t err;
    const esp_partition_t *boot_partition = esp_ota_get_boot_partition();
    const esp_partition_t *running_partition = esp_ota_get_running_partition();

    if (boot_partition != running_partition) {
        ESP_LOGW(TAG, "Configured OTA boot partition at offset 0x%08x, but running from offset 0x%08x",
                 boot_partition->address, running_partition->address);
        ESP_LOGW(TAG, "(This can happen if either the OTA boot data or preferred boot image become corrupted somehow.)");
    }

    ESP_LOGI(TAG, "Running partition label %s type %d subtype %d (offset 0x%08x)",
            running_partition->label, running_partition->type, running_partition->subtype,
            running_partition->address);

    // Get update partition
    self->update_partition = esp_ota_get_next_update_partition(NULL);
    assert(self->update_partition != NULL);

    ESP_LOGI(TAG, "Update partition label %s type %d subtype %d (offset 0x%08x)",
            self->update_partition->label, self->update_partition->type,
            self->update_partition->subtype, self->update_partition->address);

    err = esp_ota_begin(self->update_partition, OTA_SIZE_UNKNOWN, &self->update_handle);

    if (err != ESP_OK) {
        ESP_LOGI(TAG, "Cannot begin OTA, error code: %d", err);
        mp_raise_OSError(err);
    }

    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t mp_esp_ota_write(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {

    mp_esp_ota_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[1], &bufinfo, MP_BUFFER_READ);

    esp_err_t err = esp_ota_write(self->update_handle, bufinfo.buf, bufinfo.len);

    if (err != ESP_OK) {
        ESP_LOGI(TAG, "Cannot write OTA, error code: %d", err);
        mp_raise_OSError(err);
    }

    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_KW(mp_esp_ota_write_obj, 2, mp_esp_ota_write);

STATIC mp_obj_t mp_esp_ota_end(mp_obj_t self_in) {

    mp_esp_ota_obj_t *self = MP_OBJ_TO_PTR(self_in);

    esp_err_t err = esp_ota_end(self->update_handle);
    if (err != ESP_OK) {
        ESP_LOGI(TAG, "Cannot end OTA, error code: %d", err);
        mp_raise_OSError(err);
    }

    // Set the new running partition on next boot
    err = esp_ota_set_boot_partition(self->update_partition);
    if (err != ESP_OK) {
        ESP_LOGI(TAG, "Cannot set new boot partition, error code: %d", err);
        mp_raise_OSError(err);
    }

    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_esp_ota_end_obj, mp_esp_ota_end);

STATIC const mp_rom_map_elem_t mp_esp_ota_locals_dict_table[] = {
    {MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_esp_ota_write_obj)},
    {MP_ROM_QSTR(MP_QSTR_end), MP_ROM_PTR(&mp_esp_ota_end_obj)},
};
STATIC MP_DEFINE_CONST_DICT(mp_esp_ota_locals_dict, mp_esp_ota_locals_dict_table);

const mp_obj_type_t mp_esp_ota_type = {
    {&mp_type_type},
    .name = MP_QSTR_OTA,
    .make_new = mp_esp_ota_make_new,
    .locals_dict = (mp_obj_t)&mp_esp_ota_locals_dict,
};