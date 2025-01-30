/* ------------------------------------------------------------------------- */
/*  @file:    startup_MCXN947_cm33_core0.s                                   */
/*  @purpose: CMSIS Cortex-M33 Core Device Startup File                      */
/*            MCXN947_cm33_core0                                             */
/*  @version: 2.0                                                            */
/*  @date:    2023-2-1                                                       */
/*  @build:   b231018                                                        */
/* ------------------------------------------------------------------------- */
/*                                                                           */
/* Copyright 1997-2016 Freescale Semiconductor, Inc.                         */
/* Copyright 2016-2023 NXP                                                   */
/* SPDX-License-Identifier: BSD-3-Clause                                     */
/*****************************************************************************/
/* Version: GCC for ARM Embedded Processors                                  */
/*****************************************************************************/
    .syntax unified
    .arch armv8-m.main

    .section .isr_vector, "a"
    .align 2
    .globl __Vectors
__Vectors:
    .long   __StackTop                                      /* Top of Stack */
    .long   Reset_Handler                                   /* Reset Handler */
    .long   NMI_Handler                                     /* NMI Handler*/
    .long   HardFault_Handler                               /* Hard Fault Handler*/
    .long   MemManage_Handler                               /* MPU Fault Handler*/
    .long   BusFault_Handler                                /* Bus Fault Handler*/
    .long   UsageFault_Handler                              /* Usage Fault Handler*/
    .long   SecureFault_Handler                             /* Secure Fault Handler*/
    .long   0                                               /* Reserved*/
    .long   0                                               /* Reserved*/
    .long   0                                               /* Reserved*/
    .long   SVC_Handler                                     /* SVCall Handler*/
    .long   DebugMon_Handler                                /* Debug Monitor Handler*/
    .long   0                                               /* Reserved*/
    .long   PendSV_Handler                                  /* PendSV Handler*/
    .long   SysTick_Handler                                 /* SysTick Handler*/

                                                            /* External Interrupts*/
    .long   OR_IRQHandler                                   /* OR IRQ*/
    .long   EDMA_0_CH0_IRQHandler                           /* eDMA_0_CH0 error or transfer complete*/
    .long   EDMA_0_CH1_IRQHandler                           /* eDMA_0_CH1 error or transfer complete*/
    .long   EDMA_0_CH2_IRQHandler                           /* eDMA_0_CH2 error or transfer complete*/
    .long   EDMA_0_CH3_IRQHandler                           /* eDMA_0_CH3 error or transfer complete*/
    .long   EDMA_0_CH4_IRQHandler                           /* eDMA_0_CH4 error or transfer complete*/
    .long   EDMA_0_CH5_IRQHandler                           /* eDMA_0_CH5 error or transfer complete*/
    .long   EDMA_0_CH6_IRQHandler                           /* eDMA_0_CH6 error or transfer complete*/
    .long   EDMA_0_CH7_IRQHandler                           /* eDMA_0_CH7 error or transfer complete*/
    .long   EDMA_0_CH8_IRQHandler                           /* eDMA_0_CH8 error or transfer complete*/
    .long   EDMA_0_CH9_IRQHandler                           /* eDMA_0_CH9 error or transfer complete*/
    .long   EDMA_0_CH10_IRQHandler                          /* eDMA_0_CH10 error or transfer complete*/
    .long   EDMA_0_CH11_IRQHandler                          /* eDMA_0_CH11 error or transfer complete*/
    .long   EDMA_0_CH12_IRQHandler                          /* eDMA_0_CH12 error or transfer complete*/
    .long   EDMA_0_CH13_IRQHandler                          /* eDMA_0_CH13 error or transfer complete*/
    .long   EDMA_0_CH14_IRQHandler                          /* eDMA_0_CH14 error or transfer complete*/
    .long   EDMA_0_CH15_IRQHandler                          /* eDMA_0_CH15 error or transfer complete*/
    .long   GPIO00_IRQHandler                               /* GPIO0 interrupt 0*/
    .long   GPIO01_IRQHandler                               /* GPIO0 interrupt 1*/
    .long   GPIO10_IRQHandler                               /* GPIO1 interrupt 0*/
    .long   GPIO11_IRQHandler                               /* GPIO1 interrupt 1*/
    .long   GPIO20_IRQHandler                               /* GPIO2 interrupt 0*/
    .long   GPIO21_IRQHandler                               /* GPIO2 interrupt 1*/
    .long   GPIO30_IRQHandler                               /* GPIO3 interrupt 0*/
    .long   GPIO31_IRQHandler                               /* GPIO3 interrupt 1*/
    .long   GPIO40_IRQHandler                               /* GPIO4 interrupt 0*/
    .long   GPIO41_IRQHandler                               /* GPIO4 interrupt 1*/
    .long   GPIO50_IRQHandler                               /* GPIO5 interrupt 0*/
    .long   GPIO51_IRQHandler                               /* GPIO5 interrupt 1*/
    .long   UTICK0_IRQHandler                               /* Micro-Tick Timer interrupt*/
    .long   MRT0_IRQHandler                                 /* Multi-Rate Timer interrupt*/
    .long   CTIMER0_IRQHandler                              /* Standard counter/timer 0 interrupt*/
    .long   CTIMER1_IRQHandler                              /* Standard counter/timer 1 interrupt*/
    .long   SCT0_IRQHandler                                 /* SCTimer/PWM interrupt*/
    .long   CTIMER2_IRQHandler                              /* Standard counter/timer 2 interrupt*/
    .long   LP_FLEXCOMM0_IRQHandler                         /* LP_FLEXCOMM0 (LPSPI interrupt or LPI2C interrupt or LPUART Receive/Transmit interrupt)*/
    .long   LP_FLEXCOMM1_IRQHandler                         /* LP_FLEXCOMM1 (LPSPI interrupt or LPI2C interrupt or LPUART Receive/Transmit interrupt)*/
    .long   LP_FLEXCOMM2_IRQHandler                         /* LP_FLEXCOMM2 (LPSPI interrupt or LPI2C interrupt or LPUART Receive/Transmit interrupt)*/
    .long   LP_FLEXCOMM3_IRQHandler                         /* LP_FLEXCOMM3 (LPSPI interrupt or LPI2C interrupt or LPUART Receive/Transmit interrupt)*/
    .long   LP_FLEXCOMM4_IRQHandler                         /* LP_FLEXCOMM4 (LPSPI interrupt or LPI2C interrupt or LPUART Receive/Transmit interrupt)*/
    .long   LP_FLEXCOMM5_IRQHandler                         /* LP_FLEXCOMM5 (LPSPI interrupt or LPI2C interrupt or LPUART Receive/Transmit interrupt)*/
    .long   LP_FLEXCOMM6_IRQHandler                         /* LP_FLEXCOMM6 (LPSPI interrupt or LPI2C interrupt or LPUART Receive/Transmit interrupt)*/
    .long   LP_FLEXCOMM7_IRQHandler                         /* LP_FLEXCOMM7 (LPSPI interrupt or LPI2C interrupt or LPUART Receive/Transmit interrupt)*/
    .long   LP_FLEXCOMM8_IRQHandler                         /* LP_FLEXCOMM8 (LPSPI interrupt or LPI2C interrupt or LPUART Receive/Transmit interrupt)*/
    .long   LP_FLEXCOMM9_IRQHandler                         /* LP_FLEXCOMM9 (LPSPI interrupt or LPI2C interrupt or LPUART Receive/Transmit interrupt)*/
    .long   ADC0_IRQHandler                                 /* Analog-to-Digital Converter 0 - General Purpose interrupt*/
    .long   ADC1_IRQHandler                                 /* Analog-to-Digital Converter 1 - General Purpose interrupt*/
    .long   PINT0_IRQHandler                                /* Pin Interrupt Pattern Match Interrupt*/
    .long   PDM_EVENT_IRQHandler                            /* Microphone Interface interrupt*/
    .long   Reserved65_IRQHandler                           /* Reserved interrupt*/
    .long   USB0_FS_IRQHandler                              /* Universal Serial Bus - Full Speed interrupt*/
    .long   USB0_DCD_IRQHandler                             /* Universal Serial Bus - Device Charge Detect interrupt*/
    .long   RTC_IRQHandler                                  /* RTC Subsystem interrupt (RTC interrupt or Wake timer interrupt)*/
    .long   SMARTDMA_IRQHandler                             /* SmartDMA_IRQ*/
    .long   MAILBOX_IRQHandler                              /* Inter-CPU Mailbox interrupt0 for CPU0 Inter-CPU Mailbox interrupt1 for CPU1*/
    .long   CTIMER3_IRQHandler                              /* Standard counter/timer 3 interrupt*/
    .long   CTIMER4_IRQHandler                              /* Standard counter/timer 4 interrupt*/
    .long   OS_EVENT_IRQHandler                             /* OS event timer interrupt*/
    .long   FLEXSPI0_IRQHandler                             /* Flexible Serial Peripheral Interface interrupt*/
    .long   SAI0_IRQHandler                                 /* Serial Audio Interface 0 interrupt*/
    .long   SAI1_IRQHandler                                 /* Serial Audio Interface 1 interrupt*/
    .long   USDHC0_IRQHandler                               /* Ultra Secured Digital Host Controller interrupt*/
    .long   CAN0_IRQHandler                                 /* Controller Area Network 0 interrupt*/
    .long   CAN1_IRQHandler                                 /* Controller Area Network 1 interrupt*/
    .long   Reserved80_IRQHandler                           /* Reserved interrupt*/
    .long   Reserved81_IRQHandler                           /* Reserved interrupt*/
    .long   USB1_HS_PHY_IRQHandler                          /* USBHS DCD or USBHS Phy interrupt*/
    .long   USB1_HS_IRQHandler                              /* USB High Speed OTG Controller interrupt */
    .long   SEC_HYPERVISOR_CALL_IRQHandler                  /* AHB Secure Controller hypervisor call interrupt*/
    .long   Reserved85_IRQHandler                           /* Reserved interrupt*/
    .long   PLU_IRQHandler                                  /* Programmable Logic Unit interrupt*/
    .long   Freqme_IRQHandler                               /* Frequency Measurement interrupt*/
    .long   SEC_VIO_IRQHandler                              /* Secure violation interrupt (Memory Block Checker interrupt or secure AHB matrix violation interrupt)*/
    .long   ELS_IRQHandler                                  /* ELS interrupt*/
    .long   PKC_IRQHandler                                  /* PKC interrupt*/
    .long   PUF_IRQHandler                                  /* Physical Unclonable Function interrupt*/
    .long   PQ_IRQHandler                                   /* Power Quad interrupt*/
    .long   EDMA_1_CH0_IRQHandler                           /* eDMA_1_CH0 error or transfer complete*/
    .long   EDMA_1_CH1_IRQHandler                           /* eDMA_1_CH1 error or transfer complete*/
    .long   EDMA_1_CH2_IRQHandler                           /* eDMA_1_CH2 error or transfer complete*/
    .long   EDMA_1_CH3_IRQHandler                           /* eDMA_1_CH3 error or transfer complete*/
    .long   EDMA_1_CH4_IRQHandler                           /* eDMA_1_CH4 error or transfer complete*/
    .long   EDMA_1_CH5_IRQHandler                           /* eDMA_1_CH5 error or transfer complete*/
    .long   EDMA_1_CH6_IRQHandler                           /* eDMA_1_CH6 error or transfer complete*/
    .long   EDMA_1_CH7_IRQHandler                           /* eDMA_1_CH7 error or transfer complete*/
    .long   EDMA_1_CH8_IRQHandler                           /* eDMA_1_CH8 error or transfer complete*/
    .long   EDMA_1_CH9_IRQHandler                           /* eDMA_1_CH9 error or transfer complete*/
    .long   EDMA_1_CH10_IRQHandler                          /* eDMA_1_CH10 error or transfer complete*/
    .long   EDMA_1_CH11_IRQHandler                          /* eDMA_1_CH11 error or transfer complete*/
    .long   EDMA_1_CH12_IRQHandler                          /* eDMA_1_CH12 error or transfer complete*/
    .long   EDMA_1_CH13_IRQHandler                          /* eDMA_1_CH13 error or transfer complete*/
    .long   EDMA_1_CH14_IRQHandler                          /* eDMA_1_CH14 error or transfer complete*/
    .long   EDMA_1_CH15_IRQHandler                          /* eDMA_1_CH15 error or transfer complete*/
    .long   CDOG0_IRQHandler                                /* Code Watchdog Timer 0 interrupt*/
    .long   CDOG1_IRQHandler                                /* Code Watchdog Timer 1 interrupt*/
    .long   I3C0_IRQHandler                                 /* Improved Inter Integrated Circuit interrupt 0*/
    .long   I3C1_IRQHandler                                 /* Improved Inter Integrated Circuit interrupt 1*/
    .long   NPU_IRQHandler                                  /* NPU interrupt*/
    .long   GDET_IRQHandler                                 /* Digital Glitch Detect 0 interrupt  or Digital Glitch Detect 1 interrupt*/
    .long   VBAT0_IRQHandler                                /* VBAT interrupt( VBAT interrupt or digital tamper interrupt)*/
    .long   EWM0_IRQHandler                                 /* External Watchdog Monitor interrupt*/
    .long   TSI_END_OF_SCAN_IRQHandler                      /* TSI End of Scan interrupt*/
    .long   TSI_OUT_OF_SCAN_IRQHandler                      /* TSI Out of Scan interrupt*/
    .long   EMVSIM0_IRQHandler                              /* EMVSIM0 interrupt*/
    .long   EMVSIM1_IRQHandler                              /* EMVSIM1 interrupt*/
    .long   FLEXIO_IRQHandler                               /* Flexible Input/Output interrupt*/
    .long   DAC0_IRQHandler                                 /* Digital-to-Analog Converter 0 - General Purpose interrupt*/
    .long   DAC1_IRQHandler                                 /* Digital-to-Analog Converter 1 - General Purpose interrupt*/
    .long   DAC2_IRQHandler                                 /* 14-bit Digital-to-Analog Converter interrupt*/
    .long   HSCMP0_IRQHandler                               /* High-Speed comparator0 interrupt*/
    .long   HSCMP1_IRQHandler                               /* High-Speed comparator1 interrupt*/
    .long   HSCMP2_IRQHandler                               /* High-Speed comparator2 interrupt*/
    .long   FLEXPWM0_RELOAD_ERROR_IRQHandler                /* FlexPWM0_reload_error interrupt*/
    .long   FLEXPWM0_FAULT_IRQHandler                       /* FlexPWM0_fault interrupt*/
    .long   FLEXPWM0_SUBMODULE0_IRQHandler                  /* FlexPWM0 Submodule 0 capture/compare/reload interrupt*/
    .long   FLEXPWM0_SUBMODULE1_IRQHandler                  /* FlexPWM0 Submodule 1 capture/compare/reload interrupt*/
    .long   FLEXPWM0_SUBMODULE2_IRQHandler                  /* FlexPWM0 Submodule 2 capture/compare/reload interrupt*/
    .long   FLEXPWM0_SUBMODULE3_IRQHandler                  /* FlexPWM0 Submodule 3 capture/compare/reload interrupt*/
    .long   FLEXPWM1_RELOAD_ERROR_IRQHandler                /* FlexPWM1_reload_error interrupt*/
    .long   FLEXPWM1_FAULT_IRQHandler                       /* FlexPWM1_fault interrupt*/
    .long   FLEXPWM1_SUBMODULE0_IRQHandler                  /* FlexPWM1 Submodule 0 capture/compare/reload interrupt*/
    .long   FLEXPWM1_SUBMODULE1_IRQHandler                  /* FlexPWM1 Submodule 1 capture/compare/reload interrupt*/
    .long   FLEXPWM1_SUBMODULE2_IRQHandler                  /* FlexPWM1 Submodule 2 capture/compare/reload interrupt*/
    .long   FLEXPWM1_SUBMODULE3_IRQHandler                  /* FlexPWM1 Submodule 3 capture/compare/reload interrupt*/
    .long   ENC0_COMPARE_IRQHandler                         /* ENC0_Compare interrupt*/
    .long   ENC0_HOME_IRQHandler                            /* ENC0_Home interrupt*/
    .long   ENC0_WDG_SAB_IRQHandler                         /* ENC0_WDG_IRQ/SAB interrupt*/
    .long   ENC0_IDX_IRQHandler                             /* ENC0_IDX interrupt*/
    .long   ENC1_COMPARE_IRQHandler                         /* ENC1_Compare interrupt*/
    .long   ENC1_HOME_IRQHandler                            /* ENC1_Home interrupt*/
    .long   ENC1_WDG_SAB_IRQHandler                         /* ENC1_WDG_IRQ/SAB interrupt*/
    .long   ENC1_IDX_IRQHandler                             /* ENC1_IDX interrupt*/
    .long   ITRC0_IRQHandler                                /* Intrusion and Tamper Response Controller interrupt*/
    .long   BSP32_IRQHandler                                /* CoolFlux BSP32 interrupt*/
    .long   ELS_ERR_IRQHandler                              /* ELS error interrupt*/
    .long   PKC_ERR_IRQHandler                              /* PKC error interrupt*/
    .long   ERM_SINGLE_BIT_ERROR_IRQHandler                 /* ERM Single Bit error interrupt*/
    .long   ERM_MULTI_BIT_ERROR_IRQHandler                  /* ERM Multi Bit error interrupt*/
    .long   FMU0_IRQHandler                                 /* Flash Management Unit interrupt*/
    .long   ETHERNET_IRQHandler                             /* Ethernet QoS interrupt*/
    .long   ETHERNET_PMT_IRQHandler                         /* Ethernet QoS power management interrupt*/
    .long   ETHERNET_MACLP_IRQHandler                       /* Ethernet QoS MAC interrupt*/
    .long   SINC_FILTER_IRQHandler                          /* SINC Filter interrupt */
    .long   LPTMR0_IRQHandler                               /* Low Power Timer 0 interrupt*/
    .long   LPTMR1_IRQHandler                               /* Low Power Timer 1 interrupt*/
    .long   SCG_IRQHandler                                  /* System Clock Generator interrupt*/
    .long   SPC_IRQHandler                                  /* System Power Controller interrupt*/
    .long   WUU_IRQHandler                                  /* Wake Up Unit interrupt*/
    .long   PORT_EFT_IRQHandler                             /* PORT0~5 EFT interrupt*/
    .long   ETB0_IRQHandler                                 /* ETB counter expires interrupt*/
    .long   SM3_IRQHandler                                  /* Secure Generic Interface (SGI) SAFO interrupt */
    .long   TRNG0_IRQHandler                                /* True Random Number Generator interrupt*/
    .long   WWDT0_IRQHandler                                /* Windowed Watchdog Timer 0 interrupt*/
    .long   WWDT1_IRQHandler                                /* Windowed Watchdog Timer 1 interrupt*/
    .long   CMC0_IRQHandler                                 /* Core Mode Controller interrupt*/
    .long   CTI0_IRQHandler                                 /* Cross Trigger Interface interrupt*/

    .size   __Vectors, . - __Vectors

    .text
    .thumb

