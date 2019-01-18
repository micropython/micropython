#include "py/runtime.h"
#include "modeespota.h"

STATIC const mp_rom_map_elem_t esp_ota_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_esp_ota) },

    { MP_ROM_QSTR(MP_QSTR_OTA), MP_ROM_PTR(&mp_esp_ota_type) },
};

STATIC MP_DEFINE_CONST_DICT(esp_ota_module_globals, esp_ota_module_globals_table);

const mp_obj_module_t mp_module_esp_ota = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&esp_ota_module_globals,
};