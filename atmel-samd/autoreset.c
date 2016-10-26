/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
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

#include "autoreset.h"

#include "asf/sam0/drivers/tc/tc_interrupt.h"
#include "lib/mp-readline/readline.h"
#include "py/mphal.h"

struct tc_module autoreset_timer;

static bool autoreset_initialized = false;

static bool autoreset_enabled = false;

extern void inject_character(char);
void mp_keyboard_interrupt(void);

void autoreset_callback(struct tc_module *const module_inst) {
    if (autoreset_enabled) {
        mp_keyboard_interrupt();
        inject_character(CHAR_CTRL_D);
    }

    tc_stop_counter(&autoreset_timer);
}

void autoreset_init() {
    #ifdef AUTORESET_TIMER
        struct tc_config config_tc;
        tc_get_config_defaults(&config_tc);
        config_tc.counter_size    = TC_COUNTER_SIZE_16BIT;
    	config_tc.clock_prescaler = TC_CLOCK_PRESCALER_DIV256;
        config_tc.counter_16_bit.compare_capture_channel[0] = 0xFFFF;
        tc_init(&autoreset_timer, AUTORESET_TIMER, &config_tc);
        tc_enable(&autoreset_timer);
        tc_register_callback(&autoreset_timer, autoreset_callback, TC_CALLBACK_CC_CHANNEL0);
        tc_enable_callback(&autoreset_timer, TC_CALLBACK_CC_CHANNEL0);
        tc_stop_counter(&autoreset_timer);

        autoreset_initialized = true;
    #endif
}

void autoreset_enable() {
    autoreset_enabled = true;
}

void autoreset_disable() {
    autoreset_enabled = false;
}

void autoreset_start() {
    if (!autoreset_initialized) return;

    tc_stop_counter(&autoreset_timer);
    tc_start_counter(&autoreset_timer);
}

void autoreset_stop() {
    if (!autoreset_initialized) return;

    tc_stop_counter(&autoreset_timer);
}