#if defined (__cplusplus)
#ifdef __REDLIB__
#error Redlib does not support C++
#endif
#endif
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
    ldr     r1, =__Vectors
    str     r1, [r0]
    ldr     r2, [r1]
    msr     msp, r2
    ldr     r0, =__StackLimit
    msr     msplim, r0
#ifndef __NO_SYSTEM_INIT
    ldr   r0,=SystemInit
    blx   r0
#endif
/*     Loop to copy data from read only memory to RAM. The ranges
 *      of copy from/to are specified by following symbols evaluated in
 *      linker script.
 *      __etext: End of code section, i.e., begin of data sections to copy from.
 *      __data_start__/__data_end__: RAM address range that data should be
 *      copied to. Both must be aligned to 4 bytes boundary.  */

    ldr    r1, =__etext
    ldr    r2, =__data_start__
    ldr    r3, =__data_end__

#if 1
/* Here are two copies of loop implementations. First one favors code size
 * and the second one favors performance. Default uses the first one.
 * Change to "#if 0" to use the second one */
.LC0:
    cmp     r2, r3
    ittt    lt
    ldrlt   r0, [r1], #4
    strlt   r0, [r2], #4
    blt    .LC0
#else
    subs    r3, r2
    ble    .LC1
.LC0:
    subs    r3, #4
    ldr    r0, [r1, r3]
    str    r0, [r2, r3]
    bgt    .LC0
.LC1:
#endif

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
.LC2:
    cmp     r1, r2
    itt    lt
    strlt   r0, [r1], #4
    blt    .LC2
#endif /* __STARTUP_CLEAR_BSS */

/* Add stack / heap initialization */
    movs    r0, 0
    ldr     r1, =__HeapBase
    ldr     r2, =__HeapLimit
.LC3:
    cmp     r1, r2
    itt     lt
    strlt   r0, [r1], #4
    blt     .LC3

    ldr     r1, =__StackLimit
    ldr     r2, =__StackTop
.LC4:
    cmp     r1, r2
    itt     lt
    strlt   r0, [r1], #4
    blt     .LC4

/*End of stack / heap initialization */
    cpsie   i               /* Unmask interrupts */
#ifndef __START
#ifdef __REDLIB__
#define __START __main
#else
#define __START _start
#endif
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
    .weak OR_IRQHandler
    .type OR_IRQHandler, %function
OR_IRQHandler:
    ldr   r0,=OR_DriverIRQHandler
    bx    r0
    .size OR_IRQHandler, . - OR_IRQHandler

    .align 1
    .thumb_func
    .weak EDMA_0_CH0_IRQHandler
    .type EDMA_0_CH0_IRQHandler, %function
