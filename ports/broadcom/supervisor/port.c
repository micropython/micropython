/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
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

#include <string.h>
#include <stdlib.h>

#include "supervisor/board.h"
#include "supervisor/port.h"

#include "genhdr/mpversion.h"

#include "common-hal/rtc/RTC.h"
#include "common-hal/busio/I2C.h"
#include "common-hal/busio/SPI.h"
#include "common-hal/busio/UART.h"

#include "peripherals/broadcom/caches.h"
#include "peripherals/broadcom/cpu.h"
#include "peripherals/broadcom/defines.h"
#include "peripherals/broadcom/interrupts.h"
#include "peripherals/broadcom/mmu.h"

#include "shared-bindings/microcontroller/__init__.h"

#include "supervisor/shared/safe_mode.h"
#include "supervisor/shared/stack.h"
#include "supervisor/shared/tick.h"

#include "peripherals/broadcom/vcmailbox.h"

// #include "tusb.h"

extern volatile bool mp_msc_enabled;

STATIC uint32_t board_revision;

safe_mode_t port_init(void) {
    board_revision = vcmailbox_get_board_revision();
    setup_mmu_flat_map();

    init_caches();

    // Set brown out.

    // Reset everything into a known state before board_init.
    reset_port();

    // Initialize RTC
    // common_hal_rtc_init();

    // For the tick.
    // hardware_alarm_claim(0);
    // hardware_alarm_set_callback(0, _tick_callback);

    // Check brownout.

    if (board_requests_safe_mode()) {
        return SAFE_MODE_USER;
    }

    return SAFE_MODE_NONE;
}

void reset_port(void) {
    #if CIRCUITPY_BUSIO
    reset_i2c();
    reset_spi();
    reset_uart();
    #endif

    #if CIRCUITPY_PWMIO
    pwmout_reset();
    #endif

    #if CIRCUITPY_RTC
    rtc_reset();
    #endif

    #if CIRCUITPY_AUDIOPWMIO
    audiopwmout_reset();
    #endif
    #if CIRCUITPY_AUDIOCORE
    audio_dma_reset();
    #endif

    reset_all_pins();
}

void reset_to_bootloader(void) {
    // reset_usb_boot(0, 0);
    while (true) {
    }
}

void reset_cpu(void) {
    // Don't actually reset because we can't store the safe mode info.
    // PM->WDOG = 1 << PM_WDOG_TIME_Pos | PM_WDOG_PASSWD_PASSWD << PM_WDOG_PASSWD_Pos;
    // PM->RSTC = PM_RSTC_WRCFG_FULL_RESET << PM_RSTC_WRCFG_Pos | PM_RSTC_PASSWD_PASSWD << PM_RSTC_PASSWD_Pos;
    while (true) {
    }
}

// From the linker script
extern uint32_t __bss_end;
extern uint32_t _ld_ram_end;
uint32_t *port_stack_get_limit(void) {
    return (uint32_t *)0x4;
}

uint32_t *port_stack_get_top(void) {
    return (uint32_t *)0x100000;
}

uint32_t *port_heap_get_bottom(void) {
    return &__bss_end;
}

uint32_t *port_heap_get_top(void) {
    // 32M heap
    return (uint32_t *)(((size_t)&__bss_end) + 32 * 1024 * 1024);
}

void port_set_saved_word(uint32_t value) {
    // NOTE: This doesn't survive pressing the reset button (aka toggling RUN).
    __bss_end = value;
    data_clean_and_invalidate(&__bss_end, sizeof(uint32_t));
}

uint32_t port_get_saved_word(void) {
    return __bss_end;
}

uint64_t port_get_raw_ticks(uint8_t *subticks) {
    COMPLETE_MEMORY_READS;
    uint64_t hi = SYSTMR->CHI;
    uint64_t lo = SYSTMR->CLO;
    if (hi != SYSTMR->CHI) {
        hi = SYSTMR->CHI;
        lo = SYSTMR->CLO;
    }
    COMPLETE_MEMORY_READS;
    uint64_t microseconds = hi << 32 | lo;
    return 1024 * (microseconds / 1000000) + (microseconds % 1000000) / 977;
}

void TIMER_1_IRQHandler(void) {
    SYSTMR->C1 += 977;
    SYSTMR->CS_b.M1 = 1;
    supervisor_tick();
}

// Enable 1/1024 second tick.
void port_enable_tick(void) {
    COMPLETE_MEMORY_READS;
    SYSTMR->CS_b.M1 = 1;
    SYSTMR->C1 = SYSTMR->CLO + 977;
    BP_EnableIRQ(TIMER_1_IRQn);
}

// Disable 1/1024 second tick.
void port_disable_tick(void) {
    BP_DisableIRQ(TIMER_1_IRQn);
}

void port_interrupt_after_ticks(uint32_t ticks) {
}

void port_idle_until_interrupt(void) {
    common_hal_mcu_disable_interrupts();
//     if (!tud_task_event_ready()) {
// //	asm volatile ("dsb 0xF":::"memory");
// //        __wfi();
//     }
    common_hal_mcu_enable_interrupts();
}
