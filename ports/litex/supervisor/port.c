// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>
#include "supervisor/board.h"
#include "supervisor/port.h"
#include "supervisor/shared/tick.h"
#include "irq.h"
#include "csr.h"

#include "shared-bindings/microcontroller/__init__.h"

// Global millisecond tick count. 1024 per second because most RTCs are clocked with 32.768khz
// crystals.
volatile uint64_t raw_ticks = 0;
volatile int subsecond = 0;
void SysTick_Handler(void);
__attribute__((section(".ramtext")))
void SysTick_Handler(void) {
    timer0_ev_pending_write(1);
    raw_ticks += 1;
    subsecond += 1;
    // We track subsecond ticks so that we can increment raw_ticks one extra every 40 ms. We do this
    // every 40 except 0 to make it 24 increments and not 25.
    if (subsecond == 1000) {
        subsecond = 0;
    } else if (subsecond % 40 == 0) {
        raw_ticks += 1;
    }
    supervisor_tick();
}

static void tick_init(void) {
    int t;

    timer0_en_write(0);
    t = CONFIG_CLOCK_FREQUENCY / 1000; // 1000 kHz tick
    timer0_reload_write(t);
    timer0_load_write(t);
    timer0_en_write(1);
    timer0_ev_enable_write(1);
    timer0_ev_pending_write(1);
    irq_setmask(irq_getmask() | (1 << TIMER0_INTERRUPT));
}

safe_mode_t port_init(void) {
    irq_setmask(0);
    irq_setie(1);
    tick_init();
    return SAFE_MODE_NONE;
}

extern uint32_t _ebss;
extern uint32_t _heap_start;
extern uint32_t _estack;

void reset_port(void) {
    // reset_all_pins();
    // i2c_reset();
    // spi_reset();
    // uart_reset();
    // pwmout_reset();
}

void reset_to_bootloader(void) {
    reboot_ctrl_write(0xac);
    for (;;) {}
}

void reset_cpu(void) {
    // "You can reset Fomu by writing a special value to the CSR_REBOOT_CTRL
    // register at 0xe0006000L. All writes to this register must start with
    // 0xac, to ensure random values aren’t written. We can reboot Fomu by
    // simply writing this value" --
    //     https://workshop.fomu.im/en/latest/riscv.html
    reboot_ctrl_write(0xac);
    for (;;) {}
}

uint32_t *port_heap_get_bottom(void) {
    return &_ebss;
}

uint32_t *port_heap_get_top(void) {
    return port_stack_get_limit();
}

uint32_t *port_stack_get_limit(void) {
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Warray-bounds"
    return port_stack_get_top() - (CIRCUITPY_DEFAULT_STACK_SIZE + CIRCUITPY_EXCEPTION_STACK_SIZE) / sizeof(uint32_t);
    #pragma GCC diagnostic pop
}

uint32_t *port_stack_get_top(void) {
    return &_estack;
}

// Place the word to save just after our BSS section that gets blanked.
void port_set_saved_word(uint32_t value) {
    _ebss = value;
}

uint32_t port_get_saved_word(void) {
    return _ebss;
}

uint64_t port_get_raw_ticks(uint8_t *subticks) {
    // Reading 64 bits may take two loads, so turn of interrupts while we do it.
    common_hal_mcu_disable_interrupts();
    uint64_t raw_tick_snapshot = raw_ticks;
    common_hal_mcu_enable_interrupts();
    return raw_tick_snapshot;
}

// Enable 1/1024 second tick.
void port_enable_tick(void) {
}

// Disable 1/1024 second tick.
void port_disable_tick(void) {
}

void port_interrupt_after_ticks(uint32_t ticks) {
}

// TODO: Add sleep support if the SoC supports sleep.
void port_idle_until_interrupt(void) {
}
