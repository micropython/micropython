/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Damien P. George on behalf of Pycom Ltd
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

#include <errno.h>

#include "py/mpstate.h"
#include "py/mpthread.h"

#if MICROPY_PY_THREAD

STATIC pthread_key_t tls_key;

void mp_thread_init(void) {
    pthread_key_create(&tls_key, NULL);
    pthread_setspecific(tls_key, &mp_state_ctx.thread);
}

mp_state_thread_t *mp_thread_get_state(void) {
    return (mp_state_thread_t*)pthread_getspecific(tls_key);
}

void mp_thread_set_state(void *state) {
    pthread_setspecific(tls_key, state);
}

void mp_thread_create(void *(*entry)(void*), void *arg) {
    pthread_t id;
    pthread_create(&id, NULL, entry, arg);
}

#endif // MICROPY_PY_THREAD
