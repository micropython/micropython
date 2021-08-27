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

// Execute port specific actions during background tasks.
void port_background_task(void);

// Take port specific actions at the beginning and end of background tasks.
// This is used e.g., to set a monitoring pin for debug purposes.  "Actual
// work" should be done in port_background_task() instead.
void port_start_background_task(void);
void port_finish_background_task(void);

// Some ports need special handling to wake the main task from an interrupt
// context or other task.  The port must implement the necessary code in this
// function.  A default weak implementation is provided that does nothing.
void port_wake_main_task(void);
#endif  // MICROPY_INCLUDED_SUPERVISOR_PORT_H