EDMA_0_CH0_IRQHandler:
    ldr   r0,=EDMA_0_CH0_DriverIRQHandler
    bx    r0
    .size EDMA_0_CH0_IRQHandler, . - EDMA_0_CH0_IRQHandler

    .align 1
    .thumb_func
    .weak EDMA_0_CH1_IRQHandler
    .type EDMA_0_CH1_IRQHandler, %function
EDMA_0_CH1_IRQHandler:
    ldr   r0,=EDMA_0_CH1_DriverIRQHandler
    bx    r0
    .size EDMA_0_CH1_IRQHandler, . - EDMA_0_CH1_IRQHandler

    .align 1
    .thumb_func
    .weak EDMA_0_CH2_IRQHandler
    .type EDMA_0_CH2_IRQHandler, %function
EDMA_0_CH2_IRQHandler:
    ldr   r0,=EDMA_0_CH2_DriverIRQHandler
    bx    r0
    .size EDMA_0_CH2_IRQHandler, . - EDMA_0_CH2_IRQHandler

    .align 1
    .thumb_func
    .weak EDMA_0_CH3_IRQHandler
    .type EDMA_0_CH3_IRQHandler, %function
EDMA_0_CH3_IRQHandler:
    ldr   r0,=EDMA_0_CH3_DriverIRQHandler
    bx    r0
    .size EDMA_0_CH3_IRQHandler, . - EDMA_0_CH3_IRQHandler

    .align 1
    .thumb_func
    .weak EDMA_0_CH4_IRQHandler
    .type EDMA_0_CH4_IRQHandler, %function
EDMA_0_CH4_IRQHandler:
    ldr   r0,=EDMA_0_CH4_DriverIRQHandler
    bx    r0
    .size EDMA_0_CH4_IRQHandler, . - EDMA_0_CH4_IRQHandler

    .align 1
    .thumb_func
    .weak EDMA_0_CH5_IRQHandler
    .type EDMA_0_CH5_IRQHandler, %function
EDMA_0_CH5_IRQHandler:
    ldr   r0,=EDMA_0_CH5_DriverIRQHandler
    bx    r0
    .size EDMA_0_CH5_IRQHandler, . - EDMA_0_CH5_IRQHandler

    .align 1
    .thumb_func
    .weak EDMA_0_CH6_IRQHandler
    .type EDMA_0_CH6_IRQHandler, %function
EDMA_0_CH6_IRQHandler:
    ldr   r0,=EDMA_0_CH6_DriverIRQHandler
    bx    r0
    .size EDMA_0_CH6_IRQHandler, . - EDMA_0_CH6_IRQHandler

    .align 1
    .thumb_func
    .weak EDMA_0_CH7_IRQHandler
    .type EDMA_0_CH7_IRQHandler, %function
EDMA_0_CH7_IRQHandler:
    ldr   r0,=EDMA_0_CH7_DriverIRQHandler
    bx    r0
    .size EDMA_0_CH7_IRQHandler, . - EDMA_0_CH7_IRQHandler

    .align 1
    .thumb_func
    .weak EDMA_0_CH8_IRQHandler
    .type EDMA_0_CH8_IRQHandler, %function
EDMA_0_CH8_IRQHandler:
    ldr   r0,=EDMA_0_CH8_DriverIRQHandler
    bx    r0
    .size EDMA_0_CH8_IRQHandler, . - EDMA_0_CH8_IRQHandler

    .align 1
    .thumb_func
    .weak EDMA_0_CH9_IRQHandler
    .type EDMA_0_CH9_IRQHandler, %function
EDMA_0_CH9_IRQHandler:
    ldr   r0,=EDMA_0_CH9_DriverIRQHandler
    bx    r0
    .size EDMA_0_CH9_IRQHandler, . - EDMA_0_CH9_IRQHandler

    .align 1
    .thumb_func
    .weak EDMA_0_CH10_IRQHandler
    .type EDMA_0_CH10_IRQHandler, %function
EDMA_0_CH10_IRQHandler:
    ldr   r0,=EDMA_0_CH10_DriverIRQHandler
    bx    r0
    .size EDMA_0_CH10_IRQHandler, . - EDMA_0_CH10_IRQHandler

    .align 1
    .thumb_func
    .weak EDMA_0_CH11_IRQHandler
    .type EDMA_0_CH11_IRQHandler, %function
EDMA_0_CH11_IRQHandler:
    ldr   r0,=EDMA_0_CH11_DriverIRQHandler
    bx    r0
    .size EDMA_0_CH11_IRQHandler, . - EDMA_0_CH11_IRQHandler

    .align 1
    .thumb_func
    .weak EDMA_0_CH12_IRQHandler
    .type EDMA_0_CH12_IRQHandler, %function
EDMA_0_CH12_IRQHandler:
    ldr   r0,=EDMA_0_CH12_DriverIRQHandler
    bx    r0
    .size EDMA_0_CH12_IRQHandler, . - EDMA_0_CH12_IRQHandler

    .align 1
    .thumb_func
    .weak EDMA_0_CH13_IRQHandler
    .type EDMA_0_CH13_IRQHandler, %function
EDMA_0_CH13_IRQHandler:
    ldr   r0,=EDMA_0_CH13_DriverIRQHandler
    bx    r0
    .size EDMA_0_CH13_IRQHandler, . - EDMA_0_CH13_IRQHandler

    .align 1
    .thumb_func
    .weak EDMA_0_CH14_IRQHandler
    .type EDMA_0_CH14_IRQHandler, %function
EDMA_0_CH14_IRQHandler:
    ldr   r0,=EDMA_0_CH14_DriverIRQHandler
    bx    r0
    .size EDMA_0_CH14_IRQHandler, . - EDMA_0_CH14_IRQHandler

    .align 1
    .thumb_func
    .weak EDMA_0_CH15_IRQHandler
    .type EDMA_0_CH15_IRQHandler, %function
EDMA_0_CH15_IRQHandler:
    ldr   r0,=EDMA_0_CH15_DriverIRQHandler
    bx    r0
    .size EDMA_0_CH15_IRQHandler, . - EDMA_0_CH15_IRQHandler

    .align 1
    .thumb_func
    .weak GPIO00_IRQHandler
    .type GPIO00_IRQHandler, %function
GPIO00_IRQHandler:
    ldr   r0,=GPIO00_DriverIRQHandler
    bx    r0
    .size GPIO00_IRQHandler, . - GPIO00_IRQHandler

    .align 1
    .thumb_func
    .weak GPIO01_IRQHandler
    .type GPIO01_IRQHandler, %function
GPIO01_IRQHandler:
    ldr   r0,=GPIO01_DriverIRQHandler
    bx    r0
    .size GPIO01_IRQHandler, . - GPIO01_IRQHandler

    .align 1
    .thumb_func
    .weak GPIO10_IRQHandler
    .type GPIO10_IRQHandler, %function
GPIO10_IRQHandler:
    ldr   r0,=GPIO10_DriverIRQHandler
    bx    r0
    .size GPIO10_IRQHandler, . - GPIO10_IRQHandler

    .align 1
    .thumb_func
    .weak GPIO11_IRQHandler
    .type GPIO11_IRQHandler, %function
GPIO11_IRQHandler:
    ldr   r0,=GPIO11_DriverIRQHandler
    bx    r0
    .size GPIO11_IRQHandler, . - GPIO11_IRQHandler

    .align 1
    .thumb_func
    .weak GPIO20_IRQHandler
    .type GPIO20_IRQHandler, %function
GPIO20_IRQHandler:
    ldr   r0,=GPIO20_DriverIRQHandler
    bx    r0
    .size GPIO20_IRQHandler, . - GPIO20_IRQHandler

    .align 1
    .thumb_func
    .weak GPIO21_IRQHandler
    .type GPIO21_IRQHandler, %function
GPIO21_IRQHandler:
    ldr   r0,=GPIO21_DriverIRQHandler
    bx    r0
    .size GPIO21_IRQHandler, . - GPIO21_IRQHandler

    .align 1
    .thumb_func
    .weak GPIO30_IRQHandler
    .type GPIO30_IRQHandler, %function
GPIO30_IRQHandler:
    ldr   r0,=GPIO30_DriverIRQHandler
    bx    r0
    .size GPIO30_IRQHandler, . - GPIO30_IRQHandler

    .align 1
    .thumb_func
    .weak GPIO31_IRQHandler
    .type GPIO31_IRQHandler, %function
GPIO31_IRQHandler:
    ldr   r0,=GPIO31_DriverIRQHandler
    bx    r0
    .size GPIO31_IRQHandler, . - GPIO31_IRQHandler

    .align 1
    .thumb_func
    .weak GPIO40_IRQHandler
    .type GPIO40_IRQHandler, %function
GPIO40_IRQHandler:
    ldr   r0,=GPIO40_DriverIRQHandler
    bx    r0
    .size GPIO40_IRQHandler, . - GPIO40_IRQHandler

    .align 1
    .thumb_func
    .weak GPIO41_IRQHandler
    .type GPIO41_IRQHandler, %function
GPIO41_IRQHandler:
    ldr   r0,=GPIO41_DriverIRQHandler
    bx    r0
    .size GPIO41_IRQHandler, . - GPIO41_IRQHandler

    .align 1
    .thumb_func
    .weak GPIO50_IRQHandler
    .type GPIO50_IRQHandler, %function
GPIO50_IRQHandler:
    ldr   r0,=GPIO50_DriverIRQHandler
    bx    r0
    .size GPIO50_IRQHandler, . - GPIO50_IRQHandler

    .align 1
    .thumb_func
    .weak GPIO51_IRQHandler
    .type GPIO51_IRQHandler, %function
GPIO51_IRQHandler:
    ldr   r0,=GPIO51_DriverIRQHandler
    bx    r0
    .size GPIO51_IRQHandler, . - GPIO51_IRQHandler

    .align 1
    .thumb_func
    .weak UTICK0_IRQHandler
    .type UTICK0_IRQHandler, %function
UTICK0_IRQHandler:
    ldr   r0,=UTICK0_DriverIRQHandler
    bx    r0
    .size UTICK0_IRQHandler, . - UTICK0_IRQHandler

    .align 1
    .thumb_func
    .weak MRT0_IRQHandler
    .type MRT0_IRQHandler, %function
MRT0_IRQHandler:
    ldr   r0,=MRT0_DriverIRQHandler
    bx    r0
    .size MRT0_IRQHandler, . - MRT0_IRQHandler

    .align 1
    .thumb_func
    .weak CTIMER0_IRQHandler
    .type CTIMER0_IRQHandler, %function
CTIMER0_IRQHandler:
    ldr   r0,=CTIMER0_DriverIRQHandler
    bx    r0
    .size CTIMER0_IRQHandler, . - CTIMER0_IRQHandler

    .align 1
    .thumb_func
    .weak CTIMER1_IRQHandler
    .type CTIMER1_IRQHandler, %function
