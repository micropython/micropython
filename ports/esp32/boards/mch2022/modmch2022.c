#include "py/obj.h"

extern mp_obj_t wl_blockdev_make_new(void);

#define STATIC static

STATIC mp_obj_t mod_mch2022_wl_blockdev_make_new(void) {
    return wl_blockdev_make_new();
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_mch2022_wl_blockdev_make_new_obj, mod_mch2022_wl_blockdev_make_new);

STATIC const mp_rom_map_elem_t mch2022_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_mch2022) },
    { MP_ROM_QSTR(MP_QSTR_wl_blockdev_make_new), MP_ROM_PTR(&mod_mch2022_wl_blockdev_make_new_obj) },
};

STATIC MP_DEFINE_CONST_DICT(mch2022_module_globals, mch2022_module_globals_table);

const mp_obj_module_t mch2022_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mch2022_module_globals,
};
MP_REGISTER_MODULE(MP_QSTR_mch2022, mch2022_module);
