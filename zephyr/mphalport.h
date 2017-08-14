#include <zephyr.h>
#include "lib/utils/interrupt_char.h"

static inline mp_uint_t mp_hal_ticks_us(void) {
    return sys_tick_get() * sys_clock_us_per_tick;
}

static inline mp_uint_t mp_hal_ticks_ms(void) {
    int64_t us = sys_tick_get() * sys_clock_us_per_tick;
    mp_int_t ms = us / 1000;
    return ms;
}

static inline mp_uint_t mp_hal_ticks_cpu(void) {
    // ticks_cpu() is defined as using the highest-resolution timing source
    // in the system. This is usually a CPU clock, but doesn't have to be,
    // here we just use Zephyr hi-res timer.
    return sys_cycle_get_32();
}

static inline void mp_hal_delay_us(mp_uint_t delay) {
    k_busy_wait(delay);
}

static inline void mp_hal_delay_ms(mp_uint_t delay) {
    k_sleep(delay);
}
