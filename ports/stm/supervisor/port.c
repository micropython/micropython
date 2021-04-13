/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
 * Copyright (c) 2019 Lucian Copeland for Adafruit Industries
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
#include "supervisor/port.h"
#include "supervisor/board.h"
#include "lib/timeutils/timeutils.h"

#include "common-hal/microcontroller/Pin.h"

#ifdef CIRCUITPY_AUDIOPWMIO
#include "common-hal/audiopwmio/PWMAudioOut.h"
#endif
#if CIRCUITPY_BUSIO
#include "common-hal/busio/I2C.h"
#include "common-hal/busio/SPI.h"
#include "common-hal/busio/UART.h"
#endif
#if CIRCUITPY_PULSEIO
#include "common-hal/pulseio/PulseOut.h"
#include "common-hal/pulseio/PulseIn.h"
#endif
#if CIRCUITPY_PWMIO
#include "common-hal/pwmio/PWMOut.h"
#endif
#if CIRCUITPY_PULSEIO || CIRCUITPY_PWMIO
#include "timers.h"
#endif
#if CIRCUITPY_SDIOIO
#include "common-hal/sdioio/SDCard.h"
#endif

#include "clocks.h"
#include "gpio.h"

#include STM32_HAL_H

void NVIC_SystemReset(void) NORETURN;

#if (CPY_STM32H7) || (CPY_STM32F7)

// Device memories must be accessed in order.
#define DEVICE 2
// Normal memory can have accesses reorder and prefetched.
#define NORMAL 0
// Prevents instruction access.
#define NO_EXECUTION 1
#define EXECUTION 0
// Shareable if the memory system manages coherency.
#define NOT_SHAREABLE 0
#define SHAREABLE 1
#define NOT_CACHEABLE 0
#define CACHEABLE 1
#define NOT_BUFFERABLE 0
#define BUFFERABLE 1
#define NO_SUBREGIONS 0

extern uint32_t _ld_stack_top;

extern uint32_t _ld_d1_ram_bss_start;
extern uint32_t _ld_d1_ram_bss_size;
extern uint32_t _ld_d1_ram_data_destination;
extern uint32_t _ld_d1_ram_data_size;
extern uint32_t _ld_d1_ram_data_flash_copy;
extern uint32_t _ld_dtcm_bss_start;
extern uint32_t _ld_dtcm_bss_size;
extern uint32_t _ld_dtcm_data_destination;
extern uint32_t _ld_dtcm_data_size;
extern uint32_t _ld_dtcm_data_flash_copy;
extern uint32_t _ld_itcm_destination;
extern uint32_t _ld_itcm_size;
extern uint32_t _ld_itcm_flash_copy;

extern void main(void);
extern void SystemInit(void);

