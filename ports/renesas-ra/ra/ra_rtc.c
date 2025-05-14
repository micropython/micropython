/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Renesas Electronics Corporation
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

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "hal_data.h"
#include "ra_config.h"
#include "ra_rtc.h"

#include <stdio.h>
#include <stdlib.h>
#include "hal_data.h"
#include "ra_rtc.h"

static R_RTC_Type *rtc_reg = R_RTC;
static R_SYSTEM_Type *system_reg = (R_SYSTEM_Type *)0x4001E000;

#if defined(VECTOR_NUMBER_RTC_ALARM)
ra_rtc_cb_t ra_rtc_func_alarm = NULL;
void *ra_rtc_param_alarm = NULL;
#endif
#if defined(VECTOR_NUMBER_RTC_PERIOD)
ra_rtc_cb_t ra_rtc_func_period = NULL;
void *ra_rtc_param_period = NULL;
#endif

static inline uint8_t int_to_bcd(int num) {
    return (uint8_t)(((num / 10) << 4) | (num % 10));
}

static inline int bcd_to_int(uint8_t bcd) {
    return (int)(((bcd >> 4) * 10) + (bcd & 0x0F));
}

int ra_rtc_get_year(void) {
    return bcd_to_int((uint8_t)rtc_reg->RYRCNT) + 2000;
}

int ra_rtc_get_month(void) {
    return bcd_to_int((uint8_t)rtc_reg->RMONCNT);
}

int ra_rtc_get_date(void) {
    return bcd_to_int((uint8_t)rtc_reg->RDAYCNT);
}

int ra_rtc_get_hour(void) {
    return bcd_to_int((uint8_t)(0x3f & rtc_reg->RHRCNT));
}

int ra_rtc_get_minute(void) {
    return bcd_to_int((uint8_t)rtc_reg->RMINCNT);
}

int ra_rtc_get_second(void) {
    return bcd_to_int((uint8_t)rtc_reg->RSECCNT);
}

int ra_rtc_get_weekday(void) {
    return bcd_to_int((uint8_t)rtc_reg->RWKCNT);
}

#if defined(VECTOR_NUMBER_RTC_PERIOD)

void ra_rtc_period_on() {
    // Enable periodic interrupt
    rtc_reg->RCR1_b.PIE = 1;
    while (!rtc_reg->RCR1_b.PIE) {
        ;
    }
    // Enable NVIC RTC Alarm interrupt
    R_BSP_IrqCfg((IRQn_Type const)RTC_PERIOD_IRQn, (uint32_t)RA_PRI_RTC_WKUP, (void *)NULL);
    R_BSP_IrqEnable((IRQn_Type const)RTC_PERIOD_IRQn);
}

void ra_rtc_period_off() {
    // Disable NVIC RTC Alarm interrupt
    R_BSP_IrqDisable((IRQn_Type const)RTC_PERIOD_IRQn);
    // Disable periodic interrupt
    rtc_reg->RCR1_b.PIE = 0;
    while (rtc_reg->RCR1_b.PIE) {
        ;
    }
}

// period
// 6 : every 1/256(s)
// 7 : every 1/128(s)
// 8 : every 1/64(s)
// 9 : every 1/32(s)
// 10 : every 1/16(s)
// 11 : every 1/8(s)
// 12 : every 1/4(s)
// 13 : every 1/2(s)
// 14 : every 1(s)
// 15 : every 2(s)
void ra_rtc_set_period_time(uint32_t period) {
    if ((period < 6) | (period > 15)) {
        return;
    }
    ra_rtc_period_off();
    uint8_t rcr1 = rtc_reg->RCR1;
    rcr1 &= (uint8_t) ~R_RTC_RCR1_PES_Msk;
    rcr1 |= (uint8_t)(period << R_RTC_RCR1_PES_Pos);
    rtc_reg->RCR1 = rcr1;
    while (rtc_reg->RCR1 != rcr1) {
        ;
    }
    ra_rtc_period_on();
}

