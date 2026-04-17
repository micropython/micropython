#include "py/runtime.h"
#include "py/objstr.h"
#include "mphalport.h"
#include "modesp32.h"

#if MICROPY_PY_ESP32_DS

#include "esp_ds.h"
#include "hal/hmac_types.h"

typedef struct _esp32_ds_obj_t {
    mp_obj_base_t base;
} esp32_ds_obj_t;

static mp_obj_t esp32_ds_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    mp_arg_check_num(n_args, n_kw, 0, 0, false);
    return mp_obj_malloc(esp32_ds_obj_t, type);
}

static mp_obj_t esp32_ds_sign(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    mp_buffer_info_t message_info;
    mp_buffer_info_t data_info;
    mp_get_buffer_raise(args[1], &message_info, MP_BUFFER_READ);
    mp_get_buffer_raise(args[2], &data_info, MP_BUFFER_READ);

    if (data_info.len != sizeof(esp_ds_data_t)) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid DS data length"));
    }

    const esp_ds_data_t *data = (const esp_ds_data_t *)data_info.buf;
    size_t rsa_bytes = (data->rsa_length + 1) * sizeof(uint32_t);
    if (message_info.len != rsa_bytes) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid message length"));
    }

    mp_int_t key_id = mp_obj_get_int(args[3]);
    if (key_id < 0 || key_id >= HMAC_KEY_MAX || key_id == 6) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid HMAC key id"));
    }

    vstr_t vstr;
    vstr_init_len(&vstr, rsa_bytes);
    esp_err_t err = esp_ds_sign(message_info.buf, data, (hmac_key_id_t)key_id, vstr.buf);
    if (err != ESP_OK) {
        vstr_clear(&vstr);
        check_esp_err(err);
    }
    return mp_obj_new_bytes_from_vstr(&vstr);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp32_ds_sign_obj, 4, 4, esp32_ds_sign);

static const mp_rom_map_elem_t esp32_ds_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_sign), MP_ROM_PTR(&esp32_ds_sign_obj) },
    { MP_ROM_QSTR(MP_QSTR_KEY_MAX), MP_ROM_INT(HMAC_KEY_MAX) },
    { MP_ROM_QSTR(MP_QSTR_KEY0), MP_ROM_INT(HMAC_KEY0) },
    { MP_ROM_QSTR(MP_QSTR_KEY1), MP_ROM_INT(HMAC_KEY1) },
    { MP_ROM_QSTR(MP_QSTR_KEY2), MP_ROM_INT(HMAC_KEY2) },
    { MP_ROM_QSTR(MP_QSTR_KEY3), MP_ROM_INT(HMAC_KEY3) },
    { MP_ROM_QSTR(MP_QSTR_KEY4), MP_ROM_INT(HMAC_KEY4) },
    { MP_ROM_QSTR(MP_QSTR_KEY5), MP_ROM_INT(HMAC_KEY5) },
    #if SOC_KEY_MANAGER_HMAC_KEY_DEPLOY
    { MP_ROM_QSTR(MP_QSTR_KEY_KM), MP_ROM_INT(HMAC_KEY_KM) },
    #endif
};

static MP_DEFINE_CONST_DICT(esp32_ds_locals_dict, esp32_ds_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    esp32_ds_type,
    MP_QSTR_DS,
    MP_TYPE_FLAG_NONE,
    make_new, esp32_ds_make_new,
    locals_dict, &esp32_ds_locals_dict
    );

#endif
