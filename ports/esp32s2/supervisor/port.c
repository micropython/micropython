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
#include "supervisor/port.h"
#include "boards/board.h"
#include "modules/module.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/busio/I2C.h"
#include "common-hal/busio/SPI.h"
#include "common-hal/busio/UART.h"
#include "common-hal/pulseio/PulseIn.h"
#include "common-hal/pwmio/PWMOut.h"
#include "supervisor/memory.h"
#include "supervisor/shared/tick.h"

#include "rmt.h"

STATIC esp_timer_handle_t _tick_timer;

void tick_timer_cb(void* arg) {
    supervisor_tick();
}

safe_mode_t port_init(void) {
    esp_timer_create_args_t args;
    args.callback = &tick_timer_cb;
    args.arg = NULL;
    args.dispatch_method = ESP_TIMER_TASK;
    args.name = "CircuitPython Tick";
    esp_timer_create(&args, &_tick_timer);
    never_reset_module_internal_pins();
    return NO_SAFE_MODE;
}

void reset_port(void) {
    reset_all_pins();

    // A larger delay so the idle task can run and do any IDF cleanup needed.
    vTaskDelay(4);

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
}

void reset_to_bootloader(void) {
}

void reset_cpu(void) {
}

uint32_t heap[64 / sizeof(uint32_t) * 1024];

uint32_t *port_heap_get_bottom(void) {
    return heap;
}

uint32_t *port_heap_get_top(void) {
    return heap + sizeof(heap) / sizeof(heap[0]);
}

uint32_t *port_stack_get_limit(void) {
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wcast-align"
    return (uint32_t*) pxTaskGetStackStart(NULL);
    #pragma GCC diagnostic pop
}

uint32_t *port_stack_get_top(void) {
    return port_stack_get_limit() + CONFIG_ESP_MAIN_TASK_STACK_SIZE / (sizeof(uint32_t) / sizeof(StackType_t));
}

supervisor_allocation _fixed_stack;

supervisor_allocation* port_fixed_stack(void) {
    _fixed_stack.ptr = port_stack_get_limit();
    _fixed_stack.length = (port_stack_get_top() - port_stack_get_limit()) * sizeof(uint32_t);
    return &_fixed_stack;
}

// Place the word to save just after our BSS section that gets blanked.
void port_set_saved_word(uint32_t value) {
}

uint32_t port_get_saved_word(void) {
    return 0;
}

uint64_t port_get_raw_ticks(uint8_t* subticks) {
    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    // convert usec back to ticks
    uint64_t all_subticks = (uint64_t)(tv_now.tv_usec * 2) / 71;
    if (subticks != NULL) {
        *subticks = all_subticks % 32;
    }
    return (uint64_t)tv_now.tv_sec * 1024L + all_subticks / 32;
}

// Enable 1/1024 second tick.
void port_enable_tick(void) {
    esp_timer_start_periodic(_tick_timer, 1000000 / 1024);
}

// Disable 1/1024 second tick.
void port_disable_tick(void) {
    esp_timer_stop(_tick_timer);
}

TickType_t sleep_time_set;
TickType_t sleep_time_duration;
void port_interrupt_after_ticks(uint32_t ticks) {
    sleep_time_set = xTaskGetTickCount();
    sleep_time_duration = ticks / portTICK_PERIOD_MS;
    // esp_sleep_enable_timer_wakeup(uint64_t time_in_us)
}

void port_sleep_until_interrupt(void) {
    // FreeRTOS delay here maybe.
    // Light sleep shuts down BLE and wifi.
    // esp_light_sleep_start()
    if (sleep_time_duration == 0) {
        return;
    }
    vTaskDelayUntil(&sleep_time_set, sleep_time_duration);
}


// Wrap main in app_main that the IDF expects.
extern void main(void);
void app_main(void) {
    main();
}
