/*
 * This file is part of the Micro Python project, http://micropython.org/
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
#include MICROPY_HAL_H
#include "mptask.h"
#include "simplelink.h"
#include "pybwdt.h"
#include "debug.h"
#include "mperror.h"

/******************************************************************************
 DECLARE PRIVATE CONSTANTS
 ******************************************************************************/

/******************************************************************************
 DECLARE PRIVATE FUNCTIONS
 ******************************************************************************/

/******************************************************************************
 DECLARE PRIVATE DATA
 ******************************************************************************/

/******************************************************************************
 DECLARE PUBLIC DATA
 ******************************************************************************/
#ifdef DEBUG
OsiTaskHandle   mpTaskHandle;
#endif

/******************************************************************************
 DEFINE PUBLIC FUNCTIONS
 ******************************************************************************/

int main (void) {

    // Initialize the clocks and the interrupt system
    HAL_SystemInit();

    // Init the watchdog
    pybwdt_init0();

#ifdef DEBUG
    ASSERT (OSI_OK == osi_TaskCreate(TASK_Micropython,
                                     (const signed char *)"MicroPy",
                                     MICROPY_TASK_STACK_SIZE, NULL, MICROPY_TASK_PRIORITY, &mpTaskHandle));
#else
    ASSERT (OSI_OK == osi_TaskCreate(TASK_Micropython,
                                     (const signed char *)"MicroPy",
                                     MICROPY_TASK_STACK_SIZE, NULL, MICROPY_TASK_PRIORITY, NULL));
#endif

    osi_start();

    for ( ; ; );
}


void stoupper (char *str) {
    while (str && *str != '\0') {
        *str = (char)toupper((int)(*str));
        str++;
    }
}