void ra_rtc_set_period_func(void *cb, void *param) {
    ra_rtc_period_off();
    ra_rtc_func_period = (ra_rtc_cb_t)cb;
    ra_rtc_param_period = param;
    ra_rtc_period_on();
}

#endif

#if defined(VECTOR_NUMBER_RTC_ALARM)

void ra_rtc_alarm_on() {
    // Enable alarm interrupt
    rtc_reg->RCR1_b.AIE = 1;
    while (!rtc_reg->RCR1_b.AIE) {
        ;
    }
    // Enable NVIC RTC Alarm interrupt
    R_BSP_IrqCfg((IRQn_Type const)RTC_ALARM_IRQn, (uint32_t)RA_PRI_RTC_WKUP, (void *)NULL);
    R_BSP_IrqEnable((IRQn_Type const)RTC_ALARM_IRQn);
}

void ra_rtc_alarm_off() {
    // Disable NVIC RTC Alarm interrupt
    R_BSP_IrqDisable((IRQn_Type const)RTC_ALARM_IRQn);
    // Disable alarm interrupt
    rtc_reg->RCR1_b.AIE = 0;
    while (rtc_reg->RCR1_b.AIE) {
        ;
    }
}

void ra_rtc_set_alarm_time(int hour, int min, int week_flag) {
    ra_rtc_alarm_off();
    // Configure the alarm
    rtc_reg->RMINAR = (uint8_t)int_to_bcd(min);
    rtc_reg->RHRAR = (uint8_t)int_to_bcd(hour);
    if (week_flag <= 0x06) {
        rtc_reg->RWKAR = (uint8_t)week_flag;
    }
    rtc_reg->RMINAR_b.ENB = 1;
    rtc_reg->RHRAR_b.ENB = 1;
    if (week_flag <= 0x06) {
        rtc_reg->RWKAR_b.ENB = 1;
    } else {
        rtc_reg->RWKAR_b.ENB = 0;
    }
    ra_rtc_alarm_on();
}

void ra_rtc_set_alarm_func(void *cb, void *param) {
    ra_rtc_period_off();
    ra_rtc_func_alarm = (ra_rtc_cb_t)cb;
    ra_rtc_param_alarm = param;
    ra_rtc_period_on();
}

#endif

// adj: adjustment bit (number of sub clocks)
//   0 : no adjustment
//   0<:
//   0>:
// aadjp: specify adjustment period
//   0: 1 minute (RTC_PERIOD_MINUTE)
//   1: 10 seconds (RTC_PERIOD_SECOND)
void ra_rtc_set_adjustment(int adj, int aadjp) {
    int tmp_int;
    aadjp &= 1;
    if (adj == 0) {
        // no adjustment
        rtc_reg->RADJ = 0x00;
        while (rtc_reg->RADJ != 0x00) {
            ;
        }
    } else if (adj > 0) {
        // plus adjustment
        rtc_reg->RADJ = 0x00;
        while (rtc_reg->RADJ != 0x00) {
            ;
        }
        // enable auto adjustment
        rtc_reg->RCR2_b.AADJE = 1;
        while (rtc_reg->RCR2_b.AADJE != 1) {
            ;
        }
        rtc_reg->RCR2_b.AADJP =
            aadjp == RTC_PERIOD_MINUTE ? RTC_PERIOD_MINUTE : RTC_PERIOD_SECOND;
        while (rtc_reg->RCR2_b.AADJP != aadjp) {
            ;
        }
        tmp_int = 0x40 | (0x3F & adj);
        rtc_reg->RADJ = (uint8_t)tmp_int;
        while (rtc_reg->RADJ != (uint8_t)tmp_int) {
            ;
        }
    } else {
        // minus adjustment
        rtc_reg->RADJ = 0x00;
        while (rtc_reg->RADJ != 0x00) {
            ;
        }
        // enable adjustment
        rtc_reg->RCR2_b.AADJE = 1;
        while (rtc_reg->RCR2_b.AADJE != 1) {
            ;
        }
        rtc_reg->RCR2_b.AADJP =
            aadjp == RTC_PERIOD_MINUTE ? RTC_PERIOD_MINUTE : RTC_PERIOD_SECOND;
        while (rtc_reg->RCR2_b.AADJP != aadjp) {
            ;
        }
        tmp_int = 0x80 | (0x3F & abs(adj));
        rtc_reg->RADJ = (uint8_t)tmp_int;
        while (rtc_reg->RADJ != (uint8_t)tmp_int) {
            ;
        }
    }
}

