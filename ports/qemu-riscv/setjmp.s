/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Alessandro Gatti
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

/* Provide a base setjmp/longjmp implementation to not link libc in. */

	.globl setjmp
	.type  setjmp, @function

setjmp:
	sw   ra, 0(a0)
	sw   s0, 4(a0)
	sw   s1, 8(a0)
	sw   s2, 12(a0)
	sw   s3, 16(a0)
	sw   s4, 20(a0)
	sw   s5, 24(a0)
	sw   s6, 28(a0)
	sw   s7, 32(a0)
	sw   s8, 36(a0)
	sw   s9, 40(a0)
	sw   s10, 44(a0)
	sw   s11, 48(a0)
	sw   sp, 52(a0)
	addi a0, zero, 0
	jalr zero, ra, 0

	.size  setjmp, .-setjmp

	.globl longjmp
	.type  longjmp, @function

longjmp:
	lw    ra, 0(a0)
	lw    s0, 4(a0)
	lw    s1, 8(a0)
	lw    s2, 12(a0)
	lw    s3, 16(a0)
	lw    s4, 20(a0)
	lw    s5, 24(a0)
	lw    s6, 28(a0)
	lw    s7, 32(a0)
	lw    s8, 36(a0)
	lw    s9, 40(a0)
	lw    s10, 44(a0)
	lw    s11, 48(a0)
	lw    sp, 52(a0)
	sltiu a0, a1, 1
	add   a0, a0, a1
	jalr  zero, ra, 0

	.size  longjmp, .-longjmp

