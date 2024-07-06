// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdarg.h>
#include <stdint.h>
#include <sys/time.h>
#include "supervisor/board.h"
#include "supervisor/port.h"
#include "supervisor/filesystem.h"
#include "supervisor/shared/reload.h"
#include "supervisor/shared/serial.h"
#include "py/mpprint.h"
#include "py/runtime.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "bindings/espidf/__init__.h"
#include "bindings/espnow/__init__.h"
#include "bindings/espulp/__init__.h"
#include "common-hal/microcontroller/Pin.h"
#include "common-hal/analogio/AnalogOut.h"
#include "common-hal/busio/I2C.h"
#include "common-hal/busio/SPI.h"
#include "common-hal/busio/UART.h"
#include "common-hal/dualbank/__init__.h"
#include "common-hal/ps2io/Ps2.h"
#include "common-hal/watchdog/WatchDogTimer.h"
#include "common-hal/socketpool/Socket.h"
#include "common-hal/wifi/__init__.h"
#include "supervisor/background_callback.h"
#include "supervisor/shared/tick.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/RunMode.h"
#include "shared-bindings/rtc/__init__.h"
#include "shared-bindings/socketpool/__init__.h"
#include "shared-module/os/__init__.h"

#if CIRCUITPY_TOUCHIO_USE_NATIVE
#include "peripherals/touch.h"
#endif

#if CIRCUITPY_BLEIO
#include "shared-bindings/_bleio/__init__.h"
#endif

#if CIRCUITPY_ESPCAMERA
#include "esp_camera.h"
#endif

#include "soc/efuse_reg.h"
#if defined(SOC_LP_AON_SUPPORTED)
#include "soc/lp_aon_reg.h"
#define CP_SAVED_WORD_REGISTER LP_AON_STORE0_REG
#else
#include "soc/rtc_cntl_reg.h"
#define CP_SAVED_WORD_REGISTER RTC_CNTL_STORE0_REG
#endif
#include "soc/spi_pins.h"

#include "bootloader_flash_config.h"

#include "esp_debug_helpers.h"
#include "esp_efuse.h"
#include "esp_ipc.h"
#include "esp_rom_efuse.h"
#include "esp_timer.h"

#ifdef CONFIG_IDF_TARGET_ESP32
#include "hal/efuse_hal.h"
#include "esp32/rom/efuse.h"
#endif

#if CIRCUITPY_SSL
#include "shared-module/ssl/__init__.h"
#endif

#include "esp_log.h"
#define TAG "port"

static esp_timer_handle_t _tick_timer;
static esp_timer_handle_t _sleep_timer;

TaskHandle_t circuitpython_task = NULL;

extern void esp_restart(void) NORETURN;

static void tick_on_cp_core(void *arg) {
    supervisor_tick();

    // CircuitPython's VM is run in a separate FreeRTOS task from timer callbacks. So, we have to
    // notify the main task every time in case it's waiting for us.
    xTaskNotifyGive(circuitpython_task);
}

// This function may happen on the PRO core when CP is on the APP core. So, make
// sure we run on the CP core.
static void tick_timer_cb(void *arg) {
    #if defined(CONFIG_FREERTOS_UNICORE) && CONFIG_FREERTOS_UNICORE
    tick_on_cp_core(arg);
    #else
    // This only blocks until the start of the function. That's ok since the PRO
    // core shouldn't care what we do.
    esp_ipc_call(CONFIG_ESP_MAIN_TASK_AFFINITY, tick_on_cp_core, NULL);
    #endif
}

void sleep_timer_cb(void *arg);

// The ESP-IDF determines these pins at runtime so we do too. This code is based on:
// https://github.com/espressif/esp-idf/blob/6d85d53ceec30c818a92c2fff8f5437d21c4720f/components/esp_hw_support/port/esp32/spiram_psram.c#L810
// IO-pins for PSRAM.
// WARNING: PSRAM shares all but the CS and CLK pins with the flash, so these defines
// hardcode the flash pins as well, making this code incompatible with either a setup
// that has the flash on non-standard pins or ESP32s with built-in flash.
#define PSRAM_SPIQ_SD0_IO          7
#define PSRAM_SPID_SD1_IO          8
#define PSRAM_SPIWP_SD3_IO         10
#define PSRAM_SPIHD_SD2_IO         9

