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

#include "py/obj.h"
#include "py/mphal.h"
#include "py/runtime.h"

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/microcontroller/Processor.h"

#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/Processor.h"
#include "shared-bindings/nvm/ByteArray.h"

#include "supervisor/filesystem.h"
#include "supervisor/shared/safe_mode.h"

#include "freertos/FreeRTOS.h"

#include "soc/rtc_cntl_reg.h"
#include "esp_private/system_internal.h"

#ifdef CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/rom/rtc.h"
#elif defined(CONFIG_IDF_TARGET_ESP32S2)
#include "esp32s2/rom/rtc.h"
#include "esp32s2/rom/usb/usb_persist.h"
#include "esp32s2/rom/usb/chip_usb_dw_wrapper.h"
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
#include "esp32s3/rom/rtc.h"
#include "esp32s3/rom/usb/usb_persist.h"
#include "esp32s3/rom/usb/chip_usb_dw_wrapper.h"
#endif

void common_hal_mcu_delay_us(uint32_t delay) {
    mp_hal_delay_us(delay);
}

volatile uint32_t nesting_count = 0;
static portMUX_TYPE cp_mutex = portMUX_INITIALIZER_UNLOCKED;

void common_hal_mcu_disable_interrupts(void) {
    if (nesting_count == 0) {
        portENTER_CRITICAL(&cp_mutex);
    }
    nesting_count++;
}

void common_hal_mcu_enable_interrupts(void) {
    if (nesting_count == 0) {
        // Maybe log here because it's very bad.
    }
    nesting_count--;
    if (nesting_count > 0) {
        return;
    }
    portEXIT_CRITICAL(&cp_mutex);
}

void common_hal_mcu_on_next_reset(mcu_runmode_t runmode) {
    switch (runmode) {
        case RUNMODE_UF2:
            #ifndef CONFIG_IDF_TARGET_ESP32C3
            // 0x11F2 is APP_REQUEST_UF2_RESET_HINT & is defined by TinyUF2
            esp_reset_reason_set_hint(0x11F2);
            #endif
            break;
        case RUNMODE_NORMAL:
            // revert back to normal boot
            REG_WRITE(RTC_RESET_CAUSE_REG, 0);  // reset uf2
            REG_WRITE(RTC_CNTL_STORE0_REG, 0);  // reset safe mode
            REG_WRITE(RTC_CNTL_OPTION1_REG, 0); // reset bootloader
            break;
        case RUNMODE_SAFE_MODE:
            // enter safe mode on next boot
            safe_mode_on_next_reset(PROGRAMMATIC_SAFE_MODE);
            break;
        case RUNMODE_BOOTLOADER:
            // DFU download
            #ifndef CONFIG_IDF_TARGET_ESP32C3
            chip_usb_set_persist_flags(USBDC_BOOT_DFU);
            #endif
            REG_WRITE(RTC_CNTL_OPTION1_REG, RTC_CNTL_FORCE_DOWNLOAD_BOOT);
            break;
        default:
            break;
    }
}

void NORETURN common_hal_mcu_reset(void) {
    filesystem_flush(); // TODO: implement as part of flash improvements
    esp_restart();
}

// The singleton microcontroller.Processor object, bound to microcontroller.cpu
// It currently only has properties, and no state.
const mcu_processor_obj_t common_hal_mcu_processor_obj = {
    .base = {
        .type = &mcu_processor_type,
    },
};

#if CIRCUITPY_INTERNAL_NVM_SIZE > 0
// The singleton nvm.ByteArray object.
const nvm_bytearray_obj_t common_hal_mcu_nvm_obj = {
    .base = {
        .type = &nvm_bytearray_type,
    },
    .start_address = (uint8_t *)CIRCUITPY_INTERNAL_NVM_START_ADDR,
    .len = CIRCUITPY_INTERNAL_NVM_SIZE,
};
#endif

#if CIRCUITPY_WATCHDOG
// The singleton watchdog.WatchDogTimer object.
watchdog_watchdogtimer_obj_t common_hal_mcu_watchdogtimer_obj = {
    .base = {
        .type = &watchdog_watchdogtimer_type,
    },
    .timeout = 0.0f,
    .mode = WATCHDOGMODE_NONE,
};
#endif

