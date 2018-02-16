#include "xtimer.h"

static inline mp_uint_t mp_hal_ticks_ms(void) { return xtimer_now_usec() / 1000; }
static inline mp_uint_t mp_hal_ticks_us(void) { return xtimer_now_usec(); }
static inline void mp_hal_delay_ms(mp_uint_t ms) { return xtimer_usleep(ms * 1000); }
static inline void mp_hal_delay_us(mp_uint_t us) { return xtimer_usleep(us); }
static inline mp_uint_t mp_hal_ticks_cpu(void) { return xtimer_now_usec(); }

static inline void mp_hal_set_interrupt_char(char c) {}