uint8_t ra_rtc_get_adjustment(void) {
    return rtc_reg->RADJ;
}

bool ra_rtc_set_time(ra_rtc_t *time) {
    // Write 0 to RTC start bit
    rtc_reg->RCR2_b.START = 0x0;
    // Wait for start bit to clear
    while (0 != rtc_reg->RCR2_b.START) {
        ;
    }
    // Alarm enable bits are undefined after a reset,
    //  disable non-required alarm features
    rtc_reg->RWKAR_b.ENB = 0;
    rtc_reg->RDAYAR_b.ENB = 0;
    rtc_reg->RMONAR_b.ENB = 0;
    rtc_reg->RYRAREN_b.ENB = 0;
    // Operate RTC in 24-hr mode
    rtc_reg->RCR2_b.HR24 = 0x1;
    rtc_reg->RYRCNT = (uint16_t)int_to_bcd(time->year % 100);
    rtc_reg->RMONCNT = (uint8_t)int_to_bcd(time->month);
    rtc_reg->RDAYCNT = (uint8_t)int_to_bcd(time->date);
    rtc_reg->RHRCNT = (uint8_t)int_to_bcd(time->hour);
    rtc_reg->RMINCNT = (uint8_t)int_to_bcd(time->minute);
    rtc_reg->RSECCNT = (uint8_t)int_to_bcd(time->second);
    rtc_reg->RWKCNT = (uint8_t)int_to_bcd(time->weekday);
    // Start the clock
    rtc_reg->RCR2_b.START = 0x1;
    // Wait until the start bit is set to 1
    while (1 != rtc_reg->RCR2_b.START) {
        ;
    }
    return true;
}

bool ra_rtc_get_time(ra_rtc_t *time) {
    time->year = (uint16_t)(bcd_to_int((uint8_t)rtc_reg->RYRCNT) + 2000);
    time->month = (uint8_t)bcd_to_int(rtc_reg->RMONCNT);
    time->date = (uint8_t)bcd_to_int(rtc_reg->RDAYCNT);
    time->hour = (uint8_t)bcd_to_int((uint8_t)(0x3f & rtc_reg->RHRCNT));
    time->minute = (uint8_t)bcd_to_int((uint8_t)rtc_reg->RMINCNT);
    time->second = (uint8_t)bcd_to_int((uint8_t)rtc_reg->RSECCNT);
    time->weekday = (uint8_t)bcd_to_int((uint8_t)rtc_reg->RWKCNT);
    return true;
}

static void wait(volatile int count) {
    while (count--) {
        __asm__ __volatile__ ("nop");
    }
}

// source
// 0: subclock
// 1: LOCO
static void ra_rtc_set_subclock(uint8_t source) {
    // Set RTC clock input from sub-clock, and supply to RTC module
    rtc_reg->RCR4_b.RCKSEL = source;
    if (0 == source) {
        R_BSP_SoftwareDelay(100, BSP_DELAY_UNITS_MILLISECONDS);
    } else {
        R_BSP_SoftwareDelay(200, BSP_DELAY_UNITS_MICROSECONDS);
    }
    // Stop the clock
    rtc_reg->RCR2_b.START = 0x0;
    // Wait for start bit to clear
    while (0 != rtc_reg->RCR2_b.START) {
        ;
    }
    if (source == 1) {
        rtc_reg->RFRH = 0;
        rtc_reg->RFRL = (uint16_t)0x00ff;   // assume 32.768khz
    }
    rtc_reg->RCR2_b.CNTMD = 0;
    while (0 != rtc_reg->RCR2_b.CNTMD) {
        ;
    }
    // Reset the RTC unit
    rtc_reg->RCR2_b.RESET = 0x1;
    // Wait until reset is complete
    while (0 != rtc_reg->RCR2_b.RESET) {
        ;
    }
    // Start the clock
    rtc_reg->RCR2_b.START = 0x1;
    // Wait until the start bit is set to 1
    while (1 != rtc_reg->RCR2_b.START) {
        ;
    }
}

