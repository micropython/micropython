// QEMU-harness stub: the MicroPython surface halow_sched.c touches.
#ifndef HALOW_QEMUTEST_MPHAL_H
#define HALOW_QEMUTEST_MPHAL_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef uintptr_t mp_uint_t;
#define MP_WEAK

#define MICROPY_BEGIN_ATOMIC_SECTION() (0)
#define MICROPY_END_ATOMIC_SECTION(st) ((void)(st))

// Driven by the harness rather than a timer, so wait/timeout behaviour is
// deterministic instead of depending on how fast QEMU happens to run.
extern volatile uint32_t halow_test_ticks;
static inline mp_uint_t mp_hal_ticks_us(void) {
    extern volatile uint32_t halow_test_ticks;
    return halow_test_ticks * 1000u;
}

static inline mp_uint_t mp_hal_ticks_ms(void) {
    return halow_test_ticks;
}

// CMSIS bits halow_sched.c uses to detect exception context. Defined here so
// the harness does not need a CMSIS device header for a machine that is not the
// real target.
#define IPSR_ISR_Msk (0x1FFUL)
static inline uint32_t __get_IPSR(void) {
    uint32_t result;
    __asm volatile ("mrs %0, ipsr" : "=r" (result));
    return result;
}

#endif

// The scheduler sleeps here between turns.  On the target this is a WFE with a
// timeout; the harness has no tick source of its own, so stand in for it by
// advancing the clock the test drives.
#define MICROPY_INTERNAL_WFE(TIMEOUT_MS)                        \
    do {                                                        \
        extern volatile uint32_t halow_test_ticks;              \
        extern volatile uint32_t halow_test_event_waits;        \
        halow_test_ticks += (TIMEOUT_MS);                       \
        halow_test_event_waits++;                               \
    } while (0)
