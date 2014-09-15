/* Runtime ABI for the ARM Cortex-M0  
 * uidivmod.S: unsigned 32 bit division
 *
 * Copyright (c) 2012 Jörg Mische <bobbl@gmx.de>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */



	.syntax unified
	.text
	.thumb
	.cpu cortex-m0



@ unsigned __udivsi3(unsigned num, unsigned denom)
@
@ libgcc wrapper: just an alias for __aeabi_uidivmod(), the remainder is ignored
@
	.thumb_func
        .global __udivsi3
__udivsi3:



@ unsigned __aeabi_uidiv(unsigned num, unsigned denom)
@
@ Just an alias for __aeabi_uidivmod(), the remainder is ignored
@
	.thumb_func
        .global __aeabi_uidiv
__aeabi_uidiv:



@ {unsigned quotient:r0, unsigned remainder:r1}
@  __aeabi_uidivmod(unsigned numerator:r0, unsigned denominator:r1)
@
@ Divide r0 by r1 and return the quotient in r0 and the remainder in r1
@
	.thumb_func
        .global __aeabi_uidivmod
__aeabi_uidivmod:


	cmp	r1, #0
	bne	L_no_div0
	b	__aeabi_idiv0
L_no_div0:

	@ Shift left the denominator until it is greater than the numerator
	movs	r2, #1		@ counter
	movs	r3, #0		@ result
	cmp	r0, r1
	bls	L_sub_loop0
	adds	r1, #0		@ dont shift if denominator would overflow
	bmi	L_sub_loop0
	
L_denom_shift_loop:
	lsls	r2, #1
	lsls	r1, #1
	bmi	L_sub_loop0
	cmp	r0, r1
	bhi	L_denom_shift_loop	
	
L_sub_loop0:	
	cmp	r0, r1
	bcc	L_dont_sub0	@ if (num>denom)

	subs	r0, r1		@ numerator -= denom
	orrs	r3, r2		@ result(r3) |= bitmask(r2)
L_dont_sub0:

	lsrs	r1, #1		@ denom(r1) >>= 1
	lsrs	r2, #1		@ bitmask(r2) >>= 1
	bne	L_sub_loop0

	mov	r1, r0		@ remainder(r1) = numerator(r0)
	mov	r0, r3		@ quotient(r0) = result(r3)
	bx	lr
