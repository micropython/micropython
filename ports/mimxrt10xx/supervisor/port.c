/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
 * Copyright (c) 2020 Artur Pacholec
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
/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "supervisor/board.h"
#include "supervisor/port.h"

#include "fsl_device_registers.h"

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/pwmio/PWMOut.h"
#include "common-hal/rtc/RTC.h"
#include "common-hal/busio/SPI.h"
#include "shared-bindings/microcontroller/__init__.h"

#if CIRCUITPY_PEW
#include "shared-module/_pew/PewPew.h"
#endif

#include "reset.h"

#include "supervisor/background_callback.h"
#include "supervisor/linker.h"
#include "supervisor/shared/tick.h"

#include "clocks.h"

#include "fsl_gpio.h"
#include "fsl_lpuart.h"

// Device memories must be accessed in order.
#define DEVICE 2
// Normal memory can have accesses reorder and prefetched.
#define NORMAL 0

// Prevents instruction access.
#define NO_EXECUTION 1
#define EXECUTION 0

// Shareable if the memory system manages coherency. This means shared between memory bus masters,
// not just CPUs.
#define NOT_SHAREABLE 0
#define SHAREABLE 1

//
#define NOT_CACHEABLE 0
#define CACHEABLE 1

#define NOT_BUFFERABLE 0
#define BUFFERABLE 1

#define NO_SUBREGIONS 0

extern uint32_t _ld_flash_size;
extern uint32_t _ld_stack_top;

extern uint32_t __isr_vector[];

extern uint32_t _ld_ocram_bss_start;
extern uint32_t _ld_ocram_bss_size;
extern uint32_t _ld_ocram_data_destination;
extern uint32_t _ld_ocram_data_size;
extern uint32_t _ld_ocram_data_flash_copy;
extern uint32_t _ld_dtcm_bss_start;
extern uint32_t _ld_dtcm_bss_size;
extern uint32_t _ld_dtcm_data_destination;
extern uint32_t _ld_dtcm_data_size;
extern uint32_t _ld_dtcm_data_flash_copy;
extern uint32_t _ld_itcm_destination;
extern uint32_t _ld_itcm_size;
extern uint32_t _ld_itcm_flash_copy;
extern uint32_t _ld_isr_destination;
extern uint32_t _ld_isr_size;
extern uint32_t _ld_isr_flash_copy;

extern void main(void);

