/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Renesas Electronics Corporation
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

#ifndef RA_RA_UTILS_H_
#define RA_RA_UTILS_H_

#include <stdint.h>
#include <stdbool.h>

__attribute__((always_inline)) static inline void ra_enable_irq(__attribute__((unused)) uint32_t state) {
    __asm__ volatile ("cpsie f"
        :
        :
        : "memory");
}

__attribute__((always_inline)) static inline uint32_t ra_disable_irq(void) {
    uint32_t state;
    __asm__ volatile ("cpsid f"
        : "=r" (state)
        :
        : "memory");
    return state;
}

void ra_mstpcra_stop(uint32_t mod_mask);
void ra_mstpcra_start(uint32_t mod_mask);
void ra_mstpcrb_stop(uint32_t mod_mask);
void ra_mstpcrb_start(uint32_t mod_mask);
void ra_mstpcrc_stop(uint32_t mod_mask);
void ra_mstpcrc_start(uint32_t mod_mask);
void ra_mstpcrd_stop(uint32_t mod_mask);
void ra_mstpcrd_start(uint32_t mod_mask);

#endif /* RA_RA_UTILS_H_ */
