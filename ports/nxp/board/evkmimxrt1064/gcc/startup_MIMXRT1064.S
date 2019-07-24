/* ------------------------------------------------------------------------- */
/*  @file:    startup_MIMXRT1064.s                                           */
/*  @purpose: CMSIS Cortex-M7 Core Device Startup File                       */
/*            MIMXRT1064                                                     */
/*  @version: 1.1                                                            */
/*  @date:    2018-11-27                                                     */
/*  @build:   b190124                                                        */
/* ------------------------------------------------------------------------- */
/*                                                                           */
/* Copyright 1997-2016 Freescale Semiconductor, Inc.                         */
/* Copyright 2016-2019 NXP                                                   */
/* All rights reserved.                                                      */
/*                                                                           */
/* SPDX-License-Identifier: BSD-3-Clause                                     */
/*****************************************************************************/
/* Version: GCC for ARM Embedded Processors                                  */
/*****************************************************************************/
    .syntax unified
    .arch armv7-m

    .section .isr_vector, "a"
    .align 2
    .globl __isr_vector
__isr_vector:
    .long   __StackTop                                      /* Top of Stack */
    .long   Reset_Handler                                   /* Reset Handler */
    .long   NMI_Handler                                     /* NMI Handler*/
    .long   HardFault_Handler                               /* Hard Fault Handler*/
    .long   MemManage_Handler                               /* MPU Fault Handler*/
    .long   BusFault_Handler                                /* Bus Fault Handler*/
    .long   UsageFault_Handler                              /* Usage Fault Handler*/
    .long   0                                               /* Reserved*/
    .long   0                                               /* Reserved*/
    .long   0                                               /* Reserved*/
    .long   0                                               /* Reserved*/
    .long   SVC_Handler                                     /* SVCall Handler*/
    .long   DebugMon_Handler                                /* Debug Monitor Handler*/
    .long   0                                               /* Reserved*/
    .long   PendSV_Handler                                  /* PendSV Handler*/
    .long   SysTick_Handler                                 /* SysTick Handler*/

                                                            /* External Interrupts*/
    .long   DMA0_DMA16_IRQHandler                           /* DMA channel 0/16 transfer complete*/
    .long   DMA1_DMA17_IRQHandler                           /* DMA channel 1/17 transfer complete*/
    .long   DMA2_DMA18_IRQHandler                           /* DMA channel 2/18 transfer complete*/
    .long   DMA3_DMA19_IRQHandler                           /* DMA channel 3/19 transfer complete*/
    .long   DMA4_DMA20_IRQHandler                           /* DMA channel 4/20 transfer complete*/
    .long   DMA5_DMA21_IRQHandler                           /* DMA channel 5/21 transfer complete*/
    .long   DMA6_DMA22_IRQHandler                           /* DMA channel 6/22 transfer complete*/
    .long   DMA7_DMA23_IRQHandler                           /* DMA channel 7/23 transfer complete*/
    .long   DMA8_DMA24_IRQHandler                           /* DMA channel 8/24 transfer complete*/
    .long   DMA9_DMA25_IRQHandler                           /* DMA channel 9/25 transfer complete*/
    .long   DMA10_DMA26_IRQHandler                          /* DMA channel 10/26 transfer complete*/
    .long   DMA11_DMA27_IRQHandler                          /* DMA channel 11/27 transfer complete*/
    .long   DMA12_DMA28_IRQHandler                          /* DMA channel 12/28 transfer complete*/
    .long   DMA13_DMA29_IRQHandler                          /* DMA channel 13/29 transfer complete*/
    .long   DMA14_DMA30_IRQHandler                          /* DMA channel 14/30 transfer complete*/
    .long   DMA15_DMA31_IRQHandler                          /* DMA channel 15/31 transfer complete*/
    .long   DMA_ERROR_IRQHandler                            /* DMA error interrupt channels 0-15 / 16-31*/
    .long   CTI0_ERROR_IRQHandler                           /* CTI0_Error*/
    .long   CTI1_ERROR_IRQHandler                           /* CTI1_Error*/
    .long   CORE_IRQHandler                                 /* CorePlatform exception IRQ*/
    .long   LPUART1_IRQHandler                              /* LPUART1 TX interrupt and RX interrupt*/
    .long   LPUART2_IRQHandler                              /* LPUART2 TX interrupt and RX interrupt*/
    .long   LPUART3_IRQHandler                              /* LPUART3 TX interrupt and RX interrupt*/
    .long   LPUART4_IRQHandler                              /* LPUART4 TX interrupt and RX interrupt*/
    .long   LPUART5_IRQHandler                              /* LPUART5 TX interrupt and RX interrupt*/
    .long   LPUART6_IRQHandler                              /* LPUART6 TX interrupt and RX interrupt*/
    .long   LPUART7_IRQHandler                              /* LPUART7 TX interrupt and RX interrupt*/
    .long   LPUART8_IRQHandler                              /* LPUART8 TX interrupt and RX interrupt*/
    .long   LPI2C1_IRQHandler                               /* LPI2C1 interrupt*/
    .long   LPI2C2_IRQHandler                               /* LPI2C2 interrupt*/
    .long   LPI2C3_IRQHandler                               /* LPI2C3 interrupt*/
    .long   LPI2C4_IRQHandler                               /* LPI2C4 interrupt*/
    .long   LPSPI1_IRQHandler                               /* LPSPI1 single interrupt vector for all sources*/
    .long   LPSPI2_IRQHandler                               /* LPSPI2 single interrupt vector for all sources*/
    .long   LPSPI3_IRQHandler                               /* LPSPI3 single interrupt vector for all sources*/
    .long   LPSPI4_IRQHandler                               /* LPSPI4  single interrupt vector for all sources*/
    .long   CAN1_IRQHandler                                 /* CAN1 interrupt*/
    .long   CAN2_IRQHandler                                 /* CAN2 interrupt*/
    .long   FLEXRAM_IRQHandler                              /* FlexRAM address out of range Or access hit IRQ*/
    .long   KPP_IRQHandler                                  /* Keypad nterrupt*/
    .long   TSC_DIG_IRQHandler                              /* TSC interrupt*/
    .long   GPR_IRQ_IRQHandler                              /* GPR interrupt*/
    .long   LCDIF_IRQHandler                                /* LCDIF interrupt*/
    .long   CSI_IRQHandler                                  /* CSI interrupt*/
    .long   PXP_IRQHandler                                  /* PXP interrupt*/
    .long   WDOG2_IRQHandler                                /* WDOG2 interrupt*/
    .long   SNVS_HP_WRAPPER_IRQHandler                      /* SRTC Consolidated Interrupt. Non TZ*/
    .long   SNVS_HP_WRAPPER_TZ_IRQHandler                   /* SRTC Security Interrupt. TZ*/
    .long   SNVS_LP_WRAPPER_IRQHandler                      /* ON-OFF button press shorter than 5 secs (pulse event)*/
    .long   CSU_IRQHandler                                  /* CSU interrupt*/
    .long   DCP_IRQHandler                                  /* DCP_IRQ interrupt*/
    .long   DCP_VMI_IRQHandler                              /* DCP_VMI_IRQ interrupt*/
    .long   Reserved68_IRQHandler                           /* Reserved interrupt*/
    .long   TRNG_IRQHandler                                 /* TRNG interrupt*/
    .long   SJC_IRQHandler                                  /* SJC interrupt*/
    .long   BEE_IRQHandler                                  /* BEE interrupt*/
    .long   SAI1_IRQHandler                                 /* SAI1 interrupt*/
    .long   SAI2_IRQHandler                                 /* SAI1 interrupt*/
    .long   SAI3_RX_IRQHandler                              /* SAI3 interrupt*/
    .long   SAI3_TX_IRQHandler                              /* SAI3 interrupt*/
    .long   SPDIF_IRQHandler                                /* SPDIF interrupt*/
    .long   PMU_EVENT_IRQHandler                            /* Brown-out event interrupt*/
    .long   Reserved78_IRQHandler                           /* Reserved interrupt*/
    .long   TEMP_LOW_HIGH_IRQHandler                        /* TempSensor low/high interrupt*/
    .long   TEMP_PANIC_IRQHandler                           /* TempSensor panic interrupt*/
    .long   USB_PHY1_IRQHandler                             /* USBPHY (UTMI0), Interrupt*/
    .long   USB_PHY2_IRQHandler                             /* USBPHY (UTMI1), Interrupt*/
    .long   ADC1_IRQHandler                                 /* ADC1 interrupt*/
    .long   ADC2_IRQHandler                                 /* ADC2 interrupt*/
    .long   DCDC_IRQHandler                                 /* DCDC interrupt*/
    .long   Reserved86_IRQHandler                           /* Reserved interrupt*/
    .long   Reserved87_IRQHandler                           /* Reserved interrupt*/
    .long   GPIO1_INT0_IRQHandler                           /* Active HIGH Interrupt from INT0 from GPIO*/
    .long   GPIO1_INT1_IRQHandler                           /* Active HIGH Interrupt from INT1 from GPIO*/
    .long   GPIO1_INT2_IRQHandler                           /* Active HIGH Interrupt from INT2 from GPIO*/
    .long   GPIO1_INT3_IRQHandler                           /* Active HIGH Interrupt from INT3 from GPIO*/
    .long   GPIO1_INT4_IRQHandler                           /* Active HIGH Interrupt from INT4 from GPIO*/
    .long   GPIO1_INT5_IRQHandler                           /* Active HIGH Interrupt from INT5 from GPIO*/
    .long   GPIO1_INT6_IRQHandler                           /* Active HIGH Interrupt from INT6 from GPIO*/
    .long   GPIO1_INT7_IRQHandler                           /* Active HIGH Interrupt from INT7 from GPIO*/
    .long   GPIO1_Combined_0_15_IRQHandler                  /* Combined interrupt indication for GPIO1 signal 0 throughout 15*/
    .long   GPIO1_Combined_16_31_IRQHandler                 /* Combined interrupt indication for GPIO1 signal 16 throughout 31*/
    .long   GPIO2_Combined_0_15_IRQHandler                  /* Combined interrupt indication for GPIO2 signal 0 throughout 15*/
    .long   GPIO2_Combined_16_31_IRQHandler                 /* Combined interrupt indication for GPIO2 signal 16 throughout 31*/
    .long   GPIO3_Combined_0_15_IRQHandler                  /* Combined interrupt indication for GPIO3 signal 0 throughout 15*/
    .long   GPIO3_Combined_16_31_IRQHandler                 /* Combined interrupt indication for GPIO3 signal 16 throughout 31*/
    .long   GPIO4_Combined_0_15_IRQHandler                  /* Combined interrupt indication for GPIO4 signal 0 throughout 15*/
    .long   GPIO4_Combined_16_31_IRQHandler                 /* Combined interrupt indication for GPIO4 signal 16 throughout 31*/
    .long   GPIO5_Combined_0_15_IRQHandler                  /* Combined interrupt indication for GPIO5 signal 0 throughout 15*/
    .long   GPIO5_Combined_16_31_IRQHandler                 /* Combined interrupt indication for GPIO5 signal 16 throughout 31*/
    .long   FLEXIO1_IRQHandler                              /* FLEXIO1 interrupt*/
    .long   FLEXIO2_IRQHandler                              /* FLEXIO2 interrupt*/
    .long   WDOG1_IRQHandler                                /* WDOG1 interrupt*/
    .long   RTWDOG_IRQHandler                               /* RTWDOG interrupt*/
    .long   EWM_IRQHandler                                  /* EWM interrupt*/
    .long   CCM_1_IRQHandler                                /* CCM IRQ1 interrupt*/
    .long   CCM_2_IRQHandler                                /* CCM IRQ2 interrupt*/
    .long   GPC_IRQHandler                                  /* GPC interrupt*/
    .long   SRC_IRQHandler                                  /* SRC interrupt*/
    .long   Reserved115_IRQHandler                          /* Reserved interrupt*/
    .long   GPT1_IRQHandler                                 /* GPT1 interrupt*/
    .long   GPT2_IRQHandler                                 /* GPT2 interrupt*/
    .long   PWM1_0_IRQHandler                               /* PWM1 capture 0, compare 0, or reload 0 interrupt*/
    .long   PWM1_1_IRQHandler                               /* PWM1 capture 1, compare 1, or reload 0 interrupt*/
    .long   PWM1_2_IRQHandler                               /* PWM1 capture 2, compare 2, or reload 0 interrupt*/
    .long   PWM1_3_IRQHandler                               /* PWM1 capture 3, compare 3, or reload 0 interrupt*/
    .long   PWM1_FAULT_IRQHandler                           /* PWM1 fault or reload error interrupt*/
    .long   FLEXSPI2_IRQHandler                             /* FlexSPI2 interrupt*/
    .long   FLEXSPI_IRQHandler                              /* FlexSPI0 interrupt*/
    .long   SEMC_IRQHandler                                 /* Reserved interrupt*/
    .long   USDHC1_IRQHandler                               /* USDHC1 interrupt*/
    .long   USDHC2_IRQHandler                               /* USDHC2 interrupt*/
    .long   USB_OTG2_IRQHandler                             /* USBO2 USB OTG2*/
    .long   USB_OTG1_IRQHandler                             /* USBO2 USB OTG1*/
    .long   ENET_IRQHandler                                 /* ENET interrupt*/
    .long   ENET_1588_Timer_IRQHandler                      /* ENET_1588_Timer interrupt*/
    .long   XBAR1_IRQ_0_1_IRQHandler                        /* XBAR1 interrupt*/
    .long   XBAR1_IRQ_2_3_IRQHandler                        /* XBAR1 interrupt*/
    .long   ADC_ETC_IRQ0_IRQHandler                         /* ADCETC IRQ0 interrupt*/
    .long   ADC_ETC_IRQ1_IRQHandler                         /* ADCETC IRQ1 interrupt*/
    .long   ADC_ETC_IRQ2_IRQHandler                         /* ADCETC IRQ2 interrupt*/
    .long   ADC_ETC_ERROR_IRQ_IRQHandler                    /* ADCETC Error IRQ interrupt*/
    .long   PIT_IRQHandler                                  /* PIT interrupt*/
    .long   ACMP1_IRQHandler                                /* ACMP interrupt*/
    .long   ACMP2_IRQHandler                                /* ACMP interrupt*/
    .long   ACMP3_IRQHandler                                /* ACMP interrupt*/
    .long   ACMP4_IRQHandler                                /* ACMP interrupt*/
    .long   Reserved143_IRQHandler                          /* Reserved interrupt*/
    .long   Reserved144_IRQHandler                          /* Reserved interrupt*/
    .long   ENC1_IRQHandler                                 /* ENC1 interrupt*/
    .long   ENC2_IRQHandler                                 /* ENC2 interrupt*/
    .long   ENC3_IRQHandler                                 /* ENC3 interrupt*/
    .long   ENC4_IRQHandler                                 /* ENC4 interrupt*/
    .long   TMR1_IRQHandler                                 /* TMR1 interrupt*/
    .long   TMR2_IRQHandler                                 /* TMR2 interrupt*/
    .long   TMR3_IRQHandler                                 /* TMR3 interrupt*/
    .long   TMR4_IRQHandler                                 /* TMR4 interrupt*/
    .long   PWM2_0_IRQHandler                               /* PWM2 capture 0, compare 0, or reload 0 interrupt*/
    .long   PWM2_1_IRQHandler                               /* PWM2 capture 1, compare 1, or reload 0 interrupt*/
    .long   PWM2_2_IRQHandler                               /* PWM2 capture 2, compare 2, or reload 0 interrupt*/
    .long   PWM2_3_IRQHandler                               /* PWM2 capture 3, compare 3, or reload 0 interrupt*/
    .long   PWM2_FAULT_IRQHandler                           /* PWM2 fault or reload error interrupt*/
    .long   PWM3_0_IRQHandler                               /* PWM3 capture 0, compare 0, or reload 0 interrupt*/
    .long   PWM3_1_IRQHandler                               /* PWM3 capture 1, compare 1, or reload 0 interrupt*/
    .long   PWM3_2_IRQHandler                               /* PWM3 capture 2, compare 2, or reload 0 interrupt*/
    .long   PWM3_3_IRQHandler                               /* PWM3 capture 3, compare 3, or reload 0 interrupt*/
    .long   PWM3_FAULT_IRQHandler                           /* PWM3 fault or reload error interrupt*/
    .long   PWM4_0_IRQHandler                               /* PWM4 capture 0, compare 0, or reload 0 interrupt*/
    .long   PWM4_1_IRQHandler                               /* PWM4 capture 1, compare 1, or reload 0 interrupt*/
    .long   PWM4_2_IRQHandler                               /* PWM4 capture 2, compare 2, or reload 0 interrupt*/
    .long   PWM4_3_IRQHandler                               /* PWM4 capture 3, compare 3, or reload 0 interrupt*/
    .long   PWM4_FAULT_IRQHandler                           /* PWM4 fault or reload error interrupt*/
    .long   ENET2_IRQHandler                                /* ENET2 interrupt*/
    .long   ENET2_1588_Timer_IRQHandler                     /* ENET2_1588_Timer interrupt*/
    .long   CAN3_IRQHandler                                 /* CAN3 interrupt*/
    .long   Reserved171_IRQHandler                          /* Reserved interrupt*/
    .long   FLEXIO3_IRQHandler                              /* FLEXIO3 interrupt*/
    .long   GPIO6_7_8_9_IRQHandler                          /* GPIO6, GPIO7, GPIO8, GPIO9 interrupt*/
    .long   DefaultISR                                      /* 174*/
    .long   DefaultISR                                      /* 175*/
    .long   DefaultISR                                      /* 176*/
    .long   DefaultISR                                      /* 177*/
    .long   DefaultISR                                      /* 178*/
    .long   DefaultISR                                      /* 179*/
    .long   DefaultISR                                      /* 180*/
    .long   DefaultISR                                      /* 181*/
    .long   DefaultISR                                      /* 182*/
    .long   DefaultISR                                      /* 183*/
    .long   DefaultISR                                      /* 184*/
    .long   DefaultISR                                      /* 185*/
    .long   DefaultISR                                      /* 186*/
    .long   DefaultISR                                      /* 187*/
    .long   DefaultISR                                      /* 188*/
    .long   DefaultISR                                      /* 189*/
    .long   DefaultISR                                      /* 190*/
    .long   DefaultISR                                      /* 191*/
    .long   DefaultISR                                      /* 192*/
    .long   DefaultISR                                      /* 193*/
    .long   DefaultISR                                      /* 194*/
    .long   DefaultISR                                      /* 195*/
    .long   DefaultISR                                      /* 196*/
    .long   DefaultISR                                      /* 197*/
    .long   DefaultISR                                      /* 198*/
    .long   DefaultISR                                      /* 199*/
    .long   DefaultISR                                      /* 200*/
    .long   DefaultISR                                      /* 201*/
    .long   DefaultISR                                      /* 202*/
    .long   DefaultISR                                      /* 203*/
    .long   DefaultISR                                      /* 204*/
    .long   DefaultISR                                      /* 205*/
    .long   DefaultISR                                      /* 206*/
    .long   DefaultISR                                      /* 207*/
    .long   DefaultISR                                      /* 208*/
    .long   DefaultISR                                      /* 209*/
    .long   DefaultISR                                      /* 210*/
    .long   DefaultISR                                      /* 211*/
    .long   DefaultISR                                      /* 212*/
    .long   DefaultISR                                      /* 213*/
    .long   DefaultISR                                      /* 214*/
    .long   DefaultISR                                      /* 215*/
    .long   DefaultISR                                      /* 216*/
    .long   DefaultISR                                      /* 217*/
    .long   DefaultISR                                      /* 218*/
    .long   DefaultISR                                      /* 219*/
    .long   DefaultISR                                      /* 220*/
    .long   DefaultISR                                      /* 221*/
    .long   DefaultISR                                      /* 222*/
    .long   DefaultISR                                      /* 223*/
    .long   DefaultISR                                      /* 224*/
    .long   DefaultISR                                      /* 225*/
    .long   DefaultISR                                      /* 226*/
    .long   DefaultISR                                      /* 227*/
    .long   DefaultISR                                      /* 228*/
    .long   DefaultISR                                      /* 229*/
    .long   DefaultISR                                      /* 230*/
    .long   DefaultISR                                      /* 231*/
    .long   DefaultISR                                      /* 232*/
    .long   DefaultISR                                      /* 233*/
    .long   DefaultISR                                      /* 234*/
    .long   DefaultISR                                      /* 235*/
    .long   DefaultISR                                      /* 236*/
    .long   DefaultISR                                      /* 237*/
    .long   DefaultISR                                      /* 238*/
    .long   DefaultISR                                      /* 239*/
    .long   DefaultISR                                      /* 240*/
    .long   DefaultISR                                      /* 241*/
    .long   DefaultISR                                      /* 242*/
    .long   DefaultISR                                      /* 243*/
    .long   DefaultISR                                      /* 244*/
    .long   DefaultISR                                      /* 245*/
    .long   DefaultISR                                      /* 246*/
    .long   DefaultISR                                      /* 247*/
    .long   DefaultISR                                      /* 248*/
    .long   DefaultISR                                      /* 249*/
    .long   DefaultISR                                      /* 250*/
    .long   DefaultISR                                      /* 251*/
    .long   DefaultISR                                      /* 252*/
    .long   DefaultISR                                      /* 253*/
    .long   DefaultISR                                      /* 254*/
    .long   0xFFFFFFFF                                      /*  Reserved for user TRIM value*/

    .size    __isr_vector, . - __isr_vector

    .text
    .thumb

