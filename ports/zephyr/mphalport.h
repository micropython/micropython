#include <zephyr.h>
#include "shared/runtime/interrupt_char.h"

void mp_hal_init(void);
void mp_hal_wait_sem(struct k_sem *sem, uint32_t timeout_ms);

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
    mp_hal_wait_sem(NULL, delay);
}

static inline uint64_t mp_hal_time_ns(void) {
    // Not currently implemented.
    return 0;
}

#define mp_hal_delay_us_fast(us)   (mp_hal_delay_us(us))