// This replaces the Reset_Handler in startup_*.S and SystemInit in system_*.c.
// Turn off optimize("no-tree-loop-distribute-patterns") so that this isn't replaced
// by calls to memcpy because we're copying it over now.
void Reset_Handler(void);
__attribute__((used, naked, no_instrument_function, optimize("no-tree-loop-distribute-patterns"))) void Reset_Handler(void) {
    __disable_irq();
    // Set the VTOR to the flash copy since we haven't copied it into RAM.
    SCB->VTOR = (uint32_t)&_ld_isr_flash_copy;
    __set_MSP((uint32_t)&_ld_stack_top);

    // Turn off any residual ITM outputs.
    ITM->TER = 0;

    /* Disable I cache and D cache */
    SCB_DisableICache();
    SCB_DisableDCache();

    // Changing the FlexRAM must happen here where the stack is empty. If it is in a function call,
    // then the return will jump to an invalid address.
    // Configure FlexRAM. The e is one block of ITCM (0b11) and DTCM (0b10). The rest is two OCRAM
    // (0b01). We shift in zeroes for all unimplemented banks.
    IOMUXC_GPR->GPR17 = (0xe5555555) >> (32 - 2 * FSL_FEATURE_FLEXRAM_INTERNAL_RAM_TOTAL_BANK_NUMBERS);

    // Switch from FlexRAM fuse config to the IOMUXC values.
    IOMUXC_GPR->GPR16 |= IOMUXC_GPR_GPR16_FLEXRAM_BANK_CFG_SEL(1);

    // Let the core know the TCM sizes changed.
    uint32_t current_gpr14 = IOMUXC_GPR->GPR14;
    current_gpr14 &= ~IOMUXC_GPR_GPR14_CM7_CFGDTCMSZ_MASK;
    current_gpr14 |= IOMUXC_GPR_GPR14_CM7_CFGDTCMSZ(0x6);
    current_gpr14 &= ~IOMUXC_GPR_GPR14_CM7_CFGITCMSZ_MASK;
    current_gpr14 |= IOMUXC_GPR_GPR14_CM7_CFGITCMSZ(0x6);
    IOMUXC_GPR->GPR14 = current_gpr14;

    // Enable FlexRAM interrupts on invalid access.
    FLEXRAM->INT_STAT_EN = FLEXRAM_INT_STAT_EN_ITCM_ERR_STAT_EN(1) |
        FLEXRAM_INT_STAT_EN_DTCM_ERR_STAT_EN(1) |
        FLEXRAM_INT_STAT_EN_OCRAM_ERR_STAT_EN(1);

    #if ((__FPU_PRESENT == 1) && (__FPU_USED == 1))
    SCB->CPACR |= ((3UL << 10 * 2) | (3UL << 11 * 2));  /* set CP10, CP11 Full Access */
    #endif /* ((__FPU_PRESENT == 1) && (__FPU_USED == 1)) */

    /* Disable Watchdog Power Down Counter */
    WDOG1->WMCR &= ~WDOG_WMCR_PDE_MASK;
    WDOG2->WMCR &= ~WDOG_WMCR_PDE_MASK;

    /* Watchdog disable */
    WDOG1->WCR &= ~WDOG_WCR_WDE_MASK;
    WDOG2->WCR &= ~WDOG_WCR_WDE_MASK;
    RTWDOG->CNT = 0xD928C520U; /* 0xD928C520U is the update key */
    RTWDOG->TOVAL = 0xFFFF;
    RTWDOG->CS = (uint32_t)((RTWDOG->CS) & ~RTWDOG_CS_EN_MASK) | RTWDOG_CS_UPDATE_MASK;

    /* Disable Systick which might be enabled by bootrom */
    if (SysTick->CTRL & SysTick_CTRL_ENABLE_Msk) {
        SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    }

    /* Disable MPU */
    ARM_MPU_Disable();

    // Copy all of the itcm code to run from ITCM. Do this while the MPU is disabled because we write
    // protect it.
    for (uint32_t i = 0; i < ((size_t)&_ld_itcm_size) / 4; i++) {
        (&_ld_itcm_destination)[i] = (&_ld_itcm_flash_copy)[i];
    }

    for (uint32_t i = 0; i < ((size_t)&_ld_isr_size) / 4; i++) {
        (&_ld_isr_destination)[i] = (&_ld_isr_flash_copy)[i];
    }

    // Now that we've copied the ISR table over, use that VTOR.
    SCB->VTOR = (uint32_t)&_ld_isr_destination;

    // The first number in RBAR is the region number. When searching for a policy, the region with
    // the highest number wins. If none match, then the default policy set at enable applies.

    // This is an undocumented region and is likely more registers.
    MPU->RBAR = ARM_MPU_RBAR(8, 0xC0000000U);
    MPU->RASR = ARM_MPU_RASR(EXECUTION, ARM_MPU_AP_FULL, DEVICE, NOT_SHAREABLE, NOT_CACHEABLE, NOT_BUFFERABLE, NO_SUBREGIONS, ARM_MPU_REGION_SIZE_512MB);

    // This is the SEMC region where external RAM and 8+ flash would live. Disable for now, even though the EVKs have stuff here.
    MPU->RBAR = ARM_MPU_RBAR(9, 0x80000000U);
    MPU->RASR = ARM_MPU_RASR(NO_EXECUTION, ARM_MPU_AP_NONE, DEVICE, NOT_SHAREABLE, NOT_CACHEABLE, NOT_BUFFERABLE, NO_SUBREGIONS, ARM_MPU_REGION_SIZE_1GB);

    // FlexSPI2 is 0x70000000

    // This the first portion (1MB, 2MB or 4MB) of flash is the bootloader and CircuitPython read-only data.
    MPU->RBAR = ARM_MPU_RBAR(10, FlexSPI_AMBA_BASE);
    uint32_t region_size = ARM_MPU_REGION_SIZE_32B;
    uint32_t code_size = ((uint32_t)&_ld_filesystem_start) - FlexSPI_AMBA_BASE;
    while (code_size > (1u << (region_size + 1))) {
        region_size += 1;
    }
    MPU->RASR = ARM_MPU_RASR(EXECUTION, ARM_MPU_AP_FULL, NORMAL, NOT_SHAREABLE, CACHEABLE, BUFFERABLE, NO_SUBREGIONS, region_size);

    // The remainder of flash is the fat filesystem which could have code on it too. Make sure that
    // we set the region to the minimal size so that bad data doesn't get speculatively fetched.
    // Thanks to Damien for the tip!
    region_size = ARM_MPU_REGION_SIZE_32B;
    uint32_t filesystem_size = &_ld_filesystem_end - &_ld_filesystem_start;
    while (filesystem_size > (1u << (region_size + 1))) {
        region_size += 1;
    }
    // Mask out as much of the remainder as we can. For example on an 8MB flash, 7MB are for the
    // filesystem. The region_size here must be a power of 2 so it is 8MB. Using the subregion mask
    // we can ignore 1/8th size chunks. So, we ignore the last 1MB using the subregion.
    uint32_t remainder = (1u << (region_size + 1)) - filesystem_size;
    uint32_t subregion_size = (1u << (region_size + 1)) / 8;
    uint8_t subregion_mask = (0xff00 >> (remainder / subregion_size)) & 0xff;

    MPU->RBAR = ARM_MPU_RBAR(11, (size_t)&_ld_filesystem_start);
    MPU->RASR = ARM_MPU_RASR(EXECUTION, ARM_MPU_AP_FULL, NORMAL, NOT_SHAREABLE, CACHEABLE, BUFFERABLE, subregion_mask, region_size);

    // This the ITCM. Set it to read-only because we've loaded everything already and it's easy to
    // accidentally write the wrong value to 0x00000000 (aka NULL).
    MPU->RBAR = ARM_MPU_RBAR(12, 0x00000000U);
    MPU->RASR = ARM_MPU_RASR(EXECUTION, ARM_MPU_AP_RO, NORMAL, NOT_SHAREABLE, CACHEABLE, BUFFERABLE, NO_SUBREGIONS, ARM_MPU_REGION_SIZE_32KB);

    // This the DTCM.
    MPU->RBAR = ARM_MPU_RBAR(13, 0x20000000U);
    MPU->RASR = ARM_MPU_RASR(EXECUTION, ARM_MPU_AP_FULL, NORMAL, NOT_SHAREABLE, CACHEABLE, BUFFERABLE, NO_SUBREGIONS, ARM_MPU_REGION_SIZE_32KB);

    // This is OCRAM. We mark it as shareable so that it isn't cached. This makes USB work at the
    // cost of 1/4 speed OCRAM accesses. It will leave more room for caching data from the flash
    // too which might be a net win.
    MPU->RBAR = ARM_MPU_RBAR(14, 0x20200000U);
    MPU->RASR = ARM_MPU_RASR(NO_EXECUTION, ARM_MPU_AP_FULL, NORMAL, NOT_SHAREABLE, CACHEABLE, BUFFERABLE, NO_SUBREGIONS, ARM_MPU_REGION_SIZE_512KB);

    // We steal 64k from FlexRAM for ITCM and DTCM so disable those memory regions here.
    // We use 64k from FlexRAM for ITCM and DTCM so disable those memory regions here.
    MPU->RBAR = ARM_MPU_RBAR(15, 0x20280000U);
    MPU->RASR = ARM_MPU_RASR(EXECUTION, ARM_MPU_AP_FULL, NORMAL, NOT_SHAREABLE, CACHEABLE, BUFFERABLE, 0x80, ARM_MPU_REGION_SIZE_512KB);

    /* Enable MPU */
    ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);

    /* We're done mucking with memory so enable I cache and D cache */
    SCB_EnableDCache();
    SCB_EnableICache();

    // Copy all of the data to run from DTCM.
    for (uint32_t i = 0; i < ((size_t)&_ld_dtcm_data_size) / 4; i++) {
        (&_ld_dtcm_data_destination)[i] = (&_ld_dtcm_data_flash_copy)[i];
    }

    // Clear DTCM bss.
    for (uint32_t i = 0; i < ((size_t)&_ld_dtcm_bss_size) / 4; i++) {
        (&_ld_dtcm_bss_start)[i] = 0;
    }

    // Copy all of the data to run from OCRAM.
    for (uint32_t i = 0; i < ((size_t)&_ld_ocram_data_size) / 4; i++) {
        (&_ld_ocram_data_destination)[i] = (&_ld_ocram_data_flash_copy)[i];
    }

    // Clear OCRAM bss.
    for (uint32_t i = 0; i < ((size_t)&_ld_ocram_bss_size) / 4; i++) {
        (&_ld_ocram_bss_start)[i] = 0;
    }

    __enable_irq();

    main();
}

