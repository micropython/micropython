/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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

#include "py/mpstate.h"
#include "mpexception.h"


/******************************************************************************
DECLARE PRIVATE FUNCTIONS
 ******************************************************************************/
STATIC void mpexception_set_user_interrupt (int chr, void *data);

/******************************************************************************
DECLARE EXPORTED DATA
 ******************************************************************************/
const char mpexception_value_invalid_arguments[]    = "invalid argument(s) value";
const char mpexception_num_type_invalid_arguments[] = "invalid argument(s) num/type";
const char mpexception_uncaught[]                   = "uncaught exception";

int   user_interrupt_char = -1;

/******************************************************************************
DECLARE PRIVATE DATA
 ******************************************************************************/
STATIC void *user_interrupt_data = NULL;

/******************************************************************************
DEFINE PUBLIC FUNCTIONS
 ******************************************************************************/

void mpexception_init0 (void) {
    // Create an exception object for interrupting through the stdin uart
    MP_STATE_PORT(mp_const_user_interrupt) = mp_obj_new_exception(&mp_type_KeyboardInterrupt);
    mpexception_set_user_interrupt (-1, MP_STATE_PORT(mp_const_user_interrupt));
}

void mpexception_set_interrupt_char (int c) {
    if (c != -1) {
        mp_obj_exception_clear_traceback(MP_STATE_PORT(mp_const_user_interrupt));
    }
    mpexception_set_user_interrupt(c, MP_STATE_PORT(mp_const_user_interrupt));
}

// Call this function to raise a pending exception during an interrupt.
// It will try to raise the exception "softly" by setting the
// mp_pending_exception variable hoping that the VM will notice it.
void mpexception_nlr_jump (void *o) {
    if (MP_STATE_PORT(mp_pending_exception) == MP_OBJ_NULL) {
        MP_STATE_PORT(mp_pending_exception) = o;
    }
}

void mpexception_keyboard_nlr_jump (void) {
    mpexception_nlr_jump (user_interrupt_data);
}

/******************************************************************************
DEFINE PRIVATE FUNCTIONS
 ******************************************************************************/

STATIC void mpexception_set_user_interrupt (int chr, void *data) {
    user_interrupt_char = chr;
    user_interrupt_data = data;
}
