//*****************************************************************************
//
//  Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
//
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//    Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the
//    distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

//*****************************************************************************
//
// asmdefs.h - Macros to allow assembly code be portable among toolchains.
//
//*****************************************************************************

#ifndef __ASMDEFS_H__
#define __ASMDEFS_H__

//*****************************************************************************
//
// The defines required for code_red.
//
//*****************************************************************************
#ifdef codered

//
// The assembly code preamble required to put the assembler into the correct
// configuration.
//
    .syntax unified
    .thumb

//
// Section headers.
//
#define __LIBRARY__             @
#define __TEXT__                .text
#define __DATA__                .data
#define __BSS__                 .bss
#define __TEXT_NOROOT__         .text

//
// Assembler nmenonics.
//
#define __ALIGN__               .balign 4
#define __END__                 .end
#define __EXPORT__              .globl
#define __IMPORT__              .extern
#define __LABEL__               :
#define __STR__                 .ascii
#define __THUMB_LABEL__         .thumb_func
#define __WORD__                .word
#define __INLINE_DATA__

#endif // codered

//*****************************************************************************
//
// The defines required for EW-ARM.
//
//*****************************************************************************
#ifdef ewarm

//
// Section headers.
//
#define __LIBRARY__             module
#define __TEXT__                rseg CODE:CODE(2)
#define __DATA__                rseg DATA:DATA(2)
#define __BSS__                 rseg DATA:DATA(2)
#define __TEXT_NOROOT__         rseg CODE:CODE:NOROOT(2)

//
// Assembler nmenonics.
//
#define __ALIGN__               alignrom 2
#define __END__                 end
#define __EXPORT__              export
#define __IMPORT__              import
#define __LABEL__
#define __STR__                 dcb
#define __THUMB_LABEL__         thumb
#define __WORD__                dcd
#define __INLINE_DATA__         data

#endif // ewarm

//*****************************************************************************
//
// The defines required for GCC.
//
//*****************************************************************************
#if defined(gcc)

//
// The assembly code preamble required to put the assembler into the correct
// configuration.
//
    .syntax unified
    .thumb

//
// Section headers.
//
#define __LIBRARY__             @
#define __TEXT__                .text
#define __DATA__                .data
#define __BSS__                 .bss
#define __TEXT_NOROOT__         .text

//
// Assembler nmenonics.
//
#define __ALIGN__               .balign 4
#define __END__                 .end
#define __EXPORT__              .globl
#define __IMPORT__              .extern
#define __LABEL__               :
#define __STR__                 .ascii
#define __THUMB_LABEL__         .thumb_func
#define __WORD__                .word
#define __INLINE_DATA__

#endif // gcc

//*****************************************************************************
//
// The defines required for RV-MDK.
//
//*****************************************************************************
#ifdef rvmdk

//
// The assembly code preamble required to put the assembler into the correct
// configuration.
//
    thumb
    require8
    preserve8

//
// Section headers.
//
#define __LIBRARY__             ;
#define __TEXT__                area ||.text||, code, readonly, align=2
#define __DATA__                area ||.data||, data, align=2
#define __BSS__                 area ||.bss||, noinit, align=2
#define __TEXT_NOROOT__         area ||.text||, code, readonly, align=2

//
// Assembler nmenonics.
//
#define __ALIGN__               align 4
#define __END__                 end
#define __EXPORT__              export
#define __IMPORT__              import
#define __LABEL__
#define __STR__                 dcb
#define __THUMB_LABEL__
#define __WORD__                dcd
#define __INLINE_DATA__

#endif // rvmdk

//*****************************************************************************
//
// The defines required for Sourcery G++.
//
//*****************************************************************************
#if defined(sourcerygxx)

//
// The assembly code preamble required to put the assembler into the correct
// configuration.
//
    .syntax unified
    .thumb

//
// Section headers.
//
#define __LIBRARY__             @
#define __TEXT__                .text
#define __DATA__                .data
#define __BSS__                 .bss
#define __TEXT_NOROOT__         .text

//
// Assembler nmenonics.
//
#define __ALIGN__               .balign 4
#define __END__                 .end
#define __EXPORT__              .globl
#define __IMPORT__              .extern
#define __LABEL__               :
#define __STR__                 .ascii
#define __THUMB_LABEL__         .thumb_func
#define __WORD__                .word
#define __INLINE_DATA__

#endif // sourcerygxx

#endif // __ASMDEF_H__
