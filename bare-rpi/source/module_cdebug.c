#include "py/mpconfig.h"
#include "py/nlr.h"
#include "py/misc.h"
#include "py/qstr.h"
#include "py/obj.h"
#include "py/runtime.h"

extern unsigned int getSP();
extern unsigned int heap_end;

STATIC mp_obj_t cdebug_bsod(void) {
    printf (" !-- Stack: 0x%08x\t Heap: 0x%08x --!\n", getSP(), heap_end );
    /*uint32_t* ptr = 0x8000;
    printf ("MEMAP\n");
    for (; ptr < 0x9000; ptr+=4)
    {
        printf ("A: 0x%08x \t - \t 0x%08x\n", (unsigned int) ptr, *ptr);
    }   */
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(cdebug_bsod_obj, cdebug_bsod);

STATIC const mp_map_elem_t cdebug_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_bsod) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_bsod), (mp_obj_t)&cdebug_bsod_obj },

};

STATIC MP_DEFINE_CONST_DICT(mp_module_cdebug_globals, cdebug_globals_table);

const mp_obj_module_t mp_module_cdebug = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_cdebug_globals,
};

