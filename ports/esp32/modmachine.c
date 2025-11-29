/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * Development of the code in this file was sponsored by Microbric Pty Ltd
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2023 Damien P. George
 * Copyright (c) 2016 Paul Sokolovsky
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

// This file is never compiled standalone, it's included directly from
// extmod/modmachine.c via MICROPY_PY_MACHINE_INCLUDEFILE.

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_mac.h"
#include "esp_sleep.h"
#include "esp_pm.h"

#include "modmachine.h"
#include "machine_rtc.h"

#if MICROPY_HW_ENABLE_SDCARD
#define MICROPY_PY_MACHINE_SDCARD_ENTRY { MP_ROM_QSTR(MP_QSTR_SDCard), MP_ROM_PTR(&machine_sdcard_type) },
#else
#define MICROPY_PY_MACHINE_SDCARD_ENTRY
#endif

#if SOC_TOUCH_SENSOR_SUPPORTED
#define MICROPY_PY_MACHINE_TOUCH_PAD_ENTRY { MP_ROM_QSTR(MP_QSTR_TouchPad), MP_ROM_PTR(&machine_touchpad_type) },
#else
#define MICROPY_PY_MACHINE_TOUCH_PAD_ENTRY
#endif

#define MICROPY_PY_MACHINE_EXTRA_GLOBALS \
    { MP_ROM_QSTR(MP_QSTR_sleep), MP_ROM_PTR(&machine_lightsleep_obj) }, \
    \
    { MP_ROM_QSTR(MP_QSTR_Timer), MP_ROM_PTR(&machine_timer_type) }, \
    MICROPY_PY_MACHINE_SDCARD_ENTRY \
    { MP_ROM_QSTR(MP_QSTR_Pin), MP_ROM_PTR(&machine_pin_type) }, \
    MICROPY_PY_MACHINE_TOUCH_PAD_ENTRY \
    { MP_ROM_QSTR(MP_QSTR_RTC), MP_ROM_PTR(&machine_rtc_type) }, \
    \
    /* wake abilities */ \
    { MP_ROM_QSTR(MP_QSTR_SLEEP), MP_ROM_INT(MACHINE_WAKE_SLEEP) }, \
    { MP_ROM_QSTR(MP_QSTR_DEEPSLEEP), MP_ROM_INT(MACHINE_WAKE_DEEPSLEEP) }, \
    \
    /* Reset reasons */ \
    { MP_ROM_QSTR(MP_QSTR_HARD_RESET), MP_ROM_INT(MP_HARD_RESET) }, \
    { MP_ROM_QSTR(MP_QSTR_PWRON_RESET), MP_ROM_INT(MP_PWRON_RESET) }, \
    { MP_ROM_QSTR(MP_QSTR_WDT_RESET), MP_ROM_INT(MP_WDT_RESET) }, \
    { MP_ROM_QSTR(MP_QSTR_DEEPSLEEP_RESET), MP_ROM_INT(MP_DEEPSLEEP_RESET) }, \
    { MP_ROM_QSTR(MP_QSTR_SOFT_RESET), MP_ROM_INT(MP_SOFT_RESET) }, \
    \
    /* Wake reasons */ \
    { MP_ROM_QSTR(MP_QSTR_wake_reason), MP_ROM_PTR(&machine_wake_reason_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_PIN_WAKE), MP_ROM_INT(ESP_SLEEP_WAKEUP_EXT0) }, \
    { MP_ROM_QSTR(MP_QSTR_EXT0_WAKE), MP_ROM_INT(ESP_SLEEP_WAKEUP_EXT0) }, \
    { MP_ROM_QSTR(MP_QSTR_EXT1_WAKE), MP_ROM_INT(ESP_SLEEP_WAKEUP_EXT1) }, \
    { MP_ROM_QSTR(MP_QSTR_TIMER_WAKE), MP_ROM_INT(ESP_SLEEP_WAKEUP_TIMER) }, \
    { MP_ROM_QSTR(MP_QSTR_TOUCHPAD_WAKE), MP_ROM_INT(ESP_SLEEP_WAKEUP_TOUCHPAD) }, \
    { MP_ROM_QSTR(MP_QSTR_ULP_WAKE), MP_ROM_INT(ESP_SLEEP_WAKEUP_ULP) }, \

typedef enum {
    MP_PWRON_RESET = 1,
    MP_HARD_RESET,
    MP_WDT_RESET,
    MP_DEEPSLEEP_RESET,
    MP_SOFT_RESET
} reset_reason_t;

