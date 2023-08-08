/**************************************************************************//**
 * @file     startup_psoc6_02_cm0plus.S
 * @brief    CMSIS Core Device Startup File for
 *           ARMCM0plus Device Series
 * @version  V5.00
 * @date     02. March 2016
 ******************************************************************************/
/*
 * Copyright (c) 2009-2016 ARM Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

    /* Address of the NMI handler */
    #define CY_NMI_HANLDER_ADDR         0x0000000D

    /* The CPU VTOR register */
    #define CY_CPU_VTOR_ADDR            0xE000ED08

    .syntax    unified

    .section __STACK, __stack
    .align    3
#ifdef __STACK_SIZE
    .equ    Stack_Size, __STACK_SIZE
#else
    .equ    Stack_Size, 0x00001000
#endif
    .globl    __StackTop
    .globl    __StackLimit

__StackLimit:
    .space    Stack_Size
    .equ    __StackTop, . - Stack_Size

    .section __HEAP, __heap
    .align    3
#ifdef __HEAP_SIZE
    .equ    Heap_Size, __HEAP_SIZE
#else
    .equ    Heap_Size, 0x00000400
#endif
    .globl    __HeapBase
__HeapBase:
    .if    Heap_Size
    .space    Heap_Size
    .endif

    .section __VECT, ___Vectors
    .align 2
    .globl    ___Vectors
___Vectors:
    .long    __StackTop            /* Top of Stack */
    .long    Reset_Handler         /* Reset Handler */
    .long    CY_NMI_HANLDER_ADDR   /* NMI Handler */
    .long    HardFault_Handler     /* Hard Fault Handler */
    .long    0                     /* Reserved */
    .long    0                     /* Reserved */
    .long    0                     /* Reserved */
    .long    0                     /* Reserved */
    .long    0                     /* Reserved */
    .long    0                     /* Reserved */
    .long    0                     /* Reserved */
    .long    SVC_Handler           /* SVCall Handler */
    .long    0                     /* Reserved */
    .long    0                     /* Reserved */
    .long    PendSV_Handler        /* PendSV Handler */
    .long    SysTick_Handler       /* SysTick Handler */

     /* External interrupts                             Description */
    .long    NvicMux0_IRQHandler                     /* CPU User Interrupt #0 */
    .long    NvicMux1_IRQHandler                     /* CPU User Interrupt #1 */
    .long    NvicMux2_IRQHandler                     /* CPU User Interrupt #2 */
    .long    NvicMux3_IRQHandler                     /* CPU User Interrupt #3 */
    .long    NvicMux4_IRQHandler                     /* CPU User Interrupt #4 */
    .long    NvicMux5_IRQHandler                     /* CPU User Interrupt #5 */
    .long    NvicMux6_IRQHandler                     /* CPU User Interrupt #6 */
    .long    NvicMux7_IRQHandler                     /* CPU User Interrupt #7 */
    .long    Internal0_IRQHandler                    /* Internal SW Interrupt #0 */
    .long    Internal1_IRQHandler                    /* Internal SW Interrupt #1 */
    .long    Internal2_IRQHandler                    /* Internal SW Interrupt #2 */
    .long    Internal3_IRQHandler                    /* Internal SW Interrupt #3 */
    .long    Internal4_IRQHandler                    /* Internal SW Interrupt #4 */
    .long    Internal5_IRQHandler                    /* Internal SW Interrupt #5 */
    .long    Internal6_IRQHandler                    /* Internal SW Interrupt #6 */
    .long    Internal7_IRQHandler                    /* Internal SW Interrupt #7 */

    .equ    __VectorsSize, . - ___Vectors

    .section __RAMVECTORS, ___ramVectors
    .align 2
    .globl ___ramVectors

___ramVectors:
    .space  __VectorsSize


    .text
    .thumb_func
    .align 2
    /* Reset handler */
    .globl Reset_Handler

Reset_Handler:
    bl Cy_OnResetUser
    cpsid i

