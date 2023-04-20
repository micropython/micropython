/*
 * This file is part of Adafruit for EFR32 project
 *
 * The MIT License (MIT)
 *
 * Copyright 2023 Silicon Laboratories Inc. www.silabs.com
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

#include "supervisor/background_callback.h"
#include "supervisor/board.h"
#include "supervisor/port.h"
#include "shared/timeutils/timeutils.h"

#include "common-hal/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/__init__.h"

#if CIRCUITPY_AUDIOPWMIO
#include "common-hal/audiopwmio/PWMAudioOut.h"
#endif
#if CIRCUITPY_BUSIO
#include "common-hal/busio/I2C.h"
#include "common-hal/busio/SPI.h"
#include "common-hal/busio/UART.h"
#endif
#if CIRCUITPY_PULSEIO
#include "common-hal/pulseio/PulseOut.h"
#include "common-hal/pulseio/PulseIn.h"
#endif
#if CIRCUITPY_PWMIO
#include "common-hal/pwmio/PWMOut.h"
#endif
#if CIRCUITPY_PULSEIO || CIRCUITPY_PWMIO
#include "peripherals/timers.h"
#endif
#if CIRCUITPY_SDIOIO
#include "common-hal/sdioio/SDCard.h"
#endif
#if CIRCUITPY_PULSEIO || CIRCUITPY_ALARM
#include "peripherals/exti.h"
#endif
#if CIRCUITPY_ALARM
#include "common-hal/alarm/__init__.h"
#endif
#if CIRCUITPY_RTC
#include "shared-bindings/rtc/__init__.h"
#endif
#if CIRCUITPY_ANALOGIO
#include "common-hal/analogio/AnalogOut.h"
#endif

#if CIRCUITPY_BLEIO
#include "common-hal/_bleio/__init__.h"
#endif

// Include headers of EFR32
#include <cmsis_os2.h>
#include "em_chip.h"
#include "sl_cmsis_os2_common.h"
#include "sl_component_catalog.h"
#include "sl_sleeptimer.h"
#include "sl_system_init.h"
#include "sl_system_kernel.h"

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
#include "sl_power_manager.h"
#endif // SL_CATALOG_POWER_MANAGER_PRESENT

#if !defined(SL_CATALOG_KERNEL_PRESENT)
#error "Error: Requires SL_CATALOG_KERNEL_PRESENT definition"
#endif

#define SL_CIRCUITPYTHON_TASK_STACK_EXTRA_SIZE (32)
#define SL_CIRCUITPYTHON_TASK_PRIORITY (40)
#define HEAP_SIZE (88 * 1024)

extern uint32_t __bss_start__;
extern uint32_t __bss_end__;

uint32_t _sbss;
uint32_t _ebss;

uint32_t *heap;
uint32_t heap_size;

STATIC sl_sleeptimer_timer_handle_t _tick_timer;
STATIC sl_sleeptimer_timer_handle_t _sleep_timer;

// CircuitPython stack thread
STATIC void circuitpython_thread(void *p_arg);
STATIC osThreadId_t tid_thread_circuitpython;
__ALIGNED(8)
STATIC uint8_t thread_circuitpython_stk[(CIRCUITPY_DEFAULT_STACK_SIZE +
    SL_CIRCUITPYTHON_TASK_STACK_EXTRA_SIZE) &
                                        0xFFFFFFF8u];
__ALIGNED(4)
STATIC uint8_t thread_circuitpython_cb[osThreadCbSize];

STATIC const osThreadAttr_t thread_circuitpython_attr = {
    .name = "CircuitPython stack",
    .stack_mem = thread_circuitpython_stk,
    .stack_size = sizeof(thread_circuitpython_stk),
    .cb_mem = thread_circuitpython_cb,
    .cb_size = osThreadCbSize,
    .priority = (osPriority_t)SL_CIRCUITPYTHON_TASK_PRIORITY
};

STATIC bool isSchedulerStarted = false;

safe_mode_t port_init(void) {
    #if defined(SL_CATALOG_KERNEL_PRESENT)

    if (!isSchedulerStarted) {
        _sbss = __bss_start__;
        _ebss = __bss_end__;

        isSchedulerStarted = true;
        // Initialize Silicon Labs device, system, service(s) and protocol stack(s).
        // Note that if the kernel is present, processing task(s) will be created by
        // this call.
        sl_system_init();
        // Create thread for Bluetooth stack
        if (tid_thread_circuitpython == NULL) {
            tid_thread_circuitpython = osThreadNew(circuitpython_thread,
                NULL,
                &thread_circuitpython_attr);
        }

        // Create mutex for Bluetooth handle
        if (bluetooth_connection_mutex_id == NULL) {
            bluetooth_connection_mutex_id = osMutexNew(&bluetooth_connection_mutex_attr);
        }

        if (tid_thread_circuitpython == NULL) {
            for (;;) {
            }
        }
        // Start the kernel. Task(s) created in app_init() will start running.
        sl_system_kernel_start();
    }

    #endif // SL_CATALOG_KERNEL_PRESENT

    if (heap == NULL) {
        heap = malloc(HEAP_SIZE);
        heap_size = HEAP_SIZE / sizeof(uint32_t);
    }
    if (heap == NULL) {
        return SAFE_MODE_NO_HEAP;
    }
    return SAFE_MODE_NONE;
}

void reset_port(void) {
    reset_all_pins();

    #if CIRCUITPY_BUSIO
    i2c_reset();
    spi_reset();
    uart_reset();
    #endif

    #if CIRCUITPY_PWMIO
    pwmout_reset();
    #endif

    #if CIRCUITPY_ANALOGIO
    analogout_reset();
    #endif

    #if CIRCUITPY_BLEIO
    bleio_reset();
    #endif
}

void reset_to_bootloader(void) {
    CHIP_Reset();
    for (;;) {
    }
}

void reset_cpu(void) {
    CHIP_Reset();
    for (;;) {
    }
}

uint32_t *port_heap_get_bottom(void) {
    return heap;
}

uint32_t *port_heap_get_top(void) {
    return heap + heap_size;
}

bool port_has_fixed_stack(void) {
    return true;
}

uint32_t *port_stack_get_limit(void) {
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wcast-align"
    return (uint32_t *)thread_circuitpython_stk;
    #pragma GCC diagnostic pop
}

uint32_t *port_stack_get_top(void) {
    return port_stack_get_limit() + CIRCUITPY_DEFAULT_STACK_SIZE / sizeof(uint32_t);
}

uint64_t port_get_raw_ticks(uint8_t *subticks) {
    uint32_t timer_freq = sl_sleeptimer_get_timer_frequency();
    uint64_t all_subticks = (uint64_t)(sl_sleeptimer_get_tick_count()) * 1024;
    if (subticks != NULL) {
        *subticks = all_subticks % timer_freq;
    }
    return all_subticks / timer_freq;
}

// Periodic tick timer callback
STATIC void on_tick_timer_timeout(sl_sleeptimer_timer_handle_t *handle,
    void *data) {
    (void)&handle;
    (void)&data;
    supervisor_tick();

    // CircuitPython's VM is run in a separate FreeRTOS task from timer callbacks.
    // So, we have to notify the main task every time in case it's waiting for us.
    osThreadFlagsSet(tid_thread_circuitpython, 0x0001);
}

// Enable 1/1024 second tick.
void port_enable_tick(void) {
    uint32_t timer_freq = sl_sleeptimer_get_timer_frequency();

    // Create timer for waking up the system periodically.
    sl_sleeptimer_start_periodic_timer(&_tick_timer,
        timer_freq / 1024,
        on_tick_timer_timeout, NULL,
        0,
        SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG);
}

// Disable 1/1024 second tick.
void port_disable_tick(void) {
    sl_sleeptimer_stop_timer(&_tick_timer);
}

void port_wake_main_task(void) {
    osThreadFlagsSet(tid_thread_circuitpython, 0x0001);
}

STATIC void on_sleep_timer_timeout(sl_sleeptimer_timer_handle_t *handle,
    void *data) {
    port_wake_main_task();
}

void port_interrupt_after_ticks(uint32_t ticks) {
    uint32_t timer_freq = sl_sleeptimer_get_timer_frequency();

    uint32_t timer_tick = (uint32_t)((((uint64_t)ticks * timer_freq) + 1023) / 1024u);

    // Create one-shot timer for waking up the system.
    sl_sleeptimer_start_timer(&_sleep_timer,
        timer_tick,
        on_sleep_timer_timeout, NULL,
        0,
        SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG);
}

void port_idle_until_interrupt(void) {
    if (!background_callback_pending()) {
        osThreadFlagsWait(0x0001, osFlagsWaitAny, osWaitForever);
    }

}

// Place the word to save just after our BSS section that gets blanked.
void port_set_saved_word(uint32_t value) {
    __bss_end__ = value;
}

uint32_t port_get_saved_word(void) {
    return __bss_end__;
}

#if CIRCUITPY_ALARM
//  Board deinit in case boards/xxx/board.c does not provide board_deinit()
MP_WEAK void board_deinit(void) {
}
#endif

extern void main(void);

void circuitpython_thread(void *p_arg) {
    (void)p_arg;
    main();
}

__attribute__((used)) void BusFault_Handler(void) {
    reset_into_safe_mode(SAFE_MODE_HARD_FAULT);
    while (true) {
        asm ("nop;");
    }
}

__attribute__((used)) void UsageFault_Handler(void) {
    reset_into_safe_mode(SAFE_MODE_HARD_FAULT);
    while (true) {
        asm ("nop;");
    }
}

__attribute__((used)) void HardFault_Handler(void) {
    reset_into_safe_mode(SAFE_MODE_HARD_FAULT);
    while (true) {
        asm ("nop;");
    }
}
