extern "C" {
// Include the complete public API to verify everything compiles as C++.
#include <py/gc.h>
#include <py/obj.h>
#include <py/objarray.h>
#include <py/objexcept.h>
#include <py/objfun.h>
#include <py/objgenerator.h>
#include <py/objint.h>
#include <py/objlist.h>
#include <py/objmodule.h>
#include <py/objnamedtuple.h>
#include <py/objstr.h>
#include <py/objstringio.h>
#include <py/objtuple.h>
#include <py/objtype.h>
#include <py/runtime.h>
}

// Invoke all (except one, see below) public API macros which initialize structs to make sure
// they are C++-compatible, meaning they explicitly initialize all struct members.
mp_obj_t f0();
MP_DEFINE_CONST_FUN_OBJ_0(f0_obj, f0);
mp_obj_t f1(mp_obj_t);
MP_DEFINE_CONST_FUN_OBJ_1(f1_obj, f1);
mp_obj_t f2(mp_obj_t, mp_obj_t);
MP_DEFINE_CONST_FUN_OBJ_2(f2_obj, f2);
mp_obj_t f3(mp_obj_t, mp_obj_t, mp_obj_t);
MP_DEFINE_CONST_FUN_OBJ_3(f3_obj, f3);
mp_obj_t fvar(size_t, const mp_obj_t *);
MP_DEFINE_CONST_FUN_OBJ_VAR(fvar_obj, 1, fvar);
mp_obj_t fvarbetween(size_t, const mp_obj_t *);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(fvarbetween_obj, 1, 2, fvarbetween);
mp_obj_t fkw(size_t, const mp_obj_t *, mp_map_t *);
MP_DEFINE_CONST_FUN_OBJ_KW(fkw_obj, 1, fkw);

static const mp_rom_map_elem_t table[] = {
    { MP_ROM_QSTR(MP_QSTR_f0), MP_ROM_PTR(&f0_obj) },
};
MP_DEFINE_CONST_MAP(map, table);
MP_DEFINE_CONST_DICT(dict, table);

static const qstr attrtuple_fields[] = {
    MP_QSTR_f0,
};
MP_DEFINE_ATTRTUPLE(attrtuple, attrtuple_fields, 1, MP_ROM_PTR(&f0_obj));

void nlr_cb(void *);
void nlr_cb(void *){
}

// The MP_DEFINE_CONST_OBJ_TYPE macro is not C++-compatible because each of the
// MP_DEFINE_CONST_OBJ_TYPE_NARGS_X macros only initializes some of _mp_obj_type_t's
// .slot_index_xxx members but that cannot be fixed to be done in a deterministic way.


#if defined(MICROPY_UNIX_COVERAGE)

// Just to test building of C++ code.
static mp_obj_t extra_cpp_coverage_impl() {
    MP_DEFINE_NLR_JUMP_CALLBACK_FUNCTION_1(ctx, nlr_cb, (void *) nlr_cb);

    // To avoid 'error: unused variable [-Werror,-Wunused-const-variable]'.
    (void) ctx;
    (void) f0_obj;
    (void) f1_obj;
    (void) f2_obj;
    (void) f3_obj;
    (void) fvar_obj;
    (void) fvarbetween_obj;
    (void) fkw_obj;
    (void) map;
    (void) dict;
    (void) attrtuple;
    return mp_const_none;
}

extern "C" {
mp_obj_t extra_cpp_coverage(void);
mp_obj_t extra_cpp_coverage(void) {
    return extra_cpp_coverage_impl();
}

// This is extern to avoid name mangling.
extern const mp_obj_fun_builtin_fixed_t extra_cpp_coverage_obj = {{&mp_type_fun_builtin_0}, {extra_cpp_coverage}};

}

#endif
