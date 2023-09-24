/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2017 Scott Shawcroft for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_SUPERVISOR_PORT_H
#define MICROPY_INCLUDED_SUPERVISOR_PORT_H

#include "py/mpconfig.h"

#include "supervisor/memory.h"
#include "supervisor/shared/safe_mode.h"

// Provided by the linker;
extern uint32_t _ezero;

// This file defines core methods that must be implemented by a port.
extern uint32_t _estack;

// Stored at the end of the bss section (which includes the heap).
extern uint32_t _ebss;

safe_mode_t port_init(void);

// If the port does not initialize the heap during port_init(), it must provide
// this function which is called after CIRCUITPY is mounted.
// If not required, a default (weak) implementation that does nothing is used.
safe_mode_t port_heap_init(safe_mode_t);

// Reset the microcontroller completely.
void reset_cpu(void) NORETURN;

// Reset the microcontroller state.
void reset_port(void);

// Reset to the bootloader
void reset_to_bootloader(void) NORETURN;

// Get stack limit address
uint32_t *port_stack_get_limit(void);

// Get stack top address
uint32_t *port_stack_get_top(void);

// True if stack is not located inside heap (at the top)
bool port_has_fixed_stack(void);

// Get heap bottom address
uint32_t *port_heap_get_bottom(void);

// Get heap top address
uint32_t *port_heap_get_top(void);

// Save and retrieve a word from memory that is preserved over reset. Used for safe mode.
void port_set_saved_word(uint32_t);
uint32_t port_get_saved_word(void);

// Get the raw tick count since start up. A tick is 1/1024 of a second, a common low frequency
// clock rate. If subticks is not NULL then the port will fill in the number of subticks where each
// tick is 32 subticks (for a resolution of 1/32768 or 30.5ish microseconds.)
uint64_t port_get_raw_ticks(uint8_t *subticks);

// Enable 1/1024 second tick.
void port_enable_tick(void);

// Disable 1/1024 second tick.
void port_disable_tick(void);

// Wake the CPU after the given number of ticks or sooner. Only the last call to this will apply.
// Only the common sleep routine should use it.
void port_interrupt_after_ticks(uint32_t ticks);

// Sleep the CPU until an interrupt is received. We call this idle because it
// may not be a system level sleep.
void port_idle_until_interrupt(void);

// Execute port specific actions during background tick. Only if ticks are enabled.
void port_background_tick(void);

// Execute port specific actions during background tasks. This is before the
// background callback system and happens *very* often. Use
// port_background_tick() when possible.
void port_background_task(void);

// Take port specific actions at the beginning and end of background ticks.
// This is used e.g., to set a monitoring pin for debug purposes.  "Actual
// work" should be done in port_background_tick() instead.
void port_start_background_tick(void);
void port_finish_background_tick(void);

// Some ports need special handling to wake the main task from another task. The
// port must implement the necessary code in this function.  A default weak
// implementation is provided that does nothing.
void port_wake_main_task(void);

// Some ports need special handling to wake the main task from an interrupt
// context.  The port must implement the necessary code in this function.  A
// default weak implementation is provided that does nothing.
void port_wake_main_task_from_isr(void);

// Some ports may use real RTOS tasks besides the background task framework of
// CircuitPython. Calling this will yield to other tasks and then return to the
// CircuitPython task when others are done.
void port_yield(void);

// Some ports need special handling just after completing boot.py execution.
// This function is called once while boot.py's VM is still valid, and
// then a second time after the VM is finalized.
// A default weak implementation is provided that does nothing.
void port_post_boot_py(bool heap_valid);

// Some ports want to add information to boot_out.txt.
// A default weak implementation is provided that does nothing.
void port_boot_info(void);

// Some ports want to mark additional pointers as gc roots.
// A default weak implementation is provided that does nothing.
void port_gc_collect(void);

#endif  // MICROPY_INCLUDED_SUPERVISOR_PORT_H