/* Reset Handler */

    .thumb_func
    .align 2
    .globl   Reset_Handler
    .weak    Reset_Handler
    .type    Reset_Handler, %function
Reset_Handler:
    cpsid   i               /* Mask interrupts */
    .equ    VTOR, 0xE000ED08
    ldr     r0, =VTOR
    ldr     r1, =__isr_vector
    str     r1, [r0]
    ldr     r2, [r1]
    msr     msp, r2
#ifndef __NO_SYSTEM_INIT
    ldr   r0,=SystemInit
    blx   r0
#endif
/*     Loop to copy data from read only memory to RAM. The ranges
 *      of copy from/to are specified by following symbols evaluated in
 *      linker script.
 *      __etext: End of code section, i.e., begin of data sections to copy from.
 *      __data_start__/__data_end__: RAM address range that data should be
 *      __noncachedata_start__/__noncachedata_end__ : none cachable region
 *      __ram_function_start__/__ram_function_end__ : ramfunction region
 *      copied to. Both must be aligned to 4 bytes boundary.  */

    ldr    r1, =__etext
    ldr    r2, =__data_start__
    ldr    r3, =__data_end__

#ifdef __PERFORMANCE_IMPLEMENTATION
/* Here are two copies of loop implementations. First one favors performance
 * and the second one favors code size. Default uses the second one.
 * Define macro "__PERFORMANCE_IMPLEMENTATION" in project to use the first one */
    subs    r3, r2
    ble    .LC1
