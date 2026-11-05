// mphalport.h - HAL port interface for GD32
#ifndef MICROPY_INCLUDED_GD32_MPHALPORT_H
#define MICROPY_INCLUDED_GD32_MPHALPORT_H

#include <stdint.h>
#include "py/obj.h"

void mp_hal_set_interrupt_char(int c);
mp_uint_t mp_hal_ticks_ms(void);
mp_uint_t mp_hal_ticks_us(void);
mp_uint_t mp_hal_ticks_cpu(void);
void mp_hal_delay_ms(mp_uint_t ms);
void mp_hal_delay_us(mp_uint_t us);

int mp_hal_stdin_rx_chr(void);
mp_uint_t mp_hal_stdout_tx_strn(const char *str, size_t len);
void mp_hal_stdout_tx_strn_cooked(const char *str, size_t len);

static inline void mp_hal_pin_high(const mp_obj_t pin) {
    extern void mp_hal_pin_write_impl(mp_obj_t, int);
    mp_hal_pin_write_impl(pin, 1);
}
static inline void mp_hal_pin_low(const mp_obj_t pin) {
    extern void mp_hal_pin_write_impl(mp_obj_t, int);
    mp_hal_pin_write_impl(pin, 0);
}


// ---- soft timer / PendSV critical section ----
// Serialise access to the soft-timer pair-heap against the SysTick
// handler by masking interrupts above the SysTick priority.
#if MICROPY_PY_MACHINE_TIMER
#include "gd32f4xx.h"
#define MICROPY_PY_PENDSV_ENTER    uint32_t atomic_state = (uint32_t)__get_PRIMASK(); __disable_irq()
#define MICROPY_PY_PENDSV_EXIT     do { if (!atomic_state) { __enable_irq(); } } while (0)
#endif

#endif // MICROPY_INCLUDED_GD32_MPHALPORT_H
