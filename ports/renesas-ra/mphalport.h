/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Damien P. George
 * Copyright (c) 2021-2022 Renesas Electronics Corporation
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

#include RA_HAL_H
#include "pin.h"
#include "py/ringbuf.h"

#define MICROPY_BEGIN_ATOMIC_SECTION()     disable_irq()
#define MICROPY_END_ATOMIC_SECTION(state)  enable_irq(state)

#define MICROPY_PY_PENDSV_ENTER   uint32_t atomic_state = raise_irq_pri(IRQ_PRI_PENDSV)
#define MICROPY_PY_PENDSV_EXIT    restore_irq_pri(atomic_state)

#define MICROPY_PY_LWIP_ENTER
#define MICROPY_PY_LWIP_REENTER
#define MICROPY_PY_LWIP_EXIT

#define MICROPY_HW_USB_CDC_TX_TIMEOUT (500)

extern const unsigned char mp_hal_status_to_errno_table[4];
extern ringbuf_t stdin_ringbuf;

static inline int mp_hal_status_to_neg_errno(HAL_StatusTypeDef status) {
    return -mp_hal_status_to_errno_table[status];
}

NORETURN void mp_hal_raise(HAL_StatusTypeDef status);
void mp_hal_set_interrupt_char(int c); // -1 to disable

// timing functions

#include "irq.h"

#if __CORTEX_M == 0
// Don't have raise_irq_pri on Cortex-M0 so keep IRQs enabled to have SysTick timing
#define mp_hal_quiet_timing_enter() (1)
#define mp_hal_quiet_timing_exit(irq_state) (void)(irq_state)
#else
#define mp_hal_quiet_timing_enter() raise_irq_pri(1)
#define mp_hal_quiet_timing_exit(irq_state) restore_irq_pri(irq_state)
#endif
#define mp_hal_delay_us_fast(us) mp_hal_delay_us(us)

void mp_hal_ticks_cpu_enable(void);
static inline mp_uint_t mp_hal_ticks_cpu(void) {
    return 0;
}
#define SPI_FIRSTBIT_MSB                (0x00000000U)
#define SPI_FIRSTBIT_LSB                (0x00000001U)

#define MP_HAL_PIN_FMT                  "%q"
#define MP_HAL_PIN_MODE_INPUT           (GPIO_MODE_INPUT)
#define MP_HAL_PIN_MODE_OUTPUT          (GPIO_MODE_OUTPUT_PP)
#define MP_HAL_PIN_MODE_ALT             (GPIO_MODE_AF_PP)
#define MP_HAL_PIN_MODE_ANALOG          (GPIO_MODE_ANALOG)
#define MP_HAL_PIN_MODE_ADC             (GPIO_MODE_ANALOG)
#define MP_HAL_PIN_MODE_OPEN_DRAIN      (GPIO_MODE_OUTPUT_OD)
#define MP_HAL_PIN_MODE_ALT_OPEN_DRAIN  (GPIO_MODE_AF_OD)
#define MP_HAL_PIN_PULL_NONE            (GPIO_NOPULL)
#define MP_HAL_PIN_PULL_UP              (GPIO_PULLUP)
#define MP_HAL_PIN_PULL_DOWN            (GPIO_PULLDOWN)
#define MP_HAL_PIN_TRIGGER_FALLING      (GPIO_IRQ_FALLING)
#define MP_HAL_PIN_TRIGGER_RISING       (GPIO_IRQ_RISING)
#define MP_HAL_PIN_TRIGGER_LOWLEVEL     (GPIO_IRQ_LOWLEVEL)
#define MP_HAL_PIN_TRIGGER_HIGHLEVEL    (GPIO_IRQ_HIGHLEVEL)
#define MP_HAL_PIN_DRIVE_0              (GPIO_LOW_POWER)
#define MP_HAL_PIN_DRIVE_1              (GPIO_MID_POWER)
#define MP_HAL_PIN_DRIVE_2              (GPIO_MID_FAST_POWER)
#define MP_HAL_PIN_DRIVE_3              (GPIO_HIGH_POWER)

#define mp_hal_pin_obj_t        const machine_pin_obj_t *
#define mp_hal_get_pin_obj(o)   machine_pin_find(o)
#define mp_hal_pin_name(p)      ((p)->name)
#define mp_hal_pin_input(p)     ra_gpio_mode_input((p)->pin)
#define mp_hal_pin_output(p)    ra_gpio_mode_output((p)->pin)
#define mp_hal_pin_open_drain(p)    ra_gpio_config((p)->pin, MP_HAL_PIN_MODE_OPEN_DRAIN, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_DRIVE_0, 0)
#define mp_hal_pin_high(p)      ra_gpio_write((p)->pin, 1)
#define mp_hal_pin_low(p)       ra_gpio_write((p)->pin, 0)
#define mp_hal_pin_toggle(p)    ra_gpio_toggle((p)->pin)
#define mp_hal_pin_od_low(p)    mp_hal_pin_low(p)
#define mp_hal_pin_od_high(p)   mp_hal_pin_high(p)
#define mp_hal_pin_read(p)      ra_gpio_read((p)->pin)
#define mp_hal_pin_write(p, v)  do { if (v) { mp_hal_pin_high(p); } else { mp_hal_pin_low(p); } } while (0)

void mp_hal_pin_config(mp_hal_pin_obj_t pin, uint32_t mode, uint32_t pull, uint32_t drive, uint32_t alt);

enum {
    MP_HAL_MAC_WLAN0 = 0,
    MP_HAL_MAC_WLAN1,
    MP_HAL_MAC_BDADDR,
    MP_HAL_MAC_ETH0,
};

void mp_hal_get_mac(int idx, uint8_t buf[6]);

void mp_hal_set_interrupt_char(int c); // -1 to disable
