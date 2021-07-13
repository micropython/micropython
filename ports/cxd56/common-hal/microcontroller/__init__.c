/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright 2019 Sony Semiconductor Solutions Corporation
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

#include <stdbool.h> // for cxd56_clock.h
#include <cxd56_clock.h>
#include <sys/boardctl.h>

#include "py/mphal.h"
#include "py/runtime.h"

#include "shared-bindings/microcontroller/__init__.h"
#include "common-hal/microcontroller/Pin.h"
#include "supervisor/filesystem.h"
#include "supervisor/shared/safe_mode.h"

// The singleton microcontroller.Processor object, bound to microcontroller.cpu
// It currently only has properties, and no state.
const mcu_processor_obj_t common_hal_mcu_processor_obj = {
    .base = {
        .type = &mcu_processor_type,
    },
};

#define DELAY_CORRECTION    (700)

void common_hal_mcu_delay_us(uint32_t delay) {
    if (delay) {
        unsigned long long ticks = cxd56_get_cpu_baseclk() / 1000000L * delay;
        if (ticks < DELAY_CORRECTION) {
            return;                           // delay time already used in calculation

        }
        ticks -= DELAY_CORRECTION;
        ticks /= 6;
        // following loop takes 6 cycles
        do {
            __asm__ __volatile__ ("nop");
        } while (--ticks);
    }
}

void common_hal_mcu_disable_interrupts(void) {
    __asm volatile ("cpsid i" : : : "memory");
}

void common_hal_mcu_enable_interrupts(void) {
    __asm volatile ("cpsie i" : : : "memory");
}

void common_hal_mcu_on_next_reset(mcu_runmode_t runmode) {
    if (runmode == RUNMODE_BOOTLOADER) {
        mp_raise_ValueError(translate("Cannot reset into bootloader because no bootloader is present."));
    } else if (runmode == RUNMODE_SAFE_MODE) {
        safe_mode_on_next_reset(PROGRAMMATIC_SAFE_MODE);
    }
}

void common_hal_mcu_reset(void) {
    filesystem_flush();
    boardctl(BOARDIOC_RESET, 0);
}

