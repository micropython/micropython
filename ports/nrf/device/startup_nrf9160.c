/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Glenn Ruben Bakke
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

#include <stdint.h>

extern uint32_t _estack;
extern uint32_t _sidata;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;

typedef void (*func)(void);

extern void  _start(void) __attribute__((noreturn));
extern void SystemInit(void);

void Default_Handler(void) { while (1); }

void Reserved_Handler1(void) { while (1); }
void Reserved_Handler2(void) { while (1); }
void Reserved_Handler3(void) { while (1); }
void Reserved_Handler4(void) { while (1); }
void Reserved_Handler5(void) { while (1); }
void Reserved_Handler6(void) { while (1); }
void Reserved_Handler7(void) { while (1); }
void Reserved_Handler8(void) { while (1); }
void Reserved_Handler9(void) { while (1); }
void Reserved_Handler10(void) { while (1); }
void Reserved_Handler11(void) { while (1); }
void Reserved_Handler12(void) { while (1); }
void Reserved_Handler13(void) { while (1); }
void Reserved_Handler14(void) { while (1); }
void Reserved_Handler15(void) { while (1); }
void Reserved_Handler16(void) { while (1); }
void Reserved_Handler17(void) { while (1); }
void Reserved_Handler18(void) { while (1); }
void Reserved_Handler19(void) { while (1); }
void Reserved_Handler20(void) { while (1); }
void Reserved_Handler21(void) { while (1); }
void Reserved_Handler22(void) { while (1); }
void Reserved_Handler23(void) { while (1); }
void Reserved_Handler24(void) { while (1); }
void Reserved_Handler25(void) { while (1); }
void Reserved_Handler26(void) { while (1); }
void Reserved_Handler27(void) { while (1); }
void Reserved_Handler28(void) { while (1); }
void Reserved_Handler29(void) { while (1); }
void Reserved_Handler30(void) { while (1); }
void Reserved_Handler31(void) { while (1); }
void Reserved_Handler32(void) { while (1); }
void Reserved_Handler33(void) { while (1); }
void Reserved_Handler34(void) { while (1); }
void Reserved_Handler35(void) { while (1); }
void Reserved_Handler36(void) { while (1); }
void Reserved_Handler37(void) { while (1); }
void Reserved_Handler38(void) { while (1); }

void Default_NMI_Handler                 (void) { while (1); }
void Default_HardFault_Handler           (void) { while (1); }
void Default_MemoryManagement_Handler    (void) { while (1); }
void Default_BusFault_Handler            (void) { while (1); }
void Default_UsageFault_Handler          (void) { while (1); }
void Default_SecureFault_Handler         (void) { while (1); }
void Default_SVC_Handler                 (void) { while (1); }
void Default_DebugMon_Handler            (void) { while (1); }
void Default_PendSV_Handler              (void) { while (1); }
void Default_SysTick_Handler             (void) { while (1); }

void Default_SPU_IRQHandler              (void) { while (1); }
void Default_CLOCK_POWER_IRQHandler      (void) { while (1); }
void Default_UARTE0_SPIM0_SPIS0_TWIM0_TWIS0_IRQHandler (void) { while (1); }
void Default_UARTE1_SPIM1_SPIS1_TWIM1_TWIS1_IRQHandler (void) { while (1); }
void Default_UARTE2_SPIM2_SPIS2_TWIM2_TWIS2_IRQHandler (void) { while (1); }
void Default_UARTE3_SPIM3_SPIS3_TWIM3_TWIS3_IRQHandler (void) { while (1); }
void Default_GPIOTE0_IRQHandler          (void) { while (1); }
void Default_SAADC_IRQHandler            (void) { while (1); }
void Default_TIMER0_IRQHandler           (void) { while (1); }
void Default_TIMER1_IRQHandler           (void) { while (1); }
void Default_TIMER2_IRQHandler           (void) { while (1); }
void Default_RTC0_IRQHandler             (void) { while (1); }
void Default_RTC1_IRQHandler             (void) { while (1); }
void Default_WDT_IRQHandler              (void) { while (1); }
void Default_EGU0_IRQHandler             (void) { while (1); }
void Default_EGU1_IRQHandler             (void) { while (1); }
void Default_EGU2_IRQHandler             (void) { while (1); }
void Default_EGU3_IRQHandler             (void) { while (1); }
void Default_EGU4_IRQHandler             (void) { while (1); }
void Default_EGU5_IRQHandler             (void) { while (1); }
void Default_PWM0_IRQHandler             (void) { while (1); }
void Default_PWM1_IRQHandler             (void) { while (1); }
void Default_PWM2_IRQHandler             (void) { while (1); }
void Default_PWM3_IRQHandler             (void) { while (1); }
void Default_PDM_IRQHandler              (void) { while (1); }
void Default_I2S_IRQHandler              (void) { while (1); }
void Default_IPC_IRQHandler              (void) { while (1); }
void Default_FPU_IRQHandler              (void) { while (1); }
void Default_GPIOTE1_IRQHandler          (void) { while (1); }
void Default_KMU_IRQHandler              (void) { while (1); }
void Default_CRYPTOCELL_IRQHandler       (void) { while (1); }

