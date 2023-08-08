;/**************************************************************************//**
; * @file     startup_psoc6_02_cm0plus.s
; * @brief    CMSIS Core Device Startup File for
; *           ARMCM0plus Device Series
; * @version  V5.00
; * @date     08. March 2016
; ******************************************************************************/
;/*
; * Copyright (c) 2009-2016 ARM Limited. All rights reserved.
; *
; * SPDX-License-Identifier: Apache-2.0
; *
; * Licensed under the Apache License, Version 2.0 (the License); you may
; * not use this file except in compliance with the License.
; * You may obtain a copy of the License at
; *
; * www.apache.org/licenses/LICENSE-2.0
; *
; * Unless required by applicable law or agreed to in writing, software
; * distributed under the License is distributed on an AS IS BASIS, WITHOUT
; * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
; * See the License for the specific language governing permissions and
; * limitations under the License.
; */

;
; The modules in this file are included in the libraries, and may be replaced
; by any user-defined modules that define the PUBLIC symbol _program_start or
; a user defined start symbol.
; To override the cstartup defined in the library, simply add your modified
; version to the workbench project.
;
; The vector table is normally located at address 0.
; When debugging in RAM, it can be located in RAM, aligned to at least 2^6.
; The name "__vector_table" has special meaning for C-SPY:
; it is where the SP start value is found, and the NVIC vector
; table register (VTOR) is initialized to this address if != 0.
;
; Cortex-M version
;

        MODULE  ?cstartup

        ;; Forward declaration of sections.
        SECTION CSTACK:DATA:NOROOT(3)
        SECTION .intvec_ram:DATA:NOROOT(2)
        SECTION .intvec:CODE:NOROOT(2)

        EXTERN  __iar_program_start
        EXTERN  SystemInit
        EXTERN __iar_data_init3
        EXTERN __iar_dynamic_initialization
        PUBLIC  __vector_table
        PUBLIC  __vector_table_0x1c
        PUBLIC  __Vectors
        PUBLIC  __Vectors_End
        PUBLIC  __Vectors_Size
        PUBLIC  __ramVectors

        DATA

__vector_table
        DCD     sfe(CSTACK)
        DCD     Reset_Handler

        DCD     0x0000000D      ; NMI_Handler is defined in ROM code
        DCD     HardFault_Handler
        DCD     0
        DCD     0
        DCD     0
__vector_table_0x1c
        DCD     0
        DCD     0
        DCD     0
        DCD     0
        DCD     SVC_Handler
        DCD     0
        DCD     0
        DCD     PendSV_Handler
        DCD     SysTick_Handler

        ; External interrupts                           Description
        DCD     NvicMux0_IRQHandler                   ; CPU User Interrupt #0
        DCD     NvicMux1_IRQHandler                   ; CPU User Interrupt #1
        DCD     NvicMux2_IRQHandler                   ; CPU User Interrupt #2
        DCD     NvicMux3_IRQHandler                   ; CPU User Interrupt #3
        DCD     NvicMux4_IRQHandler                   ; CPU User Interrupt #4
        DCD     NvicMux5_IRQHandler                   ; CPU User Interrupt #5
        DCD     NvicMux6_IRQHandler                   ; CPU User Interrupt #6
        DCD     NvicMux7_IRQHandler                   ; CPU User Interrupt #7
        DCD     Internal0_IRQHandler                  ; Internal SW Interrupt #0
        DCD     Internal1_IRQHandler                  ; Internal SW Interrupt #1
        DCD     Internal2_IRQHandler                  ; Internal SW Interrupt #2
        DCD     Internal3_IRQHandler                  ; Internal SW Interrupt #3
        DCD     Internal4_IRQHandler                  ; Internal SW Interrupt #4
        DCD     Internal5_IRQHandler                  ; Internal SW Interrupt #5
        DCD     Internal6_IRQHandler                  ; Internal SW Interrupt #6
        DCD     Internal7_IRQHandler                  ; Internal SW Interrupt #7

__Vectors_End

__Vectors       EQU   __vector_table
__Vectors_Size  EQU   __Vectors_End - __Vectors

        SECTION .intvec_ram:DATA:REORDER:NOROOT(2)
__ramVectors
        DS8     __Vectors_Size


        THUMB

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Default handlers
;;
        PUBWEAK Default_Handler
        SECTION .text:CODE:REORDER:NOROOT(2)