.LC0:
    subs    r3, #4
    ldr    r0, [r1, r3]
    str    r0, [r2, r3]
    bgt    .LC0
.LC1:
#else  /* code size implemenation */
.LC0:
    cmp     r2, r3
    ittt    lt
    ldrlt   r0, [r1], #4
    strlt   r0, [r2], #4
    blt    .LC0
#endif
#ifdef __STARTUP_INITIALIZE_RAMFUNCTION
    ldr    r2, =__ram_function_start__
    ldr    r3, =__ram_function_end__
#ifdef __PERFORMANCE_IMPLEMENTATION
/* Here are two copies of loop implementations. First one favors performance
 * and the second one favors code size. Default uses the second one.
 * Define macro "__PERFORMANCE_IMPLEMENTATION" in project to use the first one */
    subs    r3, r2
    ble    .LC_ramfunc_copy_end
.LC_ramfunc_copy_start:
    subs    r3, #4
    ldr    r0, [r1, r3]
    str    r0, [r2, r3]
    bgt    .LC_ramfunc_copy_start
.LC_ramfunc_copy_end:
#else  /* code size implemenation */
.LC_ramfunc_copy_start:
    cmp     r2, r3
    ittt    lt
    ldrlt   r0, [r1], #4
    strlt   r0, [r2], #4
    blt    .LC_ramfunc_copy_start
