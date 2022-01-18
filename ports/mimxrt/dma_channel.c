/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Robert Hammelrath
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

#include "dma_channel.h"

// List of channel flags: true: channel used, false: channel available
static bool channel_list[FSL_FEATURE_DMAMUX_MODULE_CHANNEL] = {
    true, true, true, true, false, false, false, false,
    false, false, false, false, false, false, false, false,

    #if FSL_FEATURE_DMAMUX_MODULE_CHANNEL > 16

    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false

    #endif
};

// allocate_channel(): retrieve an available channel. Return the number or -1
int allocate_dma_channel(void) {
    for (int i = 0; i < ARRAY_SIZE(channel_list); i++) {
        if (channel_list[i] == false) { // Channel available
            channel_list[i] = true;
            return i;
        }
    }
    return -1;
}

// free_channel(n): Declare channel as free
void free_dma_channel(int n) {
    if (n >= 0 && n <= ARRAY_SIZE(channel_list)) {
        channel_list[n] = false;
    }
}
