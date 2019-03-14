#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#include "py/obj.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "extmod/modupygatt.h"

#if MICROPY_PY_BLUETOOTH

STATIC mp_obj_t GATTToolBackend(size_t n_args, const mp_obj_t *args)
{
  printf("GATTToolBackend init\r\n");
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(GATTToolBackend_obj, GATTToolBackend);

STATIC const mp_rom_map_elem_t mp_module_upygatt_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_upygatt) },

    { MP_ROM_QSTR(MP_QSTR_GATTToolBackend), MP_ROM_PTR(&GATTToolBackend_obj) },
};

STATIC MP_DEFINE_CONST_DICT(upygatt_module_globals, upygatt_module_globals_table);

const mp_obj_module_t mp_module_upygatt = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_upygatt_globals,
};

#endif //MICROPY_PY_BLUETOOTH
