//
// Created by Roy Hooper on 2018-05-14.
//

#include "reload.h"
#include "py/mpstate.h"

void mp_raise_reload_exception(void) {
    MP_STATE_VM(mp_pending_exception) = MP_OBJ_FROM_PTR(&MP_STATE_VM(mp_reload_exception));
#if MICROPY_ENABLE_SCHEDULER
    if (MP_STATE_VM(sched_state) == MP_SCHED_IDLE) {
        MP_STATE_VM(sched_state) = MP_SCHED_PENDING;
    }
#endif

}
