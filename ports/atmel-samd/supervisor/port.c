/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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

#include <string.h>
#include <stdlib.h>

#include "supervisor/board.h"
#include "supervisor/port.h"

// ASF 4
#include "atmel_start_pins.h"
#include "peripheral_clk_config.h"
#include "hal/include/hal_delay.h"
#include "hal/include/hal_flash.h"
#include "hal/include/hal_gpio.h"
#include "hal/include/hal_init.h"
#include "hpl/gclk/hpl_gclk_base.h"
#include "hpl/pm/hpl_pm_base.h"

#if defined(SAMD21)
#include "hri/hri_pm_d21.h"
#elif defined(SAME54)
#include "hri/hri_rstc_e54.h"
#elif defined(SAME51)
#include "sam.h"
#include "hri/hri_rstc_e51.h"
#elif defined(SAMD51)
#include "hri/hri_rstc_d51.h"
#else
#error Unknown chip family
#endif

#include "common-hal/analogio/AnalogIn.h"
#include "common-hal/analogio/AnalogOut.h"
#include "common-hal/audiobusio/PDMIn.h"
#include "common-hal/audiobusio/I2SOut.h"
#include "common-hal/audioio/AudioOut.h"
#include "common-hal/busio/SPI.h"
#include "common-hal/microcontroller/Pin.h"
#include "common-hal/pulseio/PulseIn.h"
#include "common-hal/pulseio/PulseOut.h"
#include "common-hal/pwmio/PWMOut.h"
#include "common-hal/ps2io/Ps2.h"
#include "common-hal/rtc/RTC.h"

#if CIRCUITPY_TOUCHIO_USE_NATIVE
#include "common-hal/touchio/TouchIn.h"
#endif

#include "samd/cache.h"
#include "samd/clocks.h"
#include "samd/events.h"
#include "samd/external_interrupts.h"
#include "samd/dma.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/rtc/__init__.h"
#include "reset.h"

#include "supervisor/shared/safe_mode.h"
#include "supervisor/shared/stack.h"
#include "supervisor/shared/tick.h"

#include "tusb.h"

#if CIRCUITPY_GAMEPAD
#include "shared-module/gamepad/__init__.h"
#endif
#if CIRCUITPY_GAMEPADSHIFT
#include "shared-module/gamepadshift/__init__.h"
#endif
#if CIRCUITPY_PEW
#include "common-hal/_pew/PewPew.h"
#endif
volatile bool hold_interrupt = false;
#ifdef SAMD21
static void rtc_set_continuous(bool continuous) {
    while (RTC->MODE0.STATUS.bit.SYNCBUSY) {
        ;
    }
    RTC->MODE0.READREQ.reg = (continuous ? RTC_READREQ_RCONT : 0) | 0x0010;
    while (RTC->MODE0.STATUS.bit.SYNCBUSY) {
        ;
    }
}

void rtc_start_pulse(void) {
    rtc_set_continuous(true);
    hold_interrupt = true;
}

void rtc_end_pulse(void) {
    hold_interrupt = false;
    rtc_set_continuous(false);
}
#endif

extern volatile bool mp_msc_enabled;

#if defined(SAMD21) && defined(ENABLE_MICRO_TRACE_BUFFER)
// Stores 2 ^ TRACE_BUFFER_MAGNITUDE_PACKETS packets.
// 7 -> 128 packets
#define TRACE_BUFFER_MAGNITUDE_PACKETS 7
// Size in uint32_t. Two per packet.
#define TRACE_BUFFER_SIZE (1 << (TRACE_BUFFER_MAGNITUDE_PACKETS + 1))
// Size in bytes. 4 bytes per uint32_t.
#define TRACE_BUFFER_SIZE_BYTES (TRACE_BUFFER_SIZE << 2)
__attribute__((__aligned__(TRACE_BUFFER_SIZE_BYTES))) uint32_t mtb[TRACE_BUFFER_SIZE] = {0};
#endif

