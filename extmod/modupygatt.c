#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#include "py/obj.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "extmod/modupygatt.h"

#if MICROPY_PY_BLUETOOTH

STATIC mp_obj_t gatt_tool_backend(void)
{
  printf("GATTToolBackend init\r\n");
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(gatt_tool_backend_obj, gatt_tool_backend);

STATIC const mp_rom_map_elem_t mp_module_upygatt_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_upygatt) },

    { MP_ROM_QSTR(MP_QSTR_GATTToolBackend), MP_ROM_PTR(&gatt_tool_backend_obj) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_upygatt_globals, mp_module_upygatt_globals_table);

const mp_obj_module_t mp_module_upygatt = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_upygatt_globals,
};

#endif //MICROPY_PY_BLUETOOTH
