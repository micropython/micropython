// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

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

#include "esp_private/system_internal.h"

#if defined(CONFIG_IDF_TARGET_ESP32)
#include "soc/rtc_cntl_reg.h"
#include "esp32/rom/rtc.h"
#elif defined(CONFIG_IDF_TARGET_ESP32C2)
#include "soc/rtc_cntl_reg.h"
#include "esp32c2/rom/rtc.h"
#elif defined(CONFIG_IDF_TARGET_ESP32C3)
#include "soc/rtc_cntl_reg.h"
#include "esp32c3/rom/rtc.h"
#elif defined(CONFIG_IDF_TARGET_ESP32C6)
#include "soc/lp_aon_reg.h"
#include "esp32c6/rom/rtc.h"
#elif defined(CONFIG_IDF_TARGET_ESP32S2)
#include "soc/rtc_cntl_reg.h"
#include "esp32s2/rom/rtc.h"
#include "esp32s2/rom/usb/usb_persist.h"
#include "esp32s2/rom/usb/chip_usb_dw_wrapper.h"
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
#include "soc/rtc_cntl_reg.h"
#include "esp32s3/rom/rtc.h"
#include "esp32s3/rom/usb/usb_persist.h"
#include "esp32s3/rom/usb/chip_usb_dw_wrapper.h"
#elif defined(CONFIG_IDF_TARGET_ESP32H2)
#include "soc/lp_aon_reg.h"
#include "esp32h2/rom/rtc.h"
#else
#error No known CONFIG_IDF_TARGET_xxx found
#endif

void common_hal_mcu_delay_us(uint32_t delay) {
    mp_hal_delay_us(delay);
}

volatile uint32_t nesting_count = 0;
static portMUX_TYPE cp_mutex = portMUX_INITIALIZER_UNLOCKED;

void common_hal_mcu_disable_interrupts(void) {
    assert(xPortGetCoreID() == CONFIG_ESP_MAIN_TASK_AFFINITY);
    if (nesting_count == 0) {
        portENTER_CRITICAL(&cp_mutex);
    }
    nesting_count++;
}

void common_hal_mcu_enable_interrupts(void) {
    assert(xPortGetCoreID() == CONFIG_ESP_MAIN_TASK_AFFINITY);
    assert(nesting_count > 0);
    nesting_count--;
    if (nesting_count > 0) {
        return;
    }
    portEXIT_CRITICAL(&cp_mutex);
}

