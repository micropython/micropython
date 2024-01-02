/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
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
 *
 */
#ifndef MICROPY_INCLUDED_RP2_MPHALPORT_H
#define MICROPY_INCLUDED_RP2_MPHALPORT_H

#include "py/mpconfig.h"
#include "py/ringbuf.h"
#include "pico/time.h"
#include "hardware/clocks.h"
#include "hardware/structs/systick.h"
#include "RP2040.h" // cmsis, for __WFI
#include "pendsv.h"

#define SYSTICK_MAX (0xffffff)
#define MICROPY_HW_USB_CDC_TX_TIMEOUT (500)

// Entering a critical section.
#if MICROPY_PY_THREAD
#define MICROPY_BEGIN_ATOMIC_SECTION()     mp_thread_begin_atomic_section()
#define MICROPY_END_ATOMIC_SECTION(state)  mp_thread_end_atomic_section(state)
#else
#define MICROPY_BEGIN_ATOMIC_SECTION()     save_and_disable_interrupts()
#define MICROPY_END_ATOMIC_SECTION(state)  restore_interrupts(state)
#endif

#define MICROPY_PY_PENDSV_ENTER   pendsv_suspend()
#define MICROPY_PY_PENDSV_EXIT    pendsv_resume()

// Prevent the "lwIP task" from running when unsafe to do so.
#define MICROPY_PY_LWIP_ENTER   lwip_lock_acquire();
#define MICROPY_PY_LWIP_REENTER lwip_lock_acquire();
#define MICROPY_PY_LWIP_EXIT    lwip_lock_release();

// Port level Wait-for-Event macro
//
// Do not use this macro directly, include py/runtime.h and
// call mp_event_wait_indefinite() or mp_event_wait_ms(timeout)
#define MICROPY_INTERNAL_WFE(TIMEOUT_MS) \
    do {                                 \
        if ((TIMEOUT_MS) < 0) { \
            __wfe(); \
        } else { \
            mp_wfe_or_timeout(TIMEOUT_MS); \
        } \
    } while (0)

extern int mp_interrupt_char;
extern ringbuf_t stdin_ringbuf;

// Port-specific function to create a wakeup interrupt after timeout_ms and enter WFE
void mp_wfe_or_timeout(uint32_t timeout_ms);

uint32_t mp_thread_begin_atomic_section(void);
void mp_thread_end_atomic_section(uint32_t);

void mp_hal_set_interrupt_char(int c);
void mp_hal_time_ns_set_from_rtc(void);

static inline void mp_hal_delay_us_fast(mp_uint_t us) {
    busy_wait_us(us);
}

#define mp_hal_quiet_timing_enter() MICROPY_BEGIN_ATOMIC_SECTION()
#define mp_hal_quiet_timing_exit(irq_state) MICROPY_END_ATOMIC_SECTION(irq_state)

static inline mp_uint_t mp_hal_ticks_us(void) {
    return time_us_32();
}

static inline mp_uint_t mp_hal_ticks_ms(void) {
    return to_ms_since_boot(get_absolute_time());
}

static inline mp_uint_t mp_hal_ticks_cpu(void) {
    // ticks_cpu() is defined as using the highest-resolution timing source
    // in the system. This is usually a CPU clock, but doesn't have to be.
    return time_us_32();
}

static inline mp_uint_t mp_hal_get_cpu_freq(void) {
    return clock_get_hz(clk_sys);
}

// C-level pin HAL

#include "py/obj.h"
#include "hardware/gpio.h"

#define MP_HAL_PIN_FMT "%u"
#define mp_hal_pin_obj_t uint
#define MP_HAL_PIN_MODE_INPUT           (GPIO_IN)
#define MP_HAL_PIN_MODE_OUTPUT          (GPIO_OUT)
#define MP_HAL_PIN_PULL_NONE            (0)
#define MP_HAL_PIN_PULL_UP              (1)
#define MP_HAL_PIN_PULL_DOWN            (2)

extern uint32_t machine_pin_open_drain_mask;

