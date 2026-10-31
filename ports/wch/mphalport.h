#include "ch32v30x.h"
#include "pin.h"

#define IS_GPIO_MODE(MODE) (((MODE) == GPIO_Mode_IN_FLOATING) || \
    ((MODE) == GPIO_Mode_Out_PP) || \
    ((MODE) == GPIO_Mode_Out_OD) || \
    ((MODE) == GPIO_Mode_AF_PP) || \
    ((MODE) == GPIO_Mode_AF_OD) || \
    ((MODE) == GPIO_Mode_AIN) || \
    ((MODE) == GPIO_Mode_IPU) || \
    ((MODE) == GPIO_Mode_IPD))

static inline mp_uint_t mp_hal_ticks_cpu(void) {
    // Not currently implemented.
    return 0;
}

static inline mp_uint_t mp_hal_ticks_ms(void) {
    // Not currently implemented.
    return 0;
}

static inline mp_uint_t mp_hal_ticks_us(void) {
    // Not currently implemented.
    return 0;
}

static inline uint64_t mp_hal_time_ns(void) {
    // Not currently implemented.
    return 0;
}

static inline void mp_hal_delay_ms(mp_uint_t ms) {
    Delay_Ms(ms);
}

static inline void mp_hal_delay_us(mp_uint_t us) {
    Delay_Us(us);
}

static inline void mp_hal_set_interrupt_char(char c) {
    // Not currently implemented.
}