CTIMER1_IRQHandler:
    ldr   r0,=CTIMER1_DriverIRQHandler
    bx    r0
    .size CTIMER1_IRQHandler, . - CTIMER1_IRQHandler

    .align 1
    .thumb_func
    .weak SCT0_IRQHandler
    .type SCT0_IRQHandler, %function
SCT0_IRQHandler:
    ldr   r0,=SCT0_DriverIRQHandler
    bx    r0
    .size SCT0_IRQHandler, . - SCT0_IRQHandler

    .align 1
    .thumb_func
    .weak CTIMER2_IRQHandler
    .type CTIMER2_IRQHandler, %function
CTIMER2_IRQHandler:
    ldr   r0,=CTIMER2_DriverIRQHandler
    bx    r0
    .size CTIMER2_IRQHandler, . - CTIMER2_IRQHandler

    .align 1
    .thumb_func
    .weak LP_FLEXCOMM0_IRQHandler
    .type LP_FLEXCOMM0_IRQHandler, %function
LP_FLEXCOMM0_IRQHandler:
    ldr   r0,=LP_FLEXCOMM0_DriverIRQHandler
    bx    r0
    .size LP_FLEXCOMM0_IRQHandler, . - LP_FLEXCOMM0_IRQHandler

    .align 1
    .thumb_func
    .weak LP_FLEXCOMM1_IRQHandler
    .type LP_FLEXCOMM1_IRQHandler, %function
LP_FLEXCOMM1_IRQHandler:
    ldr   r0,=LP_FLEXCOMM1_DriverIRQHandler
    bx    r0
    .size LP_FLEXCOMM1_IRQHandler, . - LP_FLEXCOMM1_IRQHandler

    .align 1
    .thumb_func
    .weak LP_FLEXCOMM2_IRQHandler
    .type LP_FLEXCOMM2_IRQHandler, %function
LP_FLEXCOMM2_IRQHandler:
    ldr   r0,=LP_FLEXCOMM2_DriverIRQHandler
    bx    r0
    .size LP_FLEXCOMM2_IRQHandler, . - LP_FLEXCOMM2_IRQHandler

    .align 1
    .thumb_func
    .weak LP_FLEXCOMM3_IRQHandler
    .type LP_FLEXCOMM3_IRQHandler, %function
LP_FLEXCOMM3_IRQHandler:
    ldr   r0,=LP_FLEXCOMM3_DriverIRQHandler
    bx    r0
    .size LP_FLEXCOMM3_IRQHandler, . - LP_FLEXCOMM3_IRQHandler

    .align 1
    .thumb_func
    .weak LP_FLEXCOMM4_IRQHandler
    .type LP_FLEXCOMM4_IRQHandler, %function
LP_FLEXCOMM4_IRQHandler:
    ldr   r0,=LP_FLEXCOMM4_DriverIRQHandler
    bx    r0
    .size LP_FLEXCOMM4_IRQHandler, . - LP_FLEXCOMM4_IRQHandler

    .align 1
    .thumb_func
    .weak LP_FLEXCOMM5_IRQHandler
    .type LP_FLEXCOMM5_IRQHandler, %function
LP_FLEXCOMM5_IRQHandler:
    ldr   r0,=LP_FLEXCOMM5_DriverIRQHandler
    bx    r0
    .size LP_FLEXCOMM5_IRQHandler, . - LP_FLEXCOMM5_IRQHandler

    .align 1
    .thumb_func
    .weak LP_FLEXCOMM6_IRQHandler
    .type LP_FLEXCOMM6_IRQHandler, %function
LP_FLEXCOMM6_IRQHandler:
    ldr   r0,=LP_FLEXCOMM6_DriverIRQHandler
    bx    r0
    .size LP_FLEXCOMM6_IRQHandler, . - LP_FLEXCOMM6_IRQHandler

    .align 1
    .thumb_func
    .weak LP_FLEXCOMM7_IRQHandler
    .type LP_FLEXCOMM7_IRQHandler, %function
LP_FLEXCOMM7_IRQHandler:
    ldr   r0,=LP_FLEXCOMM7_DriverIRQHandler
    bx    r0
    .size LP_FLEXCOMM7_IRQHandler, . - LP_FLEXCOMM7_IRQHandler

    .align 1
    .thumb_func
    .weak LP_FLEXCOMM8_IRQHandler
    .type LP_FLEXCOMM8_IRQHandler, %function
LP_FLEXCOMM8_IRQHandler:
    ldr   r0,=LP_FLEXCOMM8_DriverIRQHandler
    bx    r0
    .size LP_FLEXCOMM8_IRQHandler, . - LP_FLEXCOMM8_IRQHandler

    .align 1
    .thumb_func
    .weak LP_FLEXCOMM9_IRQHandler
    .type LP_FLEXCOMM9_IRQHandler, %function
LP_FLEXCOMM9_IRQHandler:
    ldr   r0,=LP_FLEXCOMM9_DriverIRQHandler
    bx    r0
    .size LP_FLEXCOMM9_IRQHandler, . - LP_FLEXCOMM9_IRQHandler

    .align 1
    .thumb_func
    .weak ADC0_IRQHandler
    .type ADC0_IRQHandler, %function
ADC0_IRQHandler:
    ldr   r0,=ADC0_DriverIRQHandler
    bx    r0
    .size ADC0_IRQHandler, . - ADC0_IRQHandler

    .align 1
    .thumb_func
    .weak ADC1_IRQHandler
    .type ADC1_IRQHandler, %function
ADC1_IRQHandler:
    ldr   r0,=ADC1_DriverIRQHandler
    bx    r0
    .size ADC1_IRQHandler, . - ADC1_IRQHandler

    .align 1
    .thumb_func
    .weak PINT0_IRQHandler
    .type PINT0_IRQHandler, %function
PINT0_IRQHandler:
    ldr   r0,=PINT0_DriverIRQHandler
    bx    r0
    .size PINT0_IRQHandler, . - PINT0_IRQHandler

    .align 1
    .thumb_func
    .weak PDM_EVENT_IRQHandler
    .type PDM_EVENT_IRQHandler, %function
PDM_EVENT_IRQHandler:
    ldr   r0,=PDM_EVENT_DriverIRQHandler
    bx    r0
    .size PDM_EVENT_IRQHandler, . - PDM_EVENT_IRQHandler

    .align 1
    .thumb_func
    .weak Reserved65_IRQHandler
    .type Reserved65_IRQHandler, %function
Reserved65_IRQHandler:
    ldr   r0,=Reserved65_DriverIRQHandler
    bx    r0
    .size Reserved65_IRQHandler, . - Reserved65_IRQHandler

    .align 1
    .thumb_func
    .weak USB0_FS_IRQHandler
    .type USB0_FS_IRQHandler, %function
USB0_FS_IRQHandler:
    ldr   r0,=USB0_FS_DriverIRQHandler
    bx    r0
    .size USB0_FS_IRQHandler, . - USB0_FS_IRQHandler

    .align 1
    .thumb_func
    .weak USB0_DCD_IRQHandler
    .type USB0_DCD_IRQHandler, %function
USB0_DCD_IRQHandler:
    ldr   r0,=USB0_DCD_DriverIRQHandler
    bx    r0
    .size USB0_DCD_IRQHandler, . - USB0_DCD_IRQHandler

    .align 1
    .thumb_func
    .weak RTC_IRQHandler
    .type RTC_IRQHandler, %function
RTC_IRQHandler:
    ldr   r0,=RTC_DriverIRQHandler
    bx    r0
    .size RTC_IRQHandler, . - RTC_IRQHandler

    .align 1
    .thumb_func
    .weak SMARTDMA_IRQHandler
    .type SMARTDMA_IRQHandler, %function
SMARTDMA_IRQHandler:
    ldr   r0,=SMARTDMA_DriverIRQHandler
    bx    r0
    .size SMARTDMA_IRQHandler, . - SMARTDMA_IRQHandler

    .align 1
    .thumb_func
    .weak MAILBOX_IRQHandler
    .type MAILBOX_IRQHandler, %function
MAILBOX_IRQHandler:
    ldr   r0,=MAILBOX_DriverIRQHandler
    bx    r0
    .size MAILBOX_IRQHandler, . - MAILBOX_IRQHandler

    .align 1
    .thumb_func
    .weak CTIMER3_IRQHandler
    .type CTIMER3_IRQHandler, %function
CTIMER3_IRQHandler:
    ldr   r0,=CTIMER3_DriverIRQHandler
    bx    r0
    .size CTIMER3_IRQHandler, . - CTIMER3_IRQHandler

    .align 1
    .thumb_func
    .weak CTIMER4_IRQHandler
    .type CTIMER4_IRQHandler, %function
CTIMER4_IRQHandler:
    ldr   r0,=CTIMER4_DriverIRQHandler
    bx    r0
    .size CTIMER4_IRQHandler, . - CTIMER4_IRQHandler

    .align 1
    .thumb_func
    .weak OS_EVENT_IRQHandler
    .type OS_EVENT_IRQHandler, %function
OS_EVENT_IRQHandler:
    ldr   r0,=OS_EVENT_DriverIRQHandler
    bx    r0
    .size OS_EVENT_IRQHandler, . - OS_EVENT_IRQHandler

    .align 1
    .thumb_func
    .weak FLEXSPI0_IRQHandler
    .type FLEXSPI0_IRQHandler, %function
FLEXSPI0_IRQHandler:
    ldr   r0,=FLEXSPI0_DriverIRQHandler
    bx    r0
    .size FLEXSPI0_IRQHandler, . - FLEXSPI0_IRQHandler

    .align 1
    .thumb_func
    .weak SAI0_IRQHandler
    .type SAI0_IRQHandler, %function
SAI0_IRQHandler:
    ldr   r0,=SAI0_DriverIRQHandler
    bx    r0
    .size SAI0_IRQHandler, . - SAI0_IRQHandler

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
    .weak USDHC0_IRQHandler
    .type USDHC0_IRQHandler, %function
USDHC0_IRQHandler:
    ldr   r0,=USDHC0_DriverIRQHandler
    bx    r0
    .size USDHC0_IRQHandler, . - USDHC0_IRQHandler

    .align 1
    .thumb_func
    .weak CAN0_IRQHandler
    .type CAN0_IRQHandler, %function
CAN0_IRQHandler:
    ldr   r0,=CAN0_DriverIRQHandler
    bx    r0
    .size CAN0_IRQHandler, . - CAN0_IRQHandler

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
    .weak Reserved80_IRQHandler
    .type Reserved80_IRQHandler, %function
Reserved80_IRQHandler:
    ldr   r0,=Reserved80_DriverIRQHandler
    bx    r0
    .size Reserved80_IRQHandler, . - Reserved80_IRQHandler

    .align 1
    .thumb_func
    .weak Reserved81_IRQHandler
    .type Reserved81_IRQHandler, %function
Reserved81_IRQHandler:
    ldr   r0,=Reserved81_DriverIRQHandler
    bx    r0
    .size Reserved81_IRQHandler, . - Reserved81_IRQHandler

    .align 1
    .thumb_func
    .weak USB1_HS_PHY_IRQHandler
    .type USB1_HS_PHY_IRQHandler, %function