#define FLASH_HSPI_CLK_IO          14
#define FLASH_HSPI_CS_IO           15
#define PSRAM_HSPI_SPIQ_SD0_IO     12
#define PSRAM_HSPI_SPID_SD1_IO     13
#define PSRAM_HSPI_SPIWP_SD3_IO    2
#define PSRAM_HSPI_SPIHD_SD2_IO    4

#ifdef CONFIG_SPIRAM
// PSRAM clock and cs IO should be configured based on hardware design.
// For ESP32-WROVER or ESP32-WROVER-B module, the clock IO is IO17, the cs IO is IO16,
// they are the default value for these two configs.
#define D0WD_PSRAM_CLK_IO          CONFIG_D0WD_PSRAM_CLK_IO  // Default value is 17
#define D0WD_PSRAM_CS_IO           CONFIG_D0WD_PSRAM_CS_IO   // Default value is 16

#define D2WD_PSRAM_CLK_IO          CONFIG_D2WD_PSRAM_CLK_IO  // Default value is 9
#define D2WD_PSRAM_CS_IO           CONFIG_D2WD_PSRAM_CS_IO   // Default value is 10

// There is no reason to change the pin of an embedded psram.
// So define the number of pin directly, instead of configurable.
#define D0WDR2_V3_PSRAM_CLK_IO    6
#define D0WDR2_V3_PSRAM_CS_IO     16

// For ESP32-PICO chip, the psram share clock with flash. The flash clock pin is fixed, which is IO6.
#define PICO_PSRAM_CLK_IO          6
#define PICO_PSRAM_CS_IO           CONFIG_PICO_PSRAM_CS_IO   // Default value is 10

#define PICO_V3_02_PSRAM_CLK_IO    10
#define PICO_V3_02_PSRAM_CS_IO     9
#endif // CONFIG_SPIRAM

