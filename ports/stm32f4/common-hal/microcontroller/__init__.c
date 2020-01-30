/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
 * Copyright (c) 2019 Lucian Copeland for Adafruit Industries
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
#include "py/runtime.h"

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/microcontroller/Processor.h"

#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/Processor.h"

#include "supervisor/filesystem.h"
#include "supervisor/shared/safe_mode.h"

#include "stm32f4xx_hal.h"

//tested divisor value for busy loop in us delay
#define LOOP_TICKS 12

STATIC uint32_t get_us(void) {
    uint32_t ticks_per_us = HAL_RCC_GetSysClockFreq()/1000000;
    uint32_t micros, sys_cycles;
    do {
        micros = supervisor_ticks_ms32();
        sys_cycles = SysTick->VAL; //counts backwards
    } while (micros != supervisor_ticks_ms32()); //try again if ticks_ms rolled over
    return (micros * 1000) + (ticks_per_us * 1000 - sys_cycles) / ticks_per_us;
}

void common_hal_mcu_delay_us(uint32_t delay) {
    if (__get_PRIMASK() == 0x00000000) {
        //by default use ticks_ms
        uint32_t start = get_us();
        while (get_us()-start < delay) {
            __asm__ __volatile__("nop");
        }
    } else {
        //when SysTick is disabled, approximate with busy loop
        const uint32_t ucount = HAL_RCC_GetSysClockFreq() / 1000000 * delay / LOOP_TICKS;
        for (uint32_t count = 0; ++count <= ucount;) {
        }
    }
}

volatile uint32_t nesting_count = 0;

void common_hal_mcu_disable_interrupts(void) {
    __disable_irq();
    __DMB();
    nesting_count++;
}

void common_hal_mcu_enable_interrupts(void) {
    if (nesting_count == 0) {
        // This is very very bad because it means there was mismatched disable/enables so we
        // "HardFault".
        asm("bkpt");
    }
    nesting_count--;
    if (nesting_count > 0) {
        return;
    }
    __DMB();
    __enable_irq();
}

void common_hal_mcu_on_next_reset(mcu_runmode_t runmode) {
  if(runmode == RUNMODE_SAFE_MODE)
    safe_mode_on_next_reset(PROGRAMMATIC_SAFE_MODE);
}

void common_hal_mcu_reset(void) {
    filesystem_flush(); //TODO: implement as part of flash improvements
    NVIC_SystemReset();
}

// The singleton microcontroller.Processor object, bound to microcontroller.cpu
// It currently only has properties, and no state.
const mcu_processor_obj_t common_hal_mcu_processor_obj = {
    .base = {
        .type = &mcu_processor_type,
    },
};

