extern "C" {
#include <examplemodule.h>
// Include the public API. This is only here to verify everything compiles as C++,
// an actual application should only include what is needed.
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