static void _never_reset_spi_ram_flash(void) {
    #if defined(CONFIG_IDF_TARGET_ESP32)
    #if defined(CONFIG_SPIRAM)
    uint32_t pkg_ver = esp_efuse_get_pkg_ver();
    if (pkg_ver == EFUSE_RD_CHIP_VER_PKG_ESP32D2WDQ5) {
        never_reset_pin_number(D2WD_PSRAM_CLK_IO);
        never_reset_pin_number(D2WD_PSRAM_CS_IO);
    } else if (pkg_ver == EFUSE_RD_CHIP_VER_PKG_ESP32PICOD4 && efuse_hal_get_major_chip_version() >= 3) {
        // This chip is ESP32-PICO-V3 and doesn't have PSRAM.
    } else if ((pkg_ver == EFUSE_RD_CHIP_VER_PKG_ESP32PICOD2) || (pkg_ver == EFUSE_RD_CHIP_VER_PKG_ESP32PICOD4)) {
        never_reset_pin_number(PICO_PSRAM_CLK_IO);
        never_reset_pin_number(PICO_PSRAM_CS_IO);
    } else if (pkg_ver == EFUSE_RD_CHIP_VER_PKG_ESP32PICOV302) {
        never_reset_pin_number(PICO_V3_02_PSRAM_CLK_IO);
        never_reset_pin_number(PICO_V3_02_PSRAM_CS_IO);
    } else if ((pkg_ver == EFUSE_RD_CHIP_VER_PKG_ESP32D0WDQ6) || (pkg_ver == EFUSE_RD_CHIP_VER_PKG_ESP32D0WDQ5)) {
        never_reset_pin_number(D0WD_PSRAM_CLK_IO);
        never_reset_pin_number(D0WD_PSRAM_CS_IO);
    } else if (pkg_ver == EFUSE_RD_CHIP_VER_PKG_ESP32D0WDR2V3) {
        never_reset_pin_number(D0WDR2_V3_PSRAM_CLK_IO);
        never_reset_pin_number(D0WDR2_V3_PSRAM_CS_IO);
    }
    #endif // CONFIG_SPIRAM

    const uint32_t spiconfig = esp_rom_efuse_get_flash_gpio_info();
    if (spiconfig == ESP_ROM_EFUSE_FLASH_DEFAULT_SPI) {
        never_reset_pin_number(SPI_IOMUX_PIN_NUM_CLK);
        never_reset_pin_number(SPI_IOMUX_PIN_NUM_CS);
        never_reset_pin_number(PSRAM_SPIQ_SD0_IO);
        never_reset_pin_number(PSRAM_SPID_SD1_IO);
        never_reset_pin_number(PSRAM_SPIWP_SD3_IO);
        never_reset_pin_number(PSRAM_SPIHD_SD2_IO);
    } else if (spiconfig == ESP_ROM_EFUSE_FLASH_DEFAULT_HSPI) {
        never_reset_pin_number(FLASH_HSPI_CLK_IO);
        never_reset_pin_number(FLASH_HSPI_CS_IO);
        never_reset_pin_number(PSRAM_HSPI_SPIQ_SD0_IO);
        never_reset_pin_number(PSRAM_HSPI_SPID_SD1_IO);
        never_reset_pin_number(PSRAM_HSPI_SPIWP_SD3_IO);
        never_reset_pin_number(PSRAM_HSPI_SPIHD_SD2_IO);
    } else {
        never_reset_pin_number(EFUSE_SPICONFIG_RET_SPICLK(spiconfig));
        never_reset_pin_number(EFUSE_SPICONFIG_RET_SPICS0(spiconfig));
        never_reset_pin_number(EFUSE_SPICONFIG_RET_SPIQ(spiconfig));
        never_reset_pin_number(EFUSE_SPICONFIG_RET_SPID(spiconfig));
        never_reset_pin_number(EFUSE_SPICONFIG_RET_SPIHD(spiconfig));
        never_reset_pin_number(bootloader_flash_get_wp_pin());
    }
    #endif // CONFIG_IDF_TARGET_ESP32
}

safe_mode_t port_init(void) {
    esp_timer_create_args_t args;
    args.callback = &tick_timer_cb;
    args.arg = NULL;
    args.dispatch_method = ESP_TIMER_TASK;
    args.name = "CircuitPython Tick";
    esp_timer_create(&args, &_tick_timer);

    args.callback = &sleep_timer_cb;
    args.arg = NULL;
    args.dispatch_method = ESP_TIMER_TASK;
    args.name = "CircuitPython Sleep";
    esp_timer_create(&args, &_sleep_timer);

    circuitpython_task = xTaskGetCurrentTaskHandle();

    #if !defined(DEBUG)
    #define DEBUG (0)
    #endif

    // Send the ROM output out of the UART. This includes early logs.
    #if DEBUG
    esp_rom_install_uart_printf();
    #endif

    #define pin_GPIOn(n) pin_GPIO##n
    #define pin_GPIOn_EXPAND(x) pin_GPIOn(x)

    #ifdef CONFIG_CONSOLE_UART_TX_GPIO
    common_hal_never_reset_pin(&pin_GPIOn_EXPAND(CONFIG_CONSOLE_UART_TX_GPIO));
    #endif

    #ifdef CONFIG_CONSOLE_UART_RX_GPIO
    common_hal_never_reset_pin(&pin_GPIOn_EXPAND(CONFIG_CONSOLE_UART_RX_GPIO));
    #endif

    #if DEBUG
    // debug UART
    #if defined(CONFIG_IDF_TARGET_ESP32C3)
    common_hal_never_reset_pin(&pin_GPIO20);
    common_hal_never_reset_pin(&pin_GPIO21);
    #elif defined(CONFIG_IDF_TARGET_ESP32C6)
    common_hal_never_reset_pin(&pin_GPIO16);
    common_hal_never_reset_pin(&pin_GPIO17);
    #elif defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)
    common_hal_never_reset_pin(&pin_GPIO43);
    common_hal_never_reset_pin(&pin_GPIO44);
    #endif
    #endif

    #ifndef ENABLE_JTAG
    #define ENABLE_JTAG (0)
    #endif

    #if ENABLE_JTAG
    ESP_LOGI(TAG, "Marking JTAG pins never_reset");
    // JTAG
    #if defined(CONFIG_IDF_TARGET_ESP32C3) || defined(CONFIG_IDF_TARGET_ESP32C6)
    common_hal_never_reset_pin(&pin_GPIO4);
    common_hal_never_reset_pin(&pin_GPIO5);
    common_hal_never_reset_pin(&pin_GPIO6);
    common_hal_never_reset_pin(&pin_GPIO7);
    #elif defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)
    common_hal_never_reset_pin(&pin_GPIO39);
    common_hal_never_reset_pin(&pin_GPIO40);
    common_hal_never_reset_pin(&pin_GPIO41);
    common_hal_never_reset_pin(&pin_GPIO42);
    #endif
    #endif

    _never_reset_spi_ram_flash();

    esp_reset_reason_t reason = esp_reset_reason();
    switch (reason) {
        case ESP_RST_BROWNOUT:
            return SAFE_MODE_BROWNOUT;
        case ESP_RST_PANIC:
            return SAFE_MODE_HARD_FAULT;
        case ESP_RST_INT_WDT:
            // The interrupt watchdog is used internally to make sure that latency sensitive
            // interrupt code isn't blocked. User watchdog resets come through ESP_RST_WDT.
            return SAFE_MODE_WATCHDOG;
        case ESP_RST_WDT:
        default:
            break;
    }

    return SAFE_MODE_NONE;
}

