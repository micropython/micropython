;
; This file is part of the MicroPython project, http://micropython.org/
;
; The MIT License (MIT)
;
; Copyright (c) 2019 Jian Wang
;
; Permission is hereby granted, free of charge, to any person obtaining a copy
; of this software and associated documentation files (the "Software"), to deal
; in the Software without restriction, including without limitation the rights
; to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
; copies of the Software, and to permit persons to whom the Software is
; furnished to do so, subject to the following conditions:
;
; The above copyright notice and this permission notice shall be included in
; all copies or substantial portions of the Software.
;
; THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
; IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
; FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
; AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
; LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
; OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
; THE SOFTWARE.
;

EXTERN      nlr_push_tail:PROC

    .code

nlr_push   PROC
    pop     rdx                 ; load rip as well as fix rsp
    mov     [rcx + 10h], rbx
    mov     [rcx + 18h], rsp
    mov     [rcx + 20h], rbp
    mov     [rcx + 28h], rdi
    mov     [rcx + 30h], rsi
    mov     [rcx + 38h], r12
    mov     [rcx + 40h], r13
    mov     [rcx + 48h], r14
    mov     [rcx + 50h], r15
    mov     [rcx + 58h], rdx   ; rip

    push   rdx                      ;nlr_push_tail needs the return address
    jmp    nlr_push_tail   ;do the rest in C
nlr_push   ENDP

asm_nlr_jump PROC
    mov     rbx, [rcx + 10h]
    mov     rsp, [rcx + 18h]
    mov     rbp, [rcx + 20h]
    mov     rdi, [rcx + 28h]
    mov     rsi, [rcx + 30h]
    mov     r12, [rcx + 38h]
    mov     r13, [rcx + 40h]
    mov     r14, [rcx + 48h]
    mov     r15, [rcx + 50h]

    mov     rax, rdx               ; set return value
    jmp     qword ptr [rcx + 58h]     ; rip
asm_nlr_jump ENDP

    END