void Reset_Handler(void) {
    uint32_t * p_src  = &_sidata;
    uint32_t * p_dest = &_sdata;

    while (p_dest < &_edata) {
      *p_dest++ = *p_src++;
    }

    uint32_t * p_bss     = &_sbss;
    uint32_t * p_bss_end = &_ebss;
    while (p_bss < p_bss_end) {
        *p_bss++ = 0ul;
    }

    SystemInit();
    _start();
}

void NMI_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void HardFault_Handler           (void) __attribute__ ((weak, alias("Default_HardFault_Handler")));
void MemoryManagement_Handler    (void) __attribute__ ((weak, alias("Default_MemoryManagement_Handler")));
void BusFault_Handler            (void) __attribute__ ((weak, alias("Default_BusFault_Handler")));
void UsageFault_Handler          (void) __attribute__ ((weak, alias("Default_UsageFault_Handler")));
void SecureFault_Handler         (void) __attribute__ ((weak, alias("Default_SecureFault_Handler")));
void SVC_Handler                 (void) __attribute__ ((weak, alias("Default_SVC_Handler")));
void DebugMon_Handler            (void) __attribute__ ((weak, alias("Default_DebugMon_Handler")));
void PendSV_Handler              (void) __attribute__ ((weak, alias("Default_PendSV_Handler")));
void SysTick_Handler             (void) __attribute__ ((weak, alias("Default_SysTick_Handler")));

void SPU_IRQHandler              (void) __attribute__ ((weak, alias("Default_SPU_IRQHandler")));
void CLOCK_POWER_IRQHandler      (void) __attribute__ ((weak, alias("Default_CLOCK_POWER_IRQHandler")));
void UARTE0_SPIM0_SPIS0_TWIM0_TWIS0_IRQHandler (void) __attribute__ ((weak, alias("Default_UARTE0_SPIM0_SPIS0_TWIM0_TWIS0_IRQHandler")));
void UARTE1_SPIM1_SPIS1_TWIM1_TWIS1_IRQHandler (void) __attribute__ ((weak, alias("Default_UARTE1_SPIM1_SPIS1_TWIM1_TWIS1_IRQHandler")));
void UARTE2_SPIM2_SPIS2_TWIM2_TWIS2_IRQHandler (void) __attribute__ ((weak, alias("Default_UARTE2_SPIM2_SPIS2_TWIM2_TWIS2_IRQHandler")));
void UARTE3_SPIM3_SPIS3_TWIM3_TWIS3_IRQHandler (void) __attribute__ ((weak, alias("Default_UARTE3_SPIM3_SPIS3_TWIM3_TWIS3_IRQHandler")));
void GPIOTE0_IRQHandler          (void) __attribute__ ((weak, alias("Default_GPIOTE0_IRQHandler")));
void SAADC_IRQHandler            (void) __attribute__ ((weak, alias("Default_SAADC_IRQHandler")));
void TIMER0_IRQHandler           (void) __attribute__ ((weak, alias("Default_TIMER0_IRQHandler")));
void TIMER1_IRQHandler           (void) __attribute__ ((weak, alias("Default_TIMER1_IRQHandler")));
void TIMER2_IRQHandler           (void) __attribute__ ((weak, alias("Default_TIMER2_IRQHandler")));
void RTC0_IRQHandler             (void) __attribute__ ((weak, alias("Default_RTC0_IRQHandler")));
void RTC1_IRQHandler             (void) __attribute__ ((weak, alias("Default_RTC1_IRQHandler")));
void WDT_IRQHandler              (void) __attribute__ ((weak, alias("Default_WDT_IRQHandler")));
void EGU0_IRQHandler             (void) __attribute__ ((weak, alias("Default_EGU0_IRQHandler")));
void EGU1_IRQHandler             (void) __attribute__ ((weak, alias("Default_EGU1_IRQHandler")));
void EGU2_IRQHandler             (void) __attribute__ ((weak, alias("Default_EGU2_IRQHandler")));
void EGU3_IRQHandler             (void) __attribute__ ((weak, alias("Default_EGU3_IRQHandler")));
void EGU4_IRQHandler             (void) __attribute__ ((weak, alias("Default_EGU4_IRQHandler")));
void EGU5_IRQHandler             (void) __attribute__ ((weak, alias("Default_EGU5_IRQHandler")));
void PWM0_IRQHandler             (void) __attribute__ ((weak, alias("Default_PWM0_IRQHandler")));
void PWM1_IRQHandler             (void) __attribute__ ((weak, alias("Default_PWM1_IRQHandler")));
void PWM2_IRQHandler             (void) __attribute__ ((weak, alias("Default_PWM2_IRQHandler")));
void PWM3_IRQHandler             (void) __attribute__ ((weak, alias("Default_PWM3_IRQHandler")));
void PDM_IRQHandler              (void) __attribute__ ((weak, alias("Default_PDM_IRQHandler")));
void I2S_IRQHandler              (void) __attribute__ ((weak, alias("Default_I2S_IRQHandler")));
void IPC_IRQHandler              (void) __attribute__ ((weak, alias("Default_IPC_IRQHandler")));
void FPU_IRQHandler              (void) __attribute__ ((weak, alias("Default_FPU_IRQHandler")));
void GPIOTE1_IRQHandler          (void) __attribute__ ((weak, alias("Default_GPIOTE1_IRQHandler")));
void KMU_IRQHandler              (void) __attribute__ ((weak, alias("Default_KMU_IRQHandler")));
void CRYPTOCELL_IRQHandler       (void) __attribute__ ((weak, alias("Default_CRYPTOCELL_IRQHandler")));

