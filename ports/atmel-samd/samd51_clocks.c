/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
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

#include "clocks.h"

#include "hpl_gclk_config.h"

#include "shared-bindings/microcontroller/__init__.h"

#include "py/runtime.h"

bool gclk_enabled(uint8_t gclk) {
    return GCLK->GENCTRL[gclk].bit.GENEN;
}

void disable_gclk(uint8_t gclk) {
    while ((GCLK->SYNCBUSY.vec.GENCTRL & (1 << gclk)) != 0) {}
    GCLK->GENCTRL[gclk].bit.GENEN = false;
    while ((GCLK->SYNCBUSY.vec.GENCTRL & (1 << gclk)) != 0) {}
}

void connect_gclk_to_peripheral(uint8_t gclk, uint8_t peripheral) {
    GCLK->PCHCTRL[peripheral].reg = GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN(gclk);
    while(GCLK->SYNCBUSY.reg != 0) {}
}

void disconnect_gclk_from_peripheral(uint8_t gclk, uint8_t peripheral) {
    GCLK->PCHCTRL[peripheral].reg = 0;
}

void enable_clock_generator(uint8_t gclk, uint32_t source, uint16_t divisor) {
    GCLK->GENCTRL[gclk].reg = GCLK_GENCTRL_SRC(source) | GCLK_GENCTRL_DIV(divisor) | GCLK_GENCTRL_GENEN;
    while ((GCLK->SYNCBUSY.vec.GENCTRL & (1 << gclk)) != 0) {}
}

void disable_clock_generator(uint8_t gclk) {
    GCLK->GENCTRL[gclk].reg = 0;
    while ((GCLK->SYNCBUSY.vec.GENCTRL & (1 << gclk)) != 0) {}
}

bool clock_get_enabled(uint8_t type, uint8_t index) {
    return false;
}

bool clock_get_parent(uint8_t type, uint8_t index, uint8_t *p_type, uint8_t *p_index) {
    return false;
}

uint32_t clock_get_frequency(uint8_t type, uint8_t index) {
    return 0;
}

uint32_t clock_get_calibration(uint8_t type, uint8_t index) {
    return 0;
}

int clock_set_calibration(uint8_t type, uint8_t index, uint32_t val) {
    return -2;
}

STATIC const mp_rom_map_elem_t samd_clock_global_dict_table[] = {
};
MP_DEFINE_CONST_DICT(samd_clock_globals, samd_clock_global_dict_table);
