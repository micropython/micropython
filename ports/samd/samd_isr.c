/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
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

#include "py/runtime.h"
#include "py/mphal.h"
#include "samd_soc.h"

#include "pendsv.h"
#include "shared/runtime/softtimer.h"

typedef void (*ISR)(void);

extern uint32_t _estack, _sidata, _sdata, _edata, _sbss, _ebss;
extern void Default_Handler(void);
extern void SysTick_Handler(void);
extern void PendSV_Handler(void);
extern void EIC_Handler(void);

const ISR isr_vector[];
volatile uint32_t systick_ms;
volatile uint32_t ticks_us64_upper;

void Reset_Handler(void) __attribute__((naked));
void Reset_Handler(void) {
    // Set stack pointer
    #if __CORTEX_M >= 0x03
    __asm volatile ("ldr sp, =_estack");
    #else
    __asm volatile ("ldr r0, =_estack");
    __asm volatile ("mov sp, r0");
    #endif
    // Copy .data section from flash to RAM
    for (uint32_t *src = &_sidata, *dest = &_sdata; dest < &_edata;) {
        *dest++ = *src++;
    }
    // Zero out .bss section
    for (uint32_t *dest = &_sbss; dest < &_ebss;) {
        *dest++ = 0;
    }

    // When we get here: stack is initialised, bss is clear, data is copied

    #if __FPU_PRESENT == 1 && __FPU_USED == 1
    // Set CP10 and CP11 Full Access
    SCB->CPACR |= (3UL << 10 * 2) | (3UL << 11 * 2);
    #endif

    // SCB->VTOR
    *((volatile uint32_t *)0xe000ed08) = (uint32_t)&isr_vector;

    // SCB->CCR: enable 8-byte stack alignment for IRQ handlers, in accord with EABI
    *((volatile uint32_t *)0xe000ed14) |= 1 << 9;

    // Initialise the cpu and peripherals
    samd_init();

    // Now that we have a basic system up and running we can call main
    samd_main();

    // we must not return
    for (;;) {
    }
}

void Default_Handler(void) {
    for (;;) {
    }
}

void SysTick_Handler(void) {
    uint32_t next_tick = systick_ms + 1;
    systick_ms = next_tick;

    if (soft_timer_next == next_tick) {
        pendsv_schedule_dispatch(PENDSV_DISPATCH_SOFT_TIMER, soft_timer_handler);
    }
}

void us_timer_IRQ(void) {
    #if defined(MCU_SAMD21)
    if (TC4->COUNT32.INTFLAG.reg & TC_INTFLAG_OVF) {
        ticks_us64_upper++;
    }
    TC4->COUNT32.INTFLAG.reg = TC_INTFLAG_OVF;
    #elif defined(MCU_SAMD51)
    if (TC0->COUNT32.INTFLAG.reg & TC_INTFLAG_OVF) {
        ticks_us64_upper++;
    }
    TC0->COUNT32.INTFLAG.reg = TC_INTFLAG_OVF;
    #endif
}

// Sercom IRQ handler support
void (*sercom_irq_handler_table[SERCOM_INST_NUM])(int num) = {};

void sercom_register_irq(int sercom_id, void (*sercom_irq_handler)) {
    if (sercom_id < SERCOM_INST_NUM) {
        sercom_irq_handler_table[sercom_id] = sercom_irq_handler;
    }
}

static inline void common_sercom_irq_handler(int sercom_id) {
    if (sercom_irq_handler_table[sercom_id]) {
        sercom_irq_handler_table[sercom_id](sercom_id);
    }
}

void Sercom0_Handler(void) {
    common_sercom_irq_handler(0);
}
void Sercom1_Handler(void) {
    common_sercom_irq_handler(1);
}
void Sercom2_Handler(void) {
    common_sercom_irq_handler(2);
}
void Sercom3_Handler(void) {
    common_sercom_irq_handler(3);
}
void Sercom4_Handler(void) {
    common_sercom_irq_handler(4);
}
void Sercom5_Handler(void) {
    common_sercom_irq_handler(5);
}
#if defined(MCU_SAMD51)
void Sercom6_Handler(void) {
    common_sercom_irq_handler(6);
}
void Sercom7_Handler(void) {
    common_sercom_irq_handler(7);
}
#endif

