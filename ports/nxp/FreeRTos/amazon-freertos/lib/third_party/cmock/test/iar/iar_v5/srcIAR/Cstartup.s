;* ----------------------------------------------------------------------------
;*         ATMEL Microcontroller Software Support  -  ROUSSET  -
;* ----------------------------------------------------------------------------
;* Copyright (c) 2006, Atmel Corporation
;
;* All rights reserved.
;*
;* Redistribution and use in source and binary forms, with or without
;* modification, are permitted provided that the following conditions are met:
;*
;* - Redistributions of source code must retain the above copyright notice,
;* this list of conditions and the disclaimer below.
;*
;* - Redistributions in binary form must reproduce the above copyright notice,
;* this list of conditions and the disclaimer below in the documentation and/or
;* other materials provided with the distribution.
;*
;* Atmel's name may not be used to endorse or promote products derived from
;* this software without specific prior written permission.
;*
;* DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
;* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
;* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
;* DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
;* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
;* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
;* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
;* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
;* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
;* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;* ----------------------------------------------------------------------------

;------------------------------------------------------------------------------
; Include your AT91 Library files
;------------------------------------------------------------------------------
#include "AT91SAM7X256_inc.h"
;------------------------------------------------------------------------------

#define TOP_OF_MEMORY    (AT91C_ISRAM + AT91C_ISRAM_SIZE)
#define IRQ_STACK_SIZE   (3*8*4)
     ; 3 words to be saved per interrupt priority level

; Mode, correspords to bits 0-5 in CPSR
MODE_BITS DEFINE  0x1F    ; Bit mask for mode bits in CPSR
USR_MODE  DEFINE  0x10    ; User mode
FIQ_MODE  DEFINE  0x11    ; Fast Interrupt Request mode
IRQ_MODE  DEFINE  0x12    ; Interrupt Request mode
SVC_MODE  DEFINE  0x13    ; Supervisor mode
ABT_MODE  DEFINE  0x17    ; Abort mode
UND_MODE  DEFINE  0x1B    ; Undefined Instruction mode
SYS_MODE  DEFINE  0x1F    ; System mode

I_BIT     DEFINE  0x80
F_BIT     DEFINE  0x40

;------------------------------------------------------------------------------
; ?RESET
; Reset Vector.
; Normally, segment INTVEC is linked at address 0.
; For debugging purposes, INTVEC may be placed at other addresses.
; A debugger that honors the entry point will start the
; program in a normal way even if INTVEC is not at address 0.
;------------------------------------------------------------------------------
        SECTION .intvec:CODE:NOROOT(2)
        PUBLIC  __vector
        PUBLIC  __iar_program_start

        ARM