#endif
#endif /* __STARTUP_INITIALIZE_RAMFUNCTION */
#ifdef __STARTUP_INITIALIZE_NONCACHEDATA
    ldr    r2, =__noncachedata_start__
    ldr    r3, =__noncachedata_init_end__
#ifdef __PERFORMANCE_IMPLEMENTATION
/* Here are two copies of loop implementations. First one favors performance
 * and the second one favors code size. Default uses the second one.
 * Define macro "__PERFORMANCE_IMPLEMENTATION" in project to use the first one */
    subs    r3, r2
    ble    .LC3
.LC2:
    subs    r3, #4
    ldr    r0, [r1, r3]
    str    r0, [r2, r3]
    bgt    .LC2
.LC3:
#else  /* code size implemenation */
.LC2:
    cmp     r2, r3
    ittt    lt
    ldrlt   r0, [r1], #4
    strlt   r0, [r2], #4
    blt    .LC2
#endif
/* zero inited ncache section initialization */
    ldr r3, =__noncachedata_end__
    movs    r0,0
.LC4:
    cmp    r2,r3
    itt    lt
    strlt   r0,[r2],#4
    blt    .LC4
#endif /* __STARTUP_INITIALIZE_NONCACHEDATA */

#ifdef __STARTUP_CLEAR_BSS
/*     This part of work usually is done in C library startup code. Otherwise,
 *     define this macro to enable it in this startup.
 *
 *     Loop to zero out BSS section, which uses following symbols
 *     in linker script:
 *      __bss_start__: start of BSS section. Must align to 4
 *      __bss_end__: end of BSS section. Must align to 4
 */
    ldr r1, =__bss_start__
    ldr r2, =__bss_end__

    movs    r0, 0
.LC5:
    cmp     r1, r2
    itt    lt
    strlt   r0, [r1], #4
    blt    .LC5
#endif /* __STARTUP_CLEAR_BSS */

    cpsie   i               /* Unmask interrupts */
#ifndef __START
#define __START _start
#endif
#ifndef __ATOLLIC__
    ldr   r0,=__START
    blx   r0
#else
    ldr   r0,=__libc_init_array
    blx   r0
    ldr   r0,=main
    bx    r0
#endif
    .pool
    .size Reset_Handler, . - Reset_Handler

    .align  1
    .thumb_func
    .weak DefaultISR
    .type DefaultISR, %function
DefaultISR:
    b DefaultISR
    .size DefaultISR, . - DefaultISR

    .align 1
    .thumb_func
    .weak NMI_Handler
    .type NMI_Handler, %function
NMI_Handler:
    ldr   r0,=NMI_Handler
    bx    r0
    .size NMI_Handler, . - NMI_Handler

    .align 1
    .thumb_func
    .weak HardFault_Handler
    .type HardFault_Handler, %function
HardFault_Handler:
    ldr   r0,=HardFault_Handler
    bx    r0
    .size HardFault_Handler, . - HardFault_Handler

    .align 1
    .thumb_func
    .weak SVC_Handler
    .type SVC_Handler, %function
SVC_Handler:
    ldr   r0,=SVC_Handler
    bx    r0
    .size SVC_Handler, . - SVC_Handler

    .align 1
    .thumb_func
    .weak PendSV_Handler
    .type PendSV_Handler, %function
PendSV_Handler:
    ldr   r0,=PendSV_Handler
    bx    r0
    .size PendSV_Handler, . - PendSV_Handler

    .align 1
    .thumb_func
    .weak SysTick_Handler
    .type SysTick_Handler, %function
