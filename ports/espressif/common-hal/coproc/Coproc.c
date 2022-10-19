/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 microDev
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

#include "shared-bindings/coproc/Coproc.h"
#include "shared-bindings/coproc/CoprocMemory.h"

void common_hal_coproc_coproc_construct(coproc_coproc_obj_t *self,
    const uint8_t *buf, const size_t buf_len, coproc_memory_obj_t *coproc_memory) {
    // set CoprocMemory object
    self->coproc_memory = coproc_memory;

    // load buffer
    self->buf_len = buf_len;
    self->buf = (uint8_t *)m_malloc(self->buf_len, false);
    memcpy(self->buf, buf, self->buf_len);
}

bool common_hal_coproc_coproc_deinited(coproc_coproc_obj_t *self) {
    return self->buf == NULL;
}

void common_hal_coproc_coproc_deinit(coproc_coproc_obj_t *self) {
    if (common_hal_coproc_coproc_deinited(self)) {
        return;
    }
    m_free(self->buf);
    self->buf = NULL;
    self->coproc_memory = NULL;
}
