#include <linux/ktime.h>
#include <linux/delay.h>


static inline mp_uint_t mp_hal_ticks_ms(void) {
    return ktime_to_ms(ktime_get_boottime());
}

static inline void mp_hal_delay_ms(mp_uint_t ms) {
    msleep(ms);
}

static inline void mp_hal_delay_us(mp_uint_t us) {
    // not currently needed because we don't expose mp_utime_sleep_us_obj.
    // we don't compile with -ffunction-section however, so this symbol
    // is still required.
}

static inline mp_uint_t mp_hal_ticks_us(void) {
    // same comment as in mp_hal_delay_us
    return 0;
}

static inline mp_uint_t mp_hal_ticks_cpu(void) {
    // same comment as in mp_hal_delay_us
    return 0;
}

static inline uint64_t mp_hal_time_ns(void) {
    return ktime_get_real_ns();
}

void mp_hal_set_interrupt_char(int c);
