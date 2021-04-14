/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
 * Copyright (c) 2019 Artur Pacholec
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
// TODO
#include "py/mphal.h"
#include "py/obj.h"
#include "py/runtime.h"

#include "fsl_device_registers.h"

#include "reset.h"

#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/Processor.h"
#include "supervisor/shared/safe_mode.h"
#include "supervisor/shared/translate.h"

#define DBL_TAP_REG              SNVS->LPGPR[3]

void common_hal_mcu_delay_us(uint32_t delay) {
    mp_hal_delay_us(delay);
}

volatile uint32_t nesting_count = 0;
void common_hal_mcu_disable_interrupts(void) {
    __disable_irq();
    __DMB();
    nesting_count++;
}

void HardFault_Handler(void);
void common_hal_mcu_enable_interrupts(void) {
    if (nesting_count == 0) {
        // This is very very bad because it means there was mismatched disable/enables so we
        // "HardFault".
        HardFault_Handler();
    }
    nesting_count--;
    if (nesting_count > 0) {
        return;
    }
    __DMB();
    __enable_irq();
}

void common_hal_mcu_on_next_reset(mcu_runmode_t runmode) {
    if (runmode == RUNMODE_BOOTLOADER) {
        if (!bootloader_available()) {
            mp_raise_ValueError(translate("Cannot reset into bootloader because no bootloader is present."));
        }
        // Pretend to be the first of the two reset presses needed to enter the
        // bootloader. That way one reset will end in the bootloader.
        DBL_TAP_REG = DBL_TAP_MAGIC;
    } else {
        // Set up the default.
        DBL_TAP_REG = DBL_TAP_MAGIC_QUICK_BOOT;
    }
    if (runmode == RUNMODE_SAFE_MODE) {
        safe_mode_on_next_reset(PROGRAMMATIC_SAFE_MODE);
    }
}

void common_hal_mcu_reset(void) {
    NVIC_SystemReset();
}

// The singleton microcontroller.Processor object, bound to microcontroller.cpu
// It currently only has properties, and no state.
const mcu_processor_obj_t common_hal_mcu_processor_obj = {
    .base = {
        .type = &mcu_processor_type,
    },
};

// NVM is only available on Express boards for now.
#if CIRCUITPY_INTERNAL_NVM_SIZE > 0
// The singleton nvm.ByteArray object.
const nvm_bytearray_obj_t common_hal_mcu_nvm_obj = {
    .base = {
        .type = &nvm_bytearray_type,
    },
    .len = CIRCUITPY_INTERNAL_NVM_SIZE,
    .start_address = (uint8_t *)(FLASH_SIZE - CIRCUITPY_INTERNAL_NVM_SIZE)
};
#endif