USB1_HS_PHY_IRQHandler:
    ldr   r0,=USB1_HS_PHY_DriverIRQHandler
    bx    r0
    .size USB1_HS_PHY_IRQHandler, . - USB1_HS_PHY_IRQHandler

    .align 1
    .thumb_func
    .weak USB1_HS_IRQHandler
    .type USB1_HS_IRQHandler, %function
USB1_HS_IRQHandler:
    ldr   r0,=USB1_HS_DriverIRQHandler
    bx    r0
    .size USB1_HS_IRQHandler, . - USB1_HS_IRQHandler

    .align 1
    .thumb_func
    .weak SEC_HYPERVISOR_CALL_IRQHandler
    .type SEC_HYPERVISOR_CALL_IRQHandler, %function
SEC_HYPERVISOR_CALL_IRQHandler:
    ldr   r0,=SEC_HYPERVISOR_CALL_DriverIRQHandler
    bx    r0
    .size SEC_HYPERVISOR_CALL_IRQHandler, . - SEC_HYPERVISOR_CALL_IRQHandler

    .align 1
    .thumb_func
    .weak Reserved85_IRQHandler
    .type Reserved85_IRQHandler, %function
Reserved85_IRQHandler:
    ldr   r0,=Reserved85_DriverIRQHandler
    bx    r0
    .size Reserved85_IRQHandler, . - Reserved85_IRQHandler

    .align 1
    .thumb_func
    .weak PLU_IRQHandler
    .type PLU_IRQHandler, %function
PLU_IRQHandler:
    ldr   r0,=PLU_DriverIRQHandler
    bx    r0
    .size PLU_IRQHandler, . - PLU_IRQHandler

    .align 1
    .thumb_func
    .weak Freqme_IRQHandler
    .type Freqme_IRQHandler, %function
Freqme_IRQHandler:
    ldr   r0,=Freqme_DriverIRQHandler
    bx    r0
    .size Freqme_IRQHandler, . - Freqme_IRQHandler

    .align 1
    .thumb_func
    .weak SEC_VIO_IRQHandler
    .type SEC_VIO_IRQHandler, %function
SEC_VIO_IRQHandler:
    ldr   r0,=SEC_VIO_DriverIRQHandler
    bx    r0
    .size SEC_VIO_IRQHandler, . - SEC_VIO_IRQHandler

    .align 1
    .thumb_func
    .weak ELS_IRQHandler
    .type ELS_IRQHandler, %function
ELS_IRQHandler:
    ldr   r0,=ELS_DriverIRQHandler
    bx    r0
    .size ELS_IRQHandler, . - ELS_IRQHandler

    .align 1
    .thumb_func
    .weak PKC_IRQHandler
    .type PKC_IRQHandler, %function
PKC_IRQHandler:
    ldr   r0,=PKC_DriverIRQHandler
    bx    r0
    .size PKC_IRQHandler, . - PKC_IRQHandler

    .align 1
    .thumb_func
    .weak PUF_IRQHandler
    .type PUF_IRQHandler, %function
PUF_IRQHandler:
    ldr   r0,=PUF_DriverIRQHandler
    bx    r0
    .size PUF_IRQHandler, . - PUF_IRQHandler

    .align 1
    .thumb_func
    .weak PQ_IRQHandler
    .type PQ_IRQHandler, %function
PQ_IRQHandler:
    ldr   r0,=PQ_DriverIRQHandler
    bx    r0
    .size PQ_IRQHandler, . - PQ_IRQHandler

    .align 1
    .thumb_func
    .weak EDMA_1_CH0_IRQHandler
    .type EDMA_1_CH0_IRQHandler, %function
EDMA_1_CH0_IRQHandler:
    ldr   r0,=EDMA_1_CH0_DriverIRQHandler
    bx    r0
    .size EDMA_1_CH0_IRQHandler, . - EDMA_1_CH0_IRQHandler

    .align 1
    .thumb_func
    .weak EDMA_1_CH1_IRQHandler
    .type EDMA_1_CH1_IRQHandler, %function
EDMA_1_CH1_IRQHandler:
    ldr   r0,=EDMA_1_CH1_DriverIRQHandler
    bx    r0
    .size EDMA_1_CH1_IRQHandler, . - EDMA_1_CH1_IRQHandler

    .align 1
    .thumb_func
    .weak EDMA_1_CH2_IRQHandler
    .type EDMA_1_CH2_IRQHandler, %function
EDMA_1_CH2_IRQHandler:
    ldr   r0,=EDMA_1_CH2_DriverIRQHandler
    bx    r0
    .size EDMA_1_CH2_IRQHandler, . - EDMA_1_CH2_IRQHandler

    .align 1
    .thumb_func
    .weak EDMA_1_CH3_IRQHandler
    .type EDMA_1_CH3_IRQHandler, %function
EDMA_1_CH3_IRQHandler:
    ldr   r0,=EDMA_1_CH3_DriverIRQHandler
    bx    r0
    .size EDMA_1_CH3_IRQHandler, . - EDMA_1_CH3_IRQHandler

    .align 1
    .thumb_func
    .weak EDMA_1_CH4_IRQHandler
    .type EDMA_1_CH4_IRQHandler, %function
EDMA_1_CH4_IRQHandler:
    ldr   r0,=EDMA_1_CH4_DriverIRQHandler
    bx    r0
    .size EDMA_1_CH4_IRQHandler, . - EDMA_1_CH4_IRQHandler

    .align 1
    .thumb_func
    .weak EDMA_1_CH5_IRQHandler
    .type EDMA_1_CH5_IRQHandler, %function
EDMA_1_CH5_IRQHandler:
    ldr   r0,=EDMA_1_CH5_DriverIRQHandler
    bx    r0
    .size EDMA_1_CH5_IRQHandler, . - EDMA_1_CH5_IRQHandler

    .align 1
    .thumb_func
    .weak EDMA_1_CH6_IRQHandler
    .type EDMA_1_CH6_IRQHandler, %function
EDMA_1_CH6_IRQHandler:
    ldr   r0,=EDMA_1_CH6_DriverIRQHandler
    bx    r0
    .size EDMA_1_CH6_IRQHandler, . - EDMA_1_CH6_IRQHandler

    .align 1
    .thumb_func
    .weak EDMA_1_CH7_IRQHandler
    .type EDMA_1_CH7_IRQHandler, %function
EDMA_1_CH7_IRQHandler:
    ldr   r0,=EDMA_1_CH7_DriverIRQHandler
    bx    r0
    .size EDMA_1_CH7_IRQHandler, . - EDMA_1_CH7_IRQHandler

    .align 1
    .thumb_func
    .weak EDMA_1_CH8_IRQHandler
    .type EDMA_1_CH8_IRQHandler, %function
EDMA_1_CH8_IRQHandler:
    ldr   r0,=EDMA_1_CH8_DriverIRQHandler
    bx    r0
    .size EDMA_1_CH8_IRQHandler, . - EDMA_1_CH8_IRQHandler

    .align 1
    .thumb_func
    .weak EDMA_1_CH9_IRQHandler
    .type EDMA_1_CH9_IRQHandler, %function
EDMA_1_CH9_IRQHandler:
    ldr   r0,=EDMA_1_CH9_DriverIRQHandler
    bx    r0
    .size EDMA_1_CH9_IRQHandler, . - EDMA_1_CH9_IRQHandler

    .align 1
    .thumb_func
    .weak EDMA_1_CH10_IRQHandler
    .type EDMA_1_CH10_IRQHandler, %function
EDMA_1_CH10_IRQHandler:
    ldr   r0,=EDMA_1_CH10_DriverIRQHandler
    bx    r0
    .size EDMA_1_CH10_IRQHandler, . - EDMA_1_CH10_IRQHandler

    .align 1
    .thumb_func
    .weak EDMA_1_CH11_IRQHandler
    .type EDMA_1_CH11_IRQHandler, %function
EDMA_1_CH11_IRQHandler:
    ldr   r0,=EDMA_1_CH11_DriverIRQHandler
    bx    r0
    .size EDMA_1_CH11_IRQHandler, . - EDMA_1_CH11_IRQHandler

    .align 1
    .thumb_func
    .weak EDMA_1_CH12_IRQHandler
    .type EDMA_1_CH12_IRQHandler, %function
EDMA_1_CH12_IRQHandler:
    ldr   r0,=EDMA_1_CH12_DriverIRQHandler
    bx    r0
    .size EDMA_1_CH12_IRQHandler, . - EDMA_1_CH12_IRQHandler

    .align 1
    .thumb_func
    .weak EDMA_1_CH13_IRQHandler
    .type EDMA_1_CH13_IRQHandler, %function
EDMA_1_CH13_IRQHandler:
    ldr   r0,=EDMA_1_CH13_DriverIRQHandler
    bx    r0
    .size EDMA_1_CH13_IRQHandler, . - EDMA_1_CH13_IRQHandler

    .align 1
    .thumb_func
    .weak EDMA_1_CH14_IRQHandler
    .type EDMA_1_CH14_IRQHandler, %function
EDMA_1_CH14_IRQHandler:
    ldr   r0,=EDMA_1_CH14_DriverIRQHandler
    bx    r0
    .size EDMA_1_CH14_IRQHandler, . - EDMA_1_CH14_IRQHandler

    .align 1
    .thumb_func
    .weak EDMA_1_CH15_IRQHandler
    .type EDMA_1_CH15_IRQHandler, %function
EDMA_1_CH15_IRQHandler:
    ldr   r0,=EDMA_1_CH15_DriverIRQHandler
    bx    r0
    .size EDMA_1_CH15_IRQHandler, . - EDMA_1_CH15_IRQHandler

    .align 1
    .thumb_func
    .weak CDOG0_IRQHandler
    .type CDOG0_IRQHandler, %function
CDOG0_IRQHandler:
    ldr   r0,=CDOG0_DriverIRQHandler
    bx    r0
    .size CDOG0_IRQHandler, . - CDOG0_IRQHandler

    .align 1
    .thumb_func
    .weak CDOG1_IRQHandler
    .type CDOG1_IRQHandler, %function
CDOG1_IRQHandler:
    ldr   r0,=CDOG1_DriverIRQHandler
    bx    r0
    .size CDOG1_IRQHandler, . - CDOG1_IRQHandler

    .align 1
    .thumb_func
    .weak I3C0_IRQHandler
    .type I3C0_IRQHandler, %function
I3C0_IRQHandler:
    ldr   r0,=I3C0_DriverIRQHandler
    bx    r0
    .size I3C0_IRQHandler, . - I3C0_IRQHandler

    .align 1
    .thumb_func
    .weak I3C1_IRQHandler
    .type I3C1_IRQHandler, %function
