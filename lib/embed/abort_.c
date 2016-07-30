#include <py/runtime.h>

NORETURN void abort_(void);

NORETURN void abort_(void) {
    nlr_raise(mp_obj_new_exception_msg(&mp_type_RuntimeError, "abort() called"));
}