STATIC const mp_rom_map_elem_t mcu_pin_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR_UART2_RXD), MP_ROM_PTR(&pin_UART2_RXD) },
    { MP_ROM_QSTR(MP_QSTR_UART2_TXD), MP_ROM_PTR(&pin_UART2_TXD) },
    { MP_ROM_QSTR(MP_QSTR_HIF_IRQ_OUT), MP_ROM_PTR(&pin_HIF_IRQ_OUT) },
    { MP_ROM_QSTR(MP_QSTR_PWM3), MP_ROM_PTR(&pin_PWM3) },
    { MP_ROM_QSTR(MP_QSTR_SPI2_MOSI), MP_ROM_PTR(&pin_SPI2_MOSI) },
    { MP_ROM_QSTR(MP_QSTR_PWM1), MP_ROM_PTR(&pin_PWM1) },
    { MP_ROM_QSTR(MP_QSTR_PWM0), MP_ROM_PTR(&pin_PWM0) },
    { MP_ROM_QSTR(MP_QSTR_SPI3_CS1_X), MP_ROM_PTR(&pin_SPI3_CS1_X) },
    { MP_ROM_QSTR(MP_QSTR_SPI2_MISO), MP_ROM_PTR(&pin_SPI2_MISO) },
    { MP_ROM_QSTR(MP_QSTR_PWM2), MP_ROM_PTR(&pin_PWM2) },
    { MP_ROM_QSTR(MP_QSTR_SPI4_CS_X), MP_ROM_PTR(&pin_SPI4_CS_X) },
    { MP_ROM_QSTR(MP_QSTR_SPI4_MOSI), MP_ROM_PTR(&pin_SPI4_MOSI) },
    { MP_ROM_QSTR(MP_QSTR_SPI4_MISO), MP_ROM_PTR(&pin_SPI4_MISO) },
    { MP_ROM_QSTR(MP_QSTR_SPI4_SCK), MP_ROM_PTR(&pin_SPI4_SCK) },
    { MP_ROM_QSTR(MP_QSTR_I2C0_BDT), MP_ROM_PTR(&pin_I2C0_BDT) },
    { MP_ROM_QSTR(MP_QSTR_I2C0_BCK), MP_ROM_PTR(&pin_I2C0_BCK) },
    { MP_ROM_QSTR(MP_QSTR_EMMC_DATA0), MP_ROM_PTR(&pin_EMMC_DATA0) },
    { MP_ROM_QSTR(MP_QSTR_EMMC_DATA1), MP_ROM_PTR(&pin_EMMC_DATA1) },
    { MP_ROM_QSTR(MP_QSTR_I2S0_DATA_OUT), MP_ROM_PTR(&pin_I2S0_DATA_OUT) },
    { MP_ROM_QSTR(MP_QSTR_I2S0_DATA_IN), MP_ROM_PTR(&pin_I2S0_DATA_IN) },
    { MP_ROM_QSTR(MP_QSTR_EMMC_DATA2), MP_ROM_PTR(&pin_EMMC_DATA2) },
    { MP_ROM_QSTR(MP_QSTR_EMMC_DATA3), MP_ROM_PTR(&pin_EMMC_DATA3) },
    { MP_ROM_QSTR(MP_QSTR_SEN_IRQ_IN), MP_ROM_PTR(&pin_SEN_IRQ_IN) },
    { MP_ROM_QSTR(MP_QSTR_EMMC_CLK), MP_ROM_PTR(&pin_EMMC_CLK) },
    { MP_ROM_QSTR(MP_QSTR_EMMC_CMD), MP_ROM_PTR(&pin_EMMC_CMD) },
    { MP_ROM_QSTR(MP_QSTR_I2S0_LRCK), MP_ROM_PTR(&pin_I2S0_LRCK) },
    { MP_ROM_QSTR(MP_QSTR_I2S0_BCK), MP_ROM_PTR(&pin_I2S0_BCK) },
    { MP_ROM_QSTR(MP_QSTR_UART2_CTS), MP_ROM_PTR(&pin_UART2_CTS) },
    { MP_ROM_QSTR(MP_QSTR_UART2_RTS), MP_ROM_PTR(&pin_UART2_RTS) },
    { MP_ROM_QSTR(MP_QSTR_I2S1_BCK), MP_ROM_PTR(&pin_I2S1_BCK) },
    { MP_ROM_QSTR(MP_QSTR_I2S1_LRCK), MP_ROM_PTR(&pin_I2S1_LRCK) },
    { MP_ROM_QSTR(MP_QSTR_I2S1_DATA_IN), MP_ROM_PTR(&pin_I2S1_DATA_IN) },
    { MP_ROM_QSTR(MP_QSTR_I2S1_DATA_OUT), MP_ROM_PTR(&pin_I2S1_DATA_OUT) },
    { MP_ROM_QSTR(MP_QSTR_LPADC0), MP_ROM_PTR(&pin_LPADC0) },
    { MP_ROM_QSTR(MP_QSTR_LPADC1), MP_ROM_PTR(&pin_LPADC1) },
    { MP_ROM_QSTR(MP_QSTR_LPADC2), MP_ROM_PTR(&pin_LPADC2) },
    { MP_ROM_QSTR(MP_QSTR_LPADC3), MP_ROM_PTR(&pin_LPADC3) },
    { MP_ROM_QSTR(MP_QSTR_HPADC0), MP_ROM_PTR(&pin_HPADC0) },
    { MP_ROM_QSTR(MP_QSTR_HPADC1), MP_ROM_PTR(&pin_HPADC1) },
};
MP_DEFINE_CONST_DICT(mcu_pin_globals, mcu_pin_globals_table);
