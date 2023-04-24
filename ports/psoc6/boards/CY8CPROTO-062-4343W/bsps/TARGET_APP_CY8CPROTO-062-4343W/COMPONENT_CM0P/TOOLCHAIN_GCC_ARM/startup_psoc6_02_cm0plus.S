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

    /* Copy flash vectors and data section to RAM */
    #define __STARTUP_COPY_MULTIPLE

    /* Clear single BSS section */
    #define __STARTUP_CLEAR_BSS

    .syntax    unified
    .arch    armv6-m

    .section .stack
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
    .size    __StackLimit, . - __StackLimit
__StackTop:
    .size    __StackTop, . - __StackTop

    .section .heap
    .align    3
#ifdef __HEAP_SIZE
    .equ    Heap_Size, __HEAP_SIZE
#else
    .equ    Heap_Size, 0x00000400
#endif
    .globl    __HeapBase
    .globl    __HeapLimit
__HeapBase:
    .if    Heap_Size
    .space    Heap_Size
    .endif
    .size    __HeapBase, . - __HeapBase
__HeapLimit:
    .size    __HeapLimit, . - __HeapLimit

    .section .vectors
    .align 2
    .globl    __Vectors
__Vectors:
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

    .size    __Vectors, . - __Vectors
    .equ    __VectorsSize, . - __Vectors

    .section .ram_vectors
    .align 2
    .globl __ramVectors
__ramVectors:
    .space  __VectorsSize
    .size   __ramVectors, . - __ramVectors


    .text
    .thumb
    .thumb_func
    .align  2

    /*
     * Device startup customization
     *
     * Note. The global resources are not yet initialized (for example global variables, peripherals, clocks)
     * because this function is executed as the first instruction in the ResetHandler.
     * The PDL is also not initialized to use the proper register offsets.
     * The user of this function is responsible for initializing the PDL and resources before using them.
     */
    .weak   Cy_OnResetUser
    .func   Cy_OnResetUser, Cy_OnResetUser
    .type   Cy_OnResetUser, %function

Cy_OnResetUser:
    bx lr
    .size   Cy_OnResetUser, . - Cy_OnResetUser
    .endfunc

    /* Reset handler */
    .weak    Reset_Handler
    .type    Reset_Handler, %function

Reset_Handler:
    bl Cy_OnResetUser
    cpsid i

/*  Firstly it copies data from read only memory to RAM. There are two schemes
 *  to copy. One can copy more than one sections. Another can only copy
 *  one section.  The former scheme needs more instructions and read-only
 *  data to implement than the latter.
 *  Macro __STARTUP_COPY_MULTIPLE is used to choose between two schemes.  */

#ifdef __STARTUP_COPY_MULTIPLE
/*  Multiple sections scheme.
 *
 *  Between symbol address __copy_table_start__ and __copy_table_end__,
 *  there are array of triplets, each of which specify:
 *    offset 0: LMA of start of a section to copy from
 *    offset 4: VMA of start of a section to copy to
 *    offset 8: size of the section to copy. Must be multiply of 4
 *
 *  All addresses must be aligned to 4 bytes boundary.
 */
    ldr    r4, =__copy_table_start__
    ldr    r5, =__copy_table_end__

