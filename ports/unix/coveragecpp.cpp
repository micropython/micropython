extern "C" {
#include "py/obj.h"
}

#if defined(MICROPY_UNIX_COVERAGE)

// Just to test building of C++ code.
static mp_obj_t extra_cpp_coverage_impl() {
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