#if CALIBRATE_CRYSTALLESS
static void save_usb_clock_calibration(void) {
    // If we are on USB lets double check our fine calibration for the clock and
    // save the new value if its different enough.
    SYSCTRL->DFLLSYNC.bit.READREQ = 1;
    uint16_t saved_calibration = 0x1ff;
    if (strcmp((char *)CIRCUITPY_INTERNAL_CONFIG_START_ADDR, "CIRCUITPYTHON1") == 0) {
        saved_calibration = ((uint16_t *)CIRCUITPY_INTERNAL_CONFIG_START_ADDR)[8];
    }
    while (SYSCTRL->PCLKSR.bit.DFLLRDY == 0) {
        // TODO(tannewt): Run the mass storage stuff if this takes a while.
    }
    int16_t current_calibration = SYSCTRL->DFLLVAL.bit.FINE;
    if (abs(current_calibration - saved_calibration) > 10) {
        // Copy the full internal config page to memory.
        uint8_t page_buffer[NVMCTRL_ROW_SIZE];
        memcpy(page_buffer, (uint8_t *)CIRCUITPY_INTERNAL_CONFIG_START_ADDR, NVMCTRL_ROW_SIZE);

        // Modify it.
        memcpy(page_buffer, "CIRCUITPYTHON1", 15);
        // First 16 bytes (0-15) are ID. Little endian!
        page_buffer[16] = current_calibration & 0xff;
        page_buffer[17] = current_calibration >> 8;

        // Write it back.
        // We don't use features that use any advanced NVMCTRL features so we can fake the descriptor
        // whenever we need it instead of storing it long term.
        struct flash_descriptor desc;
        desc.dev.hw = NVMCTRL;
        flash_write(&desc, (uint32_t)CIRCUITPY_INTERNAL_CONFIG_START_ADDR, page_buffer, NVMCTRL_ROW_SIZE);
    }
}
#endif

static void rtc_init(void) {
    #ifdef SAMD21
    _gclk_enable_channel(RTC_GCLK_ID, GCLK_CLKCTRL_GEN_GCLK2_Val);
    RTC->MODE0.CTRL.bit.SWRST = true;
    while (RTC->MODE0.CTRL.bit.SWRST != 0) {
    }

    RTC->MODE0.CTRL.reg = RTC_MODE0_CTRL_ENABLE |
        RTC_MODE0_CTRL_MODE_COUNT32 |
        RTC_MODE0_CTRL_PRESCALER_DIV2;
    #endif
    #ifdef SAM_D5X_E5X
    hri_mclk_set_APBAMASK_RTC_bit(MCLK);
    RTC->MODE0.CTRLA.bit.SWRST = true;
    while (RTC->MODE0.SYNCBUSY.bit.SWRST != 0) {
    }

    RTC->MODE0.CTRLA.reg = RTC_MODE0_CTRLA_ENABLE |
        RTC_MODE0_CTRLA_MODE_COUNT32 |
        RTC_MODE0_CTRLA_PRESCALER_DIV2 |
        RTC_MODE0_CTRLA_COUNTSYNC;
    #endif

    RTC->MODE0.INTENSET.reg = RTC_MODE0_INTENSET_OVF;

    // Set all peripheral interrupt priorities to the lowest priority by default.
    for (uint16_t i = 0; i < PERIPH_COUNT_IRQn; i++) {
        NVIC_SetPriority(i, (1UL << __NVIC_PRIO_BITS) - 1UL);
    }
    // Bump up the rtc interrupt so nothing else interferes with timekeeping.
    NVIC_SetPriority(RTC_IRQn, 0);
    #ifdef SAMD21
    NVIC_SetPriority(USB_IRQn, 1);
    #endif

    #ifdef SAM_D5X_E5X
    NVIC_SetPriority(USB_0_IRQn, 1);
    NVIC_SetPriority(USB_1_IRQn, 1);
    NVIC_SetPriority(USB_2_IRQn, 1);
    NVIC_SetPriority(USB_3_IRQn, 1);
    #endif
    NVIC_ClearPendingIRQ(RTC_IRQn);
    NVIC_EnableIRQ(RTC_IRQn);
    #if CIRCUITPY_RTC
    rtc_reset();
    #endif

}