// This maps MCU pin names to pin objects.
STATIC const mp_rom_map_elem_t mcu_pin_global_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_GPIO0), MP_ROM_PTR(&pin_GPIO0) },
    { MP_ROM_QSTR(MP_QSTR_GPIO1), MP_ROM_PTR(&pin_GPIO1) },
    { MP_ROM_QSTR(MP_QSTR_GPIO2), MP_ROM_PTR(&pin_GPIO2) },
    { MP_ROM_QSTR(MP_QSTR_GPIO3), MP_ROM_PTR(&pin_GPIO3) },
    { MP_ROM_QSTR(MP_QSTR_GPIO4), MP_ROM_PTR(&pin_GPIO4) },
    { MP_ROM_QSTR(MP_QSTR_GPIO5), MP_ROM_PTR(&pin_GPIO5) },
    { MP_ROM_QSTR(MP_QSTR_GPIO6), MP_ROM_PTR(&pin_GPIO6) },
    { MP_ROM_QSTR(MP_QSTR_GPIO7), MP_ROM_PTR(&pin_GPIO7) },
    { MP_ROM_QSTR(MP_QSTR_GPIO8), MP_ROM_PTR(&pin_GPIO8) },
    { MP_ROM_QSTR(MP_QSTR_GPIO9), MP_ROM_PTR(&pin_GPIO9) },
    { MP_ROM_QSTR(MP_QSTR_GPIO10), MP_ROM_PTR(&pin_GPIO10) },
    { MP_ROM_QSTR(MP_QSTR_GPIO11), MP_ROM_PTR(&pin_GPIO11) },
    { MP_ROM_QSTR(MP_QSTR_GPIO12), MP_ROM_PTR(&pin_GPIO12) },
    { MP_ROM_QSTR(MP_QSTR_GPIO13), MP_ROM_PTR(&pin_GPIO13) },
    { MP_ROM_QSTR(MP_QSTR_GPIO14), MP_ROM_PTR(&pin_GPIO14) },
    { MP_ROM_QSTR(MP_QSTR_GPIO15), MP_ROM_PTR(&pin_GPIO15) },
    { MP_ROM_QSTR(MP_QSTR_GPIO16), MP_ROM_PTR(&pin_GPIO16) },
    { MP_ROM_QSTR(MP_QSTR_GPIO17), MP_ROM_PTR(&pin_GPIO17) },
    { MP_ROM_QSTR(MP_QSTR_GPIO18), MP_ROM_PTR(&pin_GPIO18) },
    { MP_ROM_QSTR(MP_QSTR_GPIO19), MP_ROM_PTR(&pin_GPIO19) },
    { MP_ROM_QSTR(MP_QSTR_GPIO20), MP_ROM_PTR(&pin_GPIO20) },
    { MP_ROM_QSTR(MP_QSTR_GPIO21), MP_ROM_PTR(&pin_GPIO21) },
    #ifndef CONFIG_IDF_TARGET_ESP32C3
    { MP_ROM_QSTR(MP_QSTR_GPIO26), MP_ROM_PTR(&pin_GPIO26) },
    { MP_ROM_QSTR(MP_QSTR_GPIO27), MP_ROM_PTR(&pin_GPIO27) },
    { MP_ROM_QSTR(MP_QSTR_GPIO28), MP_ROM_PTR(&pin_GPIO28) },
    { MP_ROM_QSTR(MP_QSTR_GPIO29), MP_ROM_PTR(&pin_GPIO29) },
    { MP_ROM_QSTR(MP_QSTR_GPIO30), MP_ROM_PTR(&pin_GPIO30) },
    { MP_ROM_QSTR(MP_QSTR_GPIO31), MP_ROM_PTR(&pin_GPIO31) },
    { MP_ROM_QSTR(MP_QSTR_GPIO32), MP_ROM_PTR(&pin_GPIO32) },
    { MP_ROM_QSTR(MP_QSTR_GPIO33), MP_ROM_PTR(&pin_GPIO33) },
    { MP_ROM_QSTR(MP_QSTR_GPIO34), MP_ROM_PTR(&pin_GPIO34) },
    { MP_ROM_QSTR(MP_QSTR_GPIO35), MP_ROM_PTR(&pin_GPIO35) },
    { MP_ROM_QSTR(MP_QSTR_GPIO36), MP_ROM_PTR(&pin_GPIO36) },
    { MP_ROM_QSTR(MP_QSTR_GPIO37), MP_ROM_PTR(&pin_GPIO37) },
    { MP_ROM_QSTR(MP_QSTR_GPIO38), MP_ROM_PTR(&pin_GPIO38) },
    { MP_ROM_QSTR(MP_QSTR_GPIO39), MP_ROM_PTR(&pin_GPIO39) },
    { MP_ROM_QSTR(MP_QSTR_GPIO40), MP_ROM_PTR(&pin_GPIO40) },
    { MP_ROM_QSTR(MP_QSTR_GPIO41), MP_ROM_PTR(&pin_GPIO41) },
    { MP_ROM_QSTR(MP_QSTR_GPIO42), MP_ROM_PTR(&pin_GPIO42) },
    { MP_ROM_QSTR(MP_QSTR_GPIO43), MP_ROM_PTR(&pin_GPIO43) },
    { MP_ROM_QSTR(MP_QSTR_GPIO44), MP_ROM_PTR(&pin_GPIO44) },
    { MP_ROM_QSTR(MP_QSTR_GPIO45), MP_ROM_PTR(&pin_GPIO45) },
    { MP_ROM_QSTR(MP_QSTR_GPIO46), MP_ROM_PTR(&pin_GPIO46) },
    #endif
};
MP_DEFINE_CONST_DICT(mcu_pin_globals, mcu_pin_global_dict_table);
