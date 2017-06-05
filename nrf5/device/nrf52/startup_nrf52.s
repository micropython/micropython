/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Glenn Ruben Bakke
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

.syntax unified
.arch armv6-m

.section .stack
.align 3

.global __Vectors
.global Default_Handler

.word _sidata
.word _sdata
.word _edata
.word _sbss
.word _ebss

/* Reset Handler */

    .text
    .thumb
    .thumb_func
    .align 1
    .globl Reset_Handler
    .type Reset_Handler, %function
Reset_Handler:
    .fnstart

    ldr    r1, =_sidata
    ldr    r2, =_sdata
    ldr    r3, =_sbss

    subs   r3, r2
    ble    CopyDone
    b      CopyData

CopyData:
    subs   r3, #4
    ldr    r0, [r1,r3]
    str    r0, [r2,r3]
    bgt    CopyData

CopyDone:
    ldr r1, =_sbss
    ldr r2, =_ebss

    movs r0, 0

    subs r2, r1
    ble ZeroDone

ZeroLoop:
    subs r2, #4
    str r0, [r1, r2]
    bgt ZeroLoop

ZeroDone:

    bl     SystemInit
    bl     _start
    bx     lr

    .pool
    .cantunwind
    .fnend
    .size   Reset_Handler,.-Reset_Handler

/* Default Handler */

    .section ".text"
    .section  .text.Default_Handler,"ax",%progbits
Default_Handler:
    b  .
    .size  Default_Handler, .-Default_Handler

/* Vector Table */

    .section  .isr_vector,"a",%progbits
    .type  __Vectors, %object
    .size  __Vectors, .-__Vectors

__Vectors:
    .word   _estack
    .word   Reset_Handler
    .word   NMI_Handler
    .word   HardFault_Handler
    .word   0
    .word   0
    .word   0
    .word   0
    .word   0
    .word   0
    .word   0
    .word   SVC_Handler
    .word   0
    .word   0
    .word   PendSV_Handler
    .word   SysTick_Handler

    /* External Interrupts */
    .word   POWER_CLOCK_IRQHandler
    .word   RADIO_IRQHandler
    .word   UARTE0_UART0_IRQHandler
    .word   SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQHandler
    .word   SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQHandler
    .word   NFCT_IRQHandler
    .word   GPIOTE_IRQHandler
    .word   SAADC_IRQHandler
    .word   TIMER0_IRQHandler
    .word   TIMER1_IRQHandler
    .word   TIMER2_IRQHandler
    .word   RTC0_IRQHandler
    .word   TEMP_IRQHandler
    .word   RNG_IRQHandler
    .word   ECB_IRQHandler
    .word   CCM_AAR_IRQHandler
    .word   WDT_IRQHandler
    .word   RTC1_IRQHandler
    .word   QDEC_IRQHandler
    .word   COMP_LPCOMP_IRQHandler
    .word   SWI0_EGU0_IRQHandler
    .word   SWI1_EGU1_IRQHandler
    .word   SWI2_EGU2_IRQHandler
    .word   SWI3_EGU3_IRQHandler
    .word   SWI4_EGU4_IRQHandler
    .word   SWI5_EGU5_IRQHandler
    .word   TIMER3_IRQHandler
    .word   TIMER4_IRQHandler
    .word   PWM0_IRQHandler
    .word   PDM_IRQHandler
    .word   0
    .word   0
    .word   MWU_IRQHandler
    .word   PWM1_IRQHandler
    .word   PWM2_IRQHandler
    .word   SPIM2_SPIS2_SPI2_IRQHandler
    .word   RTC2_IRQHandler
    .word   I2S_IRQHandler

/* Dummy Exception Handlers */

    .weak   NMI_Handler
    .type   NMI_Handler, %function
NMI_Handler:
    B       .
    .size   NMI_Handler, . - NMI_Handler


    .weak   HardFault_Handler
    .type   HardFault_Handler, %function
HardFault_Handler:
    B       .
    .size   HardFault_Handler, . - HardFault_Handler


    .weak   MemoryManagement_Handler
    .type   MemoryManagement_Handler, %function
MemoryManagement_Handler:
    B       .
    .size   MemoryManagement_Handler, . - MemoryManagement_Handler


    .weak   BusFault_Handler
    .type   BusFault_Handler, %function
BusFault_Handler:
    B       .
    .size   BusFault_Handler, . - BusFault_Handler


    .weak   UsageFault_Handler
    .type   UsageFault_Handler, %function
UsageFault_Handler:
    B       .
    .size   UsageFault_Handler, . - UsageFault_Handler


    .weak   SVC_Handler
    .type   SVC_Handler, %function
SVC_Handler:
    B       .
    .size   SVC_Handler, . - SVC_Handler


    .weak   PendSV_Handler
    .type   PendSV_Handler, %function
PendSV_Handler:
    B       .
    .size   PendSV_Handler, . - PendSV_Handler


    .weak   SysTick_Handler
    .type   SysTick_Handler, %function
SysTick_Handler:
    B       .
    .size   SysTick_Handler, . - SysTick_Handler


/* IRQ Handlers */

    .macro  IRQ handler
    .weak   \handler
    .set    \handler, Default_Handler
    .endm

    IRQ  POWER_CLOCK_IRQHandler
    IRQ  RADIO_IRQHandler
    IRQ  UARTE0_UART0_IRQHandler
    IRQ  SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQHandler
    IRQ  SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQHandler
    IRQ  NFCT_IRQHandler
    IRQ  GPIOTE_IRQHandler
    IRQ  SAADC_IRQHandler
    IRQ  TIMER0_IRQHandler
    IRQ  TIMER1_IRQHandler
    IRQ  TIMER2_IRQHandler
    IRQ  RTC0_IRQHandler
    IRQ  TEMP_IRQHandler
    IRQ  RNG_IRQHandler
    IRQ  ECB_IRQHandler
    IRQ  CCM_AAR_IRQHandler
    IRQ  WDT_IRQHandler
    IRQ  RTC1_IRQHandler
    IRQ  QDEC_IRQHandler
    IRQ  COMP_LPCOMP_IRQHandler
    IRQ  SWI0_EGU0_IRQHandler
    IRQ  SWI1_EGU1_IRQHandler
    IRQ  SWI2_EGU2_IRQHandler
    IRQ  SWI3_EGU3_IRQHandler
    IRQ  SWI4_EGU4_IRQHandler
    IRQ  SWI5_EGU5_IRQHandler
    IRQ  TIMER3_IRQHandler
    IRQ  TIMER4_IRQHandler
    IRQ  PWM0_IRQHandler
    IRQ  PDM_IRQHandler
    IRQ  MWU_IRQHandler
    IRQ  PWM1_IRQHandler
    IRQ  PWM2_IRQHandler
    IRQ  SPIM2_SPIS2_SPI2_IRQHandler
    IRQ  RTC2_IRQHandler
    IRQ  I2S_IRQHandler

    .end