// This maps MCU pin names to pin objects.
// NOTE: for all i.MX chips, order MUST match _iomuxc_sw_mux_ctl_pad enum
STATIC const mp_rom_map_elem_t mcu_pin_global_dict_table[] = {
    #ifdef MIMXRT1011_SERIES
    { MP_ROM_QSTR(MP_QSTR_GPIO_AD_14), MP_ROM_PTR(&pin_GPIO_AD_14) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_AD_13), MP_ROM_PTR(&pin_GPIO_AD_13) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_AD_12), MP_ROM_PTR(&pin_GPIO_AD_12) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_AD_11), MP_ROM_PTR(&pin_GPIO_AD_11) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_AD_10), MP_ROM_PTR(&pin_GPIO_AD_10) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_AD_09), MP_ROM_PTR(&pin_GPIO_AD_09) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_AD_08), MP_ROM_PTR(&pin_GPIO_AD_08) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_AD_07), MP_ROM_PTR(&pin_GPIO_AD_07) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_AD_06), MP_ROM_PTR(&pin_GPIO_AD_06) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_AD_05), MP_ROM_PTR(&pin_GPIO_AD_05) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_AD_04), MP_ROM_PTR(&pin_GPIO_AD_04) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_AD_03), MP_ROM_PTR(&pin_GPIO_AD_03) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_AD_02), MP_ROM_PTR(&pin_GPIO_AD_02) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_AD_01), MP_ROM_PTR(&pin_GPIO_AD_01) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_AD_00), MP_ROM_PTR(&pin_GPIO_AD_00) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_SD_14), MP_ROM_PTR(&pin_GPIO_SD_14) }, // spooky ghost pin
    { MP_ROM_QSTR(MP_QSTR_GPIO_SD_13), MP_ROM_PTR(&pin_GPIO_SD_13) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_SD_12), MP_ROM_PTR(&pin_GPIO_SD_12) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_SD_11), MP_ROM_PTR(&pin_GPIO_SD_11) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_SD_10), MP_ROM_PTR(&pin_GPIO_SD_10) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_SD_09), MP_ROM_PTR(&pin_GPIO_SD_09) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_SD_08), MP_ROM_PTR(&pin_GPIO_SD_08) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_SD_07), MP_ROM_PTR(&pin_GPIO_SD_07) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_SD_06), MP_ROM_PTR(&pin_GPIO_SD_06) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_SD_05), MP_ROM_PTR(&pin_GPIO_SD_05) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_SD_04), MP_ROM_PTR(&pin_GPIO_SD_04) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_SD_03), MP_ROM_PTR(&pin_GPIO_SD_03) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_SD_02), MP_ROM_PTR(&pin_GPIO_SD_02) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_SD_01), MP_ROM_PTR(&pin_GPIO_SD_01) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_SD_00), MP_ROM_PTR(&pin_GPIO_SD_00) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_13), MP_ROM_PTR(&pin_GPIO_13) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_12), MP_ROM_PTR(&pin_GPIO_12) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_11), MP_ROM_PTR(&pin_GPIO_11) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_10), MP_ROM_PTR(&pin_GPIO_10) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_09), MP_ROM_PTR(&pin_GPIO_09) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_08), MP_ROM_PTR(&pin_GPIO_08) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_07), MP_ROM_PTR(&pin_GPIO_07) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_06), MP_ROM_PTR(&pin_GPIO_06) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_05), MP_ROM_PTR(&pin_GPIO_05) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_04), MP_ROM_PTR(&pin_GPIO_04) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_03), MP_ROM_PTR(&pin_GPIO_03) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_02), MP_ROM_PTR(&pin_GPIO_02) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_01), MP_ROM_PTR(&pin_GPIO_01) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_00), MP_ROM_PTR(&pin_GPIO_00) },
    #else
    { MP_ROM_QSTR(MP_QSTR_GPIO_EMC_00), MP_ROM_PTR(&pin_GPIO_EMC_00) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_EMC_01), MP_ROM_PTR(&pin_GPIO_EMC_01) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_EMC_02), MP_ROM_PTR(&pin_GPIO_EMC_02) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_EMC_03), MP_ROM_PTR(&pin_GPIO_EMC_03) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_EMC_04), MP_ROM_PTR(&pin_GPIO_EMC_04) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_EMC_05), MP_ROM_PTR(&pin_GPIO_EMC_05) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_EMC_06), MP_ROM_PTR(&pin_GPIO_EMC_06) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_EMC_07), MP_ROM_PTR(&pin_GPIO_EMC_07) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_EMC_08), MP_ROM_PTR(&pin_GPIO_EMC_08) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_EMC_09), MP_ROM_PTR(&pin_GPIO_EMC_09) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_EMC_10), MP_ROM_PTR(&pin_GPIO_EMC_10) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_EMC_11), MP_ROM_PTR(&pin_GPIO_EMC_11) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_EMC_12), MP_ROM_PTR(&pin_GPIO_EMC_12) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_EMC_13), MP_ROM_PTR(&pin_GPIO_EMC_13) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_EMC_14), MP_ROM_PTR(&pin_GPIO_EMC_14) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_EMC_15), MP_ROM_PTR(&pin_GPIO_EMC_15) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_EMC_16), MP_ROM_PTR(&pin_GPIO_EMC_16) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_EMC_17), MP_ROM_PTR(&pin_GPIO_EMC_17) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_EMC_18), MP_ROM_PTR(&pin_GPIO_EMC_18) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_EMC_19), MP_ROM_PTR(&pin_GPIO_EMC_19) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_EMC_20), MP_ROM_PTR(&pin_GPIO_EMC_20) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_EMC_21), MP_ROM_PTR(&pin_GPIO_EMC_21) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_EMC_22), MP_ROM_PTR(&pin_GPIO_EMC_22) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_EMC_23), MP_ROM_PTR(&pin_GPIO_EMC_23) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_EMC_24), MP_ROM_PTR(&pin_GPIO_EMC_24) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_EMC_25), MP_ROM_PTR(&pin_GPIO_EMC_25) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_EMC_26), MP_ROM_PTR(&pin_GPIO_EMC_26) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_EMC_27), MP_ROM_PTR(&pin_GPIO_EMC_27) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_EMC_28), MP_ROM_PTR(&pin_GPIO_EMC_28) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_EMC_29), MP_ROM_PTR(&pin_GPIO_EMC_29) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_EMC_30), MP_ROM_PTR(&pin_GPIO_EMC_30) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_EMC_31), MP_ROM_PTR(&pin_GPIO_EMC_31) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_EMC_32), MP_ROM_PTR(&pin_GPIO_EMC_32) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_EMC_33), MP_ROM_PTR(&pin_GPIO_EMC_33) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_EMC_34), MP_ROM_PTR(&pin_GPIO_EMC_34) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_EMC_35), MP_ROM_PTR(&pin_GPIO_EMC_35) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_EMC_36), MP_ROM_PTR(&pin_GPIO_EMC_36) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_EMC_37), MP_ROM_PTR(&pin_GPIO_EMC_37) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_EMC_38), MP_ROM_PTR(&pin_GPIO_EMC_38) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_EMC_39), MP_ROM_PTR(&pin_GPIO_EMC_39) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_EMC_40), MP_ROM_PTR(&pin_GPIO_EMC_40) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_EMC_41), MP_ROM_PTR(&pin_GPIO_EMC_41) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_AD_B0_00), MP_ROM_PTR(&pin_GPIO_AD_B0_00) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_AD_B0_01), MP_ROM_PTR(&pin_GPIO_AD_B0_01) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_AD_B0_02), MP_ROM_PTR(&pin_GPIO_AD_B0_02) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_AD_B0_03), MP_ROM_PTR(&pin_GPIO_AD_B0_03) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_AD_B0_04), MP_ROM_PTR(&pin_GPIO_AD_B0_04) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_AD_B0_05), MP_ROM_PTR(&pin_GPIO_AD_B0_05) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_AD_B0_06), MP_ROM_PTR(&pin_GPIO_AD_B0_06) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_AD_B0_07), MP_ROM_PTR(&pin_GPIO_AD_B0_07) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_AD_B0_08), MP_ROM_PTR(&pin_GPIO_AD_B0_08) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_AD_B0_09), MP_ROM_PTR(&pin_GPIO_AD_B0_09) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_AD_B0_10), MP_ROM_PTR(&pin_GPIO_AD_B0_10) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_AD_B0_11), MP_ROM_PTR(&pin_GPIO_AD_B0_11) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_AD_B0_12), MP_ROM_PTR(&pin_GPIO_AD_B0_12) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_AD_B0_13), MP_ROM_PTR(&pin_GPIO_AD_B0_13) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_AD_B0_14), MP_ROM_PTR(&pin_GPIO_AD_B0_14) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_AD_B0_15), MP_ROM_PTR(&pin_GPIO_AD_B0_15) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_AD_B1_00), MP_ROM_PTR(&pin_GPIO_AD_B1_00) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_AD_B1_01), MP_ROM_PTR(&pin_GPIO_AD_B1_01) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_AD_B1_02), MP_ROM_PTR(&pin_GPIO_AD_B1_02) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_AD_B1_03), MP_ROM_PTR(&pin_GPIO_AD_B1_03) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_AD_B1_04), MP_ROM_PTR(&pin_GPIO_AD_B1_04) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_AD_B1_05), MP_ROM_PTR(&pin_GPIO_AD_B1_05) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_AD_B1_06), MP_ROM_PTR(&pin_GPIO_AD_B1_06) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_AD_B1_07), MP_ROM_PTR(&pin_GPIO_AD_B1_07) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_AD_B1_08), MP_ROM_PTR(&pin_GPIO_AD_B1_08) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_AD_B1_09), MP_ROM_PTR(&pin_GPIO_AD_B1_09) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_AD_B1_10), MP_ROM_PTR(&pin_GPIO_AD_B1_10) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_AD_B1_11), MP_ROM_PTR(&pin_GPIO_AD_B1_11) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_AD_B1_12), MP_ROM_PTR(&pin_GPIO_AD_B1_12) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_AD_B1_13), MP_ROM_PTR(&pin_GPIO_AD_B1_13) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_AD_B1_14), MP_ROM_PTR(&pin_GPIO_AD_B1_14) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_AD_B1_15), MP_ROM_PTR(&pin_GPIO_AD_B1_15) },
    #ifdef MIMXRT1062_SERIES
    { MP_ROM_QSTR(MP_QSTR_GPIO_B0_00), MP_ROM_PTR(&pin_GPIO_B0_00) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_B0_01), MP_ROM_PTR(&pin_GPIO_B0_01) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_B0_02), MP_ROM_PTR(&pin_GPIO_B0_02) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_B0_03), MP_ROM_PTR(&pin_GPIO_B0_03) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_B0_04), MP_ROM_PTR(&pin_GPIO_B0_04) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_B0_05), MP_ROM_PTR(&pin_GPIO_B0_05) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_B0_06), MP_ROM_PTR(&pin_GPIO_B0_06) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_B0_07), MP_ROM_PTR(&pin_GPIO_B0_07) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_B0_08), MP_ROM_PTR(&pin_GPIO_B0_08) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_B0_09), MP_ROM_PTR(&pin_GPIO_B0_09) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_B0_10), MP_ROM_PTR(&pin_GPIO_B0_10) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_B0_11), MP_ROM_PTR(&pin_GPIO_B0_11) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_B0_12), MP_ROM_PTR(&pin_GPIO_B0_12) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_B0_13), MP_ROM_PTR(&pin_GPIO_B0_13) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_B0_14), MP_ROM_PTR(&pin_GPIO_B0_14) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_B0_15), MP_ROM_PTR(&pin_GPIO_B0_15) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_B1_00), MP_ROM_PTR(&pin_GPIO_B1_00) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_B1_01), MP_ROM_PTR(&pin_GPIO_B1_01) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_B1_02), MP_ROM_PTR(&pin_GPIO_B1_02) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_B1_03), MP_ROM_PTR(&pin_GPIO_B1_03) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_B1_04), MP_ROM_PTR(&pin_GPIO_B1_04) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_B1_05), MP_ROM_PTR(&pin_GPIO_B1_05) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_B1_06), MP_ROM_PTR(&pin_GPIO_B1_06) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_B1_07), MP_ROM_PTR(&pin_GPIO_B1_07) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_B1_08), MP_ROM_PTR(&pin_GPIO_B1_08) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_B1_09), MP_ROM_PTR(&pin_GPIO_B1_09) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_B1_10), MP_ROM_PTR(&pin_GPIO_B1_10) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_B1_11), MP_ROM_PTR(&pin_GPIO_B1_11) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_B1_12), MP_ROM_PTR(&pin_GPIO_B1_12) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_B1_13), MP_ROM_PTR(&pin_GPIO_B1_13) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_B1_14), MP_ROM_PTR(&pin_GPIO_B1_14) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_B1_15), MP_ROM_PTR(&pin_GPIO_B1_15) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_GPIO_SD_B0_00), MP_ROM_PTR(&pin_GPIO_SD_B0_00) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_SD_B0_01), MP_ROM_PTR(&pin_GPIO_SD_B0_01) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_SD_B0_02), MP_ROM_PTR(&pin_GPIO_SD_B0_02) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_SD_B0_03), MP_ROM_PTR(&pin_GPIO_SD_B0_03) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_SD_B0_04), MP_ROM_PTR(&pin_GPIO_SD_B0_04) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_SD_B0_05), MP_ROM_PTR(&pin_GPIO_SD_B0_05) },
    #ifdef MIMXRT1021_SERIES
    { MP_ROM_QSTR(MP_QSTR_GPIO_SD_B0_06), MP_ROM_PTR(&pin_GPIO_SD_B0_06) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_GPIO_SD_B1_00), MP_ROM_PTR(&pin_GPIO_SD_B1_00) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_SD_B1_01), MP_ROM_PTR(&pin_GPIO_SD_B1_01) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_SD_B1_02), MP_ROM_PTR(&pin_GPIO_SD_B1_02) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_SD_B1_03), MP_ROM_PTR(&pin_GPIO_SD_B1_03) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_SD_B1_04), MP_ROM_PTR(&pin_GPIO_SD_B1_04) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_SD_B1_05), MP_ROM_PTR(&pin_GPIO_SD_B1_05) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_SD_B1_06), MP_ROM_PTR(&pin_GPIO_SD_B1_06) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_SD_B1_07), MP_ROM_PTR(&pin_GPIO_SD_B1_07) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_SD_B1_08), MP_ROM_PTR(&pin_GPIO_SD_B1_08) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_SD_B1_09), MP_ROM_PTR(&pin_GPIO_SD_B1_09) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_SD_B1_10), MP_ROM_PTR(&pin_GPIO_SD_B1_10) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_SD_B1_11), MP_ROM_PTR(&pin_GPIO_SD_B1_11) },
    #endif
};
MP_DEFINE_CONST_DICT(mcu_pin_globals, mcu_pin_global_dict_table);
