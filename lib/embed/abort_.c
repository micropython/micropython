#include <py/runtime.h>

#include "supervisor/shared/translate.h"

NORETURN void abort_(void);

NORETURN void abort_(void) {
    mp_raise_msg(&mp_type_RuntimeError, translate("abort() called"));
}