// This replaces the Reset_Handler in gcc/startup_*.s, calls SystemInit from system_*.c
__attribute__((used, naked)) void Reset_Handler(void) {
    __disable_irq();
    __set_MSP((uint32_t)&_ld_stack_top);

    /* Disable MPU */
    ARM_MPU_Disable();

    // Copy all of the itcm code to run from ITCM. Do this while the MPU is disabled because we write
    // protect it.
    for (uint32_t i = 0; i < ((size_t)&_ld_itcm_size) / 4; i++) {
        (&_ld_itcm_destination)[i] = (&_ld_itcm_flash_copy)[i];
    }

    // The first number in RBAR is the region number. When searching for a policy, the region with
    // the highest number wins. If none match, then the default policy set at enable applies.

    // Mark all the flash the same until instructed otherwise.
    MPU->RBAR = ARM_MPU_RBAR(11, 0x08000000U);
    MPU->RASR = ARM_MPU_RASR(EXECUTION, ARM_MPU_AP_FULL, NORMAL, NOT_SHAREABLE, CACHEABLE, BUFFERABLE, NO_SUBREGIONS, CPY_FLASH_REGION_SIZE);

    // This the ITCM. Set it to read-only because we've loaded everything already and it's easy to
    // accidentally write the wrong value to 0x00000000 (aka NULL).
    MPU->RBAR = ARM_MPU_RBAR(12, 0x00000000U);
    MPU->RASR = ARM_MPU_RASR(EXECUTION, ARM_MPU_AP_RO, NORMAL, NOT_SHAREABLE, CACHEABLE, BUFFERABLE, NO_SUBREGIONS, CPY_ITCM_REGION_SIZE);

    // This the DTCM.
    MPU->RBAR = ARM_MPU_RBAR(14, 0x20000000U);
    MPU->RASR = ARM_MPU_RASR(EXECUTION, ARM_MPU_AP_FULL, NORMAL, NOT_SHAREABLE, CACHEABLE, BUFFERABLE, NO_SUBREGIONS, CPY_DTCM_REGION_SIZE);

    // This is AXI SRAM (D1).
    MPU->RBAR = ARM_MPU_RBAR(15, CPY_SRAM_START_ADDR);
    MPU->RASR = ARM_MPU_RASR(EXECUTION, ARM_MPU_AP_FULL, NORMAL, NOT_SHAREABLE, CACHEABLE, BUFFERABLE, CPY_SRAM_SUBMASK, CPY_SRAM_REGION_SIZE);

    /* Enable MPU */
    ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);

    // Copy all of the data to run from DTCM.
    for (uint32_t i = 0; i < ((size_t)&_ld_dtcm_data_size) / 4; i++) {
        (&_ld_dtcm_data_destination)[i] = (&_ld_dtcm_data_flash_copy)[i];
    }

    // Clear DTCM bss.
    for (uint32_t i = 0; i < ((size_t)&_ld_dtcm_bss_size) / 4; i++) {
        (&_ld_dtcm_bss_start)[i] = 0;
    }

    // Copy all of the data to run from D1 RAM.
    for (uint32_t i = 0; i < ((size_t)&_ld_d1_ram_data_size) / 4; i++) {
        (&_ld_d1_ram_data_destination)[i] = (&_ld_d1_ram_data_flash_copy)[i];
    }

    // Clear D1 RAM bss.
    for (uint32_t i = 0; i < ((size_t)&_ld_d1_ram_bss_size) / 4; i++) {
        (&_ld_d1_ram_bss_start)[i] = 0;
    }

    SystemInit();
    __enable_irq();
    main();
}
#endif // end H7 specific code

// Low power clock variables
static volatile uint32_t systick_ms;
static RTC_HandleTypeDef _hrtc;

#if BOARD_HAS_LOW_SPEED_CRYSTAL
static uint32_t rtc_clock_frequency = LSE_VALUE;
#else
static uint32_t rtc_clock_frequency = LSI_VALUE;
#endif

safe_mode_t port_init(void) {
    HAL_Init(); // Turns on SysTick
    __HAL_RCC_SYSCFG_CLK_ENABLE();

    #if (CPY_STM32F4)
    __HAL_RCC_PWR_CLK_ENABLE();
    #endif

    stm32_peripherals_clocks_init();
    stm32_peripherals_gpio_init();

    // RTC oscillator selection is handled in peripherals/<family>/<line>/clocks.c
    __HAL_RCC_RTC_ENABLE();
    _hrtc.Instance = RTC;
    _hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
    // Divide async as little as possible so that we have rtc_clock_frequency count in subseconds.
    // This ensures our timing > 1 second is correct.
    _hrtc.Init.AsynchPrediv = 0x0;
    _hrtc.Init.SynchPrediv = rtc_clock_frequency - 1;
    _hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;

    HAL_RTC_Init(&_hrtc);
    HAL_RTCEx_EnableBypassShadow(&_hrtc);
    HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);

    // Turn off SysTick
    SysTick->CTRL = 0;

    return NO_SAFE_MODE;
}

void HAL_Delay(uint32_t delay_ms) {
    if (SysTick->CTRL != 0) {
        // SysTick is on, so use it
        uint32_t tickstart = systick_ms;
        while (systick_ms - tickstart < delay_ms) {
        }
    } else {
        mp_hal_delay_ms(delay_ms);
    }
}

uint32_t HAL_GetTick() {
    if (SysTick->CTRL != 0) {
        return systick_ms;
    } else {
        uint8_t subticks;
        uint32_t result = (uint32_t)port_get_raw_ticks(&subticks);
        return result;
    }
}


void SysTick_Handler(void) {
    systick_ms += 1;
    // Read the CTRL register to clear the SysTick interrupt.
    SysTick->CTRL;
}