void __attribute__((no_instrument_function,section(".itcm.profile_enter"),long_call)) __cyg_profile_func_enter(void *this_fn,
    void *call_site) {
    if ((ITM->TER & (1 << 3)) == 0) {
        return;
    }
    uint32_t addr = (uint32_t)this_fn;
    while (ITM->PORT[3U].u32 == 0UL) {
        // addr |= 1;
    }
    ITM->PORT[3].u32 = addr;
}

void __attribute__((no_instrument_function,section(".itcm.profile_exit"),long_call)) __cyg_profile_func_exit(void *this_fn,
    void *call_site) {
    if ((ITM->TER & (1 << 4)) == 0) {
        return;
    }
    uint32_t addr = (uint32_t)this_fn;
    while (ITM->PORT[4U].u32 == 0UL) {
        // addr |= 1;
    }
    ITM->PORT[4].u32 = addr;
}

safe_mode_t port_init(void) {
    CLOCK_SetMode(kCLOCK_ModeRun);

    clocks_init();

    // Turn on the DWT so that neopixel_write can use CYCCNT for timing.
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL = 0x2 << DWT_CTRL_SYNCTAP_Pos | DWT_CTRL_CYCCNTENA_Msk;

    // Enable SWO if needed.
    #if CIRCUITPY_SWO_TRACE

    // Turn on the 528 MHz clock to the TPIU.
    CLOCK_EnableClock(kCLOCK_Trace); /* Make these edits*/
    /* Set TRACE_PODF. */
    CLOCK_SetDiv(kCLOCK_TraceDiv, 0); /* Make these edits*/
    /* Set Trace clock source. */
    CLOCK_SetMux(kCLOCK_TraceMux, 0); /* Make these edits*/

    ITM->TCR = ITM_TCR_TSENA_Msk | ITM_TCR_ITMENA_Msk | ITM_TCR_SYNCENA_Msk | ITM_TCR_DWTENA_Msk;

    // Run at 2.75 mbaud. CP2102N says it can do up to 3.
    // Base clock is 528 mhz (not 500 like the core).
    // TPI->ACPR = 191;
    // Run at 1 mbaud so that USB isn't bottlenecked.
    TPI->ACPR = 527;
    TPI->SPPR = 0x2; // NRZ aka UART
    TPI->FFCR = 0;

    IOMUXC_SetPinMux( /* Add these lines*/
        IOMUXC_GPIO_AD_09_ARM_TRACE_SWO,
        0U);
    IOMUXC_SetPinConfig( /* Add these lines*/
        IOMUXC_GPIO_AD_09_ARM_TRACE_SWO,
        0x00F9U);

    // Enable ports 0-4:
    // * 0 is serial output
    // *
    // * 3 is addresses of functions beginning.
    // * 4 is addresses of functions ending.
    ITM->TER |= 0x1f;
    ITM->PORT[0].u8 = 'C';
    ITM->PORT[0].u8 = 'P';
    ITM->PORT[0].u8 = '\n';
    #endif

    // Set all peripheral interrupt priorities to the lowest priority by default.
    for (uint16_t i = 0; i < NUMBER_OF_INT_VECTORS; i++) {
        NVIC_SetPriority(i, (1UL << __NVIC_PRIO_BITS) - 1UL);
    }
    NVIC_SetPriority(USB_OTG1_IRQn, 1);
    #ifdef USBPHY2
    NVIC_SetPriority(USB_OTG2_IRQn, 1);
    #endif

    NVIC_SetPriority(FLEXRAM_IRQn, 0);
    NVIC_EnableIRQ(FLEXRAM_IRQn);

    // Priorities 8+ will be disabled during flash operations. To run during
    // flash operations, ensure all code is in RAM (not flash) and set the
    // priority < 8.

    #if CIRCUITPY_RTC
    rtc_init();
    #endif

    // Always enable the SNVS interrupt. The GPC won't wake us up unless at least one interrupt is
    // enabled. It won't occur very often so it'll be low overhead.
    NVIC_EnableIRQ(SNVS_HP_WRAPPER_IRQn);

    // Note that `reset_port` CANNOT GO HERE, unlike other ports, because `board_init` hasn't been
    // run yet, which uses `never_reset` to protect critical pins from being reset by  `reset_port`.

    if (board_requests_safe_mode()) {
        return SAFE_MODE_USER;
    }

    return SAFE_MODE_NONE;
}

