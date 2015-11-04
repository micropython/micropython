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
#include <stdio.h>
#include <string.h>

#include "py/mpconfig.h"
#include "py/mphal.h"
#include "py/obj.h"
#include "inc/hw_memmap.h"
#include "pybuart.h"
#include "osi.h"
#include "mperror.h"


//*****************************************************************************
//
//! \brief Application defined idle task hook
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void vApplicationIdleHook (void)
{
    // signal that we are alive and kicking
    mperror_heartbeat_signal();
    // gate the processor's clock to save power
    __WFI();
}

//*****************************************************************************
//
//! \brief Application defined malloc failed hook
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void vApplicationMallocFailedHook (void)
{
#ifdef DEBUG
    // break into the debugger
    __asm volatile ("bkpt #0  \n");
#endif

    __fatal_error("FreeRTOS malloc failed!");
}

//*****************************************************************************
//
//! \brief Application defined stack overflow hook
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void vApplicationStackOverflowHook (OsiTaskHandle *pxTask, signed char *pcTaskName)
{
#ifdef DEBUG
    // Break into the debugger
    __asm volatile ("bkpt #0  \n");
#endif

    __fatal_error("Stack overflow!");
}

//*****************************************************************************
//
//! \brief Application defined tick hook
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void vApplicationTickHook (void)
{
    HAL_IncrementTick();
}
