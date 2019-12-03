#include <linux/ktime.h>

static inline mp_uint_t mp_hal_ticks_ms(void) {
    return ktime_to_ms(ktime_get_boottime());
}

void mp_hal_set_interrupt_char(int c);
