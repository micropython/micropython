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
#include "common-hal/pulseio/PulseIn.h"
#include "common-hal/pulseio/PulseOut.h"
#include "common-hal/pwmio/PWMOut.h"
#include "common-hal/rtc/RTC.h"
#include "common-hal/busio/SPI.h"

#include "reset.h"

#include "tusb.h"

#if CIRCUITPY_GAMEPAD
#include "shared-module/gamepad/__init__.h"
#endif
#if CIRCUITPY_GAMEPADSHIFT
#include "shared-module/gamepadshift/__init__.h"
#endif
#if CIRCUITPY_PEW
#include "shared-module/_pew/PewPew.h"
#endif
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

extern void main(void);

// This replaces the Reset_Handler in startup_*.S and SystemInit in system_*.c.
__attribute__((used, naked)) void Reset_Handler(void) {
    __disable_irq();
    SCB->VTOR = (uint32_t)&__isr_vector;
    __set_MSP((uint32_t)&_ld_stack_top);

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

    // The first number in RBAR is the region number. When searching for a policy, the region with
    // the highest number wins. If none match, then the default policy set at enable applies.

    // This is an undocumented region and is likely more registers.
    MPU->RBAR = ARM_MPU_RBAR(8, 0xC0000000U);
    MPU->RASR = ARM_MPU_RASR(EXECUTION, ARM_MPU_AP_FULL, DEVICE, NOT_SHAREABLE, NOT_CACHEABLE, NOT_BUFFERABLE, NO_SUBREGIONS, ARM_MPU_REGION_SIZE_512MB);

    // This is the SEMC region where external RAM and 8+ flash would live. Disable for now, even though the EVKs have stuff here.
    MPU->RBAR = ARM_MPU_RBAR(9, 0x80000000U);
    MPU->RASR = ARM_MPU_RASR(NO_EXECUTION, ARM_MPU_AP_NONE, DEVICE, NOT_SHAREABLE, NOT_CACHEABLE, NOT_BUFFERABLE, NO_SUBREGIONS, ARM_MPU_REGION_SIZE_1GB);

    // FlexSPI2 is 0x70000000

    // This the first 1MB of flash is the bootloader and CircuitPython read-only data.
    MPU->RBAR = ARM_MPU_RBAR(10, 0x60000000U);
    MPU->RASR = ARM_MPU_RASR(EXECUTION, ARM_MPU_AP_FULL, NORMAL, NOT_SHAREABLE, CACHEABLE, BUFFERABLE, NO_SUBREGIONS, ARM_MPU_REGION_SIZE_1MB);

    // The remainder of flash is the fat filesystem which could have code on it too. Make sure that
    // we set the region to the minimal size so that bad data doesn't get speculatively fetched.
    // Thanks to Damien for the tip!
    uint32_t region_size = ARM_MPU_REGION_SIZE_32B;
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

    MPU->RBAR = ARM_MPU_RBAR(11, 0x60100000U);
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
    MPU->RASR = ARM_MPU_RASR(EXECUTION, ARM_MPU_AP_FULL, NORMAL, SHAREABLE, CACHEABLE, BUFFERABLE, NO_SUBREGIONS, ARM_MPU_REGION_SIZE_512KB);

    // We steal 64k from FlexRAM for ITCM and DTCM so disable those memory regions here.
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

safe_mode_t port_init(void) {
    CLOCK_SetMode(kCLOCK_ModeRun);

    clocks_init();

    #if CIRCUITPY_RTC
    rtc_init();
    #endif

    // Always enable the SNVS interrupt. The GPC won't wake us up unless at least one interrupt is
    // enabled. It won't occur very often so it'll be low overhead.
    NVIC_EnableIRQ(SNVS_HP_WRAPPER_IRQn);

    // Note that `reset_port` CANNOT GO HERE, unlike other ports, because `board_init` hasn't been
    // run yet, which uses `never_reset` to protect critical pins from being reset by  `reset_port`.

    if (board_requests_safe_mode()) {
        return USER_SAFE_MODE;
    }

    return NO_SAFE_MODE;
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

    #if CIRCUITPY_PULSEIO
    pulseout_reset();
    #endif
    #if CIRCUITPY_PWMIO
    pwmout_reset();
    #endif

    #if CIRCUITPY_RTC
    rtc_reset();
    #endif

    #if CIRCUITPY_GAMEPAD
    gamepad_reset();
    #endif
    #if CIRCUITPY_GAMEPADSHIFT
    gamepadshift_reset();
    #endif
    #if CIRCUITPY_PEW
    pew_reset();
    #endif

    // reset_event_system();

    reset_all_pins();
}

void reset_to_bootloader(void) {
    SNVS->LPGPR[0] = DBL_TAP_MAGIC;
    reset();
}

void reset_cpu(void) {
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
void port_set_saved_word(uint32_t value) {
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

void SNVS_HP_WRAPPER_IRQHandler(void) {
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

    // Clear the FPU interrupt because it can prevent us from sleeping.
    if (__get_FPSCR() & ~(0x9f)) {
        __set_FPSCR(__get_FPSCR() & ~(0x9f));
        (void)__get_FPSCR();
    }
    NVIC_ClearPendingIRQ(SNVS_HP_WRAPPER_IRQn);
    CLOCK_SetMode(kCLOCK_ModeWait);
    __WFI();
    CLOCK_SetMode(kCLOCK_ModeRun);
}

/**
 * \brief Default interrupt handler for unused IRQs.
 */
__attribute__((used)) void MemManage_Handler(void) {
    reset_into_safe_mode(MEM_MANAGE);
    while (true) {
        asm ("nop;");
    }
}

/**
 * \brief Default interrupt handler for unused IRQs.
 */
__attribute__((used)) void BusFault_Handler(void) {
    reset_into_safe_mode(MEM_MANAGE);
    while (true) {
        asm ("nop;");
    }
}

/**
 * \brief Default interrupt handler for unused IRQs.
 */
__attribute__((used)) void UsageFault_Handler(void) {
    reset_into_safe_mode(MEM_MANAGE);
    while (true) {
        asm ("nop;");
    }
}

/**
 * \brief Default interrupt handler for unused IRQs.
 */
__attribute__((used)) void HardFault_Handler(void) {
    reset_into_safe_mode(HARD_CRASH);
    while (true) {
        asm ("nop;");
    }
}