void reset_port(void) {
    reset_all_pins();
    #if CIRCUITPY_AUDIOPWMIO
    audiopwmout_reset();
    #endif
    #if CIRCUITPY_BUSIO
    i2c_reset();
    spi_reset();
    uart_reset();
    #endif
    #if CIRCUITPY_SDIOIO
    sdioio_reset();
    #endif
    #if CIRCUITPY_PULSEIO || CIRCUITPY_PWMIO
    timers_reset();
    #endif
    #if CIRCUITPY_PULSEIO
    pulseout_reset();
    pulsein_reset();
    #endif
    #if CIRCUITPY_PWMIO
    pwmout_reset();
    #endif
}

void reset_to_bootloader(void) {
    NVIC_SystemReset();
}

void reset_cpu(void) {
    NVIC_SystemReset();
}

extern uint32_t _ld_heap_start, _ld_heap_end, _ld_stack_top, _ld_stack_bottom;

uint32_t *port_heap_get_bottom(void) {
    return &_ld_heap_start;
}

uint32_t *port_heap_get_top(void) {
    return &_ld_heap_end;
}

bool port_has_fixed_stack(void) {
    return false;
}

uint32_t *port_stack_get_limit(void) {
    return &_ld_stack_bottom;
}

uint32_t *port_stack_get_top(void) {
    return &_ld_stack_top;
}

extern uint32_t _ebss;

// Place the word to save just after our BSS section that gets blanked.
void port_set_saved_word(uint32_t value) {
    _ebss = value;
}

uint32_t port_get_saved_word(void) {
    return _ebss;
}

__attribute__((used)) void MemManage_Handler(void) {
    reset_into_safe_mode(MEM_MANAGE);
    while (true) {
        asm ("nop;");
    }
}

__attribute__((used)) void BusFault_Handler(void) {
    reset_into_safe_mode(MEM_MANAGE);
    while (true) {
        asm ("nop;");
    }
}

__attribute__((used)) void UsageFault_Handler(void) {
    reset_into_safe_mode(MEM_MANAGE);
    while (true) {
        asm ("nop;");
    }
}

__attribute__((used)) void HardFault_Handler(void) {
    reset_into_safe_mode(HARD_CRASH);
    while (true) {
        asm ("nop;");
    }
}

// This function is called often for timing so we cache the seconds elapsed computation based on the
// register value. The STM HAL always does shifts and conversion if we use it directly.
volatile uint32_t seconds_to_date = 0;
volatile uint32_t cached_date = 0;
volatile uint32_t seconds_to_minute = 0;
volatile uint32_t cached_hours_minutes = 0;
uint64_t port_get_raw_ticks(uint8_t *subticks) {
    // Disable IRQs to ensure we read all of the RTC registers as close in time as possible. Read
    // SSR twice to make sure we didn't read across a tick.
    __disable_irq();
    uint32_t first_ssr = (uint32_t)(RTC->SSR);
    uint32_t time = (uint32_t)(RTC->TR & RTC_TR_RESERVED_MASK);
    uint32_t date = (uint32_t)(RTC->DR & RTC_DR_RESERVED_MASK);
    uint32_t ssr = (uint32_t)(RTC->SSR);
    while (ssr != first_ssr) {
        first_ssr = ssr;
        time = (uint32_t)(RTC->TR & RTC_TR_RESERVED_MASK);
        date = (uint32_t)(RTC->DR & RTC_DR_RESERVED_MASK);
        ssr = (uint32_t)(RTC->SSR);
    }
    __enable_irq();

    uint32_t subseconds = rtc_clock_frequency - 1 - ssr;

    if (date != cached_date) {
        uint32_t year = (uint8_t)((date & (RTC_DR_YT | RTC_DR_YU)) >> 16U);
        uint8_t month = (uint8_t)((date & (RTC_DR_MT | RTC_DR_MU)) >> 8U);
        uint8_t day = (uint8_t)(date & (RTC_DR_DT | RTC_DR_DU));
        // Add 2000 since the year is only the last two digits.
        year = 2000 + (uint32_t)RTC_Bcd2ToByte(year);
        month = (uint8_t)RTC_Bcd2ToByte(month);
        day = (uint8_t)RTC_Bcd2ToByte(day);
        seconds_to_date = timeutils_seconds_since_2000(year, month, day, 0, 0, 0);
        cached_date = date;
    }
    uint32_t hours_minutes = time & (RTC_TR_HT | RTC_TR_HU | RTC_TR_MNT | RTC_TR_MNU);
    if (hours_minutes != cached_hours_minutes) {
        uint8_t hours = (uint8_t)((time & (RTC_TR_HT | RTC_TR_HU)) >> 16U);
        uint8_t minutes = (uint8_t)((time & (RTC_TR_MNT | RTC_TR_MNU)) >> 8U);
        hours = (uint8_t)RTC_Bcd2ToByte(hours);
        minutes = (uint8_t)RTC_Bcd2ToByte(minutes);
        seconds_to_minute = 60 * (60 * hours + minutes);
        cached_hours_minutes = hours_minutes;
    }
    uint8_t seconds = (uint8_t)(time & (RTC_TR_ST | RTC_TR_SU));
    seconds = (uint8_t)RTC_Bcd2ToByte(seconds);
    if (subticks != NULL) {
        *subticks = subseconds % 32;
    }

    uint64_t raw_ticks = ((uint64_t)1024) * (seconds_to_date + seconds_to_minute + seconds) + subseconds / 32;
    return raw_ticks;
}

