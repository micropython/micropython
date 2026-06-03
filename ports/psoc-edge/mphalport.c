/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 Infineon Technologies AG
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

// std includes
#include "stdio.h"

// MTB includes
#include "mtb_hal_system.h"

// micropython includes
#include "mpconfigport.h"
#include "mphalport.h"

#include "py/runtime.h"

extern void machine_uart_repl_init(void);
extern int machine_uart_repl_readchar(void);
extern void machine_uart_repl_write(const void *buf_in, mp_uint_t size);
extern uintptr_t machine_uart_repl_ioctl(uintptr_t arg);

void mp_hal_stdio_init(void) {
    machine_uart_repl_init();
}

uintptr_t mp_hal_stdio_poll(uintptr_t poll_flags) {
    return machine_uart_repl_ioctl(poll_flags);
}

void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    machine_uart_repl_write(str, len);
}

int mp_hal_stdin_rx_chr(void) {
    return machine_uart_repl_readchar();
}

extern uint32_t get_drive_mode(uint8_t mode, uint8_t pull);

void mp_hal_pin_config(mp_hal_pin_obj_t pin, uint32_t mode, uint32_t pull, uint32_t value) {
    uint32_t drive_mode = get_drive_mode(mode, pull);
    Cy_GPIO_Pin_FastInit(Cy_GPIO_PortToAddr(pin->port), pin->pin, drive_mode, value, HSIOM_SEL_GPIO);
}

extern uint8_t pin_get_mode(const machine_pin_obj_t *self);

uint32_t mp_hal_pin_read(mp_hal_pin_obj_t pin) {
    uint8_t mode = pin_get_mode(pin);
    if (mode == GPIO_MODE_OUT ||
        mode == GPIO_MODE_OPEN_DRAIN) {
        return Cy_GPIO_ReadOut(Cy_GPIO_PortToAddr(pin->port), pin->pin);
    }
    return Cy_GPIO_Read(Cy_GPIO_PortToAddr(pin->port), pin->pin);
}

uint32_t mp_hal_pin_get_drive(mp_hal_pin_obj_t pin) {
    return Cy_GPIO_GetDriveSel(Cy_GPIO_PortToAddr(pin->port), pin->pin);
}

void mp_hal_pin_set_drive(mp_hal_pin_obj_t pin, uint32_t drive) {
    Cy_GPIO_SetDriveSel(Cy_GPIO_PortToAddr(pin->port), pin->pin, drive);
}

void mp_hal_pin_write(mp_hal_pin_obj_t pin, uint8_t polarity) {
    Cy_GPIO_Write(Cy_GPIO_PortToAddr(pin->port), pin->pin, polarity);
}

mp_hal_pin_af_obj_t mp_hal_pin_af_find(mp_hal_pin_obj_t pin, uint32_t af_signal) {
    for (uint8_t i = 0; i < pin->af_num; i++) {
        const machine_pin_af_obj_t *af = &pin->af[i];
        if (af->signal == af_signal) {
            return af;
        }
    }

    mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Pin '%q' does not support '%s'."), pin->name, machine_pin_af_signal_str[af_signal]);
}

mp_hal_af_periph_t mp_hal_periph_pins_af_config(const mp_hal_pin_af_config_t *periph_pins_config, uint8_t num_pins) {
    /**
     * If there is more than one pin,
     * validate if all pins share
     * the same AF peripheral pointer and unit
     */
    if (num_pins > 1) {
        const mp_hal_pin_af_config_t *first_pin_cfg = &periph_pins_config[0];
        mp_hal_af_periph_t periph_ptr = first_pin_cfg->af->periph;
        uint32_t unit = first_pin_cfg->af->unit;
        for (uint8_t i = 1; i < num_pins; i++) {
            const mp_hal_pin_af_config_t *pin_cfg = &periph_pins_config[i];
            if (pin_cfg->af->periph != periph_ptr) {
                mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("All pins must belong to the same peripheral."));
            }
            if (pin_cfg->af->unit != unit) {
                mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("All pins must belong to the same unit."));
            }
        }
    }

    for (uint8_t i = 0; i < num_pins; i++) {
        const mp_hal_pin_af_config_t *pin_cfg = &periph_pins_config[i];
        Cy_GPIO_Pin_FastInit(Cy_GPIO_PortToAddr(pin_cfg->pin->port), pin_cfg->pin->pin, pin_cfg->cy_drive_mode, pin_cfg->init_value, pin_cfg->af->idx);
    }

    return periph_pins_config[0].af->periph;
}