const func __Vectors[] __attribute__ ((section(".isr_vector"),used)) = {
    (func)&_estack,
    Reset_Handler,
    NMI_Handler,
    HardFault_Handler,
    MemoryManagement_Handler,
    BusFault_Handler,
    UsageFault_Handler,
    SecureFault_Handler,
    Reserved_Handler1,
    Reserved_Handler2,
    Reserved_Handler3,
    SVC_Handler,
    DebugMon_Handler,
    Reserved_Handler4,
    PendSV_Handler,
    SysTick_Handler,

    /* External Interrupts */
    Reserved_Handler5,
    Reserved_Handler6,
    Reserved_Handler7,
    SPU_IRQHandler,
    Reserved_Handler8,
    CLOCK_POWER_IRQHandler,
    Reserved_Handler9,
    Reserved_Handler10,
    UARTE0_SPIM0_SPIS0_TWIM0_TWIS0_IRQHandler,
    UARTE1_SPIM1_SPIS1_TWIM1_TWIS1_IRQHandler,
    UARTE2_SPIM2_SPIS2_TWIM2_TWIS2_IRQHandler,
    UARTE3_SPIM3_SPIS3_TWIM3_TWIS3_IRQHandler,
    Reserved_Handler11,
    GPIOTE0_IRQHandler,
    SAADC_IRQHandler,
    TIMER0_IRQHandler,
    TIMER1_IRQHandler,
    TIMER2_IRQHandler,
    Reserved_Handler12,
    Reserved_Handler13,
    RTC0_IRQHandler,
    RTC1_IRQHandler,
    Reserved_Handler14,
    Reserved_Handler15,
    WDT_IRQHandler,
    Reserved_Handler16,
    Reserved_Handler17,
    EGU0_IRQHandler,
    EGU1_IRQHandler,
    EGU2_IRQHandler,
    EGU3_IRQHandler,
    EGU4_IRQHandler,
    EGU5_IRQHandler,
    PWM0_IRQHandler,
    PWM1_IRQHandler,
    PWM2_IRQHandler,
    PWM3_IRQHandler,
    Reserved_Handler18,
    PDM_IRQHandler,
    Reserved_Handler19,
    I2S_IRQHandler,
    Reserved_Handler20,
    IPC_IRQHandler,
    Reserved_Handler21,
    FPU_IRQHandler,
    Reserved_Handler22,
    Reserved_Handler23,
    Reserved_Handler24,
    Reserved_Handler25,
    GPIOTE1_IRQHandler,
    Reserved_Handler26,
    Reserved_Handler27,
    Reserved_Handler28,
    Reserved_Handler29,
    Reserved_Handler30,
    Reserved_Handler31,
    Reserved_Handler32,
    KMU_IRQHandler,
    Reserved_Handler33,
    Reserved_Handler34,
    Reserved_Handler35,
    Reserved_Handler36,
    Reserved_Handler37,
    Reserved_Handler38,
    CRYPTOCELL_IRQHandler,
};