mp_hal_pin_obj_t mp_hal_get_pin_obj(mp_obj_t pin_in);

static inline unsigned int mp_hal_pin_name(mp_hal_pin_obj_t pin) {
    return pin;
}

static inline void mp_hal_pin_input(mp_hal_pin_obj_t pin) {
    gpio_set_dir(pin, GPIO_IN);
    machine_pin_open_drain_mask &= ~(1 << pin);
    gpio_set_function(pin, GPIO_FUNC_SIO);
}

static inline void mp_hal_pin_output(mp_hal_pin_obj_t pin) {
    gpio_set_dir(pin, GPIO_OUT);
    machine_pin_open_drain_mask &= ~(1 << pin);
    gpio_set_function(pin, GPIO_FUNC_SIO);
}

static inline void mp_hal_pin_open_drain_with_value(mp_hal_pin_obj_t pin, int v) {
    if (v) {
        gpio_set_dir(pin, GPIO_IN);
        gpio_put(pin, 0);
    } else {
        gpio_put(pin, 0);
        gpio_set_dir(pin, GPIO_OUT);
    }
    machine_pin_open_drain_mask |= 1 << pin;
    gpio_set_function(pin, GPIO_FUNC_SIO);
}

static inline void mp_hal_pin_open_drain(mp_hal_pin_obj_t pin) {
    mp_hal_pin_open_drain_with_value(pin, 1);
}

static inline void mp_hal_pin_config(mp_hal_pin_obj_t pin, uint32_t mode, uint32_t pull, uint32_t alt) {
    assert((mode == MP_HAL_PIN_MODE_INPUT || mode == MP_HAL_PIN_MODE_OUTPUT) && alt == 0);
    gpio_set_dir(pin, mode);
    gpio_set_pulls(pin, pull == MP_HAL_PIN_PULL_UP, pull == MP_HAL_PIN_PULL_DOWN);
}

static inline int mp_hal_pin_read(mp_hal_pin_obj_t pin) {
    return gpio_get(pin);
}

static inline void mp_hal_pin_write(mp_hal_pin_obj_t pin, int v) {
    gpio_put(pin, v);
}

static inline void mp_hal_pin_od_low(mp_hal_pin_obj_t pin) {
    gpio_set_dir(pin, GPIO_OUT);
}

static inline void mp_hal_pin_od_high(mp_hal_pin_obj_t pin) {
    gpio_set_dir(pin, GPIO_IN);
}

static inline void mp_hal_pin_low(mp_hal_pin_obj_t pin) {
    gpio_clr_mask(1 << pin);
}

static inline void mp_hal_pin_high(mp_hal_pin_obj_t pin) {
    gpio_set_mask(1 << pin);
}

enum mp_hal_pin_interrupt_trigger {
    MP_HAL_PIN_TRIGGER_NONE,
    MP_HAL_PIN_TRIGGER_LOW = GPIO_IRQ_LEVEL_LOW,
    MP_HAL_PIN_TRIGGER_HIGH = GPIO_IRQ_LEVEL_HIGH,
    MP_HAL_PIN_TRIGGER_FALL = GPIO_IRQ_EDGE_FALL,
    MP_HAL_PIN_TRIGGER_RISE = GPIO_IRQ_EDGE_RISE,
};

void mp_hal_pin_interrupt(mp_hal_pin_obj_t pin, mp_obj_t handler, mp_uint_t trigger, bool hard);

mp_obj_base_t *mp_hal_get_spi_obj(mp_obj_t spi_in);

enum {
    MP_HAL_MAC_WLAN0 = 0,
    MP_HAL_MAC_BDADDR,
    MP_HAL_MAC_ETH0,
};

void mp_hal_get_mac(int idx, uint8_t buf[6]);
void mp_hal_get_mac_ascii(int idx, size_t chr_off, size_t chr_len, char *dest);
void mp_hal_generate_laa_mac(int idx, uint8_t buf[6]);

#endif // MICROPY_INCLUDED_RP2_MPHALPORT_H