I3C1_IRQHandler:
    ldr   r0,=I3C1_DriverIRQHandler
    bx    r0
    .size I3C1_IRQHandler, . - I3C1_IRQHandler

    .align 1
    .thumb_func
    .weak NPU_IRQHandler
    .type NPU_IRQHandler, %function
NPU_IRQHandler:
    ldr   r0,=NPU_DriverIRQHandler
    bx    r0
    .size NPU_IRQHandler, . - NPU_IRQHandler

    .align 1
    .thumb_func
    .weak GDET_IRQHandler
    .type GDET_IRQHandler, %function
GDET_IRQHandler:
    ldr   r0,=GDET_DriverIRQHandler
    bx    r0
    .size GDET_IRQHandler, . - GDET_IRQHandler

    .align 1
    .thumb_func
    .weak VBAT0_IRQHandler
    .type VBAT0_IRQHandler, %function
VBAT0_IRQHandler:
    ldr   r0,=VBAT0_DriverIRQHandler
    bx    r0
    .size VBAT0_IRQHandler, . - VBAT0_IRQHandler

    .align 1
    .thumb_func
    .weak EWM0_IRQHandler
    .type EWM0_IRQHandler, %function
EWM0_IRQHandler:
    ldr   r0,=EWM0_DriverIRQHandler
    bx    r0
    .size EWM0_IRQHandler, . - EWM0_IRQHandler

    .align 1
    .thumb_func
    .weak TSI_END_OF_SCAN_IRQHandler
    .type TSI_END_OF_SCAN_IRQHandler, %function
TSI_END_OF_SCAN_IRQHandler:
    ldr   r0,=TSI_END_OF_SCAN_DriverIRQHandler
    bx    r0
    .size TSI_END_OF_SCAN_IRQHandler, . - TSI_END_OF_SCAN_IRQHandler

    .align 1
    .thumb_func
    .weak TSI_OUT_OF_SCAN_IRQHandler
    .type TSI_OUT_OF_SCAN_IRQHandler, %function
TSI_OUT_OF_SCAN_IRQHandler:
    ldr   r0,=TSI_OUT_OF_SCAN_DriverIRQHandler
    bx    r0
    .size TSI_OUT_OF_SCAN_IRQHandler, . - TSI_OUT_OF_SCAN_IRQHandler

    .align 1
    .thumb_func
    .weak EMVSIM0_IRQHandler
    .type EMVSIM0_IRQHandler, %function
EMVSIM0_IRQHandler:
    ldr   r0,=EMVSIM0_DriverIRQHandler
    bx    r0
    .size EMVSIM0_IRQHandler, . - EMVSIM0_IRQHandler

    .align 1
    .thumb_func
    .weak EMVSIM1_IRQHandler
    .type EMVSIM1_IRQHandler, %function
EMVSIM1_IRQHandler:
    ldr   r0,=EMVSIM1_DriverIRQHandler
    bx    r0
    .size EMVSIM1_IRQHandler, . - EMVSIM1_IRQHandler

    .align 1
    .thumb_func
    .weak FLEXIO_IRQHandler
    .type FLEXIO_IRQHandler, %function
FLEXIO_IRQHandler:
    ldr   r0,=FLEXIO_DriverIRQHandler
    bx    r0
    .size FLEXIO_IRQHandler, . - FLEXIO_IRQHandler

    .align 1
    .thumb_func
    .weak DAC0_IRQHandler
    .type DAC0_IRQHandler, %function
DAC0_IRQHandler:
    ldr   r0,=DAC0_DriverIRQHandler
    bx    r0
    .size DAC0_IRQHandler, . - DAC0_IRQHandler

    .align 1
    .thumb_func
    .weak DAC1_IRQHandler
    .type DAC1_IRQHandler, %function
DAC1_IRQHandler:
    ldr   r0,=DAC1_DriverIRQHandler
    bx    r0
    .size DAC1_IRQHandler, . - DAC1_IRQHandler

    .align 1
    .thumb_func
    .weak DAC2_IRQHandler
    .type DAC2_IRQHandler, %function
DAC2_IRQHandler:
    ldr   r0,=DAC2_DriverIRQHandler
    bx    r0
    .size DAC2_IRQHandler, . - DAC2_IRQHandler

    .align 1
    .thumb_func
    .weak HSCMP0_IRQHandler
    .type HSCMP0_IRQHandler, %function
HSCMP0_IRQHandler:
    ldr   r0,=HSCMP0_DriverIRQHandler
    bx    r0
    .size HSCMP0_IRQHandler, . - HSCMP0_IRQHandler

    .align 1
    .thumb_func
    .weak HSCMP1_IRQHandler
    .type HSCMP1_IRQHandler, %function
HSCMP1_IRQHandler:
    ldr   r0,=HSCMP1_DriverIRQHandler
    bx    r0
    .size HSCMP1_IRQHandler, . - HSCMP1_IRQHandler

    .align 1
    .thumb_func
    .weak HSCMP2_IRQHandler
    .type HSCMP2_IRQHandler, %function
HSCMP2_IRQHandler:
    ldr   r0,=HSCMP2_DriverIRQHandler
    bx    r0
    .size HSCMP2_IRQHandler, . - HSCMP2_IRQHandler

    .align 1
    .thumb_func
    .weak FLEXPWM0_RELOAD_ERROR_IRQHandler
    .type FLEXPWM0_RELOAD_ERROR_IRQHandler, %function
FLEXPWM0_RELOAD_ERROR_IRQHandler:
    ldr   r0,=FLEXPWM0_RELOAD_ERROR_DriverIRQHandler
    bx    r0
    .size FLEXPWM0_RELOAD_ERROR_IRQHandler, . - FLEXPWM0_RELOAD_ERROR_IRQHandler

    .align 1
    .thumb_func
    .weak FLEXPWM0_FAULT_IRQHandler
    .type FLEXPWM0_FAULT_IRQHandler, %function
FLEXPWM0_FAULT_IRQHandler:
    ldr   r0,=FLEXPWM0_FAULT_DriverIRQHandler
    bx    r0
    .size FLEXPWM0_FAULT_IRQHandler, . - FLEXPWM0_FAULT_IRQHandler

    .align 1
    .thumb_func
    .weak FLEXPWM0_SUBMODULE0_IRQHandler
    .type FLEXPWM0_SUBMODULE0_IRQHandler, %function
FLEXPWM0_SUBMODULE0_IRQHandler:
    ldr   r0,=FLEXPWM0_SUBMODULE0_DriverIRQHandler
    bx    r0
    .size FLEXPWM0_SUBMODULE0_IRQHandler, . - FLEXPWM0_SUBMODULE0_IRQHandler

    .align 1
    .thumb_func
    .weak FLEXPWM0_SUBMODULE1_IRQHandler
    .type FLEXPWM0_SUBMODULE1_IRQHandler, %function
FLEXPWM0_SUBMODULE1_IRQHandler:
    ldr   r0,=FLEXPWM0_SUBMODULE1_DriverIRQHandler
    bx    r0
    .size FLEXPWM0_SUBMODULE1_IRQHandler, . - FLEXPWM0_SUBMODULE1_IRQHandler

    .align 1
    .thumb_func
    .weak FLEXPWM0_SUBMODULE2_IRQHandler
    .type FLEXPWM0_SUBMODULE2_IRQHandler, %function
FLEXPWM0_SUBMODULE2_IRQHandler:
    ldr   r0,=FLEXPWM0_SUBMODULE2_DriverIRQHandler
    bx    r0
    .size FLEXPWM0_SUBMODULE2_IRQHandler, . - FLEXPWM0_SUBMODULE2_IRQHandler

    .align 1
    .thumb_func
    .weak FLEXPWM0_SUBMODULE3_IRQHandler
    .type FLEXPWM0_SUBMODULE3_IRQHandler, %function
FLEXPWM0_SUBMODULE3_IRQHandler:
    ldr   r0,=FLEXPWM0_SUBMODULE3_DriverIRQHandler
    bx    r0
    .size FLEXPWM0_SUBMODULE3_IRQHandler, . - FLEXPWM0_SUBMODULE3_IRQHandler

    .align 1
    .thumb_func
    .weak FLEXPWM1_RELOAD_ERROR_IRQHandler
    .type FLEXPWM1_RELOAD_ERROR_IRQHandler, %function
FLEXPWM1_RELOAD_ERROR_IRQHandler:
    ldr   r0,=FLEXPWM1_RELOAD_ERROR_DriverIRQHandler
    bx    r0
    .size FLEXPWM1_RELOAD_ERROR_IRQHandler, . - FLEXPWM1_RELOAD_ERROR_IRQHandler

    .align 1
    .thumb_func
    .weak FLEXPWM1_FAULT_IRQHandler
    .type FLEXPWM1_FAULT_IRQHandler, %function
FLEXPWM1_FAULT_IRQHandler:
    ldr   r0,=FLEXPWM1_FAULT_DriverIRQHandler
    bx    r0
    .size FLEXPWM1_FAULT_IRQHandler, . - FLEXPWM1_FAULT_IRQHandler

    .align 1
    .thumb_func
    .weak FLEXPWM1_SUBMODULE0_IRQHandler
    .type FLEXPWM1_SUBMODULE0_IRQHandler, %function
FLEXPWM1_SUBMODULE0_IRQHandler:
    ldr   r0,=FLEXPWM1_SUBMODULE0_DriverIRQHandler
    bx    r0
    .size FLEXPWM1_SUBMODULE0_IRQHandler, . - FLEXPWM1_SUBMODULE0_IRQHandler

    .align 1
    .thumb_func
    .weak FLEXPWM1_SUBMODULE1_IRQHandler
    .type FLEXPWM1_SUBMODULE1_IRQHandler, %function
FLEXPWM1_SUBMODULE1_IRQHandler:
    ldr   r0,=FLEXPWM1_SUBMODULE1_DriverIRQHandler
    bx    r0
    .size FLEXPWM1_SUBMODULE1_IRQHandler, . - FLEXPWM1_SUBMODULE1_IRQHandler

    .align 1
    .thumb_func
    .weak FLEXPWM1_SUBMODULE2_IRQHandler
    .type FLEXPWM1_SUBMODULE2_IRQHandler, %function
FLEXPWM1_SUBMODULE2_IRQHandler:
    ldr   r0,=FLEXPWM1_SUBMODULE2_DriverIRQHandler
    bx    r0
    .size FLEXPWM1_SUBMODULE2_IRQHandler, . - FLEXPWM1_SUBMODULE2_IRQHandler

    .align 1
    .thumb_func
    .weak FLEXPWM1_SUBMODULE3_IRQHandler
    .type FLEXPWM1_SUBMODULE3_IRQHandler, %function
FLEXPWM1_SUBMODULE3_IRQHandler:
    ldr   r0,=FLEXPWM1_SUBMODULE3_DriverIRQHandler
    bx    r0
    .size FLEXPWM1_SUBMODULE3_IRQHandler, . - FLEXPWM1_SUBMODULE3_IRQHandler

    .align 1
    .thumb_func
    .weak ENC0_COMPARE_IRQHandler
    .type ENC0_COMPARE_IRQHandler, %function
