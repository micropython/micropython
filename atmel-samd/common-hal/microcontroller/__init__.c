/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
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

#include "py/mphal.h"
#include "py/obj.h"

#include "samd21_pins.h"

#include "shared-bindings/nvm/ByteArray.h"

void common_hal_mcu_delay_us(uint32_t delay) {
    mp_hal_delay_us(delay);
}

// Interrupt flags that will be saved and restored during disable/Enable
// interrupt functions below.
static irqflags_t irq_flags;
void common_hal_mcu_disable_interrupts(void) {
    // Disable all interrupt sources for timing critical sections.
    // Disable ASF-based interrupts.
    irq_flags = cpu_irq_save();
}

void common_hal_mcu_enable_interrupts(void) {
    // Enable all interrupt sources after timing critical sections.
    // Restore ASF-based interrupts.
    cpu_irq_restore(irq_flags);
}

// NVM is only available on Express boards for now.
#if CIRCUITPY_INTERNAL_NVM_SIZE > 0
nvm_bytearray_obj_t common_hal_mcu_nvm_obj = {
    .base = {
        .type = &nvm_bytearray_type,
    },
    .len = NVMCTRL_ROW_SIZE,
    .start_address = (uint8_t*) (FLASH_SIZE - NVMCTRL_ROW_SIZE)
};
#endif

