#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "py/obj.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "mphalport.h"
#include "modmachine.h"
#include "nvs_flash.h"
#include "nvs.h"

#define STATIC static

esp_err_t driver_nvs_init() { return ESP_OK; }

//------------------------------------------------------------------------
STATIC mp_obj_t mod_machine_nvs_set_u8 (mp_obj_t _handle, mp_obj_t _key, mp_obj_t _value) {
	const char *handle = mp_obj_str_get_str(_handle);
    const char *key = mp_obj_str_get_str(_key);
    uint8_t value = mp_obj_get_int_truncated(_value);
	
	nvs_handle my_handle;
	esp_err_t res = nvs_open(handle, NVS_READWRITE, &my_handle);
	if (res != ESP_OK) {
		mp_raise_ValueError("NVS handle not found!");
		return mp_const_none;
	}

    esp_err_t esp_err = nvs_set_u8(my_handle, key, value);
    if (ESP_OK == esp_err) {
        nvs_commit(my_handle);
    }
    else if (ESP_ERR_NVS_NOT_ENOUGH_SPACE == esp_err || ESP_ERR_NVS_PAGE_FULL == esp_err || ESP_ERR_NVS_NO_FREE_PAGES == esp_err) {
        mp_raise_msg(&mp_type_OSError, "No space available.");
    }
    else if (ESP_ERR_NVS_INVALID_NAME == esp_err || ESP_ERR_NVS_KEY_TOO_LONG == esp_err) {
        mp_raise_msg(&mp_type_OSError, "Key invalid or too long");
    }
    nvs_close(my_handle);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(mod_machine_nvs_set_u8_obj, mod_machine_nvs_set_u8);

//-------------------------------------------------------
STATIC mp_obj_t mod_machine_nvs_get_u8 (mp_obj_t _handle, mp_obj_t _key) {
    const char *handle = mp_obj_str_get_str(_handle);
    const char *key = mp_obj_str_get_str(_key);
    uint8_t value = 0;
	
	nvs_handle my_handle;
	esp_err_t res = nvs_open(handle, NVS_READWRITE, &my_handle);
	if (res != ESP_OK) {
		mp_raise_ValueError("NVS handle not found!");
		return mp_const_none;
	}

    if (ESP_ERR_NVS_NOT_FOUND == nvs_get_u8(my_handle, key, &value)) {
		nvs_close(my_handle);
        return mp_const_none;
    }
    nvs_close(my_handle);
    return mp_obj_new_int(value);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_machine_nvs_get_u8_obj, mod_machine_nvs_get_u8);

//------------------------------------------------------------------------
STATIC mp_obj_t mod_machine_nvs_set_u16 (mp_obj_t _handle, mp_obj_t _key, mp_obj_t _value) {
	const char *handle = mp_obj_str_get_str(_handle);
    const char *key = mp_obj_str_get_str(_key);
    uint16_t value = mp_obj_get_int_truncated(_value);
	
	nvs_handle my_handle;
	esp_err_t res = nvs_open(handle, NVS_READWRITE, &my_handle);
	if (res != ESP_OK) {
		mp_raise_ValueError("NVS handle not found!");
		return mp_const_none;
	}

    esp_err_t esp_err = nvs_set_u16(my_handle, key, value);
    if (ESP_OK == esp_err) {
        nvs_commit(my_handle);
    }
    else if (ESP_ERR_NVS_NOT_ENOUGH_SPACE == esp_err || ESP_ERR_NVS_PAGE_FULL == esp_err || ESP_ERR_NVS_NO_FREE_PAGES == esp_err) {
        mp_raise_msg(&mp_type_OSError, "No space available.");
    }
    else if (ESP_ERR_NVS_INVALID_NAME == esp_err || ESP_ERR_NVS_KEY_TOO_LONG == esp_err) {
        mp_raise_msg(&mp_type_OSError, "Key invalid or too long");
    }
    nvs_close(my_handle);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(mod_machine_nvs_set_u16_obj, mod_machine_nvs_set_u16);

//-------------------------------------------------------
STATIC mp_obj_t mod_machine_nvs_get_u16 (mp_obj_t _handle, mp_obj_t _key) {
    const char *handle = mp_obj_str_get_str(_handle);
    const char *key = mp_obj_str_get_str(_key);
    uint16_t value = 0;
	
	nvs_handle my_handle;
	esp_err_t res = nvs_open(handle, NVS_READWRITE, &my_handle);
	if (res != ESP_OK) {
		mp_raise_ValueError("NVS handle not found!");
		return mp_const_none;
	}

    if (ESP_ERR_NVS_NOT_FOUND == nvs_get_u16(my_handle, key, &value)) {
		nvs_close(my_handle);
        return mp_const_none;
    }
    nvs_close(my_handle);
    return mp_obj_new_int(value);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_machine_nvs_get_u16_obj, mod_machine_nvs_get_u16);

//------------------------------------------------------------------------
STATIC mp_obj_t mod_machine_nvs_set_int (mp_obj_t _handle, mp_obj_t _key, mp_obj_t _value) {
	const char *handle = mp_obj_str_get_str(_handle);
    const char *key = mp_obj_str_get_str(_key);
    uint32_t value = mp_obj_get_int_truncated(_value);
	
	nvs_handle my_handle;
	esp_err_t res = nvs_open(handle, NVS_READWRITE, &my_handle);
	if (res != ESP_OK) {
		mp_raise_ValueError("NVS handle not found!");
		return mp_const_none;
	}

    esp_err_t esp_err = nvs_set_i32(my_handle, key, value);
    if (ESP_OK == esp_err) {
        nvs_commit(my_handle);
    }
    else if (ESP_ERR_NVS_NOT_ENOUGH_SPACE == esp_err || ESP_ERR_NVS_PAGE_FULL == esp_err || ESP_ERR_NVS_NO_FREE_PAGES == esp_err) {
        mp_raise_msg(&mp_type_OSError, "No space available.");
    }
    else if (ESP_ERR_NVS_INVALID_NAME == esp_err || ESP_ERR_NVS_KEY_TOO_LONG == esp_err) {
        mp_raise_msg(&mp_type_OSError, "Key invalid or too long");
    }
    nvs_close(my_handle);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(mod_machine_nvs_set_int_obj, mod_machine_nvs_set_int);

//-------------------------------------------------------
STATIC mp_obj_t mod_machine_nvs_get_int (mp_obj_t _handle, mp_obj_t _key) {
    const char *handle = mp_obj_str_get_str(_handle);
    const char *key = mp_obj_str_get_str(_key);
    int32_t value = 0;
	
	nvs_handle my_handle;
	esp_err_t res = nvs_open(handle, NVS_READWRITE, &my_handle);
	if (res != ESP_OK) {
		mp_raise_ValueError("NVS handle not found!");
		return mp_const_none;
	}

    if (ESP_ERR_NVS_NOT_FOUND == nvs_get_i32(my_handle, key, &value)) {
		nvs_close(my_handle);
        return mp_const_none;
    }
    nvs_close(my_handle);
    return mp_obj_new_int(value);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_machine_nvs_get_int_obj, mod_machine_nvs_get_int);

//------------------------------------------------------------------------
STATIC mp_obj_t mod_machine_nvs_set_str (mp_obj_t _handle, mp_obj_t _key, mp_obj_t _value) {
    const char *handle = mp_obj_str_get_str(_handle);
    const char *key = mp_obj_str_get_str(_key);
    const char *value = mp_obj_str_get_str(_value);
	
	nvs_handle my_handle;
	esp_err_t res = nvs_open(handle, NVS_READWRITE, &my_handle);
	if (res != ESP_OK) {
		mp_raise_ValueError("NVS handle not found!");
		return mp_const_none;
	}

    esp_err_t esp_err = nvs_set_str(my_handle, key, value);
    if (ESP_OK == esp_err) {
        nvs_commit(my_handle);
	} else if (ESP_ERR_NVS_NOT_ENOUGH_SPACE == esp_err || ESP_ERR_NVS_PAGE_FULL == esp_err || ESP_ERR_NVS_NO_FREE_PAGES == esp_err) {
        mp_raise_msg(&mp_type_OSError, "No space available.");
    } else if (ESP_ERR_NVS_INVALID_NAME == esp_err || ESP_ERR_NVS_KEY_TOO_LONG == esp_err) {
        mp_raise_msg(&mp_type_OSError, "Key invalid or too long");
    }
    nvs_close(my_handle);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(mod_machine_nvs_set_str_obj, mod_machine_nvs_set_str);

//-------------------------------------------------------
STATIC mp_obj_t mod_machine_nvs_get_str (mp_obj_t _handle, mp_obj_t _key) {
    const char *handle = mp_obj_str_get_str(_handle);
    const char *key = mp_obj_str_get_str(_key);
    size_t len = 0;
    mp_obj_t strval = mp_const_none;
	
	nvs_handle my_handle;
	esp_err_t res = nvs_open(handle, NVS_READWRITE, &my_handle);
	if (res != ESP_OK) {
		mp_raise_ValueError("NVS handle not found!");
		return mp_const_none;
	}

    esp_err_t ret = nvs_get_str(my_handle, key, NULL, &len);
    if ((ret == ESP_OK ) && (len > 0)) {
        char *value = malloc(len);
        if (value) {
            esp_err_t ret = nvs_get_str(my_handle, key, value, &len);
            if ((ret == ESP_OK ) && (len > 0)) {
                strval = mp_obj_new_str(value, strlen(value));
                free(value);
            }
        }
    }
    nvs_close(my_handle);
    return strval;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_machine_nvs_get_str_obj, mod_machine_nvs_get_str);

//-----------------------------------------------------
STATIC mp_obj_t mod_machine_nvs_erase (mp_obj_t _handle, mp_obj_t _key) {
    const char *handle = mp_obj_str_get_str(_handle);
    const char *key = mp_obj_str_get_str(_key);
	
	nvs_handle my_handle;
	esp_err_t res = nvs_open(handle, NVS_READWRITE, &my_handle);
	if (res != ESP_OK) {
		mp_raise_ValueError("NVS handle not found!");
		return mp_const_none;
	}

    if (ESP_ERR_NVS_NOT_FOUND == nvs_erase_key(my_handle, key)) {
        mp_raise_ValueError("Key not found");
    }
    nvs_close(my_handle);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_machine_nvs_erase_obj, mod_machine_nvs_erase);

//------------------------------------------------
STATIC mp_obj_t mod_machine_nvs_erase_all (mp_obj_t _handle) {
    const char *handle = mp_obj_str_get_str(_handle);
	
	nvs_handle my_handle;
	esp_err_t res = nvs_open(handle, NVS_READWRITE, &my_handle);
	if (res != ESP_OK) {
		mp_raise_ValueError("NVS handle not found!");
		return mp_const_none;
	}
	
    if (ESP_OK != nvs_erase_all(my_handle)) {
        mp_raise_msg(&mp_type_OSError, "Operation failed.");
    }
    nvs_close(my_handle);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_machine_nvs_erase_all_obj, mod_machine_nvs_erase_all);


//=========================================================
STATIC const mp_rom_map_elem_t nvs_module_globals_table[] = {
	{ MP_OBJ_NEW_QSTR(MP_QSTR_nvs_set_u8),			MP_ROM_PTR(&mod_machine_nvs_set_u8_obj) },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_nvs_get_u8),			MP_ROM_PTR(&mod_machine_nvs_get_u8_obj) },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_nvs_set_u16),			MP_ROM_PTR(&mod_machine_nvs_set_u16_obj) },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_nvs_get_u16),			MP_ROM_PTR(&mod_machine_nvs_get_u16_obj) },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_nvs_setint),			MP_ROM_PTR(&mod_machine_nvs_set_int_obj) },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_nvs_getint),			MP_ROM_PTR(&mod_machine_nvs_get_int_obj) },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_nvs_setstr),			MP_ROM_PTR(&mod_machine_nvs_set_str_obj) },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_nvs_getstr),			MP_ROM_PTR(&mod_machine_nvs_get_str_obj) },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_nvs_erase),			MP_ROM_PTR(&mod_machine_nvs_erase_obj) },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_nvs_erase_all),		MP_ROM_PTR(&mod_machine_nvs_erase_all_obj) }
};
STATIC MP_DEFINE_CONST_DICT(nvs_module_globals, nvs_module_globals_table);

//===================================
const mp_obj_module_t nvs_module = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&nvs_module_globals,
};
MP_REGISTER_MODULE(MP_QSTR_nvs, nvs_module);
