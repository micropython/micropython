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

.global __Vectors
.global Default_Handler

.word _sidata
.word _sdata
.word _edata
.word _sbss
.word _ebss

/* Reset Handler */

    .equ    NRF_POWER_RAMON_ADDRESS,             0x40000524
    .equ    NRF_POWER_RAMONB_ADDRESS,            0x40000554
    .equ    NRF_POWER_RAMONx_RAMxON_ONMODE_Msk,  0x3  

    .text
    .thumb
    .thumb_func
    .align 1
    .globl Reset_Handler
    .type Reset_Handler, %function
Reset_Handler:
    .fnstart

    movs   R1, #NRF_POWER_RAMONx_RAMxON_ONMODE_Msk
    ldr    R0, =NRF_POWER_RAMON_ADDRESS
    ldr    R2, [R0]
    orrs   R2, R1
    str    R2, [R0]

    ldr    R0, =NRF_POWER_RAMONB_ADDRESS
    ldr    R2, [R0]
    orrs   R2, R1
    str    R2, [R0]

    ldr    r1, =_sidata
    ldr    r2, =_sdata
    ldr    r3, =_edata

    subs   r3, r2
    ble    LC0

LC1:
    subs   r3, 4
    ldr    r0, [r1,r3]
    str    r0, [r2,r3]
    bgt    LC1

LC0:
    bl     SystemInit
    bl     main
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
    .word   UART0_IRQHandler
    .word   SPI0_TWI0_IRQHandler
    .word   SPI1_TWI1_IRQHandler
    .word   0
    .word   GPIOTE_IRQHandler
    .word   ADC_IRQHandler
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
    .word   LPCOMP_IRQHandler
    .word   SWI0_IRQHandler
    .word   SWI1_IRQHandler
    .word   SWI2_IRQHandler
    .word   SWI3_IRQHandler
    .word   SWI4_IRQHandler
    .word   SWI5_IRQHandler

/* Dummy Exception Handlers */

    .weak   NMI_Handler
    .type   NMI_Handler, %function
NMI_Handler:
    b       .
    .size   NMI_Handler, . - NMI_Handler


    .weak   HardFault_Handler
    .type   HardFault_Handler, %function
HardFault_Handler:
    b       .
    .size   HardFault_Handler, . - HardFault_Handler


    .weak   SVC_Handler
    .type   SVC_Handler, %function
SVC_Handler:
    b       .
    .size   SVC_Handler, . - SVC_Handler


    .weak   PendSV_Handler
    .type   PendSV_Handler, %function
PendSV_Handler:
    b       .
    .size   PendSV_Handler, . - PendSV_Handler


    .weak   SysTick_Handler
    .type   SysTick_Handler, %function
SysTick_Handler:
    b       .
    .size   SysTick_Handler, . - SysTick_Handler


/* IRQ Handlers */

    .macro  IRQ handler
    .weak   \handler
    .set    \handler, Default_Handler
    .endm

    IRQ  POWER_CLOCK_IRQHandler
    IRQ  RADIO_IRQHandler
    IRQ  UART0_IRQHandler
    IRQ  SPI0_TWI0_IRQHandler
    IRQ  SPI1_TWI1_IRQHandler
    IRQ  GPIOTE_IRQHandler
    IRQ  ADC_IRQHandler
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
    IRQ  LPCOMP_IRQHandler
    IRQ  SWI0_IRQHandler
    IRQ  SWI1_IRQHandler
    IRQ  SWI2_IRQHandler
    IRQ  SWI3_IRQHandler
    IRQ  SWI4_IRQHandler
    IRQ  SWI5_IRQHandler

    .end