// This maps MCU pin names to pin objects.
STATIC const mp_map_elem_t mcu_pin_global_dict_table[] = {
// Pins in datasheet order.
#ifdef PIN_PA00
  { MP_OBJ_NEW_QSTR(MP_QSTR_PA00), (mp_obj_t)&pin_PA00 },
#endif
#ifdef PIN_PA01
  { MP_OBJ_NEW_QSTR(MP_QSTR_PA01), (mp_obj_t)&pin_PA01 },
#endif
#ifdef PIN_PA02
  { MP_OBJ_NEW_QSTR(MP_QSTR_PA02), (mp_obj_t)&pin_PA02 },
#endif
#ifdef PIN_PA03
  { MP_OBJ_NEW_QSTR(MP_QSTR_PA03), (mp_obj_t)&pin_PA03 },
#endif
#ifdef PIN_PB04
  { MP_OBJ_NEW_QSTR(MP_QSTR_PB04), (mp_obj_t)&pin_PB04 },
#endif
#ifdef PIN_PB05
  { MP_OBJ_NEW_QSTR(MP_QSTR_PB05), (mp_obj_t)&pin_PB05 },
#endif
#ifdef PIN_PB06
  { MP_OBJ_NEW_QSTR(MP_QSTR_PB06), (mp_obj_t)&pin_PB06 },
#endif
#ifdef PIN_PB07
  { MP_OBJ_NEW_QSTR(MP_QSTR_PB07), (mp_obj_t)&pin_PB07 },
#endif
#ifdef PIN_PB08
  { MP_OBJ_NEW_QSTR(MP_QSTR_PB08), (mp_obj_t)&pin_PB08 },
#endif
#ifdef PIN_PB09
  { MP_OBJ_NEW_QSTR(MP_QSTR_PB09), (mp_obj_t)&pin_PB09 },
#endif
#ifdef PIN_PA04
  { MP_OBJ_NEW_QSTR(MP_QSTR_PA04), (mp_obj_t)&pin_PA04 },
#endif
#ifdef PIN_PA05
  { MP_OBJ_NEW_QSTR(MP_QSTR_PA05), (mp_obj_t)&pin_PA05 },
#endif
#ifdef PIN_PA06
  { MP_OBJ_NEW_QSTR(MP_QSTR_PA06), (mp_obj_t)&pin_PA06 },
#endif
#ifdef PIN_PA07
  { MP_OBJ_NEW_QSTR(MP_QSTR_PA07), (mp_obj_t)&pin_PA07 },
#endif
#ifdef PIN_PA08
  { MP_OBJ_NEW_QSTR(MP_QSTR_PA08), (mp_obj_t)&pin_PA08 },
#endif
#ifdef PIN_PA09
  { MP_OBJ_NEW_QSTR(MP_QSTR_PA09), (mp_obj_t)&pin_PA09 },
#endif
#ifdef PIN_PA10
  { MP_OBJ_NEW_QSTR(MP_QSTR_PA10), (mp_obj_t)&pin_PA10 },
#endif
#ifdef PIN_PA11
  { MP_OBJ_NEW_QSTR(MP_QSTR_PA11), (mp_obj_t)&pin_PA11 },
#endif
#ifdef PIN_PB10
  { MP_OBJ_NEW_QSTR(MP_QSTR_PB10), (mp_obj_t)&pin_PB10 },
#endif
#ifdef PIN_PB11
  { MP_OBJ_NEW_QSTR(MP_QSTR_PB11), (mp_obj_t)&pin_PB11 },
#endif
#ifdef PIN_PB12
  { MP_OBJ_NEW_QSTR(MP_QSTR_PB12), (mp_obj_t)&pin_PB12 },
#endif
#ifdef PIN_PB13
  { MP_OBJ_NEW_QSTR(MP_QSTR_PB13), (mp_obj_t)&pin_PB13 },
#endif
#ifdef PIN_PB14
  { MP_OBJ_NEW_QSTR(MP_QSTR_PB14), (mp_obj_t)&pin_PB14 },
#endif

// Second page.
#ifdef PIN_PB15
  { MP_OBJ_NEW_QSTR(MP_QSTR_PB15), (mp_obj_t)&pin_PB15 },
#endif
#ifdef PIN_PA12
  { MP_OBJ_NEW_QSTR(MP_QSTR_PA12), (mp_obj_t)&pin_PA12 },
#endif
#ifdef PIN_PA13
  { MP_OBJ_NEW_QSTR(MP_QSTR_PA13), (mp_obj_t)&pin_PA13 },
#endif
#ifdef PIN_PA14
  { MP_OBJ_NEW_QSTR(MP_QSTR_PA14), (mp_obj_t)&pin_PA14 },
#endif
#ifdef PIN_PA15
  { MP_OBJ_NEW_QSTR(MP_QSTR_PA15), (mp_obj_t)&pin_PA15 },
#endif
#ifdef PIN_PA16
  { MP_OBJ_NEW_QSTR(MP_QSTR_PA16), (mp_obj_t)&pin_PA16 },
#endif
#ifdef PIN_PA17
  { MP_OBJ_NEW_QSTR(MP_QSTR_PA17), (mp_obj_t)&pin_PA17 },
#endif
#ifdef PIN_PA18
  { MP_OBJ_NEW_QSTR(MP_QSTR_PA18), (mp_obj_t)&pin_PA18 },
#endif
#ifdef PIN_PA19
  { MP_OBJ_NEW_QSTR(MP_QSTR_PA19), (mp_obj_t)&pin_PA19 },
#endif
#ifdef PIN_PB16
  { MP_OBJ_NEW_QSTR(MP_QSTR_PB16), (mp_obj_t)&pin_PB16 },
#endif
#ifdef PIN_PB17
  { MP_OBJ_NEW_QSTR(MP_QSTR_PB17), (mp_obj_t)&pin_PB17 },
#endif
#ifdef PIN_PA20
  { MP_OBJ_NEW_QSTR(MP_QSTR_PA20), (mp_obj_t)&pin_PA20 },
#endif
#ifdef PIN_PA21
  { MP_OBJ_NEW_QSTR(MP_QSTR_PA21), (mp_obj_t)&pin_PA21 },
#endif
#ifdef PIN_PA22
  { MP_OBJ_NEW_QSTR(MP_QSTR_PA22), (mp_obj_t)&pin_PA22 },
#endif
#ifdef PIN_PA23
  { MP_OBJ_NEW_QSTR(MP_QSTR_PA23), (mp_obj_t)&pin_PA23 },
#endif
#ifdef PIN_PA24
  { MP_OBJ_NEW_QSTR(MP_QSTR_PA24), (mp_obj_t)&pin_PA24 },
#endif
#ifdef PIN_PA25
  { MP_OBJ_NEW_QSTR(MP_QSTR_PA25), (mp_obj_t)&pin_PA25 },
#endif
#ifdef PIN_PB22
  { MP_OBJ_NEW_QSTR(MP_QSTR_PB22), (mp_obj_t)&pin_PB22 },
#endif
#ifdef PIN_PB23
  { MP_OBJ_NEW_QSTR(MP_QSTR_PB23), (mp_obj_t)&pin_PB23 },
#endif
#ifdef PIN_PA27
  { MP_OBJ_NEW_QSTR(MP_QSTR_PA27), (mp_obj_t)&pin_PA27 },
#endif
#ifdef PIN_PA28
  { MP_OBJ_NEW_QSTR(MP_QSTR_PA28), (mp_obj_t)&pin_PA28 },
#endif
#ifdef PIN_PA30
  { MP_OBJ_NEW_QSTR(MP_QSTR_PA30), (mp_obj_t)&pin_PA30 },
#endif
#ifdef PIN_PA31
  { MP_OBJ_NEW_QSTR(MP_QSTR_PA31), (mp_obj_t)&pin_PA31 },
#endif
#ifdef PIN_PB30
  { MP_OBJ_NEW_QSTR(MP_QSTR_PB30), (mp_obj_t)&pin_PB30 },
#endif
#ifdef PIN_PB31
  { MP_OBJ_NEW_QSTR(MP_QSTR_PB31), (mp_obj_t)&pin_PB31 },
#endif
#ifdef PIN_PB00
  { MP_OBJ_NEW_QSTR(MP_QSTR_PB00), (mp_obj_t)&pin_PB00 },
#endif
#ifdef PIN_PB01
  { MP_OBJ_NEW_QSTR(MP_QSTR_PB01), (mp_obj_t)&pin_PB01 },
#endif
#ifdef PIN_PB02
  { MP_OBJ_NEW_QSTR(MP_QSTR_PB02), (mp_obj_t)&pin_PB02 },
#endif
#ifdef PIN_PB03
  { MP_OBJ_NEW_QSTR(MP_QSTR_PB03), (mp_obj_t)&pin_PB03 }
#endif
};
MP_DEFINE_CONST_DICT(mcu_pin_globals, mcu_pin_global_dict_table);
