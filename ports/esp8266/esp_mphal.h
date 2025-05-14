/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "user_interface.h"
#include "py/ringbuf.h"
#include "shared/runtime/interrupt_char.h"
#include "xtirq.h"

#define MICROPY_BEGIN_ATOMIC_SECTION() esp_disable_irq()
#define MICROPY_END_ATOMIC_SECTION(state) esp_enable_irq(state)

void mp_sched_keyboard_interrupt(void);

struct _mp_print_t;
// Structure for UART-only output via mp_printf()
extern const struct _mp_print_t mp_debug_print;

extern ringbuf_t stdin_ringbuf;
// Call this after putting data to stdin_ringbuf
void mp_hal_signal_input(void);

// This variable counts how many times the UART is attached to dupterm
extern int uart_attached_to_dupterm;

void mp_hal_init(void);
void mp_hal_rtc_init(void);

__attribute__((always_inline)) static inline uint32_t mp_hal_ticks_us(void) {
    return system_get_time();
}

__attribute__((always_inline)) static inline uint32_t mp_hal_ticks_cpu(void) {
    uint32_t ccount;
    __asm__ __volatile__ ("rsr %0,ccount" : "=a" (ccount));
    return ccount;
}

void mp_hal_delay_us(uint32_t);
void mp_hal_set_interrupt_char(int c);
uint32_t mp_hal_get_cpu_freq(void);

#define UART_TASK_ID 0
void uart_task_init();

uint32_t esp_disable_irq(void);
void esp_enable_irq(uint32_t state);

// needed for machine.I2C
#include "osapi.h"
#define mp_hal_delay_us_fast(us) os_delay_us(us)

#define mp_hal_quiet_timing_enter() disable_irq()
#define mp_hal_quiet_timing_exit(irq_state) enable_irq(irq_state)

// C-level pin HAL
#include "etshal.h"
#include "gpio.h"
#include "modmachine.h"
#define MP_HAL_PIN_FMT "%u"
#define mp_hal_pin_obj_t uint32_t
#define mp_hal_get_pin_obj(o) mp_obj_get_pin(o)
#define mp_hal_pin_name(p) (p)
void mp_hal_pin_input(mp_hal_pin_obj_t pin);
void mp_hal_pin_output(mp_hal_pin_obj_t pin);
void mp_hal_pin_open_drain(mp_hal_pin_obj_t pin);
#define mp_hal_pin_od_low(p) do { \
        if ((p) == 16) { WRITE_PERI_REG(RTC_GPIO_ENABLE, (READ_PERI_REG(RTC_GPIO_ENABLE) & ~1) | 1); } \
        else { gpio_output_set(0, 1 << (p), 1 << (p), 0); } \
} while (0)
#define mp_hal_pin_od_high(p) do { \
        if ((p) == 16) { WRITE_PERI_REG(RTC_GPIO_ENABLE, (READ_PERI_REG(RTC_GPIO_ENABLE) & ~1)); } \
        else { gpio_output_set(0, 0, 0, 1 << (p)); /* set as input to avoid glitches */ } \
} while (0)
// The DHT driver requires using the open-drain feature of the GPIO to get it to work reliably
#define mp_hal_pin_od_high_dht(p) do { \
        if ((p) == 16) { WRITE_PERI_REG(RTC_GPIO_ENABLE, (READ_PERI_REG(RTC_GPIO_ENABLE) & ~1)); } \
        else { gpio_output_set(1 << (p), 0, 1 << (p), 0); } \
} while (0)
#define mp_hal_pin_read(p) pin_get(p)
static inline int mp_hal_pin_read_output(mp_hal_pin_obj_t pin) {
    if (pin >= 16) {
        return READ_PERI_REG(RTC_GPIO_OUT) & 1;
    } else {
        return (GPIO_REG_READ(GPIO_OUT_ADDRESS) >> pin) & 1;
    }
}
#define mp_hal_pin_write(p, v) pin_set((p), (v))

void *ets_get_esf_buf_ctlblk(void);
int ets_esf_free_bufs(int idx);
