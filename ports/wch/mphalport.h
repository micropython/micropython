#include "ch32v30x.h"

static inline mp_uint_t mp_hal_ticks_ms(void) {
    return SystemCoreClock / 8000;
}
static inline void mp_hal_set_interrupt_char(char c) {
}
