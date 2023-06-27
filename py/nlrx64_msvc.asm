;*
;* This file is part of the MicroPython project, http://micropython.org/
;*
;* The MIT License (MIT)
;*
;* Copyright (c) 2013-2017 Damien P. George
;*
;* Permission is hereby granted, free of charge, to any person obtaining a copy
;* of this software and associated documentation files (the "Software"), to deal
;* in the Software without restriction, including without limitation the rights
;* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
;* copies of the Software, and to permit persons to whom the Software is
;* furnished to do so, subject to the following conditions:
;*
;* The above copyright notice and this permission notice shall be included in
;* all copies or substantial portions of the Software.
;*
;* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
;* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
;* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
;* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
;* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
;* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
;* THE SOFTWARE.
;*

; unsigned int nlr_push_tail(nlr_buf_t *nlr);
nlr_push_tail	proto

.code

;// x86-64 callee-save registers are:
;//  rbx, rbp, rsp, r12, r13, r14, r15

;unsigned int nlr_push(nlr_buf_t *nlr)
nlr_push	PROC EXPORT
	mov   rax, [rsp]         ; load return rip
	mov   16[rcx], rax       ; store rip into nlr_buf
	mov   24[rcx], rbp       ; store rbp into nlr_buf
	mov   32[rcx], rsp       ; store rsp into nlr_buf
	mov   40[rcx], rbx       ; store rbx into nlr_buf
	mov   48[rcx], r12       ; store r12 into nlr_buf
	mov   56[rcx], r13       ; store r13 into nlr_buf
	mov   64[rcx], r14       ; store r14 into nlr_buf
	mov   72[rcx], r15       ; store r15 into nlr_buf
	mov   80[rcx], rdi       ; store rdr into nlr_buf
	mov   88[rcx], rsi       ; store rsi into nlr_buf
	jmp   nlr_push_tail      ; do the rest in C
nlr_push	ENDP


; void nlr_jump_tail(void *val)
nlr_jump_tail	PROC EXPORT
                             ; rcx points to nlr_buf
	mov   rsi, 88[rcx]       ; load saved rsi
	mov   rdi, 80[rcx]       ; load saved rdi
	mov   r15, 72[rcx]       ; load saved r15
	mov   r14, 64[rcx]       ; load saved r14
	mov   r13, 56[rcx]       ; load saved r13
	mov   r12, 48[rcx]       ; load saved r12
	mov   rbx, 40[rcx]       ; load saved rbx
	mov   rsp, 32[rcx]       ; load saved rsp
	mov   rbp, 24[rcx]       ; load saved rbp
	mov   rax, 16[rcx]       ; load saved rip
	mov   [rsp], rax         ; store saved rip to stack
	xor   rax, rax           ; clear return register
	inc   al                 ; increase to make 1, non-local return
	ret
nlr_jump_tail	ENDP

				END
