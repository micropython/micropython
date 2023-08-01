/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
 * Copyright (c) 2019 Artur Pacholec
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

<<<<<<<< HEAD:ports/mimxrt10xx/boards/teensy40/board.c
#include "supervisor/board.h"
#include "shared-bindings/microcontroller/Pin.h"
========
#include <stdbool.h>
#include "py/mpconfig.h"
#include "fsl_edma.h"
#include "dma_manager.h"
>>>>>>>> v1.19.1:ports/raspberrypi/dma_manager.c

// These pins should never ever be reset; doing so could interfere with basic operation.
// Used in common-hal/microcontroller/Pin.c
const mcu_pin_obj_t *mimxrt10xx_reset_forbidden_pins[] = {
    // FLEX flash
    &pin_GPIO_SD_B1_06,
    &pin_GPIO_SD_B1_07,
    &pin_GPIO_SD_B1_08,
    &pin_GPIO_SD_B1_09,
    &pin_GPIO_SD_B1_10,
    &pin_GPIO_SD_B1_11,

    // FLEX flash 2
    &pin_GPIO_AD_B0_04,
    &pin_GPIO_AD_B0_06,
    &pin_GPIO_AD_B0_07,
    &pin_GPIO_AD_B0_08,
    &pin_GPIO_AD_B0_09,
    &pin_GPIO_AD_B0_10,
    &pin_GPIO_EMC_01,
    &pin_GPIO_B0_13,
    &pin_GPIO_AD_B0_11,
    // Data strobe needs protection despite being grounded
    &pin_GPIO_SD_B1_05,
    NULL,                       // Must end in NULL.
};

<<<<<<<< HEAD:ports/mimxrt10xx/boards/teensy40/board.c
// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
========
STATIC bool dma_initialized = false;

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

void dma_init(void) {
    if (!dma_initialized) {
        edma_config_t dmaConfig;
        EDMA_GetDefaultConfig(&dmaConfig);
        EDMA_Init(DMA0, &dmaConfig);
        dma_initialized = true;
    }
}
>>>>>>>> v1.19.1:ports/raspberrypi/dma_manager.c
