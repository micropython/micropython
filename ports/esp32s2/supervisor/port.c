/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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

#include <stdint.h>
#include <sys/time.h>
#include "supervisor/board.h"
#include "supervisor/port.h"
#include "modules/module.h"
#include "py/runtime.h"
#include "supervisor/esp_port.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/analogio/AnalogOut.h"
#include "common-hal/busio/I2C.h"
#include "common-hal/busio/SPI.h"
#include "common-hal/busio/UART.h"
#include "common-hal/dualbank/__init__.h"
#include "common-hal/ps2io/Ps2.h"
#include "common-hal/pulseio/PulseIn.h"
#include "common-hal/pwmio/PWMOut.h"
#include "common-hal/watchdog/WatchDogTimer.h"
#include "common-hal/socketpool/Socket.h"
#include "common-hal/wifi/__init__.h"
#include "supervisor/background_callback.h"
#include "supervisor/memory.h"
#include "supervisor/shared/tick.h"
#include "shared-bindings/rtc/__init__.h"

#include "peripherals/rmt.h"
#include "peripherals/pcnt.h"
#include "peripherals/timer.h"
#include "peripherals/touch.h"
#include "components/esp_rom/include/esp32s2/rom/ets_sys.h"
#include "components/heap/include/esp_heap_caps.h"
#include "components/xtensa/include/esp_debug_helpers.h"
#include "components/soc/esp32s2/include/soc/cache_memory.h"
#include "components/soc/esp32s2/include/soc/rtc_cntl_reg.h"

#if CIRCUITPY_AUDIOBUSIO
#include "common-hal/audiobusio/__init__.h"
#endif

#if CIRCUITPY_IMAGECAPTURE
#include "cam.h"
#endif

#define HEAP_SIZE (48 * 1024)

uint32_t *heap;
uint32_t heap_size;

STATIC esp_timer_handle_t _tick_timer;
STATIC esp_timer_handle_t _sleep_timer;

TaskHandle_t circuitpython_task = NULL;

extern void esp_restart(void) NORETURN;

void tick_timer_cb(void *arg) {
    supervisor_tick();

    // CircuitPython's VM is run in a separate FreeRTOS task from timer callbacks. So, we have to
    // notify the main task every time in case it's waiting for us.
    xTaskNotifyGive(circuitpython_task);
}

void sleep_timer_cb(void *arg);

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

    // Send the ROM output out of the UART. This includes early logs.
    #ifdef DEBUG
    ets_install_uart_printf();
    #endif

    heap = NULL;
    never_reset_module_internal_pins();

    #if defined(DEBUG)
    // debug UART
    common_hal_never_reset_pin(&pin_GPIO43);
    common_hal_never_reset_pin(&pin_GPIO44);
    #endif

    #if defined(DEBUG) || defined(ENABLE_JTAG)
    // JTAG
    common_hal_never_reset_pin(&pin_GPIO39);
    common_hal_never_reset_pin(&pin_GPIO40);
    common_hal_never_reset_pin(&pin_GPIO41);
    common_hal_never_reset_pin(&pin_GPIO42);
    #endif

    #ifdef CONFIG_SPIRAM
    heap = (uint32_t *)(DRAM0_CACHE_ADDRESS_HIGH - CONFIG_SPIRAM_SIZE);
    heap_size = CONFIG_SPIRAM_SIZE / sizeof(uint32_t);
    #endif

    if (heap == NULL) {
        heap = malloc(HEAP_SIZE);
        heap_size = HEAP_SIZE / sizeof(uint32_t);
    }
    if (heap == NULL) {
        return NO_HEAP;
    }

    esp_reset_reason_t reason = esp_reset_reason();
    switch (reason) {
        case ESP_RST_BROWNOUT:
            return BROWNOUT;
        case ESP_RST_PANIC:
            return HARD_CRASH;
        case ESP_RST_INT_WDT:
        case ESP_RST_WDT:
        default:
            break;
    }

    return NO_SAFE_MODE;
}

void reset_port(void) {
    #if CIRCUITPY_IMAGECAPTURE
    cam_deinit();
    #endif

    reset_all_pins();

    // A larger delay so the idle task can run and do any IDF cleanup needed.
    vTaskDelay(4);

    #if CIRCUITPY_ANALOGIO
    analogout_reset();
    #endif

    #if CIRCUITPY_DUALBANK
    dualbank_reset();
    #endif

    #if CIRCUITPY_PS2IO
    ps2_reset();
    #endif

    #if CIRCUITPY_AUDIOBUSIO
    i2s_reset();
    #endif

    #if CIRCUITPY_PULSEIO
    esp32s2_peripherals_rmt_reset();
    pulsein_reset();
    #endif

    #if CIRCUITPY_PWMIO
    pwmout_reset();
    #endif

    #if CIRCUITPY_BUSIO
    i2c_reset();
    spi_reset();
    uart_reset();
    #endif

    #if defined(CIRCUITPY_COUNTIO) || defined(CIRCUITPY_ROTARYIO)
    peripherals_pcnt_reset();
    #endif

    #if CIRCUITPY_FREQUENCYIO
    peripherals_timer_reset();
    #endif

    #if CIRCUITPY_PULSEIO
    esp32s2_peripherals_rmt_reset();
    pulsein_reset();
    #endif

    #if CIRCUITPY_PWMIO
    pwmout_reset();
    #endif

    #if CIRCUITPY_RTC
    rtc_reset();
    #endif

    #if CIRCUITPY_TOUCHIO_USE_NATIVE
    peripherals_touch_reset();
    #endif

    #if CIRCUITPY_WATCHDOG
    watchdog_reset();
    #endif

    #if CIRCUITPY_WIFI
    wifi_reset();
    #endif

    #if CIRCUITPY_SOCKETPOOL
    socket_reset();
    #endif
}

void reset_to_bootloader(void) {
    esp_restart();
}

void reset_cpu(void) {
    esp_backtrace_print(100);
    esp_restart();
}

uint32_t *port_heap_get_bottom(void) {
    return heap;
}

uint32_t *port_heap_get_top(void) {
    return heap + heap_size;
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

bool port_has_fixed_stack(void) {
    return true;
}

// Place the word to save just after our BSS section that gets blanked.
void port_set_saved_word(uint32_t value) {
    REG_WRITE(RTC_CNTL_STORE0_REG, value);
}

uint32_t port_get_saved_word(void) {
    return REG_READ(RTC_CNTL_STORE0_REG);
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
    if (!background_callback_pending()) {
        xTaskNotifyWait(0x01, 0x01, NULL, portMAX_DELAY);
    }
}

// Wrap main in app_main that the IDF expects.
extern void main(void);
void app_main(void) {
    main();
}
