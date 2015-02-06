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

#ifndef FAULT_REGISTERS_H_
#define FAULT_REGISTERS_H_


typedef struct
{
    uint32_t    IERR        :1;
    uint32_t    DERR        :1;
    uint32_t                :1;
    uint32_t    MUSTKE      :1;
    uint32_t    MSTKE       :1;
    uint32_t    MLSPERR     :1;
    uint32_t                :1;
    uint32_t    MMARV       :1;
    uint32_t    IBUS        :1;
    uint32_t    PRECISE     :1;
    uint32_t    IMPRE       :1;
    uint32_t    BUSTKE      :1;
    uint32_t    BSTKE       :1;
    uint32_t    BLSPERR     :1;
    uint32_t                :1;
    uint32_t    BFARV       :1;
    uint32_t    UNDEF       :1;
    uint32_t    INVSTAT     :1;
    uint32_t    INVCP       :1;
    uint32_t    NOCP        :1;
    uint32_t                :4;
    uint32_t    UNALIGN     :1;
    uint32_t    DIVO0       :1;
    uint32_t                :6;

}_CFSR_t;


typedef struct
{

    uint32_t    DBG         :1;
    uint32_t    FORCED      :1;
    uint32_t                :28;
    uint32_t    VECT        :1;
    uint32_t                :1;

}_HFSR_t;


#endif /* FAULT_REGISTERS_H_ */