void reset_port(void) {
    spi_reset();

    #if CIRCUITPY_AUDIOIO
    audio_dma_reset();
    audioout_reset();
    #endif
    #if CIRCUITPY_AUDIOBUSIO
    i2sout_reset();
    // pdmin_reset();
    #endif

    #if CIRCUITPY_TOUCHIO && CIRCUITPY_TOUCHIO_USE_NATIVE
    touchin_reset();
    #endif

//    eic_reset();

    #if CIRCUITPY_PWMIO
    reset_all_flexpwm();
    #endif

    #if CIRCUITPY_RTC
    rtc_reset();
    #endif

    #if CIRCUITPY_PEW
    pew_reset();
    #endif

    // reset_event_system();

    reset_all_pins();
}

void reset_to_bootloader(void) {
    DBL_TAP_REG = DBL_TAP_MAGIC;
    reset();
}

void PLACE_IN_ITCM(reset_cpu)(void) {
    reset();
}

extern uint32_t _ld_heap_start, _ld_heap_end, _ld_stack_top, _ld_stack_bottom;
uint32_t *port_stack_get_limit(void) {
    return &_ld_stack_bottom;
}

uint32_t *port_stack_get_top(void) {
    return &_ld_stack_top;
}

bool port_has_fixed_stack(void) {
    return true;
}

