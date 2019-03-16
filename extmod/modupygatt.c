#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#include "py/obj.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "extmod/modupygatt.h"

#if MICROPY_PY_BLUETOOTH

STATIC const mp_obj_type_t gatt_tool_backend_type;

typedef struct _mp_obj_upygatt_t {
    mp_obj_base_t base;
} mp_obj_upygatt_t;

// instantiated Bluetooth object
STATIC const mp_obj_upygatt_t upygatt_obj = {
    { &gatt_tool_backend_type },
    { &gatt_tool_receiver_type },
};

STATIC mp_obj_t upygatt_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    return MP_OBJ_FROM_PTR(&upygatt_obj);
}

STATIC mp_obj_t gatt_tool_backend_start(size_t n_args, const mp_obj_t *args) {
  printf("start %d\r\n", n_args);
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(gatt_tool_backend_start_obj, 1, 2, gatt_tool_backend_start);

STATIC mp_obj_t gatt_tool_backend_scan(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
  enum { ARG_timeout, ARG_run_as_root };

  const mp_arg_t allowed_args[] = {
      { MP_QSTR_timeout, MP_ARG_INT, { .u_int = 10 } },
      { MP_QSTR_run_as_root, MP_ARG_BOOL, { .u_bool = true } },
  };

  mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
  mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

  //printf("scan: %d timeout=%d, run_as_root=%s\r\n", n_args, args[ARG_timeout].u_int, (args[ARG_run_as_root].u_bool ? "True" : "False"));
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(gatt_tool_backend_scan_obj, 0, gatt_tool_backend_scan);

STATIC mp_obj_t gatt_tool_backend(void)
{
  printf("GATTToolBackend init\r\n");
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(gatt_tool_backend_obj, gatt_tool_backend);

STATIC mp_obj_t gatt_tool_receiver(void)
{
  printf("GATTToolReceiver init\r\n");
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(gatt_tool_backend_obj, gatt_tool_backend);

STATIC const mp_rom_map_elem_t gatt_tool_backend_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_sendline),                              MP_ROM_PTR(&gatt_tool_backend_sendline_obj) },
    { MP_ROM_QSTR(MP_QSTR_supports_unbonded,                      MP_ROM_PTR(&gatt_tool_backend_supports_unbonded_obj) },
    { MP_ROM_QSTR(MP_QSTR_start),                                 MP_ROM_PTR(&gatt_tool_backend_start_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop),                                  MP_ROM_PTR(&gatt_tool_backend_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_scan),                                  MP_ROM_PTR(&gatt_tool_backend_scan_obj) },
    { MP_ROM_QSTR(MP_QSTR_connect),                               MP_ROM_PTR(&gatt_tool_backend_connect_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear_bond),                            MP_ROM_PTR(&gatt_tool_backend_clear_bond_obj) },
    { MP_ROM_QSTR(MP_QSTR__disconnect),                           MP_ROM_PTR(&gatt_tool_backend__disconnect_obj) },
    //@at_most_one_device
    { MP_ROM_QSTR(MP_QSTR_disconnect),                            MP_ROM_PTR(&gatt_tool_backend_disconnect_obj) },
    //@at_most_one_device
    { MP_ROM_QSTR(MP_QSTR_bond),                                  MP_ROM_PTR(&gatt_tool_backend_bond_obj) },
    { MP_ROM_QSTR(MP_QSTR__save_charecteristic_callback),         MP_ROM_PTR(&gatt_tool_backend__save_charecteristic_callback_obj) },
    //@at_most_one_device
    { MP_ROM_QSTR(MP_QSTR_discover_characteristics),              MP_ROM_PTR(&gatt_tool_backend_discover_characteristics_obj) },
    { MP_ROM_QSTR(MP_QSTR__handle_notification_string),           MP_ROM_PTR(&gatt_tool_backend__handle_notification_string_obj) },
    //@at_most_one_device
    { MP_ROM_QSTR(MP_QSTR_char_write_handle),                     MP_ROM_PTR(&gatt_tool_backend_char_write_handle_obj) },
    //@at_most_one_device
    { MP_ROM_QSTR(MP_QSTR_char_read),                             MP_ROM_PTR(&gatt_tool_backend_char_read_obj) },
    //@at_most_one_device
    { MP_ROM_QSTR(MP_QSTR_char_read_handle),                      MP_ROM_PTR(&gatt_tool_backend_char_read_handle_obj) },
    { MP_ROM_QSTR(MP_QSTR_char_reset),                            MP_ROM_PTR(&gatt_tool_backend_char_reset_obj) },
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
    { MP_ROM_QSTR(MP_QSTR_GATTToolReceiver, MP_ROM_PTR(&gatt_tool_receiver_type) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_upygatt_globals, mp_module_upygatt_globals_table);

const mp_obj_module_t mp_module_upygatt = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_upygatt_globals,
};

#endif //MICROPY_PY_BLUETOOTH