SysTick_Handler:
    ldr   r0,=SysTick_Handler
    bx    r0
    .size SysTick_Handler, . - SysTick_Handler

    .align 1
    .thumb_func
    .weak DMA0_DMA16_IRQHandler
    .type DMA0_DMA16_IRQHandler, %function
DMA0_DMA16_IRQHandler:
    ldr   r0,=DMA0_DMA16_DriverIRQHandler
    bx    r0
    .size DMA0_DMA16_IRQHandler, . - DMA0_DMA16_IRQHandler

    .align 1
    .thumb_func
    .weak DMA1_DMA17_IRQHandler
    .type DMA1_DMA17_IRQHandler, %function
DMA1_DMA17_IRQHandler:
    ldr   r0,=DMA1_DMA17_DriverIRQHandler
    bx    r0
    .size DMA1_DMA17_IRQHandler, . - DMA1_DMA17_IRQHandler

    .align 1
    .thumb_func
    .weak DMA2_DMA18_IRQHandler
    .type DMA2_DMA18_IRQHandler, %function
DMA2_DMA18_IRQHandler:
    ldr   r0,=DMA2_DMA18_DriverIRQHandler
    bx    r0
    .size DMA2_DMA18_IRQHandler, . - DMA2_DMA18_IRQHandler

    .align 1
    .thumb_func
    .weak DMA3_DMA19_IRQHandler
    .type DMA3_DMA19_IRQHandler, %function
DMA3_DMA19_IRQHandler:
    ldr   r0,=DMA3_DMA19_DriverIRQHandler
    bx    r0
    .size DMA3_DMA19_IRQHandler, . - DMA3_DMA19_IRQHandler

    .align 1
    .thumb_func
    .weak DMA4_DMA20_IRQHandler
    .type DMA4_DMA20_IRQHandler, %function
DMA4_DMA20_IRQHandler:
    ldr   r0,=DMA4_DMA20_DriverIRQHandler
    bx    r0
    .size DMA4_DMA20_IRQHandler, . - DMA4_DMA20_IRQHandler

    .align 1
    .thumb_func
    .weak DMA5_DMA21_IRQHandler
    .type DMA5_DMA21_IRQHandler, %function
DMA5_DMA21_IRQHandler:
    ldr   r0,=DMA5_DMA21_DriverIRQHandler
    bx    r0
    .size DMA5_DMA21_IRQHandler, . - DMA5_DMA21_IRQHandler

    .align 1
    .thumb_func
    .weak DMA6_DMA22_IRQHandler
    .type DMA6_DMA22_IRQHandler, %function
DMA6_DMA22_IRQHandler:
    ldr   r0,=DMA6_DMA22_DriverIRQHandler
    bx    r0
    .size DMA6_DMA22_IRQHandler, . - DMA6_DMA22_IRQHandler

    .align 1
    .thumb_func
    .weak DMA7_DMA23_IRQHandler
    .type DMA7_DMA23_IRQHandler, %function
DMA7_DMA23_IRQHandler:
    ldr   r0,=DMA7_DMA23_DriverIRQHandler
    bx    r0
    .size DMA7_DMA23_IRQHandler, . - DMA7_DMA23_IRQHandler

    .align 1
    .thumb_func
    .weak DMA8_DMA24_IRQHandler
    .type DMA8_DMA24_IRQHandler, %function
DMA8_DMA24_IRQHandler:
    ldr   r0,=DMA8_DMA24_DriverIRQHandler
    bx    r0
    .size DMA8_DMA24_IRQHandler, . - DMA8_DMA24_IRQHandler

    .align 1
    .thumb_func
    .weak DMA9_DMA25_IRQHandler
    .type DMA9_DMA25_IRQHandler, %function
DMA9_DMA25_IRQHandler:
    ldr   r0,=DMA9_DMA25_DriverIRQHandler
    bx    r0
    .size DMA9_DMA25_IRQHandler, . - DMA9_DMA25_IRQHandler

    .align 1
    .thumb_func
    .weak DMA10_DMA26_IRQHandler
    .type DMA10_DMA26_IRQHandler, %function
DMA10_DMA26_IRQHandler:
    ldr   r0,=DMA10_DMA26_DriverIRQHandler
    bx    r0
    .size DMA10_DMA26_IRQHandler, . - DMA10_DMA26_IRQHandler

    .align 1
    .thumb_func
    .weak DMA11_DMA27_IRQHandler
    .type DMA11_DMA27_IRQHandler, %function
DMA11_DMA27_IRQHandler:
    ldr   r0,=DMA11_DMA27_DriverIRQHandler
    bx    r0
    .size DMA11_DMA27_IRQHandler, . - DMA11_DMA27_IRQHandler

    .align 1
    .thumb_func
    .weak DMA12_DMA28_IRQHandler
    .type DMA12_DMA28_IRQHandler, %function
DMA12_DMA28_IRQHandler:
    ldr   r0,=DMA12_DMA28_DriverIRQHandler
    bx    r0
    .size DMA12_DMA28_IRQHandler, . - DMA12_DMA28_IRQHandler

    .align 1
    .thumb_func
    .weak DMA13_DMA29_IRQHandler
    .type DMA13_DMA29_IRQHandler, %function
DMA13_DMA29_IRQHandler:
    ldr   r0,=DMA13_DMA29_DriverIRQHandler
    bx    r0
    .size DMA13_DMA29_IRQHandler, . - DMA13_DMA29_IRQHandler

    .align 1
    .thumb_func
    .weak DMA14_DMA30_IRQHandler
    .type DMA14_DMA30_IRQHandler, %function
DMA14_DMA30_IRQHandler:
    ldr   r0,=DMA14_DMA30_DriverIRQHandler
    bx    r0
    .size DMA14_DMA30_IRQHandler, . - DMA14_DMA30_IRQHandler

    .align 1
    .thumb_func
    .weak DMA15_DMA31_IRQHandler
    .type DMA15_DMA31_IRQHandler, %function
DMA15_DMA31_IRQHandler:
    ldr   r0,=DMA15_DMA31_DriverIRQHandler
    bx    r0
    .size DMA15_DMA31_IRQHandler, . - DMA15_DMA31_IRQHandler

    .align 1
    .thumb_func
    .weak DMA_ERROR_IRQHandler
    .type DMA_ERROR_IRQHandler, %function
DMA_ERROR_IRQHandler:
    ldr   r0,=DMA_ERROR_DriverIRQHandler
    bx    r0
    .size DMA_ERROR_IRQHandler, . - DMA_ERROR_IRQHandler

    .align 1
    .thumb_func
    .weak LPUART1_IRQHandler
    .type LPUART1_IRQHandler, %function
LPUART1_IRQHandler:
    ldr   r0,=LPUART1_DriverIRQHandler
    bx    r0
    .size LPUART1_IRQHandler, . - LPUART1_IRQHandler

    .align 1
    .thumb_func
    .weak LPUART2_IRQHandler
    .type LPUART2_IRQHandler, %function
