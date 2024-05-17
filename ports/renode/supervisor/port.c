// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "supervisor/background_callback.h"
#include "supervisor/port.h"

#include "genhdr/mpversion.h"
#include "shared-bindings/microcontroller/__init__.h"

#include "supervisor/shared/safe_mode.h"


extern uint32_t _ld_flash_size;
extern uint32_t _ld_stack_top;

extern uint32_t __isr_vector[];

extern uint32_t _ld_ocram_start;
extern uint32_t _ld_ocram_bss_start;
extern uint32_t _ld_ocram_bss_size;
extern uint32_t _ld_ocram_data_destination;
extern uint32_t _ld_ocram_data_size;
extern uint32_t _ld_ocram_data_flash_copy;
extern uint32_t _ld_ocram_end;
extern uint32_t _ld_dtcm_bss_start;
extern uint32_t _ld_dtcm_bss_size;
extern uint32_t _ld_dtcm_data_destination;
extern uint32_t _ld_dtcm_data_size;
extern uint32_t _ld_dtcm_data_flash_copy;
extern uint32_t _ld_itcm_destination;
extern uint32_t _ld_itcm_size;
extern uint32_t _ld_itcm_flash_copy;
extern uint32_t _ld_isr_destination;
extern uint32_t _ld_isr_size;
extern uint32_t _ld_isr_flash_copy;

extern void main(void);

// This replaces the Reset_Handler in startup_*.S and SystemInit in system_*.c.
// Turn off optimize("no-tree-loop-distribute-patterns") so that this isn't replaced
// by calls to memcpy because we're copying it over now.
void Reset_Handler(void);
__attribute__((used, naked, no_instrument_function, optimize("no-tree-loop-distribute-patterns"))) void Reset_Handler(void) {
    // Copy all of the itcm code to run from ITCM. Do this while the MPU is disabled because we write
    // protect it.
    // for (uint32_t i = 0; i < ((size_t)&_ld_itcm_size) / 4; i++) {
    //     (&_ld_itcm_destination)[i] = (&_ld_itcm_flash_copy)[i];
    // }

    // for (uint32_t i = 0; i < ((size_t)&_ld_isr_size) / 4; i++) {
    //     (&_ld_isr_destination)[i] = (&_ld_isr_flash_copy)[i];
    // }

    // Copy all of the data to run from DTCM.
    // for (uint32_t i = 0; i < ((size_t)&_ld_dtcm_data_size) / 4; i++) {
    //     (&_ld_dtcm_data_destination)[i] = (&_ld_dtcm_data_flash_copy)[i];
    // }

    // // Clear DTCM bss.
    // for (uint32_t i = 0; i < ((size_t)&_ld_dtcm_bss_size) / 4; i++) {
    //     (&_ld_dtcm_bss_start)[i] = 0;
    // }

    // Copy all of the data to run from OCRAM.
    for (uint32_t i = 0; i < ((size_t)&_ld_ocram_data_size) / 4; i++) {
        (&_ld_ocram_data_destination)[i] = (&_ld_ocram_data_flash_copy)[i];
    }

    // Clear OCRAM bss.
    for (uint32_t i = 0; i < ((size_t)&_ld_ocram_bss_size) / 4; i++) {
        (&_ld_ocram_bss_start)[i] = 0;
    }

    main();
}

typedef void (*isr_vector)(void);
const isr_vector __attribute__((used)) interrupt_table[17] = {
    (isr_vector) & _ld_ocram_end,
    Reset_Handler,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

safe_mode_t __attribute__((used)) port_init(void) {
    // Reset everything into a known state before board_init.
    reset_port();

    return SAFE_MODE_NONE;
}

void reset_port(void) {
    // Older ports will do blanket resets here. Instead, move to a model that
    // uses the deinit() functions to reset internal state.
}

void reset_to_bootloader(void) {
    while (true) {
    }
}

void reset_cpu(void) {
    while (true) {
        // __wfi();
    }
}

// From the linker script
extern uint32_t _ld_heap_start;
extern uint32_t _ld_heap_end;
uint32_t *port_stack_get_limit(void) {
    #pragma GCC diagnostic push

    #pragma GCC diagnostic ignored "-Warray-bounds"
    return port_stack_get_top() - (CIRCUITPY_DEFAULT_STACK_SIZE + CIRCUITPY_EXCEPTION_STACK_SIZE) / sizeof(uint32_t);
    #pragma GCC diagnostic pop
}

uint32_t *port_stack_get_top(void) {
    return &_ld_heap_end;
}

uint32_t *port_heap_get_bottom(void) {
    return &_ld_heap_start;
}

uint32_t *port_heap_get_top(void) {
    return port_stack_get_limit();
}

uint32_t saved_word;
void port_set_saved_word(uint32_t value) {
    // Store in RAM because the watchdog scratch registers don't survive
    // resetting by pulling the RUN pin low.
    saved_word = value;
}

uint32_t port_get_saved_word(void) {
    return saved_word;
}

static volatile bool ticks_enabled;
static volatile bool _woken_up;

uint64_t port_get_raw_ticks(uint8_t *subticks) {
    volatile uint32_t *ticks_32khz = ((volatile uint32_t *)0x40001000);
    uint32_t total_ticks = *ticks_32khz;
    if (subticks != NULL) {
        *subticks = total_ticks % 32;
    }
    return total_ticks / 32;
}

// Enable 1/1024 second tick.
void port_enable_tick(void) {
    ticks_enabled = true;
}

// Disable 1/1024 second tick.
void port_disable_tick(void) {
    // One additional _tick_callback may occur, but it will just return
    // whenever !ticks_enabled. Cancel is not called just in case
    // it could nuke a timeout set by port_interrupt_after_ticks.
    ticks_enabled = false;
}

// This is called by sleep, we ignore it when our ticks are enabled because
// they'll wake us up earlier. If we don't, we'll mess up ticks by overwriting
// the next RTC wake up time.
void port_interrupt_after_ticks(uint32_t ticks) {
    _woken_up = false;
}

void port_idle_until_interrupt(void) {
    common_hal_mcu_disable_interrupts();
    if (!background_callback_pending() && !_woken_up) {
        // __DSB();
        // __WFI();
    }
    common_hal_mcu_enable_interrupts();
}

/**
 * \brief Default interrupt handler for unused IRQs.
 */
extern void HardFault_Handler(void); // provide a prototype to avoid a missing-prototypes diagnostic
__attribute__((used)) void HardFault_Handler(void) {
    while (true) {
        asm ("nop;");
    }
}

void port_yield() {
}

void port_boot_info(void) {
}
