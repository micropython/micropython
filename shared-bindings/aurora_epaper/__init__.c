#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/aurora_epaper/aurora_framebuffer.h"

STATIC const mp_rom_map_elem_t aurora_epaper_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_aurora_epaper) },
    { MP_ROM_QSTR(MP_QSTR_AuroraMemoryFramebuffer), MP_ROM_PTR(&aurora_framebuffer_type) },
};

static MP_DEFINE_CONST_DICT(aurora_epaper_globals, aurora_epaper_module_globals_table);

const mp_obj_module_t aurora_epaper_module = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&aurora_epaper_globals,
};

MP_REGISTER_MODULE(MP_QSTR_aurora_epaper, aurora_epaper_module);
