/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Jeff Epler for Adafruit Industries
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

#ifndef __INCLUDED_SUPERVISOR_TICK_H
#define __INCLUDED_SUPERVISOR_TICK_H

#include <stdint.h>
#include <stdbool.h>

/** @brief To be called once every ms
 *
 * The port must call supervisor_tick once per millisecond to perform regular tasks.
 * This is called from the SysTick interrupt or similar, and is safe to call in an
 * interrupt context.
 */
extern void supervisor_tick(void);

/** @brief Get the lower 32 bits of the time in milliseconds
 *
 * This can be more efficient than supervisor_ticks_ms64, for sites where a wraparound
 * of ~49.5 days is not harmful.
 */
extern uint32_t supervisor_ticks_ms32(void);

/** @brief Get the full time in milliseconds
 *
 * Because common ARM mcus cannot atomically work with 64-bit quantities, this
 * function must briefly disable interrupts in order to return the value.  If
 * only relative durations of less than about ~49.5 days need to be considered,
 * then it may be possible to use supervisor_ticks_ms32() instead.
 */
extern uint64_t supervisor_ticks_ms64(void);

/** @brief Run background ticks, but only about every millisecond.
 *
 * Normally, this is not called directly.  Instead use the RUN_BACKGROUND_TASKS
 * macro.
 */
extern void supervisor_run_background_if_tick(void);

extern void supervisor_enable_tick(void);
extern void supervisor_disable_tick(void);

/**
 * @brief Return true if tick-based background tasks ran within the last 1s
 *
 * Note that when ticks are not enabled, this function can return false; this is
 * intended.
 */
extern bool supervisor_background_tasks_ok(void);

#endif
