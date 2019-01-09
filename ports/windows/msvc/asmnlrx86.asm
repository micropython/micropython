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

    .model flat
    .code

EXTERN      _nlr_push_tail:PROC

_nlr_push   PROC
    pop     ecx                         ; ecx <- return address
    mov     edx, [esp]                  ; edx <- nlr
    mov     [edx + 08h], ecx            ; eip value to restore in nlr_jump
    mov     [edx + 0Ch], ebp
    mov     [edx + 10h], esp
    mov     [edx + 14h], ebx
    mov     [edx + 18h], edi
    mov     [edx + 1Ch], esi

    push    ecx                        ; make sure nlr_push_tail return back to nlr_push's caller()
    jmp     _nlr_push_tail             ; do the rest in C
_nlr_push   ENDP

_asm_nlr_jump PROC
    pop     eax                         ; skip return address
    pop     edx                         ; edx <- nlr (top)
    pop     eax                         ; eax <- val
    mov     esi, [edx + 1Ch]
    mov     edi, [edx + 18h]
    mov     ebx, [edx + 14h]
    mov     esp, [edx + 10h]
    mov     ebp, [edx + 0Ch]
    jmp     dword ptr [edx + 08h]       ; restore "eip"
_asm_nlr_jump ENDP

    END