bool ra_rtc_init(uint8_t source) {
    system_reg->PRCR = 0xA503;
    // Check if the MCU has come from a cold start (power on reset)
    if (0 == system_reg->RSTSR2_b.CWSF) {
        // cold start
        system_reg->VBTCR1_b.BPWSWSTP = 1;
        // Set the warm start flag
        system_reg->RSTSR2_b.CWSF = 1;
        // Disable the sub-clock oscillator
        system_reg->SOSCCR_b.SOSTP = 1;
        // Wait for register modification to complete
        while (1 != system_reg->SOSCCR_b.SOSTP) {
            ;
        }
        // Start sub-clock
        system_reg->SOSCCR_b.SOSTP = 0;
        // Perform 8 delay iterations
        for (uint8_t i = 0; i < 8; i++) {
            // Wait in while loop for ~0.5 seconds
            wait(0xFFFFE);
        }
    } else {
        // Start sub-clock
        system_reg->SOSCCR_b.SOSTP = 0;
        // Wait for the register modification to complete
        while (0 != system_reg->SOSCCR_b.SOSTP) {
            ;
        }
    }
    system_reg->PRCR = 0xA500;
    // call back
    #if defined(VECTOR_NUMBER_RTC_ALARM)
    ra_rtc_func_alarm = NULL;
    #endif
    #if defined(VECTOR_NUMBER_RTC_PERIOD)
    ra_rtc_func_period = NULL;
    #endif
    if ((rtc_reg->RCR2_b.START == 0) || (rtc_reg->RCR4_b.RCKSEL != source)) {
        rtc_reg->RCR1 = 0;
        rtc_reg->RCR2 = 0;
        ra_rtc_set_subclock(source);
    }
    return true;
}

bool ra_rtc_deinit(void) {
    #if defined(VECTOR_NUMBER_RTC_ALARM)
    ra_rtc_alarm_off();
    ra_rtc_func_alarm = NULL;
    ra_rtc_param_alarm = NULL;
    #endif
    #if defined(VECTOR_NUMBER_RTC_PERIOD)
    ra_rtc_period_off();
    ra_rtc_func_period = NULL;
    ra_rtc_param_period = NULL;
    #endif
    rtc_reg->RCR4_b.RCKSEL = 1;
    rtc_reg->RCR1 = 0;
    return true;
}

void rtc_alarm_periodic_isr(void) {
    IRQn_Type irq = R_FSP_CurrentIrqGet();
    #if defined(VECTOR_NUMBER_RTC_PERIOD)
    if (irq == RTC_PERIOD_IRQn) {
        if (ra_rtc_func_period) {
            ra_rtc_func_period(ra_rtc_param_period);
        }
    }
    #endif
    #if defined(VECTOR_NUMBER_RTC_ALARM)
    if (irq == RTC_ALARM_IRQn) {
        if (ra_rtc_func_alarm) {
            ra_rtc_func_alarm(ra_rtc_param_alarm);
        }
    }
    #endif
    R_BSP_IrqStatusClear(irq);
}

#if defined(VECTOR_NUMBER_RTC_CARRY)
void rtc_carry_isr(void) {
    IRQn_Type irq = R_FSP_CurrentIrqGet();
    R_BSP_IrqStatusClear(irq);
}
#endif
