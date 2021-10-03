/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Daniel Campora
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

#include <stdint.h>
#include <string.h>
#include <ctype.h>

#include "py/mpconfig.h"
#include "py/mphal.h"
#include "mptask.h"
#include "simplelink.h"
#include "pybwdt.h"
#include "debug.h"
#include "antenna.h"
#include "mperror.h"
#include "task.h"

/******************************************************************************
 DECLARE PRIVATE CONSTANTS
 ******************************************************************************/

/******************************************************************************
 DECLARE PRIVATE FUNCTIONS
 ******************************************************************************/

/******************************************************************************
 DECLARE PRIVATE DATA
 ******************************************************************************/

// This is the static memory (TCB and stack) for the idle task
static StaticTask_t xIdleTaskTCB __attribute__ ((section(".rtos_heap")));
static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE] __attribute__ ((section(".rtos_heap"))) __attribute__((aligned(8)));

/******************************************************************************
 DECLARE PUBLIC DATA
 ******************************************************************************/
#ifdef DEBUG
OsiTaskHandle mpTaskHandle;
#endif

// This is the FreeRTOS heap, defined here so we can put it in a special segment
uint8_t ucHeap[ configTOTAL_HEAP_SIZE ] __attribute__ ((section(".rtos_heap"))) __attribute__((aligned(8)));

// This is the static memory (TCB and stack) for the main MicroPython task
StaticTask_t mpTaskTCB __attribute__ ((section(".rtos_heap")));
StackType_t mpTaskStack[MICROPY_TASK_STACK_LEN] __attribute__ ((section(".rtos_heap"))) __attribute__((aligned(8)));

/******************************************************************************
 DEFINE PUBLIC FUNCTIONS
 ******************************************************************************/

__attribute__ ((section(".boot")))
int main(void) {

    // Initialize the clocks and the interrupt system
    HAL_SystemInit();

    #if MICROPY_HW_ANTENNA_DIVERSITY
    // configure the antenna selection pins
    antenna_init0();
    #endif

    // Init the watchdog
    pybwdt_init0();

    #ifndef DEBUG
    OsiTaskHandle mpTaskHandle;
    #endif
    mpTaskHandle = xTaskCreateStatic(TASK_MicroPython, "MicroPy",
        MICROPY_TASK_STACK_LEN, NULL, MICROPY_TASK_PRIORITY, mpTaskStack, &mpTaskTCB);
    ASSERT(mpTaskHandle != NULL);

    osi_start();

    for ( ; ;) {;
    }
}

// We need this when configSUPPORT_STATIC_ALLOCATION is enabled
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
    StackType_t **ppxIdleTaskStackBuffer,
    uint32_t *pulIdleTaskStackSize) {
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
