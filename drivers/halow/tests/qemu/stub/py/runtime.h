#ifndef HALOW_QEMUTEST_RUNTIME_H
#define HALOW_QEMUTEST_RUNTIME_H
#include "py/mphal.h"
// The scheduler services pending callbacks and then sleeps when idling outside
// task context. The harness counts the calls instead, so the yield path is
// still exercised and observable.
typedef enum {
    MP_HANDLE_PENDING_CALLBACKS_AND_CLEAR_EXCEPTIONS = 0,
    MP_HANDLE_PENDING_CALLBACKS_AND_EXCEPTIONS = 1,
    MP_HANDLE_PENDING_CALLBACKS_ONLY = 2,
} mp_handle_pending_behaviour_t;

extern volatile uint32_t halow_test_event_waits;
static inline void mp_handle_pending(mp_handle_pending_behaviour_t behaviour) {
    (void)behaviour;
    halow_test_event_waits++;
    halow_test_ticks++;
}
#endif