Default_Handler
        B Default_Handler


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Weak function for startup customization
;;
;; Note. The global resources are not yet initialized (for example global variables, peripherals, clocks)
;; because this function is executed as the first instruction in the ResetHandler.
;; The PDL is also not initialized to use the proper register offsets.
;; The user of this function is responsible for initializing the PDL and resources before using them.
;;
        PUBWEAK Cy_OnResetUser
        SECTION .text:CODE:REORDER:NOROOT(2)
Cy_OnResetUser
        BX LR

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Define strong version to return zero for
;; __iar_program_start to skip data sections
;; initialization.
;;
        PUBLIC __low_level_init
        SECTION .text:CODE:REORDER:NOROOT(2)
__low_level_init
        MOVS R0, #0
        BX LR

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Default interrupt handlers.
;;
        THUMB
        PUBWEAK Reset_Handler
        SECTION .text:CODE:REORDER:NOROOT(2)
Reset_Handler

        ; Define strong function for startup customization
        LDR     R0, =Cy_OnResetUser
        BLX     R0

        ; Disable global interrupts
        CPSID I

        ; Copy vectors from ROM to RAM
        LDR r1, =__vector_table
        LDR r0, =__ramVectors
        LDR r2, =__Vectors_Size
intvec_copy
        LDR r3, [r1]
        STR r3, [r0]
        ADDS r0, r0, #4
        ADDS r1, r1, #4
        SUBS r2, r2, #4
        CMP r2, #0
        BNE intvec_copy

        ; Update Vector Table Offset Register
        LDR r0, =__ramVectors
        LDR r1, =0xE000ED08
        STR r0, [r1]
        dsb

        ; Initialize data sections
        LDR     R0, =__iar_data_init3
        BLX     R0

        ; --manual_dynamic_initialization
        BL      __iar_dynamic_initialization

        LDR     R0, =SystemInit
        BLX     R0

        LDR     R0, =__iar_program_start
        BLX     R0

; Should never get here
Cy_Main_Exited
        B Cy_Main_Exited


        PUBWEAK NMI_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
NMI_Handler
        B NMI_Handler


        PUBWEAK Cy_SysLib_FaultHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
Cy_SysLib_FaultHandler
        B Cy_SysLib_FaultHandler

        PUBWEAK HardFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
HardFault_Handler
        IMPORT Cy_SysLib_FaultHandler
        movs r0, #4
        mov r1, LR
        tst r0, r1
        beq L_MSP
        mrs r0, PSP
        b L_API_call
L_MSP
        mrs r0, MSP
L_API_call
        ; Storing LR content for Creator call stack trace
        push {LR}
        bl Cy_SysLib_FaultHandler


        PUBWEAK SVC_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
SVC_Handler
        B SVC_Handler

        PUBWEAK PendSV_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
PendSV_Handler
        B PendSV_Handler

        PUBWEAK SysTick_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
SysTick_Handler
        B SysTick_Handler


        ; External interrupts
        PUBWEAK NvicMux0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
NvicMux0_IRQHandler
        B       NvicMux0_IRQHandler

        PUBWEAK NvicMux1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
NvicMux1_IRQHandler
        B       NvicMux1_IRQHandler

        PUBWEAK NvicMux2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
NvicMux2_IRQHandler
        B       NvicMux2_IRQHandler

        PUBWEAK NvicMux3_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
NvicMux3_IRQHandler
        B       NvicMux3_IRQHandler

        PUBWEAK NvicMux4_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
NvicMux4_IRQHandler
        B       NvicMux4_IRQHandler

        PUBWEAK NvicMux5_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
NvicMux5_IRQHandler
        B       NvicMux5_IRQHandler

        PUBWEAK NvicMux6_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
NvicMux6_IRQHandler
        B       NvicMux6_IRQHandler

        PUBWEAK NvicMux7_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
NvicMux7_IRQHandler
        B       NvicMux7_IRQHandler

        PUBWEAK Internal0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
Internal0_IRQHandler
        B       Internal0_IRQHandler

        PUBWEAK Internal1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
Internal1_IRQHandler
        B       Internal1_IRQHandler

        PUBWEAK Internal2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
Internal2_IRQHandler
        B       Internal2_IRQHandler

        PUBWEAK Internal3_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
Internal3_IRQHandler
        B       Internal3_IRQHandler

        PUBWEAK Internal4_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
Internal4_IRQHandler
        B       Internal4_IRQHandler

        PUBWEAK Internal5_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
Internal5_IRQHandler
        B       Internal5_IRQHandler

        PUBWEAK Internal6_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
Internal6_IRQHandler
        B       Internal6_IRQHandler

        PUBWEAK Internal7_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
Internal7_IRQHandler
        B       Internal7_IRQHandler


        END


; [] END OF FILE