void port_heap_init(void) {
    // The IDF sets up the heap, so we don't need to.
}

void *port_malloc(size_t size, bool dma_capable) {
    size_t caps = MALLOC_CAP_8BIT;
    if (dma_capable) {
        caps |= MALLOC_CAP_DMA;
    }

    void *ptr = NULL;
    // Try SPIRAM first when available.
    #ifdef CONFIG_SPIRAM
    ptr = heap_caps_malloc(size, caps | MALLOC_CAP_SPIRAM);
    #endif
    if (ptr == NULL) {
        ptr = heap_caps_malloc(size, caps);
    }
    return ptr;
}

void port_free(void *ptr) {
    heap_caps_free(ptr);
}

void *port_realloc(void *ptr, size_t size) {
    return heap_caps_realloc(ptr, size, MALLOC_CAP_8BIT);
}

size_t port_heap_get_largest_free_size(void) {
    size_t free_size = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT);
    return free_size;
}

void reset_port(void) {
    // TODO deinit for esp32-camera
    #if CIRCUITPY_ESPCAMERA
    esp_camera_deinit();
    #endif

    #if CIRCUITPY_SSL
    ssl_reset();
    #endif

    reset_all_pins();

    #if CIRCUITPY_ANALOGIO
    analogout_reset();
    #endif

    #if CIRCUITPY_BUSIO
    i2c_reset();
    spi_reset();
    uart_reset();
    #endif

    #if CIRCUITPY_DUALBANK
    dualbank_reset();
    #endif

    #if CIRCUITPY_ESPNOW
    espnow_reset();
    #endif

    #if CIRCUITPY_ESPULP
    espulp_reset();
    #endif

    #if CIRCUITPY_PS2IO
    ps2_reset();
    #endif

    #if CIRCUITPY_RTC
    rtc_reset();
    #endif

    #if CIRCUITPY_SOCKETPOOL
    socketpool_user_reset();
    #endif

    #if CIRCUITPY_TOUCHIO_USE_NATIVE
    peripherals_touch_reset();
    #endif

    #if CIRCUITPY_WATCHDOG
    watchdog_reset();
    #endif

    // Yield so the idle task can run and do any IDF cleanup needed.
    port_yield();
}