.L_loop0:
    cmp    r4, r5
    bge    .L_loop0_done
    ldr    r1, [r4]
    ldr    r2, [r4, #4]
    ldr    r3, [r4, #8]

.L_loop0_0:
    subs    r3, #4
    blt    .L_loop0_0_done
    ldr    r0, [r1, r3]
    str    r0, [r2, r3]
    b    .L_loop0_0

.L_loop0_0_done:
    adds    r4, #12
    b    .L_loop0

.L_loop0_done:
#else
/*  Single section scheme.
 *
 *  The ranges of copy from/to are specified by following symbols
 *    __etext: LMA of start of the section to copy from. Usually end of text
 *    __data_start__: VMA of start of the section to copy to
 *    __data_end__: VMA of end of the section to copy to
 *
 *  All addresses must be aligned to 4 bytes boundary.
 */
    ldr    r1, =__etext
    ldr    r2, =__data_start__
    ldr    r3, =__data_end__

    subs    r3, r2
    ble    .L_loop1_done

.L_loop1:
    subs    r3, #4
    ldr    r0, [r1,r3]
    str    r0, [r2,r3]
    bgt    .L_loop1

.L_loop1_done:
#endif /*__STARTUP_COPY_MULTIPLE */

/*  This part of work usually is done in C library startup code. Otherwise,
 *  define this macro to enable it in this startup.
 *
 *  There are two schemes too. One can clear multiple BSS sections. Another
 *  can only clear one section. The former is more size expensive than the
 *  latter.
 *
 *  Define macro __STARTUP_CLEAR_BSS_MULTIPLE to choose the former.
 *  Otherwise define macro __STARTUP_CLEAR_BSS to choose the later.
 */
#ifdef __STARTUP_CLEAR_BSS_MULTIPLE
/*  Multiple sections scheme.
 *
 *  Between symbol address __copy_table_start__ and __copy_table_end__,
 *  there are array of tuples specifying:
 *    offset 0: Start of a BSS section
 *    offset 4: Size of this BSS section. Must be multiply of 4
 */
    ldr    r3, =__zero_table_start__
    ldr    r4, =__zero_table_end__

.L_loop2:
    cmp    r3, r4
    bge    .L_loop2_done
    ldr    r1, [r3]
    ldr    r2, [r3, #4]
    movs    r0, 0

.L_loop2_0:
    subs    r2, #4
    blt    .L_loop2_0_done
    str    r0, [r1, r2]
    b    .L_loop2_0
.L_loop2_0_done:

    adds    r3, #8
    b    .L_loop2
.L_loop2_done:
#elif defined (__STARTUP_CLEAR_BSS)
/*  Single BSS section scheme.
 *
 *  The BSS section is specified by following symbols
 *    __bss_start__: start of the BSS section.
 *    __bss_end__: end of the BSS section.
 *
 *  Both addresses must be aligned to 4 bytes boundary.
 */
    ldr    r1, =__bss_start__
    ldr    r2, =__bss_end__

    movs    r0, 0

    subs    r2, r1
    ble    .L_loop3_done

.L_loop3:
    subs    r2, #4
    str    r0, [r1, r2]
    bgt    .L_loop3
.L_loop3_done:
#endif /* __STARTUP_CLEAR_BSS_MULTIPLE || __STARTUP_CLEAR_BSS */

    /* Update Vector Table Offset Register. */
    ldr r0, =__ramVectors
    ldr r1, =CY_CPU_VTOR_ADDR
    str r0, [r1]
    dsb 0xF

#ifndef __NO_SYSTEM_INIT
    bl    SystemInit
#endif

    bl    main

    /* Should never get here */
    b   .

    .pool
    .size    Reset_Handler, . - Reset_Handler

    .align    1
    .thumb_func
    .weak    Default_Handler
    .type    Default_Handler, %function
Default_Handler:
    b    .
    .size    Default_Handler, . - Default_Handler
    .weak    Cy_SysLib_FaultHandler
    .type    Cy_SysLib_FaultHandler, %function

Cy_SysLib_FaultHandler:
    b    .
    .size    Cy_SysLib_FaultHandler, . - Cy_SysLib_FaultHandler
    .type Fault_Handler, %function

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
.L_API_call:
    bl Cy_SysLib_FaultHandler
    b   .
    .size    Fault_Handler, . - Fault_Handler

.macro    def_fault_Handler    fault_handler_name
    .weak    \fault_handler_name
    .set    \fault_handler_name, Fault_Handler
    .endm

/*    Macro to define default handlers. Default handler
 *    will be weak symbol and just dead loops. They can be
 *    overwritten by other handlers */
    .macro    def_irq_handler    handler_name
    .weak    \handler_name
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