ENC0_COMPARE_IRQHandler:
    ldr   r0,=ENC0_COMPARE_DriverIRQHandler
    bx    r0
    .size ENC0_COMPARE_IRQHandler, . - ENC0_COMPARE_IRQHandler

    .align 1
    .thumb_func
    .weak ENC0_HOME_IRQHandler
    .type ENC0_HOME_IRQHandler, %function
ENC0_HOME_IRQHandler:
    ldr   r0,=ENC0_HOME_DriverIRQHandler
    bx    r0
    .size ENC0_HOME_IRQHandler, . - ENC0_HOME_IRQHandler

    .align 1
    .thumb_func
    .weak ENC0_WDG_SAB_IRQHandler
    .type ENC0_WDG_SAB_IRQHandler, %function
ENC0_WDG_SAB_IRQHandler:
    ldr   r0,=ENC0_WDG_SAB_DriverIRQHandler
    bx    r0
    .size ENC0_WDG_SAB_IRQHandler, . - ENC0_WDG_SAB_IRQHandler

    .align 1
    .thumb_func
    .weak ENC0_IDX_IRQHandler
    .type ENC0_IDX_IRQHandler, %function
ENC0_IDX_IRQHandler:
    ldr   r0,=ENC0_IDX_DriverIRQHandler
    bx    r0
    .size ENC0_IDX_IRQHandler, . - ENC0_IDX_IRQHandler

    .align 1
    .thumb_func
    .weak ENC1_COMPARE_IRQHandler
    .type ENC1_COMPARE_IRQHandler, %function
ENC1_COMPARE_IRQHandler:
    ldr   r0,=ENC1_COMPARE_DriverIRQHandler
    bx    r0
    .size ENC1_COMPARE_IRQHandler, . - ENC1_COMPARE_IRQHandler

    .align 1
    .thumb_func
    .weak ENC1_HOME_IRQHandler
    .type ENC1_HOME_IRQHandler, %function
ENC1_HOME_IRQHandler:
    ldr   r0,=ENC1_HOME_DriverIRQHandler
    bx    r0
    .size ENC1_HOME_IRQHandler, . - ENC1_HOME_IRQHandler

    .align 1
    .thumb_func
    .weak ENC1_WDG_SAB_IRQHandler
    .type ENC1_WDG_SAB_IRQHandler, %function
ENC1_WDG_SAB_IRQHandler:
    ldr   r0,=ENC1_WDG_SAB_DriverIRQHandler
    bx    r0
    .size ENC1_WDG_SAB_IRQHandler, . - ENC1_WDG_SAB_IRQHandler

    .align 1
    .thumb_func
    .weak ENC1_IDX_IRQHandler
    .type ENC1_IDX_IRQHandler, %function
ENC1_IDX_IRQHandler:
    ldr   r0,=ENC1_IDX_DriverIRQHandler
    bx    r0
    .size ENC1_IDX_IRQHandler, . - ENC1_IDX_IRQHandler

    .align 1
    .thumb_func
    .weak ITRC0_IRQHandler
    .type ITRC0_IRQHandler, %function
ITRC0_IRQHandler:
    ldr   r0,=ITRC0_DriverIRQHandler
    bx    r0
    .size ITRC0_IRQHandler, . - ITRC0_IRQHandler

    .align 1
    .thumb_func
    .weak BSP32_IRQHandler
    .type BSP32_IRQHandler, %function
BSP32_IRQHandler:
    ldr   r0,=BSP32_DriverIRQHandler
    bx    r0
    .size BSP32_IRQHandler, . - BSP32_IRQHandler

    .align 1
    .thumb_func
    .weak ELS_ERR_IRQHandler
    .type ELS_ERR_IRQHandler, %function
ELS_ERR_IRQHandler:
    ldr   r0,=ELS_ERR_DriverIRQHandler
    bx    r0
    .size ELS_ERR_IRQHandler, . - ELS_ERR_IRQHandler

    .align 1
    .thumb_func
    .weak PKC_ERR_IRQHandler
    .type PKC_ERR_IRQHandler, %function
PKC_ERR_IRQHandler:
    ldr   r0,=PKC_ERR_DriverIRQHandler
    bx    r0
    .size PKC_ERR_IRQHandler, . - PKC_ERR_IRQHandler

    .align 1
    .thumb_func
    .weak ERM_SINGLE_BIT_ERROR_IRQHandler
    .type ERM_SINGLE_BIT_ERROR_IRQHandler, %function
ERM_SINGLE_BIT_ERROR_IRQHandler:
    ldr   r0,=ERM_SINGLE_BIT_ERROR_DriverIRQHandler
    bx    r0
    .size ERM_SINGLE_BIT_ERROR_IRQHandler, . - ERM_SINGLE_BIT_ERROR_IRQHandler

    .align 1
    .thumb_func
    .weak ERM_MULTI_BIT_ERROR_IRQHandler
    .type ERM_MULTI_BIT_ERROR_IRQHandler, %function
ERM_MULTI_BIT_ERROR_IRQHandler:
    ldr   r0,=ERM_MULTI_BIT_ERROR_DriverIRQHandler
    bx    r0
    .size ERM_MULTI_BIT_ERROR_IRQHandler, . - ERM_MULTI_BIT_ERROR_IRQHandler

    .align 1
    .thumb_func
    .weak FMU0_IRQHandler
    .type FMU0_IRQHandler, %function
FMU0_IRQHandler:
    ldr   r0,=FMU0_DriverIRQHandler
    bx    r0
    .size FMU0_IRQHandler, . - FMU0_IRQHandler

    .align 1
    .thumb_func
    .weak ETHERNET_IRQHandler
    .type ETHERNET_IRQHandler, %function
ETHERNET_IRQHandler:
    ldr   r0,=ETHERNET_DriverIRQHandler
    bx    r0
    .size ETHERNET_IRQHandler, . - ETHERNET_IRQHandler

    .align 1
    .thumb_func
    .weak ETHERNET_PMT_IRQHandler
    .type ETHERNET_PMT_IRQHandler, %function
ETHERNET_PMT_IRQHandler:
    ldr   r0,=ETHERNET_PMT_DriverIRQHandler
    bx    r0
    .size ETHERNET_PMT_IRQHandler, . - ETHERNET_PMT_IRQHandler

    .align 1
    .thumb_func
    .weak ETHERNET_MACLP_IRQHandler
    .type ETHERNET_MACLP_IRQHandler, %function
ETHERNET_MACLP_IRQHandler:
    ldr   r0,=ETHERNET_MACLP_DriverIRQHandler
    bx    r0
    .size ETHERNET_MACLP_IRQHandler, . - ETHERNET_MACLP_IRQHandler

    .align 1
    .thumb_func
    .weak SINC_FILTER_IRQHandler
    .type SINC_FILTER_IRQHandler, %function
SINC_FILTER_IRQHandler:
    ldr   r0,=SINC_FILTER_DriverIRQHandler
    bx    r0
    .size SINC_FILTER_IRQHandler, . - SINC_FILTER_IRQHandler

    .align 1
    .thumb_func
    .weak LPTMR0_IRQHandler
    .type LPTMR0_IRQHandler, %function
LPTMR0_IRQHandler:
    ldr   r0,=LPTMR0_DriverIRQHandler
    bx    r0
    .size LPTMR0_IRQHandler, . - LPTMR0_IRQHandler

    .align 1
    .thumb_func
    .weak LPTMR1_IRQHandler
    .type LPTMR1_IRQHandler, %function
LPTMR1_IRQHandler:
    ldr   r0,=LPTMR1_DriverIRQHandler
    bx    r0
    .size LPTMR1_IRQHandler, . - LPTMR1_IRQHandler

    .align 1
    .thumb_func
    .weak SCG_IRQHandler
    .type SCG_IRQHandler, %function
SCG_IRQHandler:
    ldr   r0,=SCG_DriverIRQHandler
    bx    r0
    .size SCG_IRQHandler, . - SCG_IRQHandler

    .align 1
    .thumb_func
    .weak SPC_IRQHandler
    .type SPC_IRQHandler, %function
SPC_IRQHandler:
    ldr   r0,=SPC_DriverIRQHandler
    bx    r0
    .size SPC_IRQHandler, . - SPC_IRQHandler

    .align 1
    .thumb_func
    .weak WUU_IRQHandler
    .type WUU_IRQHandler, %function
WUU_IRQHandler:
    ldr   r0,=WUU_DriverIRQHandler
    bx    r0
    .size WUU_IRQHandler, . - WUU_IRQHandler

    .align 1
    .thumb_func
    .weak PORT_EFT_IRQHandler
    .type PORT_EFT_IRQHandler, %function
PORT_EFT_IRQHandler:
    ldr   r0,=PORT_EFT_DriverIRQHandler
    bx    r0
    .size PORT_EFT_IRQHandler, . - PORT_EFT_IRQHandler

    .align 1
    .thumb_func
    .weak ETB0_IRQHandler
    .type ETB0_IRQHandler, %function
ETB0_IRQHandler:
    ldr   r0,=ETB0_DriverIRQHandler
    bx    r0
    .size ETB0_IRQHandler, . - ETB0_IRQHandler

    .align 1
    .thumb_func
    .weak SM3_IRQHandler
    .type SM3_IRQHandler, %function
SM3_IRQHandler:
    ldr   r0,=SM3_DriverIRQHandler
    bx    r0
    .size SM3_IRQHandler, . - SM3_IRQHandler

    .align 1
    .thumb_func
    .weak TRNG0_IRQHandler
    .type TRNG0_IRQHandler, %function
TRNG0_IRQHandler:
    ldr   r0,=TRNG0_DriverIRQHandler
    bx    r0
    .size TRNG0_IRQHandler, . - TRNG0_IRQHandler

    .align 1
    .thumb_func
    .weak WWDT0_IRQHandler
    .type WWDT0_IRQHandler, %function
WWDT0_IRQHandler:
    ldr   r0,=WWDT0_DriverIRQHandler
    bx    r0
    .size WWDT0_IRQHandler, . - WWDT0_IRQHandler

    .align 1
    .thumb_func
    .weak WWDT1_IRQHandler
    .type WWDT1_IRQHandler, %function
WWDT1_IRQHandler:
    ldr   r0,=WWDT1_DriverIRQHandler
    bx    r0
    .size WWDT1_IRQHandler, . - WWDT1_IRQHandler

    .align 1
    .thumb_func
    .weak CMC0_IRQHandler
    .type CMC0_IRQHandler, %function
CMC0_IRQHandler:
    ldr   r0,=CMC0_DriverIRQHandler
    bx    r0
    .size CMC0_IRQHandler, . - CMC0_IRQHandler

    .align 1
    .thumb_func
    .weak CTI0_IRQHandler
    .type CTI0_IRQHandler, %function