/*  Single section scheme.
 *
 *  The ranges of copy from/to are specified by following symbols
 *    __etext: LMA of start of the section to copy from. Usually end of text
 *    __data_start__: VMA of start of the section to copy to
 *    __data_end__: VMA of end of the section to copy to
 *
 *  All addresses must be aligned to 4 bytes boundary.
 */
    ldr    r0, =___ramVectors
    ldr    r1, =___Vectors
    ldr    r2, =__VectorsSize
    bl     _memcpy

    ldr    r0, =segment$start$__DATA
    ldr    r1, =segment$end$__TEXT
    ldr    r2, =section$start$__DATA$__zerofill
    sub    r2, r0
    bl     _memcpy

    ldr    r0, =section$start$__DATA$__zerofill
    eor    r1, r1
    ldr    r2, =section$end$__DATA$__zerofill
    sub    r2, r0
    bl     _memset

    /* Update Vector Table Offset Register. */
    ldr r0, =___ramVectors
    ldr r1, =CY_CPU_VTOR_ADDR
    str r0, [r1]
    dsb 0xF

    bl _HeapInit
#ifndef __NO_SYSTEM_INIT
    bl  _SystemInit
#endif

    bl  _main

    /* Should never get here */
    b   .

    .pool

    .text
    .thumb
    .thumb_func
    .align 2

    /* Device startup customization */
    .weak_definition   Cy_OnResetUser
    .global Cy_OnResetUser, Cy_OnResetUser
Cy_OnResetUser:
    bx lr

    .text
    .align    1
    .thumb_func
    .weak_reference    Default_Handler

Default_Handler:
    b    .

    .text
    .thumb_func
    .align  2
    .weak_definition    Cy_SysLib_FaultHandler

Cy_SysLib_FaultHandler:
    b    .

    .text
    .thumb_func
    .align  2

Fault_Handler:
    /* Storing LR content for Creator call stack trace */
    push {LR}
    movs r0, #4
    mov r1, LR
    tst r0, r1
    beq .L_MSP
    mrs r0, PSP
    b .L_API_call
.L_MSP:
    mrs r0, MSP
    /* Compensation of stack pointer address due to pushing 4 bytes of LR */
    adds r0, r0, #4
    nop
.L_API_call:
    bl Cy_SysLib_FaultHandler
    b   .

.macro    def_fault_Handler    fault_handler_name
    .weak_definition    \fault_handler_name
    .set    \fault_handler_name, Fault_Handler
    .endm

/*    Macro to define default handlers. Default handler
 *    will be weak symbol and just dead loops. They can be
 *    overwritten by other handlers */
    .macro    def_irq_handler    handler_name
    .weak_definition    \handler_name
    .set    \handler_name, Default_Handler
    .endm

    def_irq_handler    NMI_Handler

    def_fault_Handler  HardFault_Handler

    def_irq_handler    SVC_Handler
    def_irq_handler    PendSV_Handler
    def_irq_handler    SysTick_Handler

    def_irq_handler  NvicMux0_IRQHandler                     /* CPU User Interrupt #0 */
    def_irq_handler  NvicMux1_IRQHandler                     /* CPU User Interrupt #1 */
    def_irq_handler  NvicMux2_IRQHandler                     /* CPU User Interrupt #2 */
    def_irq_handler  NvicMux3_IRQHandler                     /* CPU User Interrupt #3 */
    def_irq_handler  NvicMux4_IRQHandler                     /* CPU User Interrupt #4 */
    def_irq_handler  NvicMux5_IRQHandler                     /* CPU User Interrupt #5 */
    def_irq_handler  NvicMux6_IRQHandler                     /* CPU User Interrupt #6 */
    def_irq_handler  NvicMux7_IRQHandler                     /* CPU User Interrupt #7 */
    def_irq_handler  Internal0_IRQHandler                    /* Internal SW Interrupt #0 */
    def_irq_handler  Internal1_IRQHandler                    /* Internal SW Interrupt #1 */
    def_irq_handler  Internal2_IRQHandler                    /* Internal SW Interrupt #2 */
    def_irq_handler  Internal3_IRQHandler                    /* Internal SW Interrupt #3 */
    def_irq_handler  Internal4_IRQHandler                    /* Internal SW Interrupt #4 */
    def_irq_handler  Internal5_IRQHandler                    /* Internal SW Interrupt #5 */
    def_irq_handler  Internal6_IRQHandler                    /* Internal SW Interrupt #6 */
    def_irq_handler  Internal7_IRQHandler                    /* Internal SW Interrupt #7 */

    .end


/* [] END OF FILE */