void common_hal_mcu_on_next_reset(mcu_runmode_t runmode) {
    switch (runmode) {
        case RUNMODE_UF2:
            #if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32C3) || defined(CONFIG_IDF_TARGET_ESP32C6)
            mp_arg_error_invalid(MP_QSTR_run_mode);
            #else
            // 0x11F2 is APP_REQUEST_UF2_RESET_HINT & is defined by TinyUF2
            esp_reset_reason_set_hint(0x11F2);
            #endif
            break;
        case RUNMODE_NORMAL:
            // revert back to normal boot
            #if defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)
            REG_WRITE(RTC_RESET_CAUSE_REG, 0);  // reset uf2
            #endif
            #ifdef SOC_LP_AON_SUPPORTED
            REG_WRITE(LP_AON_STORE0_REG, 0);  // reset safe mode
            #else
            REG_WRITE(RTC_CNTL_STORE0_REG, 0);  // reset safe mode
            #endif
            #if !defined(CONFIG_IDF_TARGET_ESP32)
            #ifdef SOC_LP_AON_SUPPORTED
            REG_WRITE(LP_AON_SYS_CFG_REG, 0); // reset bootloader
            #else
            REG_WRITE(RTC_CNTL_OPTION1_REG, 0); // reset bootloader
            #endif
            #endif
            break;
        case RUNMODE_SAFE_MODE:
            // enter safe mode on next boot
            safe_mode_on_next_reset(SAFE_MODE_PROGRAMMATIC);
            break;
        case RUNMODE_BOOTLOADER:
            // DFU download
            #if defined(CONFIG_IDF_TARGET_ESP32)
            mp_arg_error_invalid(MP_QSTR_run_mode);
            #else
            #if defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)
            chip_usb_set_persist_flags(USBDC_BOOT_DFU);
            #endif
            #ifdef SOC_LP_AON_SUPPORTED
            REG_WRITE(LP_AON_SYS_CFG_REG, LP_AON_FORCE_DOWNLOAD_BOOT);
            #else
            REG_WRITE(RTC_CNTL_OPTION1_REG, RTC_CNTL_FORCE_DOWNLOAD_BOOT);
            #endif
            #endif
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
static const mp_rom_map_elem_t mcu_pin_global_dict_table[] = {
    #ifdef GPIO0_EXISTS
    { MP_ROM_QSTR(MP_QSTR_GPIO0), MP_ROM_PTR(&pin_GPIO0) },
    #endif
    #ifdef GPIO1_EXISTS
    { MP_ROM_QSTR(MP_QSTR_GPIO1), MP_ROM_PTR(&pin_GPIO1) },
    #endif
    #ifdef GPIO2_EXISTS
    { MP_ROM_QSTR(MP_QSTR_GPIO2), MP_ROM_PTR(&pin_GPIO2) },
    #endif
    #ifdef GPIO3_EXISTS
    { MP_ROM_QSTR(MP_QSTR_GPIO3), MP_ROM_PTR(&pin_GPIO3) },
    #endif
    #ifdef GPIO4_EXISTS
    { MP_ROM_QSTR(MP_QSTR_GPIO4), MP_ROM_PTR(&pin_GPIO4) },
    #endif
    #ifdef GPIO5_EXISTS
    { MP_ROM_QSTR(MP_QSTR_GPIO5), MP_ROM_PTR(&pin_GPIO5) },
    #endif
    #ifdef GPIO6_EXISTS
    { MP_ROM_QSTR(MP_QSTR_GPIO6), MP_ROM_PTR(&pin_GPIO6) },
    #endif
    #ifdef GPIO7_EXISTS
    { MP_ROM_QSTR(MP_QSTR_GPIO7), MP_ROM_PTR(&pin_GPIO7) },
    #endif
    #ifdef GPIO8_EXISTS
    { MP_ROM_QSTR(MP_QSTR_GPIO8), MP_ROM_PTR(&pin_GPIO8) },
    #endif
    #ifdef GPIO9_EXISTS
    { MP_ROM_QSTR(MP_QSTR_GPIO9), MP_ROM_PTR(&pin_GPIO9) },
    #endif
    #ifdef GPIO10_EXISTS
    { MP_ROM_QSTR(MP_QSTR_GPIO10), MP_ROM_PTR(&pin_GPIO10) },
    #endif
    #ifdef GPIO11_EXISTS
    { MP_ROM_QSTR(MP_QSTR_GPIO11), MP_ROM_PTR(&pin_GPIO11) },
    #endif
    #ifdef GPIO12_EXISTS
    { MP_ROM_QSTR(MP_QSTR_GPIO12), MP_ROM_PTR(&pin_GPIO12) },
    #endif
    #ifdef GPIO13_EXISTS
    { MP_ROM_QSTR(MP_QSTR_GPIO13), MP_ROM_PTR(&pin_GPIO13) },
    #endif
    #ifdef GPIO14_EXISTS
    { MP_ROM_QSTR(MP_QSTR_GPIO14), MP_ROM_PTR(&pin_GPIO14) },
    #endif
    #ifdef GPIO15_EXISTS
    { MP_ROM_QSTR(MP_QSTR_GPIO15), MP_ROM_PTR(&pin_GPIO15) },
    #endif
    #ifdef GPIO16_EXISTS
    { MP_ROM_QSTR(MP_QSTR_GPIO16), MP_ROM_PTR(&pin_GPIO16) },
    #endif
    #ifdef GPIO17_EXISTS
    { MP_ROM_QSTR(MP_QSTR_GPIO17), MP_ROM_PTR(&pin_GPIO17) },
    #endif
    #ifdef GPIO18_EXISTS
    { MP_ROM_QSTR(MP_QSTR_GPIO18), MP_ROM_PTR(&pin_GPIO18) },
    #endif
    #ifdef GPIO19_EXISTS
    { MP_ROM_QSTR(MP_QSTR_GPIO19), MP_ROM_PTR(&pin_GPIO19) },
    #endif
    #ifdef GPIO20_EXISTS
    { MP_ROM_QSTR(MP_QSTR_GPIO20), MP_ROM_PTR(&pin_GPIO20) },
    #endif
    #ifdef GPIO21_EXISTS
    { MP_ROM_QSTR(MP_QSTR_GPIO21), MP_ROM_PTR(&pin_GPIO21) },
    #endif
    #ifdef GPIO22_EXISTS
    { MP_ROM_QSTR(MP_QSTR_GPIO22), MP_ROM_PTR(&pin_GPIO22) },
    #endif
    #ifdef GPIO23_EXISTS
    { MP_ROM_QSTR(MP_QSTR_GPIO23), MP_ROM_PTR(&pin_GPIO23) },
    #endif
    #ifdef GPIO24_EXISTS
    { MP_ROM_QSTR(MP_QSTR_GPIO24), MP_ROM_PTR(&pin_GPIO24) },
    #endif
    #ifdef GPIO25_EXISTS
    { MP_ROM_QSTR(MP_QSTR_GPIO25), MP_ROM_PTR(&pin_GPIO25) },
    #endif
    #ifdef GPIO26_EXISTS
    { MP_ROM_QSTR(MP_QSTR_GPIO26), MP_ROM_PTR(&pin_GPIO26) },
    #endif
    #ifdef GPIO27_EXISTS
    { MP_ROM_QSTR(MP_QSTR_GPIO27), MP_ROM_PTR(&pin_GPIO27) },
    #endif
    #ifdef GPIO28_EXISTS
    { MP_ROM_QSTR(MP_QSTR_GPIO28), MP_ROM_PTR(&pin_GPIO28) },
    #endif
    #ifdef GPIO29_EXISTS
    { MP_ROM_QSTR(MP_QSTR_GPIO29), MP_ROM_PTR(&pin_GPIO29) },
    #endif
    #ifdef GPIO30_EXISTS
    { MP_ROM_QSTR(MP_QSTR_GPIO30), MP_ROM_PTR(&pin_GPIO30) },
    #endif
    #ifdef GPIO31_EXISTS
    { MP_ROM_QSTR(MP_QSTR_GPIO31), MP_ROM_PTR(&pin_GPIO31) },
    #endif
    #ifdef GPIO32_EXISTS
    { MP_ROM_QSTR(MP_QSTR_GPIO32), MP_ROM_PTR(&pin_GPIO32) },
    #endif
    #ifdef GPIO33_EXISTS
    { MP_ROM_QSTR(MP_QSTR_GPIO33), MP_ROM_PTR(&pin_GPIO33) },
    #endif
    #ifdef GPIO34_EXISTS
    { MP_ROM_QSTR(MP_QSTR_GPIO34), MP_ROM_PTR(&pin_GPIO34) },
    #endif
    #ifdef GPIO35_EXISTS
    { MP_ROM_QSTR(MP_QSTR_GPIO35), MP_ROM_PTR(&pin_GPIO35) },
    #endif
    #ifdef GPIO36_EXISTS
    { MP_ROM_QSTR(MP_QSTR_GPIO36), MP_ROM_PTR(&pin_GPIO36) },
    #endif
    #ifdef GPIO37_EXISTS
    { MP_ROM_QSTR(MP_QSTR_GPIO37), MP_ROM_PTR(&pin_GPIO37) },
    #endif
    #ifdef GPIO38_EXISTS
    { MP_ROM_QSTR(MP_QSTR_GPIO38), MP_ROM_PTR(&pin_GPIO38) },
    #endif
    #ifdef GPIO39_EXISTS
    { MP_ROM_QSTR(MP_QSTR_GPIO39), MP_ROM_PTR(&pin_GPIO39) },
    #endif
    #ifdef GPIO40_EXISTS
    { MP_ROM_QSTR(MP_QSTR_GPIO40), MP_ROM_PTR(&pin_GPIO40) },
    #endif
    #ifdef GPIO41_EXISTS
    { MP_ROM_QSTR(MP_QSTR_GPIO41), MP_ROM_PTR(&pin_GPIO41) },
    #endif
    #ifdef GPIO42_EXISTS
    { MP_ROM_QSTR(MP_QSTR_GPIO42), MP_ROM_PTR(&pin_GPIO42) },
    #endif
    #ifdef GPIO43_EXISTS
    { MP_ROM_QSTR(MP_QSTR_GPIO43), MP_ROM_PTR(&pin_GPIO43) },
    #endif
    #ifdef GPIO44_EXISTS
    { MP_ROM_QSTR(MP_QSTR_GPIO44), MP_ROM_PTR(&pin_GPIO44) },
    #endif
    #ifdef GPIO45_EXISTS
    { MP_ROM_QSTR(MP_QSTR_GPIO45), MP_ROM_PTR(&pin_GPIO45) },
    #endif
    #ifdef GPIO46_EXISTS
    { MP_ROM_QSTR(MP_QSTR_GPIO46), MP_ROM_PTR(&pin_GPIO46) },
    #endif
    #ifdef GPIO47_EXISTS
    { MP_ROM_QSTR(MP_QSTR_GPIO47), MP_ROM_PTR(&pin_GPIO47) },
    #endif
    #ifdef GPIO48_EXISTS
    { MP_ROM_QSTR(MP_QSTR_GPIO48), MP_ROM_PTR(&pin_GPIO48) },
    #endif
};
MP_DEFINE_CONST_DICT(mcu_pin_globals, mcu_pin_global_dict_table);
