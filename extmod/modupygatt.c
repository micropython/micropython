#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <stdbool.h>

#include "py/obj.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "extmod/modupygatt.h"
#include "esp_bt_defs.h"

#if MICROPY_PY_BLUETOOTH

STATIC const mp_obj_type_t gatt_tool_backend_type;

typedef struct _mp_upygatt_obj_t {
    mp_obj_base_t base;
} mp_upygatt_obj_t;

// instantiated Bluetooth object
STATIC const mp_upygatt_obj_t upygatt_obj = {
    { &gatt_tool_backend_type },
};

// Easier (hopefully tail-called) error handling.
STATIC mp_obj_t pygatt_handle_errno(int errno_) {
    if (errno_ != 0) {
        mp_raise_OSError(errno_);
    }
    return mp_const_none;
}

// Parse string UUIDs, which are probably 128-bit UUIDs.
void mp_bt_parse_uuid_str(mp_obj_t obj, uint8_t *uuid) {
    GET_STR_DATA_LEN(obj, str_data, str_len);
    int uuid_i = 32;
    for (int i = 0; i < str_len; i++) {
        char c = str_data[i];
        if (c == '-') {
            continue;
        }
        if (c >= '0' && c <= '9') {
            c = c - '0';
        } else if (c >= 'a' && c <= 'f') {
            c = c - 'a' + 10;
        } else if (c >= 'A' && c <= 'F') {
            c = c - 'A' + 10;
        } else {
            mp_raise_ValueError("unknown char in UUID");
        }
        uuid_i--;
        if (uuid_i < 0) {
            mp_raise_ValueError("UUID too long");
        }
        if (uuid_i % 2 == 0) {
            // lower nibble
            uuid[uuid_i/2] |= c;
        } else {
            // upper nibble
            uuid[uuid_i/2] = c << 4;
        }
    }
    if (uuid_i > 0) {
        mp_raise_ValueError("UUID too short");
    }
}

STATIC mp_obj_t upygatt_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    return MP_OBJ_FROM_PTR(&upygatt_obj);
}