LPUART2_IRQHandler:
    ldr   r0,=LPUART2_DriverIRQHandler
    bx    r0
    .size LPUART2_IRQHandler, . - LPUART2_IRQHandler

    .align 1
    .thumb_func
    .weak LPUART3_IRQHandler
    .type LPUART3_IRQHandler, %function
LPUART3_IRQHandler:
    ldr   r0,=LPUART3_DriverIRQHandler
    bx    r0
    .size LPUART3_IRQHandler, . - LPUART3_IRQHandler

    .align 1
    .thumb_func
    .weak LPUART4_IRQHandler
    .type LPUART4_IRQHandler, %function
LPUART4_IRQHandler:
    ldr   r0,=LPUART4_DriverIRQHandler
    bx    r0
    .size LPUART4_IRQHandler, . - LPUART4_IRQHandler

    .align 1
    .thumb_func
    .weak LPUART5_IRQHandler
    .type LPUART5_IRQHandler, %function
LPUART5_IRQHandler:
    ldr   r0,=LPUART5_DriverIRQHandler
    bx    r0
    .size LPUART5_IRQHandler, . - LPUART5_IRQHandler

    .align 1
    .thumb_func
    .weak LPUART6_IRQHandler
    .type LPUART6_IRQHandler, %function
LPUART6_IRQHandler:
    ldr   r0,=LPUART6_DriverIRQHandler
    bx    r0
    .size LPUART6_IRQHandler, . - LPUART6_IRQHandler

    .align 1
    .thumb_func
    .weak LPUART7_IRQHandler
    .type LPUART7_IRQHandler, %function
LPUART7_IRQHandler:
    ldr   r0,=LPUART7_DriverIRQHandler
    bx    r0
    .size LPUART7_IRQHandler, . - LPUART7_IRQHandler

    .align 1
    .thumb_func
    .weak LPUART8_IRQHandler
    .type LPUART8_IRQHandler, %function
LPUART8_IRQHandler:
    ldr   r0,=LPUART8_DriverIRQHandler
    bx    r0
    .size LPUART8_IRQHandler, . - LPUART8_IRQHandler

    .align 1
    .thumb_func
    .weak LPI2C1_IRQHandler
    .type LPI2C1_IRQHandler, %function
LPI2C1_IRQHandler:
    ldr   r0,=LPI2C1_DriverIRQHandler
    bx    r0
    .size LPI2C1_IRQHandler, . - LPI2C1_IRQHandler

    .align 1
    .thumb_func
    .weak LPI2C2_IRQHandler
    .type LPI2C2_IRQHandler, %function
LPI2C2_IRQHandler:
    ldr   r0,=LPI2C2_DriverIRQHandler
    bx    r0
    .size LPI2C2_IRQHandler, . - LPI2C2_IRQHandler

    .align 1
    .thumb_func
    .weak LPI2C3_IRQHandler
    .type LPI2C3_IRQHandler, %function
LPI2C3_IRQHandler:
    ldr   r0,=LPI2C3_DriverIRQHandler
    bx    r0
    .size LPI2C3_IRQHandler, . - LPI2C3_IRQHandler

    .align 1
    .thumb_func
    .weak LPI2C4_IRQHandler
    .type LPI2C4_IRQHandler, %function
LPI2C4_IRQHandler:
    ldr   r0,=LPI2C4_DriverIRQHandler
    bx    r0
    .size LPI2C4_IRQHandler, . - LPI2C4_IRQHandler

    .align 1
    .thumb_func
    .weak LPSPI1_IRQHandler
    .type LPSPI1_IRQHandler, %function
LPSPI1_IRQHandler:
    ldr   r0,=LPSPI1_DriverIRQHandler
    bx    r0
    .size LPSPI1_IRQHandler, . - LPSPI1_IRQHandler

    .align 1
    .thumb_func
    .weak LPSPI2_IRQHandler
    .type LPSPI2_IRQHandler, %function
LPSPI2_IRQHandler:
    ldr   r0,=LPSPI2_DriverIRQHandler
    bx    r0
    .size LPSPI2_IRQHandler, . - LPSPI2_IRQHandler

    .align 1
    .thumb_func
    .weak LPSPI3_IRQHandler
    .type LPSPI3_IRQHandler, %function
LPSPI3_IRQHandler:
    ldr   r0,=LPSPI3_DriverIRQHandler
    bx    r0
    .size LPSPI3_IRQHandler, . - LPSPI3_IRQHandler

    .align 1
    .thumb_func
    .weak LPSPI4_IRQHandler
    .type LPSPI4_IRQHandler, %function
LPSPI4_IRQHandler:
    ldr   r0,=LPSPI4_DriverIRQHandler
    bx    r0
    .size LPSPI4_IRQHandler, . - LPSPI4_IRQHandler

    .align 1
    .thumb_func
    .weak CAN1_IRQHandler
    .type CAN1_IRQHandler, %function
CAN1_IRQHandler:
    ldr   r0,=CAN1_DriverIRQHandler
    bx    r0
    .size CAN1_IRQHandler, . - CAN1_IRQHandler

    .align 1
    .thumb_func
    .weak CAN2_IRQHandler
    .type CAN2_IRQHandler, %function
CAN2_IRQHandler:
    ldr   r0,=CAN2_DriverIRQHandler
    bx    r0
    .size CAN2_IRQHandler, . - CAN2_IRQHandler

    .align 1
    .thumb_func
    .weak SAI1_IRQHandler
    .type SAI1_IRQHandler, %function
SAI1_IRQHandler:
    ldr   r0,=SAI1_DriverIRQHandler
    bx    r0
    .size SAI1_IRQHandler, . - SAI1_IRQHandler

    .align 1
    .thumb_func
    .weak SAI2_IRQHandler
    .type SAI2_IRQHandler, %function
SAI2_IRQHandler:
    ldr   r0,=SAI2_DriverIRQHandler
    bx    r0
    .size SAI2_IRQHandler, . - SAI2_IRQHandler

    .align 1
    .thumb_func
    .weak SAI3_RX_IRQHandler
    .type SAI3_RX_IRQHandler, %function
SAI3_RX_IRQHandler:
    ldr   r0,=SAI3_RX_DriverIRQHandler
    bx    r0
    .size SAI3_RX_IRQHandler, . - SAI3_RX_IRQHandler

    .align 1
    .thumb_func
    .weak SAI3_TX_IRQHandler
    .type SAI3_TX_IRQHandler, %function
SAI3_TX_IRQHandler:
    ldr   r0,=SAI3_TX_DriverIRQHandler
    bx    r0
    .size SAI3_TX_IRQHandler, . - SAI3_TX_IRQHandler

    .align 1
    .thumb_func
    .weak SPDIF_IRQHandler
    .type SPDIF_IRQHandler, %function
SPDIF_IRQHandler:
    ldr   r0,=SPDIF_DriverIRQHandler
    bx    r0
    .size SPDIF_IRQHandler, . - SPDIF_IRQHandler

    .align 1
    .thumb_func
    .weak FLEXIO1_IRQHandler
    .type FLEXIO1_IRQHandler, %function
