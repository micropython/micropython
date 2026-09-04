// Host-test stub. Only the handful of MicroPython facilities halow_osal.c
// actually touches, so the REAL allocator source can be compiled and tested
// natively instead of copied into a test where it would silently drift.
#ifndef HALOW_HOSTTEST_MPHAL_H
#define HALOW_HOSTTEST_MPHAL_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef uintptr_t mp_uint_t;

#define MP_WEAK

// The driver takes an atomic section around every heap operation because
// morselib allocates from PendSV. On the host there is no preemption, but the
// test counts nesting to prove the sections are balanced -- an unbalanced
// section on target leaves interrupts disabled forever.
extern int halow_test_atomic_depth;
extern int halow_test_atomic_max;

static inline mp_uint_t halow_test_begin_atomic(void) {
    halow_test_atomic_depth++;
    if (halow_test_atomic_depth > halow_test_atomic_max) {
        halow_test_atomic_max = halow_test_atomic_depth;
    }
    return 0;
}

static inline void halow_test_end_atomic(mp_uint_t state) {
    (void)state;
    halow_test_atomic_depth--;
}

#define MICROPY_BEGIN_ATOMIC_SECTION() halow_test_begin_atomic()
#define MICROPY_END_ATOMIC_SECTION(st) halow_test_end_atomic(st)

extern uint32_t halow_test_ticks_ms;
static inline mp_uint_t mp_hal_ticks_ms(void) {
    return halow_test_ticks_ms;
}

#endif