safe_mode_t port_init(void) {
    #if defined(SAMD21)

    // Set brownout detection.
    // Disable while changing level.
    SYSCTRL->BOD33.bit.ENABLE = 0;
    SYSCTRL->BOD33.bit.LEVEL = SAMD21_BOD33_LEVEL;
    SYSCTRL->BOD33.bit.ENABLE = 1;

    #ifdef ENABLE_MICRO_TRACE_BUFFER
    REG_MTB_POSITION = ((uint32_t)(mtb - REG_MTB_BASE)) & 0xFFFFFFF8;
    REG_MTB_FLOW = (((uint32_t)mtb - REG_MTB_BASE) + TRACE_BUFFER_SIZE_BYTES) & 0xFFFFFFF8;
    REG_MTB_MASTER = 0x80000000 + (TRACE_BUFFER_MAGNITUDE_PACKETS - 1);
    #else
    // Triple check that the MTB is off. Switching between debug and non-debug
    // builds can leave it set over reset and wreak havok as a result.
    REG_MTB_MASTER = 0x00000000 + 6;
    #endif
    #endif

    #if defined(SAM_D5X_E5X)
    // Set brownout detection.
    // Disable while changing level.
    SUPC->BOD33.bit.ENABLE = 0;
    SUPC->BOD33.bit.LEVEL = SAMD5x_E5x_BOD33_LEVEL;
    SUPC->BOD33.bit.ENABLE = 1;

    // MPU (Memory Protection Unit) setup.
    // We hoped we could make the QSPI region be non-cachable with the MPU,
    // but the CMCC doesn't seem to pay attention to the MPU settings.
    // Leaving this code here disabled,
    // because it was hard enough to figure out, and maybe there's
    // a mistake that could make it work in the future.
    #if 0
    // Designate QSPI memory mapped region as not cachable.

    // Turn off MPU in case it is on.
    MPU->CTRL = 0;
    // Configure region 0.
    MPU->RNR = 0;
    // Region base: start of QSPI mapping area.
    // QSPI region runs from 0x04000000 up to and not including 0x05000000: 16 megabytes
    MPU->RBAR = QSPI_AHB;
    MPU->RASR =
        0b011 << MPU_RASR_AP_Pos |     // full read/write access for privileged and user mode
            0b000 << MPU_RASR_TEX_Pos | // caching not allowed, strongly ordered
            1 << MPU_RASR_S_Pos |      // sharable
            0 << MPU_RASR_C_Pos |      // not cachable
            0 << MPU_RASR_B_Pos |      // not bufferable
            0b10111 << MPU_RASR_SIZE_Pos | // 16MB region size
            1 << MPU_RASR_ENABLE_Pos   // enable this region
    ;
    // Turn off regions 1-7.
    for (uint32_t i = 1; i < 8; i++) {
        MPU->RNR = i;
        MPU->RBAR = 0;
        MPU->RASR = 0;
    }

    // Turn on MPU. Turn on PRIVDEFENA, which defines a default memory
    // map for all privileged access, so we don't have to set up other regions
    // besides QSPI.
    MPU->CTRL = MPU_CTRL_PRIVDEFENA_Msk | MPU_CTRL_ENABLE_Msk;
    #endif

    samd_peripherals_enable_cache();
    #endif

    #ifdef SAMD21
    hri_nvmctrl_set_CTRLB_RWS_bf(NVMCTRL, 2);
    _pm_init();
    #endif

    #if CALIBRATE_CRYSTALLESS
    uint32_t fine = DEFAULT_DFLL48M_FINE_CALIBRATION;
    // The fine calibration data is stored in an NVM page after the text and data storage but before
    // the optional file system. The first 16 bytes are the identifier for the section.
    if (strcmp((char *)CIRCUITPY_INTERNAL_CONFIG_START_ADDR, "CIRCUITPYTHON1") == 0) {
        fine = ((uint16_t *)CIRCUITPY_INTERNAL_CONFIG_START_ADDR)[8];
    }
    clock_init(BOARD_HAS_CRYSTAL, fine);
    #else
    // Use a default fine value
    clock_init(BOARD_HAS_CRYSTAL, DEFAULT_DFLL48M_FINE_CALIBRATION);
    #endif

    rtc_init();

    init_shared_dma();

    // Reset everything into a known state before board_init.
    reset_port();

    #ifdef SAMD21
    if (PM->RCAUSE.bit.BOD33 == 1 || PM->RCAUSE.bit.BOD12 == 1) {
        return BROWNOUT;
    }
    #endif
    #ifdef SAM_D5X_E5X
    if (RSTC->RCAUSE.bit.BODVDD == 1 || RSTC->RCAUSE.bit.BODCORE == 1) {
        return BROWNOUT;
    }
    #endif

    if (board_requests_safe_mode()) {
        return USER_SAFE_MODE;
    }

    return NO_SAFE_MODE;
}

