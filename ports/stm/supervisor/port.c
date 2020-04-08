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
#include "boards/board.h"
#include "lib/timeutils/timeutils.h"

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/busio/I2C.h"
#include "common-hal/busio/SPI.h"
#include "common-hal/busio/UART.h"
#include "common-hal/pulseio/PWMOut.h"
#include "common-hal/pulseio/PulseOut.h"
#include "common-hal/pulseio/PulseIn.h"

#include "stm32f4/clocks.h"
#include "stm32f4/gpio.h"

#include "stm32f4xx_hal.h"

static RTC_HandleTypeDef _hrtc;

#if BOARD_RTC_CLOCK == RCC_RTCCLKSOURCE_LSE
#define RTC_CLOCK_FREQUENCY LSE_VALUE
#else
#define RTC_CLOCK_FREQUENCY LSI_VALUE
#endif

safe_mode_t port_init(void) {
    HAL_Init();
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();

    stm32f4_peripherals_clocks_init();
    stm32f4_peripherals_gpio_init();

    HAL_PWR_EnableBkUpAccess();
    #if BOARD_RTC_CLOCK == RCC_RTCCLKSOURCE_LSE
    __HAL_RCC_LSE_CONFIG(RCC_LSE_ON);
    while(__HAL_RCC_GET_FLAG(RCC_FLAG_LSERDY) == RESET) {}
    #else
    __HAL_RCC_LSI_ENABLE();
    #endif

    __HAL_RCC_RTC_CONFIG(BOARD_RTC_CLOCK);
    __HAL_RCC_RTC_ENABLE();
    _hrtc.Instance = RTC;
    _hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
    // Divide async as little as possible so that we have RTC_CLOCK_FREQUENCY count in subseconds.
    // This ensures our timing > 1 second is correct.
    _hrtc.Init.AsynchPrediv = 0x0;
    _hrtc.Init.SynchPrediv = RTC_CLOCK_FREQUENCY - 1;
    _hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;

    HAL_RTC_Init(&_hrtc);

    HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);

    return NO_SAFE_MODE;
}

void SysTick_Handler(void) {
    // Read the CTRL register to clear the SysTick interrupt.
    SysTick->CTRL;
    HAL_IncTick();
}

void reset_port(void) {
    reset_all_pins();
    i2c_reset();
    spi_reset();
    uart_reset();
    pwmout_reset();
    pulseout_reset();
    pulsein_reset();
}

void reset_to_bootloader(void) {

}

void reset_cpu(void) {
    NVIC_SystemReset();
}

uint32_t *port_heap_get_bottom(void) {
    return port_stack_get_limit();
}

uint32_t *port_heap_get_top(void) {
    return port_stack_get_top();
}

uint32_t *port_stack_get_limit(void) {
    return &_ebss;
}

uint32_t *port_stack_get_top(void) {
    return &_estack;
}

extern uint32_t _ebss;
// Place the word to save just after our BSS section that gets blanked.
void port_set_saved_word(uint32_t value) {
    _ebss = value;
}

uint32_t port_get_saved_word(void) {
    return _ebss;
}

void HardFault_Handler(void) {
    reset_into_safe_mode(HARD_CRASH);
    while (true) {
        asm("nop;");
    }
}

// This function is called often for timing so we cache the seconds elapsed computation based on the
// register value. The STM HAL always does shifts and conversion if we use it directly.
volatile uint32_t seconds_to_date = 0;
volatile uint32_t cached_date = 0;
volatile uint32_t seconds_to_minute = 0;
volatile uint32_t cached_hours_minutes = 0;
uint64_t port_get_raw_ticks(uint8_t* subticks) {
    uint32_t subseconds = RTC_CLOCK_FREQUENCY - (uint32_t)(RTC->SSR);
    uint32_t time = (uint32_t)(RTC->TR & RTC_TR_RESERVED_MASK);
    uint32_t date = (uint32_t)(RTC->DR & RTC_DR_RESERVED_MASK);
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
    }
    uint8_t seconds = (uint8_t)(time & (RTC_TR_ST | RTC_TR_SU));
    seconds = (uint8_t)RTC_Bcd2ToByte(seconds);
    if (subticks != NULL) {
        *subticks = subseconds % 32;
    }

    return ((uint64_t) 1024) * (seconds_to_date + seconds_to_minute + seconds) + subseconds / 32;
}

void RTC_WKUP_IRQHandler(void) {
    supervisor_tick();
    __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&_hrtc, RTC_FLAG_WUTF);
    __HAL_RTC_WAKEUPTIMER_EXTI_CLEAR_FLAG();
}
volatile bool alarmed_already = false;
void RTC_Alarm_IRQHandler(void) {
    __HAL_RTC_ALARM_CLEAR_FLAG(&_hrtc, RTC_FLAG_ALRAF);
    __HAL_RTC_ALARM_EXTI_CLEAR_FLAG();
    HAL_RTC_DeactivateAlarm(&_hrtc, RTC_ALARM_A);
    alarmed_already = true;
}

// Enable 1/1024 second tick.
void port_enable_tick(void) {
    HAL_RTCEx_SetWakeUpTimer_IT(&_hrtc, RTC_CLOCK_FREQUENCY / 1024 / 2, RTC_WAKEUPCLOCK_RTCCLK_DIV2);
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
        alarm.AlarmMask = RTC_ALARMMASK_ALL;
    } else {
        alarm.AlarmMask = RTC_ALARMMASK_NONE;
    }

    alarm.AlarmTime.SubSeconds = RTC_CLOCK_FREQUENCY -
                                 ((raw_ticks % (1024)) * 32);
    alarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    alarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_SET;
    alarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_NONE;
    alarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
    alarm.Alarm = RTC_ALARM_A;

    HAL_RTC_SetAlarm_IT(&_hrtc, &alarm, RTC_FORMAT_BIN);
    alarmed_already = false;
}

void port_sleep_until_interrupt(void) {
    // Clear the FPU interrupt because it can prevent us from sleeping.
    if (__get_FPSCR()  & ~(0x9f)) {
        __set_FPSCR(__get_FPSCR()  & ~(0x9f));
        (void) __get_FPSCR();
    }
    if (alarmed_already) {
        return;
    }
    __WFI();
}

