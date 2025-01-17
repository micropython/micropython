/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * Development of the code in this file was sponsored by Microbric Pty Ltd
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

#ifndef INCLUDED_MPHALPORT_H
#define INCLUDED_MPHALPORT_H

#include "py/ringbuf.h"
#include "shared/runtime/interrupt_char.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/spi_master.h"
#include "soc/gpio_reg.h"

#define MICROPY_PLATFORM_VERSION "IDF" IDF_VER

// The core that the MicroPython task(s) are pinned to.
// Now that we have IDF 4.2.0+, we are once again able to pin to core 1
// and avoid the Wifi/BLE timing problems on the same core.
// Best effort here to remain backwards compatible in rare version edge cases...
// See https://github.com/micropython/micropython/issues/5489 for history
#if CONFIG_FREERTOS_UNICORE
#define MP_TASK_COREID (0)
#else
#define MP_TASK_COREID (1)
#endif

extern TaskHandle_t mp_main_task_handle;

extern ringbuf_t stdin_ringbuf;

extern portMUX_TYPE mp_atomic_mux;

// Check the ESP-IDF error code and raise an OSError if it's not ESP_OK.
#if MICROPY_ERROR_REPORTING <= MICROPY_ERROR_REPORTING_NORMAL
#define check_esp_err(code) check_esp_err_(code)
void check_esp_err_(esp_err_t code);
#else
#define check_esp_err(code) check_esp_err_(code, __FUNCTION__, __LINE__, __FILE__)
void check_esp_err_(esp_err_t code, const char *func, const int line, const char *file);
#endif

static inline mp_uint_t mp_begin_atomic_section(void) {
    portENTER_CRITICAL(&mp_atomic_mux);
    return 0;
}

static inline void mp_end_atomic_section(mp_uint_t state) {
    (void)state;
    portEXIT_CRITICAL(&mp_atomic_mux);
}

// Note: These atomic macros disable interrupts on the calling CPU, and on SMP
// systems also protect against concurrent access to an atomic section on the
// other CPU.
#define MICROPY_BEGIN_ATOMIC_SECTION() mp_begin_atomic_section()
#define MICROPY_END_ATOMIC_SECTION(state) mp_end_atomic_section(state)

uint32_t mp_hal_ticks_us(void);
__attribute__((always_inline)) static inline uint32_t mp_hal_ticks_cpu(void) {
    uint32_t ccount;
    #if CONFIG_IDF_TARGET_ARCH_RISCV
    __asm__ __volatile__ ("csrr %0, 0x7E2" : "=r" (ccount)); // Machine Performance Counter Value
    #else
    __asm__ __volatile__ ("rsr %0,ccount" : "=a" (ccount));
    #endif
    return ccount;
}

void mp_hal_delay_us(uint32_t);
#define mp_hal_delay_us_fast(us) esp_rom_delay_us(us)
void mp_hal_set_interrupt_char(int c);
uint32_t mp_hal_get_cpu_freq(void);

#define mp_hal_quiet_timing_enter() MICROPY_BEGIN_ATOMIC_SECTION()
#define mp_hal_quiet_timing_exit(irq_state) MICROPY_END_ATOMIC_SECTION(irq_state)

// Wake up the main task if it is sleeping
void mp_hal_wake_main_task(void);
void mp_hal_wake_main_task_from_isr(void);

// C-level pin HAL
#include "py/obj.h"
#include "driver/gpio.h"
#define MP_HAL_PIN_FMT "%u"
#define mp_hal_pin_obj_t gpio_num_t
mp_hal_pin_obj_t machine_pin_get_id(mp_obj_t pin_in);
#define mp_hal_get_pin_obj(o) machine_pin_get_id(o)
#define mp_hal_pin_name(p) (p)
static inline void mp_hal_pin_input(mp_hal_pin_obj_t pin) {
    esp_rom_gpio_pad_select_gpio(pin);
    gpio_set_direction(pin, GPIO_MODE_INPUT);
}
static inline void mp_hal_pin_output(mp_hal_pin_obj_t pin) {
    esp_rom_gpio_pad_select_gpio(pin);
    gpio_set_direction(pin, GPIO_MODE_INPUT_OUTPUT);
}
static inline void mp_hal_pin_open_drain(mp_hal_pin_obj_t pin) {
    esp_rom_gpio_pad_select_gpio(pin);
    gpio_set_direction(pin, GPIO_MODE_INPUT_OUTPUT_OD);
}
static inline void mp_hal_pin_od_low(mp_hal_pin_obj_t pin) {
    gpio_set_level(pin, 0);
}
static inline void mp_hal_pin_od_high(mp_hal_pin_obj_t pin) {
    gpio_set_level(pin, 1);
}
static inline int mp_hal_pin_read(mp_hal_pin_obj_t pin) {
    return gpio_get_level(pin);
}
static inline int mp_hal_pin_read_output(mp_hal_pin_obj_t pin) {
    #if defined(GPIO_OUT1_REG)
    return pin < 32
        ? (*(uint32_t *)GPIO_OUT_REG >> pin) & 1
        : (*(uint32_t *)GPIO_OUT1_REG >> (pin - 32)) & 1;
    #else
    return (*(uint32_t *)GPIO_OUT_REG >> pin) & 1;
    #endif
}
static inline void mp_hal_pin_write(mp_hal_pin_obj_t pin, int v) {
    gpio_set_level(pin, v);
}

spi_host_device_t machine_hw_spi_get_host(mp_obj_t in);

#endif // INCLUDED_MPHALPORT_H