STATIC mp_obj_t gatt_tool_backend_stop(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
  enum { ARG_initialization_timeout };

  const mp_arg_t allowed_args[] = {
      { MP_QSTR_initialization_timeout, MP_ARG_INT, { .u_int = 3 } },
  };

  mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
  mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

  int errno_ = mp_bt_disable();
  if (errno_ != 0) {
      mp_raise_OSError(errno_);
  }

  printf("Bluetooth stop\r\n");
  return mp_obj_new_bool(mp_bt_is_enabled());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(gatt_tool_backend_stop_obj, 0, gatt_tool_backend_stop);

STATIC mp_obj_t gatt_tool_backend_start(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
  enum { ARG_reset_on_start, ARG_initialization_timeout };

  const mp_arg_t allowed_args[] = {
      { MP_QSTR_reset_on_start, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = mp_const_true } },
      { MP_QSTR_initialization_timeout, MP_ARG_INT, { .u_int = 3 } },
  };

  mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
  mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

  int errno_ = mp_bt_enable();
  if (errno_ != 0) {
      mp_raise_OSError(errno_);
  }

  printf("Bluetooth start %d\r\n", n_args);
  return mp_obj_new_bool(mp_bt_is_enabled());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(gatt_tool_backend_start_obj, 0, gatt_tool_backend_start);

STATIC mp_obj_t gatt_tool_backend_scan(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
  enum { ARG_timeout, ARG_run_as_root };

  const mp_arg_t allowed_args[] = {
      { MP_QSTR_timeout, MP_ARG_INT, { .u_int = 10 } },
      { MP_QSTR_run_as_root, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = mp_const_false } },
  };

  mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
  mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

  mp_int_t timeout = args[ARG_timeout].u_int;

  printf("scan: %d timeout=" UINT_FMT " - %d, run_as_root=%s\r\n", n_args, timeout, timeout, mp_obj_is_true(args[ARG_run_as_root].u_obj) ? "True" : "False");
  int errno_ = mp_bt_scan();
  if (errno_ != 0) {
      mp_raise_OSError(errno_);
  }
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(gatt_tool_backend_scan_obj, 0, gatt_tool_backend_scan);

STATIC mp_obj_t gatt_tool_backend_connect(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
  enum { ARG_device, ARG_address_type };

  const mp_arg_t allowed_args[] = {
      { MP_QSTR_device, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_obj = mp_const_none } },
      { MP_QSTR_address_type, MP_ARG_INT, {.u_int = 2 } },
  };

  mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
  mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

  char *addr_str = (char *)mp_obj_str_get_str(args[ARG_device].u_obj);
  esp_bd_addr_t addr;

  for (int i = 0, j = 0; i < strlen(addr_str); i += 3) {
    if (strncmp(&addr_str[i], ":", 1) != 0) {
      addr[j++] = (((addr_str[i]%32+9)%25*16+(addr_str[i+1]%32+9)%25) & 0xFF);
    }
    else i--;
  }

  int errno_ = mp_bt_connect(addr);
  if (errno_ != 0) {
      mp_raise_OSError(errno_);
  }

  return MP_OBJ_NEW_SMALL_INT(errno_);//mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(gatt_tool_backend_connect_obj, 1, gatt_tool_backend_connect);

STATIC mp_obj_t gatt_tool_backend_disconnect(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
  enum { ARG_device, ARG_address_type };

  const mp_arg_t allowed_args[] = {
      { MP_QSTR_device, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_obj = mp_const_none } },
      { MP_QSTR_address_type, MP_ARG_INT, {.u_int = 2 } },
  };

  mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
  mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

  char *addr_str = (char *)mp_obj_str_get_str(args[ARG_device].u_obj);
  esp_bd_addr_t addr;

  for (int i = 0, j = 0; i < strlen(addr_str); i += 3) {
    if (strncmp(&addr_str[i], ":", 1) != 0) {
      addr[j++] = (((addr_str[i]%32+9)%25*16+(addr_str[i+1]%32+9)%25) & 0xFF);
    }
    else i--;
  }

  int errno_ = mp_bt_disconnect(addr);
  if (errno_ != 0) {
      mp_raise_OSError(errno_);
  }

  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(gatt_tool_backend_disconnect_obj, 1, gatt_tool_backend_disconnect);

STATIC mp_obj_t gatt_tool_backend_discover_characteristics(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
  enum { ARG_timeout };

  const mp_arg_t allowed_args[] = {
      { MP_QSTR_timeout, MP_ARG_INT, {.u_int = 5 } },
  };

  mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
  mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);



  int errno_ = mp_bt_discover_characteristics();
  if (errno_ != 0) {
      mp_raise_OSError(errno_);
  }

  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(gatt_tool_backend_discover_characteristics_obj, 0, gatt_tool_backend_discover_characteristics);

STATIC mp_obj_t gatt_tool_backend_char_write_handle(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
  enum { ARG_handle, ARG_value, ARG_wait_for_response, ARG_timeout };

  const mp_arg_t allowed_args[] = {
      { MP_QSTR_handle, MP_ARG_REQUIRED | MP_ARG_INT | MP_ARG_KW_ONLY, { .u_int = 0x000e } },
      { MP_QSTR_value, MP_ARG_REQUIRED | MP_ARG_OBJ | MP_ARG_KW_ONLY, { .u_obj = mp_const_none } },
      { MP_QSTR_wait_for_response, MP_ARG_OBJ | MP_ARG_KW_ONLY, { .u_obj = mp_const_false } },
      { MP_QSTR_timeout, MP_ARG_INT, {.u_int = 1 } },
  };

  mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
  mp_arg_parse_all(n_args-1, pos_args+1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

  uint16_t handle = args[ARG_handle].u_int;
  bool wait_for_response = args[ARG_wait_for_response].u_obj;

  mp_buffer_info_t buffer;
  mp_get_buffer_raise(args[ARG_value].u_obj, &buffer, MP_BUFFER_READ);
  uint8_t* value = (uint8_t*)malloc(buffer.len);
  memcpy(value, buffer.buf, buffer.len);

  int errno_ = mp_bt_char_write_handle(handle, value, buffer.len, wait_for_response);
  if (errno_ != 0) {
      mp_raise_OSError(errno_);
  }
  free(value);
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(gatt_tool_backend_char_write_handle_obj, 0, gatt_tool_backend_char_write_handle);

STATIC mp_obj_t gatt_tool_backend_char_read(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
  enum { ARG_uuid, ARG_value_handle };

  const mp_arg_t allowed_args[] = {
      { MP_QSTR_uuid, MP_ARG_REQUIRED | MP_ARG_OBJ | MP_ARG_KW_ONLY, { .u_obj = mp_const_none } },
      { MP_QSTR_value_handle, MP_ARG_REQUIRED | MP_ARG_OBJ | MP_ARG_KW_ONLY, { .u_int = 0x000b } },
  };

  mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
  mp_arg_parse_all(n_args-1, pos_args+1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

  uint16_t value_handle = (uint16_t)(args[ARG_value_handle].u_int);
  printf("[upygatt] char_read handle: 0x%04x\r\n", value_handle);
  uint8_t data[MP_BT_MAX_ATTR_SIZE];
  size_t value_len = MP_BT_MAX_ATTR_SIZE;
  int errno_ = mp_bt_char_read(value_handle, data, &value_len);
  if (errno_ != 0) {
      mp_raise_OSError(errno_);
  }
  return mp_obj_new_bytes(data, value_len);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(gatt_tool_backend_char_read_obj, 1, gatt_tool_backend_char_read);

STATIC mp_obj_t gatt_tool_backend(void) {
  printf("GATTToolBackend init\r\n");
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(gatt_tool_backend_obj, gatt_tool_backend);

STATIC const mp_rom_map_elem_t gatt_tool_backend_locals_dict_table[] = {
    // { MP_ROM_QSTR(MP_QSTR_sendline),                              MP_ROM_PTR(&gatt_tool_backend_sendline_obj) },
    // { MP_ROM_QSTR(MP_QSTR_supports_unbonded,                      MP_ROM_PTR(&gatt_tool_backend_supports_unbonded_obj) },
    { MP_ROM_QSTR(MP_QSTR_start),                                 MP_ROM_PTR(&gatt_tool_backend_start_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop),                                  MP_ROM_PTR(&gatt_tool_backend_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_scan),                                  MP_ROM_PTR(&gatt_tool_backend_scan_obj) },
    { MP_ROM_QSTR(MP_QSTR_connect),                               MP_ROM_PTR(&gatt_tool_backend_connect_obj) },
    // { MP_ROM_QSTR(MP_QSTR_clear_bond),                            MP_ROM_PTR(&gatt_tool_backend_clear_bond_obj) },
    // { MP_ROM_QSTR(MP_QSTR__disconnect),                           MP_ROM_PTR(&gatt_tool_backend__disconnect_obj) },
    // //@at_most_one_device
    { MP_ROM_QSTR(MP_QSTR_disconnect),                            MP_ROM_PTR(&gatt_tool_backend_disconnect_obj) },
    // //@at_most_one_device
    // { MP_ROM_QSTR(MP_QSTR_bond),                                  MP_ROM_PTR(&gatt_tool_backend_bond_obj) },
    // { MP_ROM_QSTR(MP_QSTR__save_charecteristic_callback),         MP_ROM_PTR(&gatt_tool_backend__save_charecteristic_callback_obj) },
    // //@at_most_one_device
    { MP_ROM_QSTR(MP_QSTR_discover_characteristics),              MP_ROM_PTR(&gatt_tool_backend_discover_characteristics_obj) },
    // { MP_ROM_QSTR(MP_QSTR__handle_notification_string),           MP_ROM_PTR(&gatt_tool_backend__handle_notification_string_obj) },
    // //@at_most_one_device
    { MP_ROM_QSTR(MP_QSTR_char_write_handle),                     MP_ROM_PTR(&gatt_tool_backend_char_write_handle_obj) },
    // //@at_most_one_device
    { MP_ROM_QSTR(MP_QSTR_char_read),                             MP_ROM_PTR(&gatt_tool_backend_char_read_obj) },
    // //@at_most_one_device
    // { MP_ROM_QSTR(MP_QSTR_char_read_handle),                      MP_ROM_PTR(&gatt_tool_backend_char_read_handle_obj) },
    // { MP_ROM_QSTR(MP_QSTR_char_reset),                            MP_ROM_PTR(&gatt_tool_backend_char_reset_obj) },
};
STATIC MP_DEFINE_CONST_DICT(gatt_tool_backend_locals_dict, gatt_tool_backend_locals_dict_table);

STATIC const mp_obj_type_t gatt_tool_backend_type = {
    { &mp_type_type },
    .name = MP_QSTR_upygatt,
    .make_new = upygatt_make_new,
    .locals_dict = (void*)&gatt_tool_backend_locals_dict,
};

STATIC const mp_rom_map_elem_t mp_module_upygatt_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_upygatt) },

    { MP_ROM_QSTR(MP_QSTR_GATTToolBackend), MP_ROM_PTR(&gatt_tool_backend_type) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_upygatt_globals, mp_module_upygatt_globals_table);

const mp_obj_module_t mp_module_upygatt = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_upygatt_globals,
};

#endif //MICROPY_PY_BLUETOOTH
