#include <errno.h>

#include "mpconfig.h"
#include "nlr.h"
#include "misc.h"
#include "qstr.h"
#include "obj.h"
#include "usb.h"
#include "mphal.h"

// this table converts from HAL_StatusTypeDef to POSIX errno
const byte mp_hal_status_to_errno_table[4] = {
    [HAL_OK] = 0,
    [HAL_ERROR] = EIO,
    [HAL_BUSY] = EBUSY,
    [HAL_TIMEOUT] = ETIMEDOUT,
};

NORETURN void mp_hal_raise(HAL_StatusTypeDef status) {
    nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(mp_hal_status_to_errno_table[status])));
}

void mp_hal_set_interrupt_char(int c) {
    usb_vcp_set_interrupt_char(c);
}
