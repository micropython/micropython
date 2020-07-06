#include <zephyr.h>
#include "lib/utils/interrupt_char.h"

static inline mp_uint_t mp_hal_ticks_us(void) {
    return k_cyc_to_ns_floor64(k_cycle_get_32()) / 1000;
}

static inline mp_uint_t mp_hal_ticks_ms(void) {
    return k_uptime_get();
}

static inline mp_uint_t mp_hal_ticks_cpu(void) {
    // ticks_cpu() is defined as using the highest-resolution timing source
    // in the system. This is usually a CPU clock, but doesn't have to be,
    // here we just use Zephyr hi-res timer.
    return k_cycle_get_32();
}

static inline void mp_hal_delay_us(mp_uint_t delay) {
    k_busy_wait(delay);
}

static inline void mp_hal_delay_ms(mp_uint_t delay) {
    k_msleep(delay);
}

#define mp_hal_delay_us_fast(us)   (mp_hal_delay_us(us))
#define mp_hal_pin_od_low(p)       (mp_raise_NotImplementedError("mp_hal_pin_od_low"))
#define mp_hal_pin_od_high(p)      (mp_raise_NotImplementedError("mp_hal_pin_od_high"))
#define mp_hal_pin_open_drain(p)   (mp_raise_NotImplementedError("mp_hal_pin_open_drain"))