uint32_t *port_heap_get_bottom(void) {
    return &_ld_heap_start;
}

// Get heap top address
uint32_t *port_heap_get_top(void) {
    return &_ld_heap_end;
}

// Place the word into the low power section of the SNVS.
void PLACE_IN_ITCM(port_set_saved_word)(uint32_t value) {
    SNVS->LPGPR[1] = value;
}

uint32_t port_get_saved_word(void) {
    return SNVS->LPGPR[1];
}

uint64_t port_get_raw_ticks(uint8_t *subticks) {
    uint64_t ticks = 0;
    uint64_t next_ticks = 1;
    while (ticks != next_ticks) {
        ticks = next_ticks;
        next_ticks = ((uint64_t)SNVS->HPRTCMR) << 32 | SNVS->HPRTCLR;
    }
    if (subticks != NULL) {
        *subticks = ticks % 32;
    }
    return ticks / 32;
}

void SNVS_HP_WRAPPER_IRQHandler(void);
__attribute__((used))
void PLACE_IN_ITCM(SNVS_HP_WRAPPER_IRQHandler)(void) {
    if ((SNVS->HPSR & SNVS_HPSR_PI_MASK) != 0) {
        supervisor_tick();
        SNVS->HPSR = SNVS_HPSR_PI_MASK;
    }
    if ((SNVS->HPSR & SNVS_HPSR_HPTA_MASK) != 0) {
        SNVS->HPSR = SNVS_HPSR_HPTA_MASK;
    }
}

