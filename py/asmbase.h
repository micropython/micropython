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
#ifndef MICROPY_INCLUDED_PY_ASMBASE_H
#define MICROPY_INCLUDED_PY_ASMBASE_H

#include <stdint.h>
#include <stdbool.h>

#define MP_ASM_PASS_COMPUTE (1)
#define MP_ASM_PASS_EMIT    (2)

typedef struct _mp_asm_base_t {
    int pass;
    size_t code_offset;
    size_t code_size;
    uint8_t *code_base;

    size_t max_num_labels;
    size_t *label_offsets;
} mp_asm_base_t;

void mp_asm_base_init(mp_asm_base_t *as, size_t max_num_labels);
void mp_asm_base_deinit(mp_asm_base_t *as, bool free_code);
void mp_asm_base_start_pass(mp_asm_base_t *as, int pass);
uint8_t *mp_asm_base_get_cur_to_write_bytes(mp_asm_base_t *as, size_t num_bytes_to_write);
void mp_asm_base_label_assign(mp_asm_base_t *as, size_t label);
void mp_asm_base_align(mp_asm_base_t* as, unsigned int align);
void mp_asm_base_data(mp_asm_base_t* as, unsigned int bytesize, uintptr_t val);

static inline size_t mp_asm_base_get_code_pos(mp_asm_base_t *as) {
    return as->code_offset;
}

static inline size_t mp_asm_base_get_code_size(mp_asm_base_t *as) {
    return as->code_size;
}

static inline void *mp_asm_base_get_code(mp_asm_base_t *as) {
    #if defined(MP_PLAT_COMMIT_EXEC)
    return MP_PLAT_COMMIT_EXEC(as->code_base, as->code_size, NULL);
    #else
    return as->code_base;
    #endif
}

#endif // MICROPY_INCLUDED_PY_ASMBASE_H