FLEXIO1_IRQHandler:
    ldr   r0,=FLEXIO1_DriverIRQHandler
    bx    r0
    .size FLEXIO1_IRQHandler, . - FLEXIO1_IRQHandler

    .align 1
    .thumb_func
    .weak FLEXIO2_IRQHandler
    .type FLEXIO2_IRQHandler, %function
FLEXIO2_IRQHandler:
    ldr   r0,=FLEXIO2_DriverIRQHandler
    bx    r0
    .size FLEXIO2_IRQHandler, . - FLEXIO2_IRQHandler

    .align 1
    .thumb_func
    .weak FLEXSPI2_IRQHandler
    .type FLEXSPI2_IRQHandler, %function
FLEXSPI2_IRQHandler:
    ldr   r0,=FLEXSPI2_DriverIRQHandler
    bx    r0
    .size FLEXSPI2_IRQHandler, . - FLEXSPI2_IRQHandler

    .align 1
    .thumb_func
    .weak FLEXSPI_IRQHandler
    .type FLEXSPI_IRQHandler, %function
FLEXSPI_IRQHandler:
    ldr   r0,=FLEXSPI_DriverIRQHandler
    bx    r0
    .size FLEXSPI_IRQHandler, . - FLEXSPI_IRQHandler

    .align 1
    .thumb_func
    .weak USDHC1_IRQHandler
    .type USDHC1_IRQHandler, %function
USDHC1_IRQHandler:
    ldr   r0,=USDHC1_DriverIRQHandler
    bx    r0
    .size USDHC1_IRQHandler, . - USDHC1_IRQHandler

    .align 1
    .thumb_func
    .weak USDHC2_IRQHandler
    .type USDHC2_IRQHandler, %function
USDHC2_IRQHandler:
    ldr   r0,=USDHC2_DriverIRQHandler
    bx    r0
    .size USDHC2_IRQHandler, . - USDHC2_IRQHandler

    .align 1
    .thumb_func
    .weak ENET_IRQHandler
    .type ENET_IRQHandler, %function
ENET_IRQHandler:
    ldr   r0,=ENET_DriverIRQHandler
    bx    r0
    .size ENET_IRQHandler, . - ENET_IRQHandler

    .align 1
    .thumb_func
    .weak ENET_1588_Timer_IRQHandler
    .type ENET_1588_Timer_IRQHandler, %function
ENET_1588_Timer_IRQHandler:
    ldr   r0,=ENET_1588_Timer_DriverIRQHandler
    bx    r0
    .size ENET_1588_Timer_IRQHandler, . - ENET_1588_Timer_IRQHandler

    .align 1
    .thumb_func
    .weak ENET2_IRQHandler
    .type ENET2_IRQHandler, %function
ENET2_IRQHandler:
    ldr   r0,=ENET2_DriverIRQHandler
    bx    r0
    .size ENET2_IRQHandler, . - ENET2_IRQHandler

    .align 1
    .thumb_func
    .weak ENET2_1588_Timer_IRQHandler
    .type ENET2_1588_Timer_IRQHandler, %function
ENET2_1588_Timer_IRQHandler:
    ldr   r0,=ENET2_1588_Timer_DriverIRQHandler
    bx    r0
    .size ENET2_1588_Timer_IRQHandler, . - ENET2_1588_Timer_IRQHandler

    .align 1
    .thumb_func
    .weak CAN3_IRQHandler
    .type CAN3_IRQHandler, %function
CAN3_IRQHandler:
    ldr   r0,=CAN3_DriverIRQHandler
    bx    r0
    .size CAN3_IRQHandler, . - CAN3_IRQHandler

    .align 1
    .thumb_func
    .weak FLEXIO3_IRQHandler
    .type FLEXIO3_IRQHandler, %function
FLEXIO3_IRQHandler:
    ldr   r0,=FLEXIO3_DriverIRQHandler
    bx    r0
    .size FLEXIO3_IRQHandler, . - FLEXIO3_IRQHandler


/*    Macro to define default handlers. Default handler
 *    will be weak symbol and just dead loops. They can be
 *    overwritten by other handlers */
    .macro def_irq_handler  handler_name
    .weak \handler_name
    .set  \handler_name, DefaultISR
    .endm