void RTC_WKUP_IRQHandler(void) {
    supervisor_tick();
    __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&_hrtc, RTC_FLAG_WUTF);
    __HAL_RTC_WAKEUPTIMER_EXTI_CLEAR_FLAG();
}

volatile bool alarmed_already = false;
void RTC_Alarm_IRQHandler(void) {
    HAL_RTC_DeactivateAlarm(&_hrtc, RTC_ALARM_A);
    __HAL_RTC_ALARM_EXTI_CLEAR_FLAG();
    __HAL_RTC_ALARM_CLEAR_FLAG(&_hrtc, RTC_FLAG_ALRAF);
    alarmed_already = true;
}

// Enable 1/1024 second tick.
void port_enable_tick(void) {
    HAL_RTCEx_SetWakeUpTimer_IT(&_hrtc, rtc_clock_frequency / 1024 / 2, RTC_WAKEUPCLOCK_RTCCLK_DIV2);
    HAL_NVIC_SetPriority(RTC_WKUP_IRQn, 1, 0U);
    HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
}
extern volatile uint32_t autoreload_delay_ms;

// Disable 1/1024 second tick.
void port_disable_tick(void) {
    HAL_NVIC_DisableIRQ(RTC_WKUP_IRQn);
    HAL_RTCEx_DeactivateWakeUpTimer(&_hrtc);
}

void port_interrupt_after_ticks(uint32_t ticks) {
    uint64_t raw_ticks = port_get_raw_ticks(NULL) + ticks;

    RTC_AlarmTypeDef alarm;
    if (ticks > 1024) {
        timeutils_struct_time_t tm;
        timeutils_seconds_since_2000_to_struct_time(raw_ticks / 1024, &tm);
        alarm.AlarmTime.Hours = tm.tm_hour;
        alarm.AlarmTime.Minutes = tm.tm_min;
        alarm.AlarmTime.Seconds = tm.tm_sec;
        alarm.AlarmDateWeekDay = tm.tm_mday;
        // Masking here means that the value is ignored so we set none.
        alarm.AlarmMask = RTC_ALARMMASK_NONE;
    } else {
        // Masking here means that the value is ignored so we set them all. Only the subseconds
        // value matters.
        alarm.AlarmMask = RTC_ALARMMASK_ALL;
    }

    alarm.AlarmTime.SubSeconds = rtc_clock_frequency - 1 -
        ((raw_ticks % 1024) * 32);
    alarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    alarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_SET;
    // Masking here means that the bits are ignored so we set none of them.
    alarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_NONE;
    alarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
    alarm.Alarm = RTC_ALARM_A;

    HAL_RTC_SetAlarm_IT(&_hrtc, &alarm, RTC_FORMAT_BIN);
    alarmed_already = false;
}

void port_idle_until_interrupt(void) {
    // Clear the FPU interrupt because it can prevent us from sleeping.
    if (__get_FPSCR() & ~(0x9f)) {
        __set_FPSCR(__get_FPSCR() & ~(0x9f));
        (void)__get_FPSCR();
    }
    if (alarmed_already) {
        return;
    }
    __WFI();
}

// Required by __libc_init_array in startup code if we are compiling using
// -nostdlib/-nostartfiles.
void _init(void) {

}
