#include "py/obj.h"
#include "py/runtime.h"
#include "DemuxKeyMatrixBindings.h"

STATIC const mp_rom_map_elem_t cardputer_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_cardputer) },
    { MP_ROM_QSTR(MP_QSTR_DemuxKeyMatrix), MP_ROM_PTR(&cardputer_demuxkeymatrix_type) }
};

STATIC MP_DEFINE_CONST_DICT(cardputer_module_globals, cardputer_module_globals_table);

const mp_obj_module_t cardputer_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&cardputer_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_cardputer, cardputer_user_cmodule);