static bool is_soft_reset = 0;

// Note: this is from a private IDF header
extern int esp_clk_cpu_freq(void);

static mp_obj_t mp_machine_get_freq(void) {
    return mp_obj_new_int(esp_rom_get_cpu_ticks_per_us() * 1000000);
}

static void mp_machine_set_freq(size_t n_args, const mp_obj_t *args) {
    mp_int_t freq = mp_obj_get_int(args[0]) / 1000000;
    #if CONFIG_IDF_TARGET_ESP32C2
    if (freq != 80 && freq != 120) {
        mp_raise_ValueError(MP_ERROR_TEXT("frequency must be 80MHz or 120MHz"));
    }
    #else
    if (freq != 20 && freq != 40 && freq != 80 && freq != 160
        #if !(CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32C6)
        && freq != 240
        #endif
        ) {
        #if CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32C6
        mp_raise_ValueError(MP_ERROR_TEXT("frequency must be 20MHz, 40MHz, 80Mhz or 160MHz"));
        #else
        mp_raise_ValueError(MP_ERROR_TEXT("frequency must be 20MHz, 40MHz, 80Mhz, 160MHz or 240MHz"));
        #endif
    }
    #endif
    esp_pm_config_t pm = {
        .max_freq_mhz = freq,
        .min_freq_mhz = freq,
        .light_sleep_enable = false,
    };
    esp_err_t ret = esp_pm_configure(&pm);
    if (ret != ESP_OK) {
        mp_raise_ValueError(NULL);
    }
    while (esp_rom_get_cpu_ticks_per_us() != freq) {
        vTaskDelay(1);
    }
}

static void machine_sleep_helper(wake_type_t wake_type, size_t n_args, const mp_obj_t *args) {
    #if !SOC_DEEP_SLEEP_SUPPORTED
    if (MACHINE_WAKE_DEEPSLEEP == wake_type) {
        mp_raise_ValueError(MP_ERROR_TEXT("DEEPSLEEP not supported on this chip"));
    }
    #endif

    // First, disable any previously set wake-up source
    esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);

    // Set the expiry time of the sleep, if given.
    if (n_args != 0) {
        mp_int_t expiry = mp_obj_get_int(args[0]);
        esp_sleep_enable_timer_wakeup(((uint64_t)expiry) * 1000);
    }

    #if SOC_PM_SUPPORT_EXT0_WAKEUP
    if (machine_rtc_config.ext0_pin != -1 && (machine_rtc_config.ext0_wake_types & wake_type)) {
        esp_sleep_enable_ext0_wakeup(machine_rtc_config.ext0_pin, machine_rtc_config.ext0_level ? 1 : 0);
    }
    #endif

    #if SOC_PM_SUPPORT_EXT1_WAKEUP
    if (machine_rtc_config.ext1_pins != 0) {
        esp_sleep_enable_ext1_wakeup(
            machine_rtc_config.ext1_pins,
            machine_rtc_config.ext1_level ? ESP_EXT1_WAKEUP_ANY_HIGH : ESP_EXT1_WAKEUP_ALL_LOW);
    }
    #endif

    #if SOC_TOUCH_SENSOR_SUPPORTED
    if (machine_rtc_config.wake_on_touch) {
        if (esp_sleep_enable_touchpad_wakeup() != ESP_OK) {
            mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("esp_sleep_enable_touchpad_wakeup() failed"));
        }
    }
    #endif

    #if SOC_ULP_SUPPORTED
    if (machine_rtc_config.wake_on_ulp) {
        if (esp_sleep_enable_ulp_wakeup() != ESP_OK) {
            mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("esp_sleep_enable_ulp_wakeup() failed"));
        }
    }
    #endif

    if (machine_rtc_config.gpio_pins != 0) {
        #if !SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP
        if (MACHINE_WAKE_DEEPSLEEP == wake_type) {
            mp_raise_ValueError(MP_ERROR_TEXT("DEEPSLEEP with gpio pins not supported on this chip"));
        }
        #endif

        gpio_int_type_t intr_type = machine_rtc_config.gpio_level ? GPIO_INTR_HIGH_LEVEL : GPIO_INTR_LOW_LEVEL;

        for (int i = 0; i < GPIO_NUM_MAX; ++i) {
            gpio_num_t gpio = (gpio_num_t)i;
            uint64_t bm = 1ULL << i;

            if (machine_rtc_config.gpio_pins & bm) {
                gpio_sleep_set_direction(gpio, GPIO_MODE_INPUT);

                if (MACHINE_WAKE_SLEEP == wake_type) {
                    gpio_wakeup_enable(gpio, intr_type);
                }
            }
        }

        if (MACHINE_WAKE_DEEPSLEEP == wake_type) {
            #if SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP
            if (ESP_OK != esp_deep_sleep_enable_gpio_wakeup(
                machine_rtc_config.gpio_pins,
                machine_rtc_config.gpio_level ? ESP_GPIO_WAKEUP_GPIO_HIGH : ESP_GPIO_WAKEUP_GPIO_LOW)) {
                mp_raise_ValueError(MP_ERROR_TEXT("wake-up pin not supported"));
            }
            #endif
        } else {
            esp_sleep_enable_gpio_wakeup();
        }
    }

    switch (wake_type) {
        case MACHINE_WAKE_SLEEP:
            esp_light_sleep_start();
            break;
        case MACHINE_WAKE_DEEPSLEEP:
            esp_deep_sleep_start();
            break;
    }
}