__vector:
        ldr  pc,[pc,#+24]             ;; Reset
__und_handler:
        ldr  pc,[pc,#+24]             ;; Undefined instructions
__swi_handler:
        ldr  pc,[pc,#+24]             ;; Software interrupt (SWI/SVC)
__prefetch_handler:
        ldr  pc,[pc,#+24]             ;; Prefetch abort
__data_handler:
        ldr  pc,[pc,#+24]             ;; Data abort
        DC32  0xFFFFFFFF              ;; RESERVED
__irq_handler:
        ldr  pc,[pc,#+24]             ;; IRQ
__fiq_handler:
        ldr  pc,[pc,#+24]             ;; FIQ

        DC32  __iar_program_start
        DC32  __und_handler
        DC32  __swi_handler
        DC32  __prefetch_handler
        DC32  __data_handler
        B .
        DC32  IRQ_Handler_Entry
        DC32  FIQ_Handler_Entry

;------------------------------------------------------------------------------
;- Manage exception: The exception must be ensure in ARM mode
;------------------------------------------------------------------------------
        SECTION text:CODE:NOROOT(2)
        ARM
;------------------------------------------------------------------------------
;- Function             : FIQ_Handler_Entry
;- Treatments           : FIQ Controller Interrupt Handler.
;-                        R8 is initialize in Cstartup
;- Called Functions     : None only by FIQ
;------------------------------------------------------------------------------
FIQ_Handler_Entry:

;- Switch in SVC/User Mode to allow User Stack access for C code
; because the FIQ is not yet acknowledged

;- Save and r0 in FIQ_Register
        mov         r9,r0
        ldr         r0 , [r8, #AIC_FVR]
        msr         CPSR_c,#I_BIT | F_BIT | SVC_MODE
;- Save scratch/used registers and LR in User Stack
        stmfd       sp!, { r1-r3, r12, lr}

;- Branch to the routine pointed by the AIC_FVR
        mov         r14, pc
        bx          r0

;- Restore scratch/used registers and LR from User Stack
        ldmia       sp!, { r1-r3, r12, lr}

;- Leave Interrupts disabled and switch back in FIQ mode
        msr         CPSR_c, #I_BIT | F_BIT | FIQ_MODE

;- Restore the R0 ARM_MODE_SVC register
        mov         r0,r9

;- Restore the Program Counter using the LR_fiq directly in the PC
        subs        pc,lr,#4
;------------------------------------------------------------------------------
;- Function             : IRQ_Handler_Entry
;- Treatments           : IRQ Controller Interrupt Handler.
;- Called Functions     : AIC_IVR[interrupt]
;------------------------------------------------------------------------------
IRQ_Handler_Entry:
;-------------------------
;- Manage Exception Entry
;-------------------------
;- Adjust and save LR_irq in IRQ stack
    sub         lr, lr, #4
    stmfd       sp!, {lr}

;- Save r0 and SPSR (need to be saved for nested interrupt)
    mrs         r14, SPSR
    stmfd       sp!, {r0,r14}

;- Write in the IVR to support Protect Mode
;- No effect in Normal Mode
;- De-assert the NIRQ and clear the source in Protect Mode
    ldr         r14, =AT91C_BASE_AIC
    ldr         r0 , [r14, #AIC_IVR]
    str         r14, [r14, #AIC_IVR]

;- Enable Interrupt and Switch in Supervisor Mode
    msr         CPSR_c, #SVC_MODE

;- Save scratch/used registers and LR in User Stack
    stmfd       sp!, { r1-r3, r12, r14}

;----------------------------------------------
;- Branch to the routine pointed by the AIC_IVR
;----------------------------------------------
    mov         r14, pc
    bx          r0

;----------------------------------------------
;- Manage Exception Exit
;----------------------------------------------
;- Restore scratch/used registers and LR from User Stack
    ldmia       sp!, { r1-r3, r12, r14}

;- Disable Interrupt and switch back in IRQ mode
    msr         CPSR_c, #I_BIT | IRQ_MODE

;- Mark the End of Interrupt on the AIC
    ldr         r14, =AT91C_BASE_AIC
    str         r14, [r14, #AIC_EOICR]

;- Restore SPSR_irq and r0 from IRQ stack
    ldmia       sp!, {r0,r14}
    msr         SPSR_cxsf, r14

;- Restore adjusted  LR_irq from IRQ stack directly in the PC
    ldmia       sp!, {pc}^

;------------------------------------------------------------------------------
;- Exception Vectors
;------------------------------------------------------------------------------
    PUBLIC    AT91F_Default_FIQ_handler
    PUBLIC    AT91F_Default_IRQ_handler
    PUBLIC    AT91F_Spurious_handler

    ARM      ; Always ARM mode after exeption

AT91F_Default_FIQ_handler
    b         AT91F_Default_FIQ_handler

AT91F_Default_IRQ_handler
    b         AT91F_Default_IRQ_handler

AT91F_Spurious_handler
    b         AT91F_Spurious_handler


;------------------------------------------------------------------------------
; ?INIT
; Program entry.
;------------------------------------------------------------------------------

    SECTION FIQ_STACK:DATA:NOROOT(3)
    SECTION IRQ_STACK:DATA:NOROOT(3)
    SECTION SVC_STACK:DATA:NOROOT(3)
    SECTION ABT_STACK:DATA:NOROOT(3)
    SECTION UND_STACK:DATA:NOROOT(3)
    SECTION CSTACK:DATA:NOROOT(3)
    SECTION text:CODE:NOROOT(2)
    REQUIRE __vector
    EXTERN  ?main
    PUBLIC  __iar_program_start
    EXTERN  AT91F_LowLevelInit


__iar_program_start:

;------------------------------------------------------------------------------
;- Low level Init is performed in a C function: AT91F_LowLevelInit
;- Init Stack Pointer to a valid memory area before calling AT91F_LowLevelInit
;------------------------------------------------------------------------------

;- Retrieve end of RAM address

                ldr     r13,=TOP_OF_MEMORY          ;- Temporary stack in internal RAM for Low Level Init execution
                ldr     r0,=AT91F_LowLevelInit
                mov     lr, pc
                bx      r0                          ;- Branch on C function (with interworking)

; Initialize the stack pointers.
; The pattern below can be used for any of the exception stacks:
; FIQ, IRQ, SVC, ABT, UND, SYS.
; The USR mode uses the same stack as SYS.
; The stack segments must be defined in the linker command file,
; and be declared above.

                mrs     r0,cpsr                             ; Original PSR value
                bic     r0,r0,#MODE_BITS                    ; Clear the mode bits
                orr     r0,r0,#SVC_MODE                     ; Set SVC mode bits
                msr     cpsr_c,r0                           ; Change the mode
                ldr     sp,=SFE(SVC_STACK)                  ; End of SVC_STACK

                bic     r0,r0,#MODE_BITS                    ; Clear the mode bits
                orr     r0,r0,#UND_MODE                     ; Set UND mode bits
                msr     cpsr_c,r0                           ; Change the mode
                ldr     sp,=SFE(UND_STACK)                  ; End of UND_STACK

                bic     r0,r0,#MODE_BITS                    ; Clear the mode bits
                orr     r0,r0,#ABT_MODE                     ; Set ABT mode bits
                msr     cpsr_c,r0                           ; Change the mode
                ldr     sp,=SFE(ABT_STACK)                  ; End of ABT_STACK

                bic     r0,r0,#MODE_BITS                    ; Clear the mode bits
                orr     r0,r0,#FIQ_MODE                     ; Set FIQ mode bits
                msr     cpsr_c,r0                           ; Change the mode
                ldr     sp,=SFE(FIQ_STACK)                  ; End of FIQ_STACK
                ;- Init the FIQ register
                ldr     r8, =AT91C_BASE_AIC

                bic     r0,r0,#MODE_BITS                    ; Clear the mode bits
                orr     r0,r0,#IRQ_MODE                     ; Set IRQ mode bits
                msr     cpsr_c,r0                           ; Change the mode
                ldr     sp,=SFE(IRQ_STACK)                  ; End of IRQ_STACK

                bic     r0,r0,#MODE_BITS                    ; Clear the mode bits
                orr     r0,r0,#SYS_MODE                     ; Set System mode bits
                msr     cpsr_c,r0                           ; Change the mode
                ldr     sp,=SFE(CSTACK)                     ; End of CSTACK

#ifdef __ARMVFP__
; Enable the VFP coprocessor.
                mov     r0, #0x40000000                 ; Set EN bit in VFP
                fmxr    fpexc, r0                       ; FPEXC, clear others.

; Disable underflow exceptions by setting flush to zero mode.
; For full IEEE 754 underflow compliance this code should be removed
; and the appropriate exception handler installed.
                mov     r0, #0x01000000		        ; Set FZ bit in VFP
                fmxr    fpscr, r0                       ; FPSCR, clear others.
#endif

; Add more initialization here


; Continue to ?main for more IAR specific system startup

                ldr     r0,=?main
                bx      r0

    END         ;- Terminates the assembly of the last module in a file
