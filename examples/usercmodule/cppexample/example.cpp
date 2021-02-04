extern "C" {
#include <examplemodule.h>

// Here we implement the function using C++ code, but since it's
// declaration has to be compatible with C everything goes in extern "C" scope.
mp_obj_t cppfunc(mp_obj_t a_obj, mp_obj_t b_obj) {
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