static void mp_machine_lightsleep(size_t n_args, const mp_obj_t *args) {
    machine_sleep_helper(MACHINE_WAKE_SLEEP, n_args, args);
};

MP_NORETURN static void mp_machine_deepsleep(size_t n_args, const mp_obj_t *args) {
    machine_sleep_helper(MACHINE_WAKE_DEEPSLEEP, n_args, args);
    mp_machine_reset();
};

static mp_int_t mp_machine_reset_cause(void) {
    if (is_soft_reset) {
        return MP_SOFT_RESET;
    }
    switch (esp_reset_reason()) {
        case ESP_RST_POWERON:
        case ESP_RST_BROWNOUT:
            return MP_PWRON_RESET;

        case ESP_RST_INT_WDT:
        case ESP_RST_TASK_WDT:
        case ESP_RST_WDT:
            return MP_WDT_RESET;

        case ESP_RST_DEEPSLEEP:
            return MP_DEEPSLEEP_RESET;

        case ESP_RST_SW:
        case ESP_RST_PANIC:
        case ESP_RST_EXT: // Comment in ESP-IDF: "For ESP32, ESP_RST_EXT is never returned"
            return MP_HARD_RESET;

        case ESP_RST_SDIO:
        case ESP_RST_UNKNOWN:
        default:
            return 0;
    }
}

#if MICROPY_ESP32_USE_BOOTLOADER_RTC
#include "soc/rtc_cntl_reg.h"
#include "usb.h"
#if CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/usb/usb_dc.h"
#include "esp32s3/rom/usb/usb_persist.h"
#include "esp32s3/rom/usb/chip_usb_dw_wrapper.h"
#endif

MP_NORETURN static void machine_bootloader_rtc(void) {
    #if CONFIG_IDF_TARGET_ESP32S3 && MICROPY_HW_USB_CDC
    usb_usj_mode();
    usb_dc_prepare_persist();
    chip_usb_set_persist_flags(USBDC_BOOT_DFU);
    #endif
    REG_WRITE(RTC_CNTL_OPTION1_REG, RTC_CNTL_FORCE_DOWNLOAD_BOOT);
    esp_restart();
}
#endif

#ifdef MICROPY_BOARD_ENTER_BOOTLOADER
MP_NORETURN void mp_machine_bootloader(size_t n_args, const mp_obj_t *args) {
    MICROPY_BOARD_ENTER_BOOTLOADER(n_args, args);
    for (;;) {
    }
}
#endif

void machine_init(void) {
    is_soft_reset = 0;
}

void machine_deinit(void) {
    // we are doing a soft-reset so change the reset_cause
    is_soft_reset = 1;
}

static mp_obj_t machine_wake_reason(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return MP_OBJ_NEW_SMALL_INT(esp_sleep_get_wakeup_cause());
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_wake_reason_obj, 0,  machine_wake_reason);

MP_NORETURN static void mp_machine_reset(void) {
    esp_restart();
}

static mp_obj_t mp_machine_unique_id(void) {
    uint8_t chipid[6];
    esp_efuse_mac_get_default(chipid);
    return mp_obj_new_bytes(chipid, 6);
}

static void mp_machine_idle(void) {
    MP_THREAD_GIL_EXIT();
    taskYIELD();
    MP_THREAD_GIL_ENTER();
}
