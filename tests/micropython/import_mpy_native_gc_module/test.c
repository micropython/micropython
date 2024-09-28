// This test native module is used by import_mpy_native_gc.py.
// It has:
//  - A variable in the BSS, to check that the BSS is not reclaimed by the GC.
//  - An unused native function at the start so that subsequent native functions
//    don't start at the beginning of the native function data.  This tests that the
//    GC doesn't reclaim the native function data even when the only pointer to that
//    data is pointing inside the allocated memory.

#include "py/dynruntime.h"

uint32_t bss_variable;

static mp_obj_t unused(mp_obj_t x_obj) {
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(unused_obj, unused);

static mp_obj_t get(void) {
    return mp_obj_new_int(bss_variable);
}
static MP_DEFINE_CONST_FUN_OBJ_0(get_obj, get);

static mp_obj_t add1(mp_obj_t x_obj) {
    return mp_obj_new_int(mp_obj_get_int(x_obj) + 1);
}
static MP_DEFINE_CONST_FUN_OBJ_1(add1_obj, add1);

mp_obj_t mpy_init(mp_obj_fun_bc_t *self, size_t n_args, size_t n_kw, mp_obj_t *args) {
    MP_DYNRUNTIME_INIT_ENTRY

    bss_variable = 123456;

    mp_store_global(MP_QSTR_unused, MP_OBJ_FROM_PTR(&unused_obj));
    mp_store_global(MP_QSTR_get, MP_OBJ_FROM_PTR(&get_obj));
    mp_store_global(MP_QSTR_add1, MP_OBJ_FROM_PTR(&add1_obj));

    MP_DYNRUNTIME_INIT_EXIT
}
