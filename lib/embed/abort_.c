#include <py/runtime.h>

NORETURN void abort_(void);

NORETURN void abort_(void) {
    mp_raise_msg(&mp_type_RuntimeError, "abort() called");
}
