/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Damien P. George
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

#include <assert.h>
#include <string.h>

#include "py/obj.h"
#include "py/misc.h"
#include "py/asmbase.h"

#if MICROPY_EMIT_NATIVE || MICROPY_EMIT_INLINE_ASM

void mp_asm_base_init(mp_asm_base_t *as, size_t max_num_labels) {
    as->max_num_labels = max_num_labels;
    as->label_offsets = m_new(size_t, max_num_labels);
}

void mp_asm_base_deinit(mp_asm_base_t *as, bool free_code) {
    if (free_code) {
        MP_PLAT_FREE_EXEC(as->code_base, as->code_size);
    }
    m_del(size_t, as->label_offsets, as->max_num_labels);
}

void mp_asm_base_start_pass(mp_asm_base_t *as, int pass) {
    if (pass < MP_ASM_PASS_EMIT) {
        // Reset labels so we can detect backwards jumps (and verify unique assignment)
        memset(as->label_offsets, -1, as->max_num_labels * sizeof(size_t));
    } else {
        // allocating executable RAM is platform specific
        MP_PLAT_ALLOC_EXEC(as->code_offset, (void**)&as->code_base, &as->code_size);
        assert(as->code_base != NULL);
    }
    as->pass = pass;
    as->code_offset = 0;
}

// all functions must go through this one to emit bytes
// if as->pass < MP_ASM_PASS_EMIT, then this function just counts the number
// of bytes needed and returns NULL, and callers should not store any data
uint8_t *mp_asm_base_get_cur_to_write_bytes(mp_asm_base_t *as, size_t num_bytes_to_write) {
    uint8_t *c = NULL;
    if (as->pass == MP_ASM_PASS_EMIT) {
        assert(as->code_offset + num_bytes_to_write <= as->code_size);
        c = as->code_base + as->code_offset;
    }
    as->code_offset += num_bytes_to_write;
    return c;
}

void mp_asm_base_label_assign(mp_asm_base_t *as, size_t label) {
    assert(label < as->max_num_labels);
    if (as->pass < MP_ASM_PASS_EMIT) {
        // assign label offset
        assert(as->label_offsets[label] == (size_t)-1);
        as->label_offsets[label] = as->code_offset;
    } else {
        // ensure label offset has not changed from PASS_COMPUTE to PASS_EMIT
        assert(as->label_offsets[label] == as->code_offset);
    }
}

// align must be a multiple of 2
void mp_asm_base_align(mp_asm_base_t* as, unsigned int align) {
    as->code_offset = (as->code_offset + align - 1) & (~(align - 1));
}

// this function assumes a little endian machine
void mp_asm_base_data(mp_asm_base_t* as, unsigned int bytesize, uintptr_t val) {
    uint8_t *c = mp_asm_base_get_cur_to_write_bytes(as, bytesize);
    if (c != NULL) {
        for (unsigned int i = 0; i < bytesize; i++) {
            *c++ = val;
            val >>= 8;
        }
    }
}

#endif // MICROPY_EMIT_NATIVE || MICROPY_EMIT_INLINE_ASM