STATIC const mp_rom_map_elem_t mcu_pin_globals_table[] = {
#if MCU_PACKAGE >= 100
  { MP_ROM_QSTR(MP_QSTR_PE02), MP_ROM_PTR(&pin_PE02) },
  { MP_ROM_QSTR(MP_QSTR_PE03), MP_ROM_PTR(&pin_PE03) },
  { MP_ROM_QSTR(MP_QSTR_PE04), MP_ROM_PTR(&pin_PE04) },
  { MP_ROM_QSTR(MP_QSTR_PE05), MP_ROM_PTR(&pin_PE05) },
  { MP_ROM_QSTR(MP_QSTR_PE06), MP_ROM_PTR(&pin_PE06) },
#endif
  { MP_ROM_QSTR(MP_QSTR_PC13), MP_ROM_PTR(&pin_PC13) },
#if MCU_PACKAGE == 144
  { MP_ROM_QSTR(MP_QSTR_PF00), MP_ROM_PTR(&pin_PF00) },
  { MP_ROM_QSTR(MP_QSTR_PF01), MP_ROM_PTR(&pin_PF01) },
  { MP_ROM_QSTR(MP_QSTR_PF02), MP_ROM_PTR(&pin_PF02) },
  { MP_ROM_QSTR(MP_QSTR_PF03), MP_ROM_PTR(&pin_PF03) },
  { MP_ROM_QSTR(MP_QSTR_PF04), MP_ROM_PTR(&pin_PF04) },
  { MP_ROM_QSTR(MP_QSTR_PF05), MP_ROM_PTR(&pin_PF05) },
  { MP_ROM_QSTR(MP_QSTR_PF06), MP_ROM_PTR(&pin_PF06) },
  { MP_ROM_QSTR(MP_QSTR_PF07), MP_ROM_PTR(&pin_PF07) },
  { MP_ROM_QSTR(MP_QSTR_PF08), MP_ROM_PTR(&pin_PF08) },
  { MP_ROM_QSTR(MP_QSTR_PF09), MP_ROM_PTR(&pin_PF09) },
  { MP_ROM_QSTR(MP_QSTR_PF10), MP_ROM_PTR(&pin_PF10) },
#endif
  { MP_ROM_QSTR(MP_QSTR_PC00), MP_ROM_PTR(&pin_PC00) },
  { MP_ROM_QSTR(MP_QSTR_PC01), MP_ROM_PTR(&pin_PC01) },
  { MP_ROM_QSTR(MP_QSTR_PC02), MP_ROM_PTR(&pin_PC02) },
  { MP_ROM_QSTR(MP_QSTR_PC03), MP_ROM_PTR(&pin_PC03) },
  { MP_ROM_QSTR(MP_QSTR_PA00), MP_ROM_PTR(&pin_PA00) },
  { MP_ROM_QSTR(MP_QSTR_PA01), MP_ROM_PTR(&pin_PA01) },
  { MP_ROM_QSTR(MP_QSTR_PA02), MP_ROM_PTR(&pin_PA02) },
  { MP_ROM_QSTR(MP_QSTR_PA03), MP_ROM_PTR(&pin_PA03) },
  { MP_ROM_QSTR(MP_QSTR_PA04), MP_ROM_PTR(&pin_PA04) },
  { MP_ROM_QSTR(MP_QSTR_PA05), MP_ROM_PTR(&pin_PA05) },
  { MP_ROM_QSTR(MP_QSTR_PA06), MP_ROM_PTR(&pin_PA06) },
  { MP_ROM_QSTR(MP_QSTR_PA07), MP_ROM_PTR(&pin_PA07) },
  { MP_ROM_QSTR(MP_QSTR_PC04), MP_ROM_PTR(&pin_PC04) },
  { MP_ROM_QSTR(MP_QSTR_PC05), MP_ROM_PTR(&pin_PC05) },
  { MP_ROM_QSTR(MP_QSTR_PB00), MP_ROM_PTR(&pin_PB00) },
  { MP_ROM_QSTR(MP_QSTR_PB01), MP_ROM_PTR(&pin_PB01) },
  { MP_ROM_QSTR(MP_QSTR_PB02), MP_ROM_PTR(&pin_PB02) },
#if MCU_PACKAGE == 144
  { MP_ROM_QSTR(MP_QSTR_PF11), MP_ROM_PTR(&pin_PF11) },
  { MP_ROM_QSTR(MP_QSTR_PF12), MP_ROM_PTR(&pin_PF12) },
  { MP_ROM_QSTR(MP_QSTR_PF13), MP_ROM_PTR(&pin_PF13) },
  { MP_ROM_QSTR(MP_QSTR_PF14), MP_ROM_PTR(&pin_PF14) },
  { MP_ROM_QSTR(MP_QSTR_PF15), MP_ROM_PTR(&pin_PF15) },
  { MP_ROM_QSTR(MP_QSTR_PG00), MP_ROM_PTR(&pin_PG00) },
  { MP_ROM_QSTR(MP_QSTR_PG01), MP_ROM_PTR(&pin_PG01) },
#endif
#if MCU_PACKAGE >= 100
  { MP_ROM_QSTR(MP_QSTR_PE07), MP_ROM_PTR(&pin_PE07) },
  { MP_ROM_QSTR(MP_QSTR_PE08), MP_ROM_PTR(&pin_PE08) },
  { MP_ROM_QSTR(MP_QSTR_PE09), MP_ROM_PTR(&pin_PE09) },
  { MP_ROM_QSTR(MP_QSTR_PE10), MP_ROM_PTR(&pin_PE10) },
  { MP_ROM_QSTR(MP_QSTR_PE11), MP_ROM_PTR(&pin_PE11) },
  { MP_ROM_QSTR(MP_QSTR_PE12), MP_ROM_PTR(&pin_PE12) },
  { MP_ROM_QSTR(MP_QSTR_PE13), MP_ROM_PTR(&pin_PE13) },
  { MP_ROM_QSTR(MP_QSTR_PE14), MP_ROM_PTR(&pin_PE14) },
  { MP_ROM_QSTR(MP_QSTR_PE15), MP_ROM_PTR(&pin_PE15) },
#endif
  { MP_ROM_QSTR(MP_QSTR_PB10), MP_ROM_PTR(&pin_PB10) },
#if MCU_PACKAGE == 144 || defined STM32F405xx
  { MP_ROM_QSTR(MP_QSTR_PB11), MP_ROM_PTR(&pin_PB11) },
#endif  
  { MP_ROM_QSTR(MP_QSTR_PB12), MP_ROM_PTR(&pin_PB12) },
  { MP_ROM_QSTR(MP_QSTR_PB13), MP_ROM_PTR(&pin_PB13) },
  { MP_ROM_QSTR(MP_QSTR_PB14), MP_ROM_PTR(&pin_PB14) },
  { MP_ROM_QSTR(MP_QSTR_PB15), MP_ROM_PTR(&pin_PB15) },
#if MCU_PACKAGE >= 100
  { MP_ROM_QSTR(MP_QSTR_PD08), MP_ROM_PTR(&pin_PD08) },
  { MP_ROM_QSTR(MP_QSTR_PD09), MP_ROM_PTR(&pin_PD09) },
  { MP_ROM_QSTR(MP_QSTR_PD10), MP_ROM_PTR(&pin_PD10) },
  { MP_ROM_QSTR(MP_QSTR_PD11), MP_ROM_PTR(&pin_PD11) },
  { MP_ROM_QSTR(MP_QSTR_PD12), MP_ROM_PTR(&pin_PD12) },
  { MP_ROM_QSTR(MP_QSTR_PD13), MP_ROM_PTR(&pin_PD13) },
  { MP_ROM_QSTR(MP_QSTR_PD14), MP_ROM_PTR(&pin_PD14) },
  { MP_ROM_QSTR(MP_QSTR_PD15), MP_ROM_PTR(&pin_PD15) },
#endif
#if MCU_PACKAGE == 144
  { MP_ROM_QSTR(MP_QSTR_PG02), MP_ROM_PTR(&pin_PG02) },
  { MP_ROM_QSTR(MP_QSTR_PG03), MP_ROM_PTR(&pin_PG03) },
  { MP_ROM_QSTR(MP_QSTR_PG04), MP_ROM_PTR(&pin_PG04) },
  { MP_ROM_QSTR(MP_QSTR_PG05), MP_ROM_PTR(&pin_PG05) },
  { MP_ROM_QSTR(MP_QSTR_PG06), MP_ROM_PTR(&pin_PG06) },
  { MP_ROM_QSTR(MP_QSTR_PG07), MP_ROM_PTR(&pin_PG07) },
  { MP_ROM_QSTR(MP_QSTR_PG08), MP_ROM_PTR(&pin_PG08) },
#endif
  { MP_ROM_QSTR(MP_QSTR_PC06), MP_ROM_PTR(&pin_PC06) },
  { MP_ROM_QSTR(MP_QSTR_PC07), MP_ROM_PTR(&pin_PC07) },
  { MP_ROM_QSTR(MP_QSTR_PC08), MP_ROM_PTR(&pin_PC08) },
  { MP_ROM_QSTR(MP_QSTR_PC09), MP_ROM_PTR(&pin_PC09) },
  { MP_ROM_QSTR(MP_QSTR_PA08), MP_ROM_PTR(&pin_PA08) },
  { MP_ROM_QSTR(MP_QSTR_PA09), MP_ROM_PTR(&pin_PA09) },
  { MP_ROM_QSTR(MP_QSTR_PA10), MP_ROM_PTR(&pin_PA10) },
  { MP_ROM_QSTR(MP_QSTR_PA11), MP_ROM_PTR(&pin_PA11) },
  { MP_ROM_QSTR(MP_QSTR_PA12), MP_ROM_PTR(&pin_PA12) },
  { MP_ROM_QSTR(MP_QSTR_PA13), MP_ROM_PTR(&pin_PA13) },
  { MP_ROM_QSTR(MP_QSTR_PA14), MP_ROM_PTR(&pin_PA14) },
  { MP_ROM_QSTR(MP_QSTR_PA15), MP_ROM_PTR(&pin_PA15) },
  { MP_ROM_QSTR(MP_QSTR_PC10), MP_ROM_PTR(&pin_PC10) },
  { MP_ROM_QSTR(MP_QSTR_PC11), MP_ROM_PTR(&pin_PC11) },
  { MP_ROM_QSTR(MP_QSTR_PC12), MP_ROM_PTR(&pin_PC12) },
#if MCU_PACKAGE >= 100
  { MP_ROM_QSTR(MP_QSTR_PD00), MP_ROM_PTR(&pin_PD00) },
  { MP_ROM_QSTR(MP_QSTR_PD01), MP_ROM_PTR(&pin_PD01) },
  { MP_ROM_QSTR(MP_QSTR_PD02), MP_ROM_PTR(&pin_PD02) },
  { MP_ROM_QSTR(MP_QSTR_PD03), MP_ROM_PTR(&pin_PD03) },
  { MP_ROM_QSTR(MP_QSTR_PD04), MP_ROM_PTR(&pin_PD04) },
  { MP_ROM_QSTR(MP_QSTR_PD05), MP_ROM_PTR(&pin_PD05) },
  { MP_ROM_QSTR(MP_QSTR_PD06), MP_ROM_PTR(&pin_PD06) },
  { MP_ROM_QSTR(MP_QSTR_PD07), MP_ROM_PTR(&pin_PD07) },
#endif
#if MCU_PACKAGE == 144
  { MP_ROM_QSTR(MP_QSTR_PG09), MP_ROM_PTR(&pin_PG09) },
  { MP_ROM_QSTR(MP_QSTR_PG10), MP_ROM_PTR(&pin_PG10) },
  { MP_ROM_QSTR(MP_QSTR_PG11), MP_ROM_PTR(&pin_PG11) },
  { MP_ROM_QSTR(MP_QSTR_PG12), MP_ROM_PTR(&pin_PG12) },
  { MP_ROM_QSTR(MP_QSTR_PG13), MP_ROM_PTR(&pin_PG13) },
  { MP_ROM_QSTR(MP_QSTR_PG14), MP_ROM_PTR(&pin_PG14) },
  { MP_ROM_QSTR(MP_QSTR_PG15), MP_ROM_PTR(&pin_PG15) },
#endif
  { MP_ROM_QSTR(MP_QSTR_PB03), MP_ROM_PTR(&pin_PB03) },
  { MP_ROM_QSTR(MP_QSTR_PB04), MP_ROM_PTR(&pin_PB04) },
  { MP_ROM_QSTR(MP_QSTR_PB05), MP_ROM_PTR(&pin_PB05) },
  { MP_ROM_QSTR(MP_QSTR_PB06), MP_ROM_PTR(&pin_PB06) },
  { MP_ROM_QSTR(MP_QSTR_PB07), MP_ROM_PTR(&pin_PB07) },
  { MP_ROM_QSTR(MP_QSTR_PB08), MP_ROM_PTR(&pin_PB08) },
  { MP_ROM_QSTR(MP_QSTR_PB09), MP_ROM_PTR(&pin_PB09) },
#if MCU_PACKAGE >= 100
  { MP_ROM_QSTR(MP_QSTR_PE00), MP_ROM_PTR(&pin_PE00) },
  { MP_ROM_QSTR(MP_QSTR_PE01), MP_ROM_PTR(&pin_PE01) },
#endif
};
MP_DEFINE_CONST_DICT(mcu_pin_globals, mcu_pin_globals_table);