void reset_port(void) {
    #if CIRCUITPY_BUSIO
    reset_sercoms();
    #endif
    #if CIRCUITPY_AUDIOIO
    audio_dma_reset();
    audioout_reset();
    #endif
    #if CIRCUITPY_AUDIOBUSIO
    // pdmin_reset();
    #endif
    #if CIRCUITPY_AUDIOBUSIO_I2SOUT
    i2sout_reset();
    #endif

    #if CIRCUITPY_TOUCHIO && CIRCUITPY_TOUCHIO_USE_NATIVE
    touchin_reset();
    #endif
    eic_reset();
    #if CIRCUITPY_PULSEIO
    pulsein_reset();
    pulseout_reset();
    #endif
    #if CIRCUITPY_PWMIO
    pwmout_reset();
    #endif

    #if CIRCUITPY_ANALOGIO
    analogin_reset();
    analogout_reset();
    #endif

    reset_gclks();

    #if CIRCUITPY_GAMEPAD
    gamepad_reset();
    #endif
    #if CIRCUITPY_GAMEPADSHIFT
    gamepadshift_reset();
    #endif
    #if CIRCUITPY_PEW
    pew_reset();
    #endif

    reset_event_system();

    reset_all_pins();

    // Output clocks for debugging.
    // not supported by SAMD51G; uncomment for SAMD51J or update for 51G
    // #ifdef SAM_D5X_E5X
    // gpio_set_pin_function(PIN_PA10, GPIO_PIN_FUNCTION_M); // GCLK4, D3
    // gpio_set_pin_function(PIN_PA11, GPIO_PIN_FUNCTION_M); // GCLK5, A4
    // gpio_set_pin_function(PIN_PB14, GPIO_PIN_FUNCTION_M); // GCLK0, D5
    // gpio_set_pin_function(PIN_PB15, GPIO_PIN_FUNCTION_M); // GCLK1, D6
    // #endif

    #if CALIBRATE_CRYSTALLESS
    if (tud_cdc_connected()) {
        save_usb_clock_calibration();
    }
    #endif
}

void reset_to_bootloader(void) {
    _bootloader_dbl_tap = DBL_TAP_MAGIC;
    reset();
}

void reset_cpu(void) {
    reset();
}

bool port_has_fixed_stack(void) {
    return false;
}

uint32_t *port_stack_get_limit(void) {
    return &_ebss;
}

uint32_t *port_stack_get_top(void) {
    return &_estack;
}

uint32_t *port_heap_get_bottom(void) {
    return port_stack_get_limit();
}

uint32_t *port_heap_get_top(void) {
    return port_stack_get_top();
}

// Place the word to save 8k from the end of RAM so we and the bootloader don't clobber it.
#ifdef SAMD21
uint32_t *safe_word = (uint32_t *)(HMCRAMC0_ADDR + HMCRAMC0_SIZE - 0x2000);
#endif
#ifdef SAM_D5X_E5X
uint32_t *safe_word = (uint32_t *)(HSRAM_ADDR + HSRAM_SIZE - 0x2000);
#endif

void port_set_saved_word(uint32_t value) {
    *safe_word = value;
}

uint32_t port_get_saved_word(void) {
    return *safe_word;
}

// TODO: Move this to an RTC backup register so we can preserve it when only the BACKUP power domain
// is enabled.
static volatile uint64_t overflowed_ticks = 0;
#ifdef SAMD21
static volatile bool _ticks_enabled = false;
#endif

static uint32_t _get_count(uint64_t *overflow_count) {
    #ifdef SAM_D5X_E5X
    while ((RTC->MODE0.SYNCBUSY.reg & (RTC_MODE0_SYNCBUSY_COUNTSYNC | RTC_MODE0_SYNCBUSY_COUNT)) != 0) {
    }
    #endif
    #ifdef SAMD21
    // Request a read so we don't stall the bus later. See section 14.3.1.5 Read Request
    RTC->MODE0.READREQ.reg = RTC_READREQ_RREQ | 0x0010;
    while (RTC->MODE0.STATUS.bit.SYNCBUSY != 0) {
    }
    #endif
    // Disable interrupts so we can grab the count and the overflow.
    common_hal_mcu_disable_interrupts();
    uint32_t count = RTC->MODE0.COUNT.reg;
    if (overflow_count != NULL) {
        *overflow_count = overflowed_ticks;
    }
    common_hal_mcu_enable_interrupts();

    return count;
}

volatile bool _woken_up;

static void _port_interrupt_after_ticks(uint32_t ticks) {
    uint32_t current_ticks = _get_count(NULL);
    if (ticks > 1 << 28) {
        // We'll interrupt sooner with an overflow.
        return;
    }
    #ifdef SAMD21
    if (hold_interrupt) {
        return;
    }
    #endif
    uint32_t target = current_ticks + (ticks << 4);
    RTC->MODE0.COMP[0].reg = target;
    #ifdef SAM_D5X_E5X
    while ((RTC->MODE0.SYNCBUSY.reg & (RTC_MODE0_SYNCBUSY_COMP0)) != 0) {
    }
    #endif
    RTC->MODE0.INTFLAG.reg = RTC_MODE0_INTFLAG_CMP0;
    RTC->MODE0.INTENSET.reg = RTC_MODE0_INTENSET_CMP0;
    current_ticks = _get_count(NULL);
    _woken_up = current_ticks >= target;
}

