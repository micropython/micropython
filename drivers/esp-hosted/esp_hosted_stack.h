/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Arduino SA
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
 *
 * Simple stack for control messages.
 */

#ifndef MICROPY_INCLUDED_DRIVERS_ESP_HOSTED_STACK_H
#define MICROPY_INCLUDED_DRIVERS_ESP_HOSTED_STACK_H

#include <stdbool.h>
#include <stddef.h>
#include "esp_hosted_internal.h"

static inline void esp_hosted_stack_init(esp_hosted_stack_t *stack) {
    stack->capacity = ESP_STACK_CAPACITY;
    stack->top = 0;
}

static inline bool esp_hosted_stack_empty(esp_hosted_stack_t *stack) {
    return stack->top == 0;
}

static inline bool esp_hosted_stack_push(esp_hosted_stack_t *stack, void *data) {
    if (stack->top < stack->capacity) {
        stack->buff[stack->top++] = data;
        return true;
    }
    return false;
}

static inline void *esp_hosted_stack_pop(esp_hosted_stack_t *stack, bool peek) {
    void *data = NULL;
    if (stack->top > 0) {
        data = stack->buff[stack->top - 1];
        if (!peek) {
            stack->top -= 1;
        }
    }
    return data;
}
#endif // MICROPY_INCLUDED_DRIVERS_ESP_HOSTED_STACK_H