// Enable 1/1024 second tick.
void port_enable_tick(void) {
    uint32_t hpcr = SNVS->HPCR;
    hpcr &= ~SNVS_HPCR_PI_FREQ_MASK;
    SNVS->HPCR = hpcr | SNVS_HPCR_PI_FREQ(5) | SNVS_HPCR_PI_EN_MASK;
}

// Disable 1/1024 second tick.
void port_disable_tick(void) {
    SNVS->HPCR &= ~SNVS_HPCR_PI_EN_MASK;
}

void port_interrupt_after_ticks(uint32_t ticks) {
    uint8_t subticks;
    uint64_t current_ticks = port_get_raw_ticks(&subticks);
    current_ticks += ticks;
    SNVS->HPCR &= ~SNVS_HPCR_HPTA_EN_MASK;
    // Wait for the alarm to be disabled.
    while ((SNVS->HPCR & SNVS_HPCR_HPTA_EN_MASK) != 0) {
    }
    SNVS->HPTAMR = current_ticks >> (32 - 5);
    SNVS->HPTALR = current_ticks << 5 | subticks;
    SNVS->HPCR |= SNVS_HPCR_HPTA_EN_MASK;
}

void port_idle_until_interrupt(void) {
    // App note here: https://www.nxp.com/docs/en/application-note/AN12085.pdf
    // Currently I have disabled the setting into wait mode as this impacts lots of different
    // subsystems and it is unclear if you can or should set it generically without having
    // a better understanding of user intent.  For example by default it will kill PWM
    // when in this mode, unless PWM_CTRL2_WAITEN_MASK is set, and even with this set
    // it may not work properly if the same timer/subtimer is trying to PWM on multiple channels.
    // Maybe at later date, revisit after we have a better understanding on things like which
    // timers it impacts and how each subsystem is configured.

    // Clear the FPU interrupt because it can prevent us from sleeping.
    if (__get_FPSCR() & ~(0x9f)) {
        __set_FPSCR(__get_FPSCR() & ~(0x9f));
        (void)__get_FPSCR();
    }

    common_hal_mcu_disable_interrupts();
    if (!background_callback_pending()) {
        NVIC_ClearPendingIRQ(SNVS_HP_WRAPPER_IRQn);
        __WFI();
    }
    common_hal_mcu_enable_interrupts();
}

// Catch faults where the memory access violates MPU settings.
void MemManage_Handler(void);
__attribute__((used)) void PLACE_IN_ITCM(MemManage_Handler)(void) {
    reset_into_safe_mode(SAFE_MODE_HARD_FAULT);
    while (true) {
        asm ("nop;");
    }
}

void BusFault_Handler(void);
__attribute__((used)) void PLACE_IN_ITCM(BusFault_Handler)(void) {
    reset_into_safe_mode(SAFE_MODE_HARD_FAULT);
    while (true) {
        asm ("nop;");
    }
}

void UsageFault_Handler(void);
__attribute__((used)) void PLACE_IN_ITCM(UsageFault_Handler)(void) {
    reset_into_safe_mode(SAFE_MODE_HARD_FAULT);
    while (true) {
        asm ("nop;");
    }
}

// Default fault handler.
void HardFault_Handler(void);
__attribute__((used)) void PLACE_IN_ITCM(HardFault_Handler)(void) {
    reset_into_safe_mode(SAFE_MODE_HARD_FAULT);
    while (true) {
        asm ("nop;");
    }
}

// Catch access errors to FlexRAM (if the MPU didn't catch it first.)
void FLEXRAM_IRQHandler(void);
__attribute__((used)) void PLACE_IN_ITCM(FLEXRAM_IRQHandler)(void) {
    reset_into_safe_mode(SAFE_MODE_HARD_FAULT);
    while (true) {
        asm ("nop;");
    }
}
