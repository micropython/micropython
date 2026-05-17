extern "C" {
#include <examplemodule.h>
#include <py/objstr.h>

// Here we implement the function using C++ code, but since it's
// declaration has to be compatible with C everything goes in extern "C" scope.
mp_obj_t cppfunc(mp_obj_t a_obj, mp_obj_t b_obj) {
    // The following no-ops are just here to verify the static assertions used in
    // the public API all compile with C++.
    MP_STATIC_ASSERT_STR_ARRAY_COMPATIBLE;
    if (mp_obj_is_type(a_obj, &mp_type_BaseException)) {
    }

    // Prove we have (at least) C++11 features.
    const auto a = mp_obj_get_int(a_obj);
    const auto b = mp_obj_get_int(b_obj);
    const auto sum = [&]() {
        return mp_obj_new_int(a + b);
    } ();
    // Prove we're being scanned for QSTRs.
    mp_obj_t tup[] = {sum, MP_ROM_QSTR(MP_QSTR_hellocpp)};
    return mp_obj_new_tuple(2, tup);
}
}