void reset_to_bootloader(void) {
    common_hal_mcu_on_next_reset(RUNMODE_BOOTLOADER);
    esp_restart();
}

void reset_cpu(void) {
    #if CIRCUITPY_DEBUG
    esp_backtrace_print(100);
    #endif
    esp_restart();
}

uint32_t *port_stack_get_limit(void) {
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wcast-align"
    return (uint32_t *)pxTaskGetStackStart(NULL);
    #pragma GCC diagnostic pop
}

uint32_t *port_stack_get_top(void) {
    // The sizeof-arithmetic is so that the pointer arithmetic is done on units
    // of uint32_t instead of units of StackType_t.  StackType_t is an alias
    // for a byte sized type.
    //
    // The main stack is bigger than CONFIG_ESP_MAIN_TASK_STACK_SIZE -- an
    // "extra" size is added to it (TASK_EXTRA_STACK_SIZE).  This total size is
    // available as ESP_TASK_MAIN_STACK.  Presumably TASK_EXTRA_STACK_SIZE is
    // additional stack that can be used by the esp-idf runtime.  But what's
    // important for us is that some very outermost stack frames, such as
    // pyexec_friendly_repl, could lie inside the "extra" area and be invisible
    // to the garbage collector.
    return port_stack_get_limit() + ESP_TASK_MAIN_STACK / (sizeof(uint32_t) / sizeof(StackType_t));
}

void port_set_saved_word(uint32_t value) {
    REG_WRITE(CP_SAVED_WORD_REGISTER, value);
}

uint32_t port_get_saved_word(void) {
    return REG_READ(CP_SAVED_WORD_REGISTER);
}

uint64_t port_get_raw_ticks(uint8_t *subticks) {
    // Convert microseconds to subticks of 1/32768 seconds
    // 32768/1000000 = 64/15625 in lowest terms
    // this arithmetic overflows after 570 years
    int64_t all_subticks = esp_timer_get_time() * 512 / 15625;
    if (subticks != NULL) {
        *subticks = all_subticks % 32;
    }
    return all_subticks / 32;
}

// Enable 1/1024 second tick.
void port_enable_tick(void) {
    esp_timer_start_periodic(_tick_timer, 1000000 / 1024);
}

// Disable 1/1024 second tick.
void port_disable_tick(void) {
    esp_timer_stop(_tick_timer);
}

void port_wake_main_task() {
    xTaskNotifyGive(circuitpython_task);
}

void port_wake_main_task_from_isr() {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR(circuitpython_task, &xHigherPriorityTaskWoken);
    if (xHigherPriorityTaskWoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

void port_yield() {
    vTaskDelay(4);
}

void sleep_timer_cb(void *arg) {
    port_wake_main_task();
}

void port_interrupt_after_ticks(uint32_t ticks) {
    uint64_t timeout_us = ticks * 1000000ull / 1024;
    if (esp_timer_start_once(_sleep_timer, timeout_us) != ESP_OK) {
        esp_timer_stop(_sleep_timer);
        esp_timer_start_once(_sleep_timer, timeout_us);
    }
}

// On the ESP we use FreeRTOS notifications instead of interrupts so this is a
// bit of a misnomer.
void port_idle_until_interrupt(void) {
    if (!background_callback_pending() && !autoreload_pending()) {
        xTaskNotifyWait(0x01, 0x01, NULL, portMAX_DELAY);
    }
}

void port_post_boot_py(bool heap_valid) {
    if (!heap_valid && filesystem_present()) {
    }
}


#if CIRCUITPY_CONSOLE_UART
static int vprintf_adapter(const char *fmt, va_list ap) {
    return mp_vprintf(&mp_plat_print, fmt, ap);
}

void port_serial_early_init(void) {
    esp_log_set_vprintf(vprintf_adapter);
}
#endif

// Wrap main in app_main that the IDF expects.
extern void main(void);
extern void app_main(void);
void app_main(void) {
    main();
}

portMUX_TYPE background_task_mutex = portMUX_INITIALIZER_UNLOCKED;
