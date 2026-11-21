/* This example demonstrates calling libm functions with runtime (non-constant)
   arguments when LINK_RUNTIME=1 is set.  It is specifically relevant for RISC-V
   (rv32imc/rv64imc) with picolibc, where constants like sin(1.0f) are folded by
   the compiler and never reach the linker, but runtime values go through the
   picolibc libm and exercise .srodata sections and absolute LO12 relocations
   that were previously unsupported by mpy_ld.

   >>> import features5
   >>> import math
   >>> abs(features5.libm_combo(1.0) - (math.sin(1.0) + math.cos(1.0))) < 1e-5
   True
*/

#include "py/dynruntime.h"
#include <math.h>

// Returns sinf(x) + cosf(x) + atan2f(x, 0.5) + sqrtf(|x|)
// All arguments are runtime values so the compiler cannot fold the calls away.
static mp_obj_t libm_combo(mp_obj_t x_obj) {
    float x = mp_obj_get_float_to_f(x_obj);
    float r = sinf(x) + cosf(x) + atan2f(x, 0.5f) + sqrtf(x < 0 ? -x : x);
    return mp_obj_new_float_from_f(r);
}
static MP_DEFINE_CONST_FUN_OBJ_1(libm_combo_obj, libm_combo);

mp_obj_t mpy_init(mp_obj_fun_bc_t *self, size_t n_args, size_t n_kw, mp_obj_t *args) {
    MP_DYNRUNTIME_INIT_ENTRY
    mp_store_global(MP_QSTR_libm_combo, MP_OBJ_FROM_PTR(&libm_combo_obj));
    MP_DYNRUNTIME_INIT_EXIT
}