void RTC_Handler(void) {
    uint32_t intflag = RTC->MODE0.INTFLAG.reg;
    if (intflag & RTC_MODE0_INTFLAG_OVF) {
        RTC->MODE0.INTFLAG.reg = RTC_MODE0_INTFLAG_OVF;
        // Our RTC is 32 bits and we're clocking it at 16.384khz which is 16 (2 ** 4) subticks per
        // tick.
        overflowed_ticks += (1L << (32 - 4));
    }
    #ifdef SAM_D5X_E5X
    if (intflag & RTC_MODE0_INTFLAG_PER2) {
        RTC->MODE0.INTFLAG.reg = RTC_MODE0_INTFLAG_PER2;
        // Do things common to all ports when the tick occurs
        supervisor_tick();
    }
    #endif
    if (intflag & RTC_MODE0_INTFLAG_CMP0) {
        // Clear the interrupt because we may have hit a sleep and _ticks_enabled
        RTC->MODE0.INTFLAG.reg = RTC_MODE0_INTFLAG_CMP0;
        _woken_up = true;
        #ifdef SAMD21
        if (_ticks_enabled) {
            // Do things common to all ports when the tick occurs.
            supervisor_tick();
            // Check _ticks_enabled again because a tick handler may have turned it off.
            if (_ticks_enabled) {
                _port_interrupt_after_ticks(1);
            }
        }
        #endif
        #ifdef SAM_D5X_E5X
        RTC->MODE0.INTENCLR.reg = RTC_MODE0_INTENCLR_CMP0;
        #endif
    }
}

uint64_t port_get_raw_ticks(uint8_t *subticks) {
    uint64_t overflow_count;
    uint32_t current_ticks = _get_count(&overflow_count);
    if (subticks != NULL) {
        *subticks = (current_ticks % 16) * 2;
    }

    return overflow_count + current_ticks / 16;
}

// Enable 1/1024 second tick.
void port_enable_tick(void) {
    #ifdef SAM_D5X_E5X
    // PER2 will generate an interrupt every 32 ticks of the source 32.768 clock.
    RTC->MODE0.INTENSET.reg = RTC_MODE0_INTENSET_PER2;
    #endif
    #ifdef SAMD21
    // TODO: Switch to using the PER *event* from the RTC to generate an interrupt via EVSYS.
    _ticks_enabled = true;
    _port_interrupt_after_ticks(1);
    #endif
}

// Disable 1/1024 second tick.
void port_disable_tick(void) {
    #ifdef SAM_D5X_E5X
    RTC->MODE0.INTENCLR.reg = RTC_MODE0_INTENCLR_PER2;
    #endif
    #ifdef SAMD21
    _ticks_enabled = false;
    RTC->MODE0.INTENCLR.reg = RTC_MODE0_INTENCLR_CMP0;
    #endif
}

// This is called by sleep, we ignore it when our ticks are enabled because
// they'll wake us up earlier. If we don't, we'll mess up ticks by overwriting
// the next RTC wake up time.
void port_interrupt_after_ticks(uint32_t ticks) {
    #ifdef SAMD21
    if (_ticks_enabled) {
        return;
    }
    #endif
    _port_interrupt_after_ticks(ticks);
}

void port_idle_until_interrupt(void) {
    #ifdef SAM_D5X_E5X
    // Clear the FPU interrupt because it can prevent us from sleeping.
    if (__get_FPSCR() & ~(0x9f)) {
        __set_FPSCR(__get_FPSCR() & ~(0x9f));
        (void)__get_FPSCR();
    }
    #endif
    common_hal_mcu_disable_interrupts();
    if (!tud_task_event_ready() && !hold_interrupt && !_woken_up) {
        __DSB();
        __WFI();
    }
    common_hal_mcu_enable_interrupts();
}

/**
 * \brief Default interrupt handler for unused IRQs.
 */
__attribute__((used)) void HardFault_Handler(void) {
    #ifdef ENABLE_MICRO_TRACE_BUFFER
    // Turn off the micro trace buffer so we don't fill it up in the infinite
    // loop below.
    REG_MTB_MASTER = 0x00000000 + 6;
    #endif

    reset_into_safe_mode(HARD_CRASH);
    while (true) {
        asm ("nop;");
    }
}