/* Exception Handlers */
    def_irq_handler    MemManage_Handler
    def_irq_handler    BusFault_Handler
    def_irq_handler    UsageFault_Handler
    def_irq_handler    DebugMon_Handler
    def_irq_handler    DMA0_DMA16_DriverIRQHandler
    def_irq_handler    DMA1_DMA17_DriverIRQHandler
    def_irq_handler    DMA2_DMA18_DriverIRQHandler
    def_irq_handler    DMA3_DMA19_DriverIRQHandler
    def_irq_handler    DMA4_DMA20_DriverIRQHandler
    def_irq_handler    DMA5_DMA21_DriverIRQHandler
    def_irq_handler    DMA6_DMA22_DriverIRQHandler
    def_irq_handler    DMA7_DMA23_DriverIRQHandler
    def_irq_handler    DMA8_DMA24_DriverIRQHandler
    def_irq_handler    DMA9_DMA25_DriverIRQHandler
    def_irq_handler    DMA10_DMA26_DriverIRQHandler
    def_irq_handler    DMA11_DMA27_DriverIRQHandler
    def_irq_handler    DMA12_DMA28_DriverIRQHandler
    def_irq_handler    DMA13_DMA29_DriverIRQHandler
    def_irq_handler    DMA14_DMA30_DriverIRQHandler
    def_irq_handler    DMA15_DMA31_DriverIRQHandler
    def_irq_handler    DMA_ERROR_DriverIRQHandler
    def_irq_handler    CTI0_ERROR_IRQHandler
    def_irq_handler    CTI1_ERROR_IRQHandler
    def_irq_handler    CORE_IRQHandler
    def_irq_handler    LPUART1_DriverIRQHandler
    def_irq_handler    LPUART2_DriverIRQHandler
    def_irq_handler    LPUART3_DriverIRQHandler
    def_irq_handler    LPUART4_DriverIRQHandler
    def_irq_handler    LPUART5_DriverIRQHandler
    def_irq_handler    LPUART6_DriverIRQHandler
    def_irq_handler    LPUART7_DriverIRQHandler
    def_irq_handler    LPUART8_DriverIRQHandler
    def_irq_handler    LPI2C1_DriverIRQHandler
    def_irq_handler    LPI2C2_DriverIRQHandler
    def_irq_handler    LPI2C3_DriverIRQHandler
    def_irq_handler    LPI2C4_DriverIRQHandler
    def_irq_handler    LPSPI1_DriverIRQHandler
    def_irq_handler    LPSPI2_DriverIRQHandler
    def_irq_handler    LPSPI3_DriverIRQHandler
    def_irq_handler    LPSPI4_DriverIRQHandler
    def_irq_handler    CAN1_DriverIRQHandler
    def_irq_handler    CAN2_DriverIRQHandler
    def_irq_handler    FLEXRAM_IRQHandler
    def_irq_handler    KPP_IRQHandler
    def_irq_handler    TSC_DIG_IRQHandler
    def_irq_handler    GPR_IRQ_IRQHandler
    def_irq_handler    LCDIF_IRQHandler
    def_irq_handler    CSI_IRQHandler
    def_irq_handler    PXP_IRQHandler
    def_irq_handler    WDOG2_IRQHandler
    def_irq_handler    SNVS_HP_WRAPPER_IRQHandler
    def_irq_handler    SNVS_HP_WRAPPER_TZ_IRQHandler
    def_irq_handler    SNVS_LP_WRAPPER_IRQHandler
    def_irq_handler    CSU_IRQHandler
    def_irq_handler    DCP_IRQHandler
    def_irq_handler    DCP_VMI_IRQHandler
    def_irq_handler    Reserved68_IRQHandler
    def_irq_handler    TRNG_IRQHandler
    def_irq_handler    SJC_IRQHandler
    def_irq_handler    BEE_IRQHandler
    def_irq_handler    SAI1_DriverIRQHandler
    def_irq_handler    SAI2_DriverIRQHandler
    def_irq_handler    SAI3_RX_DriverIRQHandler
    def_irq_handler    SAI3_TX_DriverIRQHandler
    def_irq_handler    SPDIF_DriverIRQHandler
    def_irq_handler    PMU_EVENT_IRQHandler
    def_irq_handler    Reserved78_IRQHandler
    def_irq_handler    TEMP_LOW_HIGH_IRQHandler
    def_irq_handler    TEMP_PANIC_IRQHandler
    def_irq_handler    USB_PHY1_IRQHandler
    def_irq_handler    USB_PHY2_IRQHandler
    def_irq_handler    ADC1_IRQHandler
    def_irq_handler    ADC2_IRQHandler
    def_irq_handler    DCDC_IRQHandler
    def_irq_handler    Reserved86_IRQHandler
    def_irq_handler    Reserved87_IRQHandler
    def_irq_handler    GPIO1_INT0_IRQHandler
    def_irq_handler    GPIO1_INT1_IRQHandler
    def_irq_handler    GPIO1_INT2_IRQHandler
    def_irq_handler    GPIO1_INT3_IRQHandler
    def_irq_handler    GPIO1_INT4_IRQHandler
    def_irq_handler    GPIO1_INT5_IRQHandler
    def_irq_handler    GPIO1_INT6_IRQHandler
    def_irq_handler    GPIO1_INT7_IRQHandler
    def_irq_handler    GPIO1_Combined_0_15_IRQHandler
    def_irq_handler    GPIO1_Combined_16_31_IRQHandler
    def_irq_handler    GPIO2_Combined_0_15_IRQHandler
    def_irq_handler    GPIO2_Combined_16_31_IRQHandler
    def_irq_handler    GPIO3_Combined_0_15_IRQHandler
    def_irq_handler    GPIO3_Combined_16_31_IRQHandler
    def_irq_handler    GPIO4_Combined_0_15_IRQHandler
    def_irq_handler    GPIO4_Combined_16_31_IRQHandler
    def_irq_handler    GPIO5_Combined_0_15_IRQHandler
    def_irq_handler    GPIO5_Combined_16_31_IRQHandler
    def_irq_handler    FLEXIO1_DriverIRQHandler
    def_irq_handler    FLEXIO2_DriverIRQHandler
    def_irq_handler    WDOG1_IRQHandler
    def_irq_handler    RTWDOG_IRQHandler
    def_irq_handler    EWM_IRQHandler
    def_irq_handler    CCM_1_IRQHandler
    def_irq_handler    CCM_2_IRQHandler
    def_irq_handler    GPC_IRQHandler
    def_irq_handler    SRC_IRQHandler
    def_irq_handler    Reserved115_IRQHandler
    def_irq_handler    GPT1_IRQHandler
    def_irq_handler    GPT2_IRQHandler
    def_irq_handler    PWM1_0_IRQHandler
    def_irq_handler    PWM1_1_IRQHandler
    def_irq_handler    PWM1_2_IRQHandler
    def_irq_handler    PWM1_3_IRQHandler
    def_irq_handler    PWM1_FAULT_IRQHandler
    def_irq_handler    FLEXSPI2_DriverIRQHandler
    def_irq_handler    FLEXSPI_DriverIRQHandler
    def_irq_handler    SEMC_IRQHandler
    def_irq_handler    USDHC1_DriverIRQHandler
    def_irq_handler    USDHC2_DriverIRQHandler
    def_irq_handler    USB_OTG2_IRQHandler
    def_irq_handler    USB_OTG1_IRQHandler
    def_irq_handler    ENET_DriverIRQHandler
    def_irq_handler    ENET_1588_Timer_DriverIRQHandler
    def_irq_handler    XBAR1_IRQ_0_1_IRQHandler
    def_irq_handler    XBAR1_IRQ_2_3_IRQHandler
    def_irq_handler    ADC_ETC_IRQ0_IRQHandler
    def_irq_handler    ADC_ETC_IRQ1_IRQHandler
    def_irq_handler    ADC_ETC_IRQ2_IRQHandler
    def_irq_handler    ADC_ETC_ERROR_IRQ_IRQHandler
    def_irq_handler    PIT_IRQHandler
    def_irq_handler    ACMP1_IRQHandler
    def_irq_handler    ACMP2_IRQHandler
    def_irq_handler    ACMP3_IRQHandler
    def_irq_handler    ACMP4_IRQHandler
    def_irq_handler    Reserved143_IRQHandler
    def_irq_handler    Reserved144_IRQHandler
    def_irq_handler    ENC1_IRQHandler
    def_irq_handler    ENC2_IRQHandler
    def_irq_handler    ENC3_IRQHandler
    def_irq_handler    ENC4_IRQHandler
    def_irq_handler    TMR1_IRQHandler
    def_irq_handler    TMR2_IRQHandler
    def_irq_handler    TMR3_IRQHandler
    def_irq_handler    TMR4_IRQHandler
    def_irq_handler    PWM2_0_IRQHandler
    def_irq_handler    PWM2_1_IRQHandler
    def_irq_handler    PWM2_2_IRQHandler
    def_irq_handler    PWM2_3_IRQHandler
    def_irq_handler    PWM2_FAULT_IRQHandler
    def_irq_handler    PWM3_0_IRQHandler
    def_irq_handler    PWM3_1_IRQHandler
    def_irq_handler    PWM3_2_IRQHandler
    def_irq_handler    PWM3_3_IRQHandler
    def_irq_handler    PWM3_FAULT_IRQHandler
    def_irq_handler    PWM4_0_IRQHandler
    def_irq_handler    PWM4_1_IRQHandler
    def_irq_handler    PWM4_2_IRQHandler
    def_irq_handler    PWM4_3_IRQHandler
    def_irq_handler    PWM4_FAULT_IRQHandler
    def_irq_handler    ENET2_DriverIRQHandler
    def_irq_handler    ENET2_1588_Timer_DriverIRQHandler
    def_irq_handler    CAN3_DriverIRQHandler
    def_irq_handler    Reserved171_IRQHandler
    def_irq_handler    FLEXIO3_DriverIRQHandler
    def_irq_handler    GPIO6_7_8_9_IRQHandler

    .end