CTI0_IRQHandler:
    ldr   r0,=CTI0_DriverIRQHandler
    bx    r0
    .size CTI0_IRQHandler, . - CTI0_IRQHandler


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
    def_irq_handler    SecureFault_Handler
    def_irq_handler    DebugMon_Handler
    def_irq_handler    OR_DriverIRQHandler
    def_irq_handler    EDMA_0_CH0_DriverIRQHandler
    def_irq_handler    EDMA_0_CH1_DriverIRQHandler
    def_irq_handler    EDMA_0_CH2_DriverIRQHandler
    def_irq_handler    EDMA_0_CH3_DriverIRQHandler
    def_irq_handler    EDMA_0_CH4_DriverIRQHandler
    def_irq_handler    EDMA_0_CH5_DriverIRQHandler
    def_irq_handler    EDMA_0_CH6_DriverIRQHandler
    def_irq_handler    EDMA_0_CH7_DriverIRQHandler
    def_irq_handler    EDMA_0_CH8_DriverIRQHandler
    def_irq_handler    EDMA_0_CH9_DriverIRQHandler
    def_irq_handler    EDMA_0_CH10_DriverIRQHandler
    def_irq_handler    EDMA_0_CH11_DriverIRQHandler
    def_irq_handler    EDMA_0_CH12_DriverIRQHandler
    def_irq_handler    EDMA_0_CH13_DriverIRQHandler
    def_irq_handler    EDMA_0_CH14_DriverIRQHandler
    def_irq_handler    EDMA_0_CH15_DriverIRQHandler
    def_irq_handler    GPIO00_DriverIRQHandler
    def_irq_handler    GPIO01_DriverIRQHandler
    def_irq_handler    GPIO10_DriverIRQHandler
    def_irq_handler    GPIO11_DriverIRQHandler
    def_irq_handler    GPIO20_DriverIRQHandler
    def_irq_handler    GPIO21_DriverIRQHandler
    def_irq_handler    GPIO30_DriverIRQHandler
    def_irq_handler    GPIO31_DriverIRQHandler
    def_irq_handler    GPIO40_DriverIRQHandler
    def_irq_handler    GPIO41_DriverIRQHandler
    def_irq_handler    GPIO50_DriverIRQHandler
    def_irq_handler    GPIO51_DriverIRQHandler
    def_irq_handler    UTICK0_DriverIRQHandler
    def_irq_handler    MRT0_DriverIRQHandler
    def_irq_handler    CTIMER0_DriverIRQHandler
    def_irq_handler    CTIMER1_DriverIRQHandler
    def_irq_handler    SCT0_DriverIRQHandler
    def_irq_handler    CTIMER2_DriverIRQHandler
    def_irq_handler    LP_FLEXCOMM0_DriverIRQHandler
    def_irq_handler    LP_FLEXCOMM1_DriverIRQHandler
    def_irq_handler    LP_FLEXCOMM2_DriverIRQHandler
    def_irq_handler    LP_FLEXCOMM3_DriverIRQHandler
    def_irq_handler    LP_FLEXCOMM4_DriverIRQHandler
    def_irq_handler    LP_FLEXCOMM5_DriverIRQHandler
    def_irq_handler    LP_FLEXCOMM6_DriverIRQHandler
    def_irq_handler    LP_FLEXCOMM7_DriverIRQHandler
    def_irq_handler    LP_FLEXCOMM8_DriverIRQHandler
    def_irq_handler    LP_FLEXCOMM9_DriverIRQHandler
    def_irq_handler    ADC0_DriverIRQHandler
    def_irq_handler    ADC1_DriverIRQHandler
    def_irq_handler    PINT0_DriverIRQHandler
    def_irq_handler    PDM_EVENT_DriverIRQHandler
    def_irq_handler    Reserved65_DriverIRQHandler
    def_irq_handler    USB0_FS_DriverIRQHandler
    def_irq_handler    USB0_DCD_DriverIRQHandler
    def_irq_handler    RTC_DriverIRQHandler
    def_irq_handler    SMARTDMA_DriverIRQHandler
    def_irq_handler    MAILBOX_DriverIRQHandler
    def_irq_handler    CTIMER3_DriverIRQHandler
    def_irq_handler    CTIMER4_DriverIRQHandler
    def_irq_handler    OS_EVENT_DriverIRQHandler
    def_irq_handler    FLEXSPI0_DriverIRQHandler
    def_irq_handler    SAI0_DriverIRQHandler
    def_irq_handler    SAI1_DriverIRQHandler
    def_irq_handler    USDHC0_DriverIRQHandler
    def_irq_handler    CAN0_DriverIRQHandler
    def_irq_handler    CAN1_DriverIRQHandler
    def_irq_handler    Reserved80_DriverIRQHandler
    def_irq_handler    Reserved81_DriverIRQHandler
    def_irq_handler    USB1_HS_PHY_DriverIRQHandler
    def_irq_handler    USB1_HS_DriverIRQHandler
    def_irq_handler    SEC_HYPERVISOR_CALL_DriverIRQHandler
    def_irq_handler    Reserved85_DriverIRQHandler
    def_irq_handler    PLU_DriverIRQHandler
    def_irq_handler    Freqme_DriverIRQHandler
    def_irq_handler    SEC_VIO_DriverIRQHandler
    def_irq_handler    ELS_DriverIRQHandler
    def_irq_handler    PKC_DriverIRQHandler
    def_irq_handler    PUF_DriverIRQHandler
    def_irq_handler    PQ_DriverIRQHandler
    def_irq_handler    EDMA_1_CH0_DriverIRQHandler
    def_irq_handler    EDMA_1_CH1_DriverIRQHandler
    def_irq_handler    EDMA_1_CH2_DriverIRQHandler
    def_irq_handler    EDMA_1_CH3_DriverIRQHandler
    def_irq_handler    EDMA_1_CH4_DriverIRQHandler
    def_irq_handler    EDMA_1_CH5_DriverIRQHandler
    def_irq_handler    EDMA_1_CH6_DriverIRQHandler
    def_irq_handler    EDMA_1_CH7_DriverIRQHandler
    def_irq_handler    EDMA_1_CH8_DriverIRQHandler
    def_irq_handler    EDMA_1_CH9_DriverIRQHandler
    def_irq_handler    EDMA_1_CH10_DriverIRQHandler
    def_irq_handler    EDMA_1_CH11_DriverIRQHandler
    def_irq_handler    EDMA_1_CH12_DriverIRQHandler
    def_irq_handler    EDMA_1_CH13_DriverIRQHandler
    def_irq_handler    EDMA_1_CH14_DriverIRQHandler
    def_irq_handler    EDMA_1_CH15_DriverIRQHandler
    def_irq_handler    CDOG0_DriverIRQHandler
    def_irq_handler    CDOG1_DriverIRQHandler
    def_irq_handler    I3C0_DriverIRQHandler
    def_irq_handler    I3C1_DriverIRQHandler
    def_irq_handler    NPU_DriverIRQHandler
    def_irq_handler    GDET_DriverIRQHandler
    def_irq_handler    VBAT0_DriverIRQHandler
    def_irq_handler    EWM0_DriverIRQHandler
    def_irq_handler    TSI_END_OF_SCAN_DriverIRQHandler
    def_irq_handler    TSI_OUT_OF_SCAN_DriverIRQHandler
    def_irq_handler    EMVSIM0_DriverIRQHandler
    def_irq_handler    EMVSIM1_DriverIRQHandler
    def_irq_handler    FLEXIO_DriverIRQHandler
    def_irq_handler    DAC0_DriverIRQHandler
    def_irq_handler    DAC1_DriverIRQHandler
    def_irq_handler    DAC2_DriverIRQHandler
    def_irq_handler    HSCMP0_DriverIRQHandler
    def_irq_handler    HSCMP1_DriverIRQHandler
    def_irq_handler    HSCMP2_DriverIRQHandler
    def_irq_handler    FLEXPWM0_RELOAD_ERROR_DriverIRQHandler
    def_irq_handler    FLEXPWM0_FAULT_DriverIRQHandler
    def_irq_handler    FLEXPWM0_SUBMODULE0_DriverIRQHandler
    def_irq_handler    FLEXPWM0_SUBMODULE1_DriverIRQHandler
    def_irq_handler    FLEXPWM0_SUBMODULE2_DriverIRQHandler
    def_irq_handler    FLEXPWM0_SUBMODULE3_DriverIRQHandler
    def_irq_handler    FLEXPWM1_RELOAD_ERROR_DriverIRQHandler
    def_irq_handler    FLEXPWM1_FAULT_DriverIRQHandler
    def_irq_handler    FLEXPWM1_SUBMODULE0_DriverIRQHandler
    def_irq_handler    FLEXPWM1_SUBMODULE1_DriverIRQHandler
    def_irq_handler    FLEXPWM1_SUBMODULE2_DriverIRQHandler
    def_irq_handler    FLEXPWM1_SUBMODULE3_DriverIRQHandler
    def_irq_handler    ENC0_COMPARE_DriverIRQHandler
    def_irq_handler    ENC0_HOME_DriverIRQHandler
    def_irq_handler    ENC0_WDG_SAB_DriverIRQHandler
    def_irq_handler    ENC0_IDX_DriverIRQHandler
    def_irq_handler    ENC1_COMPARE_DriverIRQHandler
    def_irq_handler    ENC1_HOME_DriverIRQHandler
    def_irq_handler    ENC1_WDG_SAB_DriverIRQHandler
    def_irq_handler    ENC1_IDX_DriverIRQHandler
    def_irq_handler    ITRC0_DriverIRQHandler
    def_irq_handler    BSP32_DriverIRQHandler
    def_irq_handler    ELS_ERR_DriverIRQHandler
    def_irq_handler    PKC_ERR_DriverIRQHandler
    def_irq_handler    ERM_SINGLE_BIT_ERROR_DriverIRQHandler
    def_irq_handler    ERM_MULTI_BIT_ERROR_DriverIRQHandler
    def_irq_handler    FMU0_DriverIRQHandler
    def_irq_handler    ETHERNET_DriverIRQHandler
    def_irq_handler    ETHERNET_PMT_DriverIRQHandler
    def_irq_handler    ETHERNET_MACLP_DriverIRQHandler
    def_irq_handler    SINC_FILTER_DriverIRQHandler
    def_irq_handler    LPTMR0_DriverIRQHandler
    def_irq_handler    LPTMR1_DriverIRQHandler
    def_irq_handler    SCG_DriverIRQHandler
    def_irq_handler    SPC_DriverIRQHandler
    def_irq_handler    WUU_DriverIRQHandler
    def_irq_handler    PORT_EFT_DriverIRQHandler
    def_irq_handler    ETB0_DriverIRQHandler
    def_irq_handler    SM3_DriverIRQHandler
    def_irq_handler    TRNG0_DriverIRQHandler
    def_irq_handler    WWDT0_DriverIRQHandler
    def_irq_handler    WWDT1_DriverIRQHandler
    def_irq_handler    CMC0_DriverIRQHandler
    def_irq_handler    CTI0_DriverIRQHandler

    .end