#if defined(MCU_SAMD21)
const ISR isr_vector[] __attribute__((section(".isr_vector"))) = {
    (ISR)&_estack,
    &Reset_Handler,
    &Default_Handler,   // NMI_Handler
    &Default_Handler,   // HardFault_Handler
    &Default_Handler,   // MemManage_Handler
    &Default_Handler,   // BusFault_Handler
    &Default_Handler,   // UsageFault_Handler
    0,
    0,
    0,
    0,
    &Default_Handler,   //  SVC_Handler
    &Default_Handler,   //  DebugMon_Handler
    0,
    &PendSV_Handler,    //  PendSV_Handler
    &SysTick_Handler,   //  SysTick_Handler
    0,                  //  0 Power Manager (PM)
    0,                  //  1 System Control (SYSCTRL)
    0,                  //  2 Watchdog Timer (WDT)
    0,                  //  3 Real-Time Counter (RTC)
    &EIC_Handler,       //  4 External Interrupt Controller (EIC)
    0,                  //  5 Non-Volatile Memory Controller (NVMCTRL)
    0,                  //  6 Direct Memory Access Controller (DMAC)
    USB_Handler_wrapper,//  7 Universal Serial Bus (USB)
    0,                  //  8 Event System Interface (EVSYS)
    &Sercom0_Handler,   //  9 Serial Communication Interface 0 (SERCOM0)
    &Sercom1_Handler,   // 10 SAMD21G18A Serial Communication Interface 1 (SERCOM1)
    &Sercom2_Handler,   // 11 SAMD21G18A Serial Communication Interface 2 (SERCOM2)
    &Sercom3_Handler,   // 12 SAMD21G18A Serial Communication Interface 3 (SERCOM3)
    &Sercom4_Handler,   // 13 SAMD21G18A Serial Communication Interface 4 (SERCOM4)
    &Sercom5_Handler,   // 14 SAMD21G18A Serial Communication Interface 5 (SERCOM5)
    0,                  // 15 Timer Counter Control 0 (TCC0)
    0,                  // 16 Timer Counter Control 1 (TCC1)
    0,                  // 17 Timer Counter Control 2 (TCC2)
    0,                  // 18 Basic Timer Counter 3 (TC3)
    &us_timer_IRQ,      // 19 Basic Timer Counter 4 (TC4)
    0,                  // 20 Basic Timer Counter 5 (TC5)
    0,                  // 21 Basic Timer Counter 6 (TC6)
    0,                  // 22 Basic Timer Counter 7 (TC7)
    0,                  // 23 Analog Digital Converter (ADC)
    0,                  // 24 Analog Comparators (AC)
    0,                  // 25 Digital Analog Converter (DAC)
    0,                  // 26 Peripheral Touch Controller (PTC)
    0,                  // 27 Inter-IC Sound Interface (I2S)

};
#else
const ISR isr_vector[] __attribute__((section(".isr_vector"))) = {
    (ISR)&_estack,
    &Reset_Handler,
    &Default_Handler, // NMI_Handler
    &Default_Handler, // HardFault_Handler
    &Default_Handler, // MemManage_Handler
    &Default_Handler, // BusFault_Handler
    &Default_Handler, // UsageFault_Handler
    0,
    0,
    0,
    0,
    &Default_Handler, // SVC_Handler
    &Default_Handler, // DebugMon_Handler
    0,
    &PendSV_Handler,  //  PendSV_Handler
    &SysTick_Handler, // SysTick_Handler
    0,                //  0 Power Manager (PM)
    0,                //  1 Main Clock (MCLK)
    0,                //  2 Oscillators Control (OSCCTRL): OSCCTRL_XOSCFAIL_0, OSCCTRL_XOSCRDY_0
    0,                //  3 Oscillators Control (OSCCTRL): OSCCTRL_XOSCFAIL_1, OSCCTRL_XOSCRDY_1
    0,                //  4 Oscillators Control (OSCCTRL): OSCCTRL_DFLLLOCKC, OSCCTRL_DFLLLOCKF, OSCCTRL_DFLLOOB, OSCCTRL_DFLLRCS, OSCCTRL_DFLLRDY
    0,                //  5 Oscillators Control (OSCCTRL): OSCCTRL_DPLLLCKF_0, OSCCTRL_DPLLLCKR_0, OSCCTRL_DPLLLDRTO_0, OSCCTRL_DPLLLTO_0
    0,                //  6 Oscillators Control (OSCCTRL): OSCCTRL_DPLLLCKF_1, OSCCTRL_DPLLLCKR_1, OSCCTRL_DPLLLDRTO_1, OSCCTRL_DPLLLTO_1
    0,                //  7 32kHz Oscillators Control (OSC32KCTRL)
    0,                //  8 Supply Controller (SUPC): SUPC_B12SRDY, SUPC_B33SRDY, SUP
    0,                //  9 Supply Controller (SUPC): SUPC_BOD12DET, SUPC_BOD33DET
    0,                // 10 Watchdog Timer (WDT)
    0,                // 11 Real-Time Counter (RTC)
    &EIC_Handler,     // 12 External Interrupt Controller (EIC): EIC_EXTINT_0
    &EIC_Handler,     // 13 External Interrupt Controller (EIC): EIC_EXTINT_1
    &EIC_Handler,     // 14 External Interrupt Controller (EIC): EIC_EXTINT_2
    &EIC_Handler,     // 15 External Interrupt Controller (EIC): EIC_EXTINT_3
    &EIC_Handler,     // 16 External Interrupt Controller (EIC): EIC_EXTINT_4
    &EIC_Handler,     // 17 External Interrupt Controller (EIC): EIC_EXTINT_5
    &EIC_Handler,     // 18 External Interrupt Controller (EIC): EIC_EXTINT_6
    &EIC_Handler,     // 19 External Interrupt Controller (EIC): EIC_EXTINT_7
    &EIC_Handler,     // 20 External Interrupt Controller (EIC): EIC_EXTINT_8
    &EIC_Handler,     // 21 External Interrupt Controller (EIC): EIC_EXTINT_9
    &EIC_Handler,     // 22 External Interrupt Controller (EIC): EIC_EXTINT_10
    &EIC_Handler,     // 23 External Interrupt Controller (EIC): EIC_EXTINT_11
    &EIC_Handler,     // 24 External Interrupt Controller (EIC): EIC_EXTINT_12
    &EIC_Handler,     // 25 External Interrupt Controller (EIC): EIC_EXTINT_13
    &EIC_Handler,     // 26 External Interrupt Controller (EIC): EIC_EXTINT_14
    &EIC_Handler,     // 27 External Interrupt Controller (EIC): EIC_EXTINT_15
    0,                // 28 Frequency Meter (FREQM)
    0,                // 29 Non-Volatile Memory Controller (NVMCTRL): NVMCTRL_0 - _7
    0,                // 30 Non-Volatile Memory Controller (NVMCTRL): NVMCTRL_8 - _10
    0,                // 31 Direct Memory Access Controller (DMAC): DMAC_SUSP_0, DMAC_TCMPL_0, DMAC_TERR_0
    0,                // 32 Direct Memory Access Controller (DMAC): DMAC_SUSP_1, DMAC_TCMPL_1, DMAC_TERR_1
    0,                // 33 Direct Memory Access Controller (DMAC): DMAC_SUSP_2, DMAC_TCMPL_2, DMAC_TERR_2
    0,                // 34 Direct Memory Access Controller (DMAC): DMAC_SUSP_3, DMAC_TCMPL_3, DMAC_TERR_3
    0,                // 35 Direct Memory Access Controller (DMAC): DMAC_SUSP_4 - _31, DMAC_TCMPL_4 _31, DMAC_TERR_4- _31
    0,                // 36 Event System Interface (EVSYS): EVSYS_EVD_0, EVSYS_OVR_0
    0,                // 37 Event System Interface (EVSYS): EVSYS_EVD_1, EVSYS_OVR_1
    0,                // 38 Event System Interface (EVSYS): EVSYS_EVD_2, EVSYS_OVR_2
    0,                // 39 Event System Interface (EVSYS): EVSYS_EVD_3, EVSYS_OVR_3
    0,                // 40 Event System Interface (EVSYS): EVSYS_EVD_10, EVSYS_EVD_11
    0,                // 41 Peripheral Access Controller (PAC)
    0,                // 42 Trigger Allocator (TAL): TAL_BRK
    0,                // 43 Trigger Allocator (TAL): TAL_IPS_x
    0,
    0,                // 45 RAM ECC (RAMECC)
    &Sercom0_Handler, // 46 Serial Communication Interface 0 (SERCOM0): SERCOM0_0
    &Sercom0_Handler, // 47 Serial Communication Interface 0 (SERCOM0): SERCOM0_1
    &Sercom0_Handler, // 48 Serial Communication Interface 0 (SERCOM0): SERCOM0_2
    &Sercom0_Handler, // 49 Serial Communication Interface 0 (SERCOM0): SERCOM0_3 - 6
    &Sercom1_Handler, // 50 Serial Communication Interface 1 (SERCOM1): SERCOM1_0
    &Sercom1_Handler, // 51 Serial Communication Interface 1 (SERCOM1): SERCOM1_1
    &Sercom1_Handler, // 52 Serial Communication Interface 1 (SERCOM1): SERCOM1_2
    &Sercom1_Handler, // 53 Serial Communication Interface 1 (SERCOM1): SERCOM1_3 - 6
    &Sercom2_Handler, // 54 Serial Communication Interface 2 (SERCOM2): SERCOM2_0
    &Sercom2_Handler, // 55 Serial Communication Interface 2 (SERCOM2): SERCOM2_1
    &Sercom2_Handler, // 56 Serial Communication Interface 2 (SERCOM2): SERCOM2_2
    &Sercom2_Handler, // 57 Serial Communication Interface 2 (SERCOM2): SERCOM2_3 - 6
    &Sercom3_Handler, // 58 Serial Communication Interface 3 (SERCOM3): SERCOM3_0
    &Sercom3_Handler, // 59 Serial Communication Interface 3 (SERCOM3): SERCOM3_1
    &Sercom3_Handler, // 60 Serial Communication Interface 3 (SERCOM3): SERCOM3_2
    &Sercom3_Handler, // 61 Serial Communication Interface 3 (SERCOM3): SERCOM3_3 - 6
    &Sercom4_Handler, // 62 Serial Communication Interface 4 (SERCOM4): SERCOM4_0
    &Sercom4_Handler, // 63 Serial Communication Interface 4 (SERCOM4): SERCOM4_1
    &Sercom4_Handler, // 64 Serial Communication Interface 4 (SERCOM4): SERCOM4_2
    &Sercom4_Handler, // 65 Serial Communication Interface 4 (SERCOM4): SERCOM4_3 - 6
    &Sercom5_Handler, // 66 Serial Communication Interface 5 (SERCOM5): SERCOM5_0
    &Sercom5_Handler, // 67 Serial Communication Interface 5 (SERCOM5): SERCOM5_1
    &Sercom5_Handler, // 68 Serial Communication Interface 5 (SERCOM5): SERCOM5_2
    &Sercom5_Handler, // 69 Serial Communication Interface 5 (SERCOM5): SERCOM5_3 - 6
    &Sercom6_Handler, // 70 Serial Communication Interface 6 (SERCOM6): SERCOM6_0
    &Sercom6_Handler, // 71 Serial Communication Interface 6 (SERCOM6): SERCOM6_1
    &Sercom6_Handler, // 72 Serial Communication Interface 6 (SERCOM6): SERCOM6_2
    &Sercom6_Handler, // 73 Serial Communication Interface 6 (SERCOM6): SERCOM6_3 - 6
    &Sercom7_Handler, // 74 Serial Communication Interface 7 (SERCOM7): SERCOM7_0
    &Sercom7_Handler, // 75 Serial Communication Interface 7 (SERCOM7): SERCOM7_1
    &Sercom7_Handler, // 76 Serial Communication Interface 7 (SERCOM7): SERCOM7_2
    &Sercom7_Handler, // 77 Serial Communication Interface 7 (SERCOM7): SERCOM7_3 - 6
    0,                // 78 Control Area Network 0 (CAN0)
    0,                // 79 Control Area Network 1 (CAN1)
    &USB_0_Handler_wrapper, // 80 Universal Serial Bus (USB): USB_EORSM_DNRS, ...
    &USB_1_Handler_wrapper, // 81 Universal Serial Bus (USB): USB_SOF_HSOF
    &USB_2_Handler_wrapper, // 82 Universal Serial Bus (USB): USB_TRCPT0_0 - _7
    &USB_3_Handler_wrapper, // 83 Universal Serial Bus (USB): USB_TRCPT1_0 - _7
    0,                // 84 Ethernet MAC (GMAC)
    0,                // 85 Timer Counter Control 0 (TCC0): TCC0_CNT_A ...
    0,                // 86 Timer Counter Control 0 (TCC0): TCC0_MC_0
    0,                // 87 Timer Counter Control 0 (TCC0): TCC0_MC_1
    0,                // 88 Timer Counter Control 0 (TCC0): TCC0_MC_2
    0,                // 89 Timer Counter Control 0 (TCC0): TCC0_MC_3
    0,                // 90 Timer Counter Control 0 (TCC0): TCC0_MC_4
    0,                // 91 Timer Counter Control 0 (TCC0): TCC0_MC_5
    0,                // 92 Timer Counter Control 1 (TCC1): TCC1_CNT_A ...
    0,                // 93 Timer Counter Control 1 (TCC1): TCC1_MC_0
    0,                // 94 Timer Counter Control 1 (TCC1): TCC1_MC_1
    0,                // 95 Timer Counter Control 1 (TCC1): TCC1_MC_2
    0,                // 96 Timer Counter Control 1 (TCC1): TCC1_MC_3
    0,                // 97 Timer Counter Control 2 (TCC2): TCC2_CNT_A ...
    0,                // 98 Timer Counter Control 2 (TCC2): TCC2_MC_0
    0,                // 99 Timer Counter Control 2 (TCC2): TCC2_MC_1
    0,                // 100 Timer Counter Control 2 (TCC2): TCC2_MC_2
    0,                // 101 Timer Counter Control 3 (TCC3): TCC3_CNT_A ...
    0,                // 102 Timer Counter Control 3 (TCC3): TCC3_MC_0
    0,                // 103 Timer Counter Control 3 (TCC3): TCC3_MC_1
    0,                // 104 Timer Counter Control 4 (TCC4): TCC4_CNT_A ...
    0,                // 105 Timer Counter Control 4 (TCC4): TCC4_MC_0
    0,                // 106 Timer Counter Control 4 (TCC4): TCC4_MC_1
    &us_timer_IRQ,    // 107 Basic Timer Counter 0 (TC0)
    0,                // 108 Basic Timer Counter 1 (TC1)
    0,                // 109 Basic Timer Counter 2 (TC2)
    0,                // 110 Basic Timer Counter 3 (TC3)
    0,                // 111 Basic Timer Counter 4 (TC4)
    0,                // 112 Basic Timer Counter 5 (TC5)
    0,                // 113 Basic Timer Counter 6 (TC6)
    0,                // 114 Basic Timer Counter 7 (TC7)
    0,                // 115 Quadrature Decoder (PDEC): PDEC_DIR_A, PDEC_ERR_A, PDEC_OVF, PDEC_VLC_A
    0,                // 116 Quadrature Decoder (PDEC): PDEC_MC_0
    0,                // 117 Quadrature Decoder (PDEC): PDEC_MC_1
    0,                // 118 Analog Digital Converter 0 (ADC0): ADC0_OVERRUN, ADC0_WINMON
    0,                // 119 Analog Digital Converter 0 (ADC0): ADC0_RESRDY
    0,                // 120 Analog Digital Converter 1 (ADC1): ADC1_OVERRUN, ADC1_WINMON
    0,                // 121 Analog Digital Converter 1 (ADC1): ADC1_RESRDY
    0,                // 122 Analog Comparators (AC)
    0,                // 123 Digital-to-Analog Converter (DAC): DAC_OVERRUN_A_x, DAC_UNDERRUN_A_x
    0,                // 124 Digital-to-Analog Converter (DAC): DAC_EMPTY_0
    0,                // 125 Digital-to-Analog Converter (DAC): DAC_EMPTY_1
    0,                // 126 Digital-to-Analog Converter (DAC): DAC_RESRDY_0
    0,                // 127 Digital-to-Analog Converter (DAC): DAC_RESRDY_1
    0,                // 128 Inter-IC Sound Interface (I2S)
    0,                // 129 Parallel Capture Controller (PCC)
    0,                // 130 Advanced Encryption Standard (AES)
    0,                // 131 True Random Generator (TRNG)
    0,                // 132 Integrity Check Monitor (ICM)
    0,                // 133 PUblic-Key Cryptography Controller (PUKCC)
    0,                // 134 Quad SPI interface (QSPI)
    0,                // 135 SD/MMC Host Controller 0 (SDHC0)
    0,                // 136 SD/MMC Host Controller 1 (SDHC1)
};
#endif
