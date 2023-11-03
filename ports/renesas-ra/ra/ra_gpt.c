/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Vekatech Ltd.
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

/* FSP has available API for PWM (r_gpt)

R_GPT_Open ( ... )
R_GPT_Stop ( ... )
R_GPT_Start ( ... )
R_GPT_Reset ( ... )
R_GPT_Enable ( ... )
R_GPT_Disable ( ... )
R_GPT_PeriodSet ( ... )
R_GPT_DutyCycleSet ( ... )
R_GPT_InfoGet ( ... )
R_GPT_StatusGet ( ... )
R_GPT_CounterSet ( ... )
R_GPT_OutputEnable ( ... )
R_GPT_OutputDisable ( ... )
R_GPT_AdcTriggerSet ( ... )
R_GPT_CallbackSet ( ... )
R_GPT_Close ( ... )

and this is (The Lazy way)

   ... but looking to other drivers implementation (for example AGT [ra_timer.c/h]), Renesas want to be hard, so ...

   (The Hard way it is)
*/

#include "hal_data.h"
#include "ra_config.h"
#include "ra_gpio.h"
#include "ra_sci.h"
// #include "ra_int.h"
#include "ra_utils.h"
#include "ra_gpt.h"



#if defined(RA4M1)
#define GPT_CH_SIZE 8
#define CH_GAP 0
#elif defined(RA4W1)
#define GPT_CH_SIZE 9
#define CH_GAP (ch == 6) || (ch == 7)
#elif defined(RA6M1)
#define GPT_CH_SIZE 13
#define CH_GAP 0
#elif defined(RA6M2) || defined(RA6M3)
#define GPT_CH_SIZE 14
#define CH_GAP 0
#elif defined(RA6M5)
#define GPT_CH_SIZE 10
#define CH_GAP 0
#else
#error "CMSIS MCU Series is not specified."
#endif

enum GPT_SOURCE {
    GPT_PCLKD = 0,
    GPT_PCLKD4,
    GPT_PCLKD16,
    GPT_PCLKD64,
    GPT_PCLKD256,
    GPT_PCLKD1024
};

static R_GPT0_Type *gpt_regs[GPT_CH_SIZE] = {
    #if defined(RA4M1)
    R_GPT0,     // GPT320 General PWM Timer 0 (32-bit)
    R_GPT1,     // GPT321 General PWM Timer 1 (32-bit)
    R_GPT2,     // GPT162 General PWM Timer 2 (16-bit)
    R_GPT3,     // GPT163 General PWM Timer 3 (16-bit)
    R_GPT4,     // GPT164 General PWM Timer 4 (16-bit)
    R_GPT5,     // GPT165 General PWM Timer 5 (16-bit)
    R_GPT6,     // GPT166 General PWM Timer 6 (16-bit)
    R_GPT7,     // GPT167 General PWM Timer 7 (16-bit)
    #elif defined(RA4W1)
    R_GPT0,     // GPT320 General PWM Timer 0 (32-bit)
    R_GPT1,     // GPT321 General PWM Timer 1 (32-bit)
    R_GPT2,     // GPT322 General PWM Timer 2 (32-bit)
    R_GPT3,     // GPT323 General PWM Timer 3 (32-bit)
    R_GPT4,     // GPT164 General PWM Timer 4 (16-bit)
    R_GPT5,     // GPT165 General PWM Timer 5 (16-bit)
    NULL,       // N/A            PWM Timer 6 (Does not exist)
    NULL,       // N/A            PWM Timer 7 (Does not exist)
    R_GPT8      // GPT168 General PWM Timer 8 (16-bit)
    #elif defined(RA6M1) || defined(RA6M2) || defined(RA6M3)
    R_GPT0,     // GPT32EH0 General PWM Timer 0  (32-bit)
    R_GPT1,     // GPT32EH1 General PWM Timer 1  (32-bit)
    R_GPT2,     // GPT32EH2 General PWM Timer 2  (32-bit)
    R_GPT3,     // GPT32EH3 General PWM Timer 3  (32-bit)
    R_GPT4,     // GPT32E4  General PWM Timer 4  (32-bit)
    R_GPT5,     // GPT32E5  General PWM Timer 5  (32-bit)
    R_GPT6,     // GPT32E6  General PWM Timer 6  (32-bit)
    R_GPT7,     // GPT32E7  General PWM Timer 7  (32-bit)
    R_GPT8,     // GPT328   General PWM Timer 8  (32-bit)
    R_GPT9,     // GPT329   General PWM Timer 9  (32-bit)
    R_GPT10,    // GPT3210  General PWM Timer 10 (32-bit)
    R_GPT11,    // GPT3211  General PWM Timer 11 (32-bit)
    R_GPT12,    // GPT3212  General PWM Timer 12 (32-bit)
    #if defined(RA6M2) || defined(RA6M3)
    R_GPT13,    // GPT3213  General PWM Timer 13 (32-bit)
    #endif
    #elif defined(RA6M5)
    R_GPT0,     // GPT320 General PWM Timer 0  (32-bit)
    R_GPT1,     // GPT321 General PWM Timer 1  (32-bit)
    R_GPT2,     // GPT322 General PWM Timer 2  (32-bit)
    R_GPT3,     // GPT323 General PWM Timer 3  (32-bit)
    R_GPT4,     // GPT164 General PWM Timer 4  (16-bit)
    R_GPT5,     // GPT165 General PWM Timer 5  (16-bit)
    R_GPT6,     // GPT166 General PWM Timer 6  (16-bit)
    R_GPT7,     // GPT167 General PWM Timer 7  (16-bit)
    R_GPT8,     // GPT168 General PWM Timer 8  (16-bit)
    R_GPT9,     // GPT169 General PWM Timer 9  (16-bit)
    #else
    #error "CMSIS MCU Series is not specified."
    #endif
};

#define GPT_TMR_PINS_SIZE sizeof(ra_gpt_timer_pins) / sizeof(ra_af_pin_t)

static const ra_af_pin_t ra_gpt_timer_pins[] = {
    #if defined(RA4M1)
    { AF_GPT2, 0,  P107 }, { AF_GPT2, 0,  P213 }, { AF_GPT2, 0,  P300 }, { AF_GPT2, 0,  P415 }, // GTIOC0A
    { AF_GPT2, 0,  P106 }, { AF_GPT2, 0,  P108 }, { AF_GPT2, 0,  P212 }, { AF_GPT2, 0,  P414 }, // GTIOC0B
    { AF_GPT2, 1,  P105 }, { AF_GPT2, 1,  P109 }, { AF_GPT2, 1,  P405 },                        // GTIOC1A
    { AF_GPT2, 1,  P104 }, { AF_GPT2, 1,  P110 }, { AF_GPT2, 1,  P406 },                        // GTIOC1B
    { AF_GPT2, 2,  P103 }, { AF_GPT2, 2,  P113 }, { AF_GPT2, 2,  P500 },                        // GTIOC2A
    { AF_GPT2, 2,  P102 }, { AF_GPT2, 2,  P114 }, { AF_GPT2, 2,  P501 },                        // GTIOC2B
    { AF_GPT2, 3,  P111 }, { AF_GPT2, 3,  P403 },                                               // GTIOC3A
    { AF_GPT2, 3,  P112 }, { AF_GPT2, 3,  P404 }, { AF_GPT2, 3,  P502 },                        // GTIOC3B
    { AF_GPT2, 4,  P115 }, { AF_GPT2, 4,  P205 }, { AF_GPT2, 4,  P302 },                        // GTIOC4A
    { AF_GPT2, 4,  P204 }, { AF_GPT2, 4,  P301 }, { AF_GPT2, 4,  P608 },                        // GTIOC4B
    { AF_GPT2, 5,  P101 }, { AF_GPT2, 5,  P203 }, { AF_GPT2, 5,  P409 }, { AF_GPT2, 5,  P609 }, // GTIOC5A
    { AF_GPT2, 5,  P100 }, { AF_GPT2, 5,  P202 }, { AF_GPT2, 5,  P408 }, { AF_GPT2, 5,  P610 }, // GTIOC5B
    { AF_GPT2, 6,  P400 }, { AF_GPT2, 6,  P411 }, { AF_GPT2, 6,  P601 },                        // GTIOC6A
    { AF_GPT2, 6,  P401 }, { AF_GPT2, 6,  P410 }, { AF_GPT2, 6,  P600 },                        // GTIOC6B
    { AF_GPT2, 7,  P304 }, { AF_GPT2, 7,  P603 },                                               // GTIOC7A
    { AF_GPT2, 7,  P303 }, { AF_GPT2, 7,  P602 },                                               // GTIOC7B
    #elif defined(RA4W1)
    { AF_GPT2, 0,  P213 }, { AF_GPT2, 0,  P300 },                        // GTIOC0A
    { AF_GPT2, 0,  P108 }, { AF_GPT2, 0,  P212 }, { AF_GPT2, 0,  P414 }, // GTIOC0B
    { AF_GPT2, 1,  P105 }, { AF_GPT2, 1,  P109 },                        // GTIOC1A
    { AF_GPT2, 1,  P104 }, { AF_GPT2, 1,  P110 },                        // GTIOC1B
    { AF_GPT2, 2,  P103 },                                               // GTIOC2A
    { AF_GPT2, 2,  P102 }, { AF_GPT2, 2,  P501 },                        // GTIOC2B
    { AF_GPT2, 3,  P111 },                                               // GTIOC3A
    { AF_GPT2, 3,  P404 },                                               // GTIOC3B
    { AF_GPT2, 4,  P205 },                                               // GTIOC4A
    { AF_GPT2, 4,  P204 },                                               // GTIOC4B
    { AF_GPT2, 5,  P101 }, { AF_GPT2, 5,  P409 },                        // GTIOC5A
    { AF_GPT2, 5,  P100 },                                               // GTIOC5B
    { AF_GPT2, 8,  P107 },                                               // GTIOC8A
    { AF_GPT2, 8,  P106 },                                               // GTIOC8B
    #elif defined(RA6M1)
    { AF_GPT2, 0,  P213 }, { AF_GPT2, 0,  P415 },                        // GTIOC0A
    { AF_GPT2, 0,  P212 }, { AF_GPT2, 0,  P414 },                        // GTIOC0B
    { AF_GPT2, 1,  P105 }, { AF_GPT2, 1,  P405 },                        // GTIOC1A
    { AF_GPT2, 1,  P104 }, { AF_GPT2, 1,  P406 },                        // GTIOC1B
    { AF_GPT2, 2,  P113 },                                               // GTIOC2A
    { AF_GPT2, 2,  P114 },                                               // GTIOC2B
    { AF_GPT2, 3,  P403 },                                               // GTIOC3A
    { AF_GPT2, 3,  P404 },                                               // GTIOC3B
    { AF_GPT2, 4,  P115 }, { AF_GPT2, 4,  P205 }, { AF_GPT2, 4,  P302 }, // GTIOC4A
    { AF_GPT2, 4,  P301 }, { AF_GPT2, 4,  P608 },                        // GTIOC4B
    { AF_GPT2, 5,  P101 }, { AF_GPT2, 5,  P609 },                        // GTIOC5A
    { AF_GPT2, 5,  P100 }, { AF_GPT2, 5,  P610 },                        // GTIOC5B
    { AF_GPT2, 6,  P400 }, { AF_GPT2, 6,  P601 },                        // GTIOC6A
    { AF_GPT2, 6,  P401 }, { AF_GPT2, 6,  P600 },                        // GTIOC6B
    { AF_GPT2, 7,  P304 },                                               // GTIOC7A
    { AF_GPT2, 7,  P303 }, { AF_GPT2, 7,  P602 },                        // GTIOC7B
    { AF_GPT2, 8,  P107 },                                               // GTIOC8A
    { AF_GPT2, 8,  P106 },                                               // GTIOC8B
    { AF_GPT2, 9,  P411 },                                               // GTIOC9A
    { AF_GPT2, 9,  P410 },                                               // GTIOC9B
    { AF_GPT2, 10, P409 },                                               // GTIOC10A
    { AF_GPT2, 10, P408 },                                               // GTIOC10B
    { AF_GPT2, 11, P500 },                                               // GTIOC11A
    { AF_GPT2, 11, P501 },                                               // GTIOC11B
    { AF_GPT2, 12, P502 },                                               // GTIOC12A
    { AF_GPT2, 12, P503 },                                               // GTIOC12B
    #elif defined(RA6M2)
    { AF_GPT2, 0,  P213 }, { AF_GPT2, 0,  P415 }, { AF_GPT2, 0,  P512 },                        // GTIOC0A
    { AF_GPT2, 0,  P212 }, { AF_GPT2, 0,  P414 }, { AF_GPT2, 0,  P511 },                        // GTIOC0B
    { AF_GPT2, 1,  P105 }, { AF_GPT2, 1,  P405 },                                               // GTIOC1A
    { AF_GPT2, 1,  P104 }, { AF_GPT2, 1,  P406 },                                               // GTIOC1B
    { AF_GPT2, 2,  P113 }, { AF_GPT2, 2,  P713 },                                               // GTIOC2A
    { AF_GPT2, 2,  P114 }, { AF_GPT2, 2,  P712 },                                               // GTIOC2B
    { AF_GPT2, 3,  P403 },                                                                      // GTIOC3A
    { AF_GPT2, 3,  P404 },                                                                      // GTIOC3B
    { AF_GPT2, 4,  P115 }, { AF_GPT2, 4,  P205 }, { AF_GPT2, 4,  P302 },                        // GTIOC4A
    { AF_GPT2, 4,  P204 }, { AF_GPT2, 4,  P301 }, { AF_GPT2, 4,  P608 },                        // GTIOC4B
    { AF_GPT2, 5,  P101 }, { AF_GPT2, 5,  P203 }, { AF_GPT2, 5,  P609 }, { AF_GPT2, 5,  P700 }, // GTIOC5A
    { AF_GPT2, 5,  P100 }, { AF_GPT2, 5,  P202 }, { AF_GPT2, 5,  P610 }, { AF_GPT2, 5,  P701 }, // GTIOC5B
    { AF_GPT2, 6,  P400 }, { AF_GPT2, 6,  P601 }, { AF_GPT2, 6,  P702 },                        // GTIOC6A
    { AF_GPT2, 6,  P401 }, { AF_GPT2, 6,  P600 }, { AF_GPT2, 6,  P703 },                        // GTIOC6B
    { AF_GPT2, 7,  P304 }, { AF_GPT2, 7,  P603 },                                               // GTIOC7A
    { AF_GPT2, 7,  P303 }, { AF_GPT2, 7,  P602 },                                               // GTIOC7B
    { AF_GPT2, 8,  P107 }, { AF_GPT2, 8,  P605 },                                               // GTIOC8A
    { AF_GPT2, 8,  P106 }, { AF_GPT2, 8,  P604 },                                               // GTIOC8B
    { AF_GPT2, 9,  P411 },                                                                      // GTIOC9A
    { AF_GPT2, 9,  P410 },                                                                      // GTIOC9B
    { AF_GPT2, 10, P409 },                                                                      // GTIOC10A
    { AF_GPT2, 10, P408 },                                                                      // GTIOC10B
    { AF_GPT2, 11, P500 },                                                                      // GTIOC11A
    { AF_GPT2, 11, P501 },                                                                      // GTIOC11B
    { AF_GPT2, 12, P502 },                                                                      // GTIOC12A
    { AF_GPT2, 12, P503 },                                                                      // GTIOC12B
    { AF_GPT2, 13, P504 },                                                                      // GTIOC13A
    { AF_GPT2, 13, P505 },                                                                      // GTIOC13B
    #elif defined(RA6M3)
    { AF_GPT2, 0,  P213 }, { AF_GPT2, 0,  P415 }, { AF_GPT2, 0,  P512 },                        // GTIOC0A
    { AF_GPT2, 0,  P212 }, { AF_GPT2, 0,  P414 }, { AF_GPT2, 0,  P511 },                        // GTIOC0B
    { AF_GPT2, 1,  P105 }, { AF_GPT2, 1,  P405 },                                               // GTIOC1A
    { AF_GPT2, 1,  P104 }, { AF_GPT2, 1,  P406 },                                               // GTIOC1B
    { AF_GPT2, 2,  P113 }, { AF_GPT2, 2,  P713 },                                               // GTIOC2A
    { AF_GPT2, 2,  P114 }, { AF_GPT2, 2,  P712 },                                               // GTIOC2B
    { AF_GPT2, 3,  P403 },                                                                      // GTIOC3A
    { AF_GPT2, 3,  P404 },                                                                      // GTIOC3B
    { AF_GPT2, 4,  P115 }, { AF_GPT2, 4,  P205 }, { AF_GPT2, 4,  P302 },                        // GTIOC4A
    { AF_GPT2, 4,  P204 }, { AF_GPT2, 4,  P301 }, { AF_GPT2, 4,  P608 },                        // GTIOC4B
    { AF_GPT2, 5,  P101 }, { AF_GPT2, 5,  P203 }, { AF_GPT2, 5,  P609 }, { AF_GPT2, 5,  P700 }, // GTIOC5A
    { AF_GPT2, 5,  P100 }, { AF_GPT2, 5,  P202 }, { AF_GPT2, 5,  P610 }, { AF_GPT2, 5,  P701 }, // GTIOC5B
    { AF_GPT2, 6,  P400 }, { AF_GPT2, 6,  P601 }, { AF_GPT2, 6,  P702 },                        // GTIOC6A
    { AF_GPT2, 6,  P401 }, { AF_GPT2, 6,  P600 }, { AF_GPT2, 6,  P703 },                        // GTIOC6B
    { AF_GPT2, 7,  P304 }, { AF_GPT2, 7,  P603 },                                               // GTIOC7A
    { AF_GPT2, 7,  P303 }, { AF_GPT2, 7,  P602 },                                               // GTIOC7B
    { AF_GPT2, 8,  P107 }, { AF_GPT2, 8,  P605 },                                               // GTIOC8A
    { AF_GPT2, 8,  P106 }, { AF_GPT2, 8,  P604 },                                               // GTIOC8B
    { AF_GPT2, 9,  P411 },                                                                      // GTIOC9A
    { AF_GPT2, 9,  P410 },                                                                      // GTIOC9B
    { AF_GPT2, 10, P409 },                                                                      // GTIOC10A
    { AF_GPT2, 10, P408 },                                                                      // GTIOC10B
    { AF_GPT2, 11, P500 },                                                                      // GTIOC11A
    { AF_GPT2, 11, P501 },                                                                      // GTIOC11B
    { AF_GPT2, 12, P502 }, { AF_GPT2, 12, P908 },                                               // GTIOC12A
    { AF_GPT2, 12, P503 }, { AF_GPT2, 12, P907 },                                               // GTIOC12B
    { AF_GPT2, 13, P504 }, { AF_GPT2, 13, P906 },                                               // GTIOC13A
    { AF_GPT2, 13, P505 }, { AF_GPT2, 13, P905 },                                               // GTIOC13B
    #elif defined(RA6M5)
    { AF_GPT2, 0,  P213 }, { AF_GPT2, 0,  P300 }, { AF_GPT2, 0,  P415 }, { AF_GPT2, 0,  P512 }, // GTIOC0A
    { AF_GPT2, 0,  P108 }, { AF_GPT2, 0,  P212 }, { AF_GPT2, 0,  P414 }, { AF_GPT2, 0,  P511 }, // GTIOC0B
    { AF_GPT2, 1,  P105 }, { AF_GPT2, 1,  P109 }, { AF_GPT2, 1,  P405 },                        // GTIOC1A
    { AF_GPT2, 1,  P104 }, { AF_GPT2, 1,  P110 }, { AF_GPT2, 1,  P406 },                        // GTIOC1B
    { AF_GPT2, 2,  P103 }, { AF_GPT2, 2,  P113 }, { AF_GPT2, 2,  P713 },                        // GTIOC2A
    { AF_GPT2, 2,  P102 }, { AF_GPT2, 2,  P114 }, { AF_GPT2, 2,  P712 },                        // GTIOC2B
    { AF_GPT2, 3,  P111 }, { AF_GPT2, 3,  P403 },                                               // GTIOC3A
    { AF_GPT2, 3,  P112 }, { AF_GPT2, 3,  P404 },                                               // GTIOC3B
    { AF_GPT2, 4,  P115 }, { AF_GPT2, 4,  P205 }, { AF_GPT2, 4,  P302 },                        // GTIOC4A
    { AF_GPT2, 4,  P204 }, { AF_GPT2, 4,  P301 }, { AF_GPT2, 4,  P608 },                        // GTIOC4B
    { AF_GPT2, 5,  P101 }, { AF_GPT2, 5,  P203 }, { AF_GPT2, 5,  P609 }, { AF_GPT2, 5,  P700 }, // GTIOC5A
    { AF_GPT2, 5,  P100 }, { AF_GPT2, 5,  P202 }, { AF_GPT2, 5,  P610 }, { AF_GPT2, 5,  P701 }, // GTIOC5B
    { AF_GPT2, 6,  P400 }, { AF_GPT2, 6,  P407 }, { AF_GPT2, 6,  P601 }, { AF_GPT2, 6,  P702 }, // GTIOC6A
    { AF_GPT2, 6,  P401 }, { AF_GPT2, 6,  P408 }, { AF_GPT2, 6,  P600 }, { AF_GPT2, 6,  P703 }, // GTIOC6B
    { AF_GPT2, 7,  P304 }, { AF_GPT2, 7,  P603 },                                               // GTIOC7A
    { AF_GPT2, 7,  P303 }, { AF_GPT2, 7,  P602 },                                               // GTIOC7B
    { AF_GPT2, 8,  P107 }, { AF_GPT2, 8,  P605 },                                               // GTIOC8A
    { AF_GPT2, 8,  P106 }, { AF_GPT2, 8,  P604 },                                               // GTIOC8B
    { AF_GPT2, 9,  P411 },                                                                      // GTIOC9A
    { AF_GPT2, 9,  P410 },                                                                      // GTIOC9B
    #else
    #error "CMSIS MCU Series is not specified."
    #endif
};

static float ra_gpt_freq[GPT_CH_SIZE];
static volatile uint16_t ra_gpt_div[GPT_CH_SIZE];

static void ra_gpt_timer_set_pin(uint32_t pin) {
    bool find = false;
    uint32_t ch;
    uint32_t af;
    find = ra_af_find_ch_af((ra_af_pin_t *)&ra_gpt_timer_pins, GPT_TMR_PINS_SIZE, pin, &ch, &af);
    if (find) {             // GPIO_MODE_OUTPUT_PP
        ra_gpio_config(pin, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_LOW_POWER, af);
    }
}

static void ra_gpt_timer_release_pin(uint32_t pin) {
    bool find = false;
    uint32_t ch;
    uint32_t af;
    find = ra_af_find_ch_af((ra_af_pin_t *)&ra_gpt_timer_pins, GPT_TMR_PINS_SIZE, pin, &ch, &af);
    if (find) {                                                    // GPIO_PULLUP
        ra_gpio_config(pin, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_LOW_POWER, AF_GPIO);
    }
}

bool ra_gpt_timer_is_pwm_pin(uint32_t pin) {
    uint32_t ch;
    uint32_t af;
    return ra_af_find_ch_af((ra_af_pin_t *)&ra_gpt_timer_pins, GPT_TMR_PINS_SIZE, pin, &ch, &af);
}

void ra_gpt_timer_start(uint32_t ch) {
    if (CH_GAP || (ch >= GPT_CH_SIZE)) {
        return;
    }
    gpt_regs[ch]->GTCR_b.CST = 1;
}

void ra_gpt_timer_stop(uint32_t ch) {
    if (CH_GAP || (ch >= GPT_CH_SIZE)) {
        return;
    }
    gpt_regs[ch]->GTCR_b.CST = 0;
}

void ra_gpt_timer_set_freq(uint32_t ch, float freq) {
    R_GPT0_Type *gpt_reg = gpt_regs[ch];
    uint8_t source = 0;
    uint32_t period = 0;
    ra_gpt_freq[ch] = 0.0f;

    if ((!gpt_reg) || (ch >= GPT_CH_SIZE) || (freq == ra_gpt_freq[ch]) || (freq == 0.0f)) {
        return;
    }

    ra_gpt_div[ch] = 0;

    #ifdef RA4M1
    if (ch <= 1) { // 32bit
    #elif defined(RA4W1) || defined(RA6M5)
    if (ch <= 3) { // 32bit
    #elif defined(RA6M1) || defined(RA6M2) || defined(RA6M3)
    if (1) { // all is 32bit
    #else
    #error What is the width of the timer? (How many bits)
        #endif
        // float min_freq = ((float)PCLK)/4294967295.0f;
        if ((freq > (float)(PCLK / 2)) || (freq <= 0.0f)) {
            return;
        } else {
            source = GPT_PCLKD;
            period = (uint32_t)((float)(PCLK) / freq);
        }
    } else { // 16bit
        float min_freq = ((float)PCLK) / 65565.0f;

        if (freq > (float)(PCLK / 2)) {
            return;
        } else if (freq > min_freq) {
            source = GPT_PCLKD;
            period = (uint16_t)((float)(PCLK) / freq);
        } else if (freq > (min_freq / 4.0f)) {
            source = GPT_PCLKD4;
            ra_gpt_div[ch] = 4;
            period = (uint16_t)(((float)(PCLK) / 4.0f) / freq);
        } else if (freq > (min_freq / 16.0f)) {
            source = GPT_PCLKD16;
            ra_gpt_div[ch] = 16;
            period = (uint16_t)(((float)(PCLK) / 16.0f) / freq);
        } else if (freq > (min_freq / 64.0f)) {
            source = GPT_PCLKD64;
            ra_gpt_div[ch] = 64;
            period = (uint16_t)(((float)(PCLK) / 64.0f) / freq);
        } else if (freq > (min_freq / 256.0f)) {
            source = GPT_PCLKD256;
            ra_gpt_div[ch] = 256;
            period = (uint16_t)(((float)(PCLK) / 256.0f) / freq);
        } else if (freq > (min_freq / 1024.0f)) {
            source = GPT_PCLKD1024;
            ra_gpt_div[ch] = 1024;
            period = (uint16_t)(((float)(PCLK) / 1024.0f) / freq);
        } else {
            return;
        }
    }

    ra_gpt_freq[ch] = freq;

    float dutyA = (gpt_reg->GTIOR_b.OAE && gpt_reg->GTCCR[0] && gpt_reg->GTPR)? (float)(gpt_reg->GTCCR[0] + 1) / (float)(gpt_reg->GTPR + 1) : 0.0f;
    float dutyB = (gpt_reg->GTIOR_b.OBE && gpt_reg->GTCCR[0] && gpt_reg->GTPR)? (float)(gpt_reg->GTCCR[1] + 1) / (float)(gpt_reg->GTPR + 1) : 0.0f;

    if (gpt_reg->GTCR_b.CST) { // running
        if ((gpt_reg->GTCR >> 23) != source) {    // Can't use gpt_reg->GTCR_b.TPCS this structure is declared wrong by FPS v2.3.0 so get the whole reg
            gpt_reg->GTCR_b.CST = 0;               // stop counter
            gpt_reg->GTCR_b.TPCS = source << 1;    // Again gpt_reg->GTCR_b.TPCS is wrong and incoming value should be shifted
            gpt_reg->GTPR = period - 1;
            gpt_reg->GTPBR = period - 1;

            if (dutyA > 0.0f) {
                dutyA = dutyA * ((float)(period));
                gpt_reg->GTCCR[0] = (uint32_t)(dutyA - 1.0f);
                gpt_reg->GTCCR[2] = gpt_reg->GTCCR[0];
            }

            if (dutyB > 0.0f) {
                dutyB = dutyB * ((float)(period));
                gpt_reg->GTCCR[1] = (uint32_t)(dutyB - 1.0f);
                gpt_reg->GTCCR[3] = gpt_reg->GTCCR[1];
            }

            gpt_reg->GTCNT = 0ul;        // clear counter
            gpt_reg->GTCR_b.CST = 1;     // start counter
        } else {
            // gpt_reg->GTPR = period-1;
            gpt_reg->GTPBR = period - 1;

            if (dutyA > 0.0f) {
                dutyA = dutyA * ((float)(period));
                // gpt_reg->GTCCR[0] = (uint32_t)(dutyA - 1.0f);
                gpt_reg->GTCCR[2] = (uint32_t)(dutyA - 1.0f);
            }

            if (dutyB > 0.0f) {
                dutyB = dutyB * ((float)(period));
                // gpt_reg->GTCCR[1] = (uint32_t)(dutyB - 1.0f);
                gpt_reg->GTCCR[3] = (uint32_t)(dutyB - 1.0f);
            }
        }
    } else { // stopped
        gpt_reg->GTCR_b.TPCS = source << 1;
        gpt_reg->GTPR = period - 1;
        gpt_reg->GTPBR = period - 1;

        if (dutyA > 0.0f) {
            dutyA = dutyA * ((float)(period));
            gpt_reg->GTCCR[0] = (uint32_t)(dutyA - 1.0f);
            gpt_reg->GTCCR[2] = gpt_reg->GTCCR[0];
        }

        if (dutyB > 0.0f) {
            dutyB = dutyB * ((float)(period));
            gpt_reg->GTCCR[1] = (uint32_t)(dutyB - 1.0f);
            gpt_reg->GTCCR[3] = gpt_reg->GTCCR[1];
        }
    }
}

float ra_gpt_timer_get_freq(uint32_t ch) {
    return (CH_GAP || (ch >= GPT_CH_SIZE))? 0.0f : ra_gpt_freq[ch];
}

float ra_gpt_timer_tick_time(uint32_t ch) {
    if (CH_GAP || (ch >= GPT_CH_SIZE)) {
        return 0.0f;
    } else {
        float NS = 1000000000.0f;
        if (ra_gpt_div[ch]) {
            NS *= (float)ra_gpt_div[ch];
        }
        return NS / (float)(PCLK);
    }
}

/*
void ra_gpt_timer_set_period(uint32_t ch, uint32_t ns)
{
    R_GPT0_Type *gpt_reg = gpt_regs[ch];

    if((!gpt_reg) || (ch >= GPT_CH_SIZE))
        return;

    if(ch <= 3)
    {  //32bit
    }
    else
    {  //16bit
    }
}
*/

uint32_t ra_gpt_timer_get_period(uint32_t ch) {
    R_GPT0_Type *gpt_reg = gpt_regs[ch];

    if ((!gpt_reg) || (ch >= GPT_CH_SIZE)) {
        return 0ul;
    } else {
        return gpt_reg->GTPR + 1;
    }
}

void ra_gpt_timer_set_duty(uint32_t ch, uint8_t id, uint32_t duty) {
    R_GPT0_Type *gpt_reg = gpt_regs[ch];

    if ((!gpt_reg) || (ch >= GPT_CH_SIZE)) {
        return;
    } else {
        if (gpt_reg->GTCR_b.CST) {
            gpt_reg->GTCCR[(id == 'A')? 2 : 3] = duty? duty - 1 : duty;
        } else {
            gpt_reg->GTCCR[(id == 'A')? 0 : 1] = duty? duty - 1 : duty;
            if (id == 'A') {
                gpt_reg->GTCCR[2] = gpt_reg->GTCCR[0];
            } else {
                gpt_reg->GTCCR[3] = gpt_reg->GTCCR[1];
            }
        }
    }
}

uint32_t ra_gpt_timer_get_duty(uint32_t ch, uint8_t id) {
    R_GPT0_Type *gpt_reg = gpt_regs[ch];
    if ((!gpt_reg) || (ch >= GPT_CH_SIZE)) {
        return 0ul;
    } else
    if (id == 'A') {
        return (gpt_reg->GTCCR[0])? gpt_reg->GTCCR[0] + 1 : 0;
    } else {
        return (gpt_reg->GTCCR[1])? gpt_reg->GTCCR[1] + 1 : 0;
    }
}

void ra_gpt_timer_init(uint32_t pwm_pin, uint32_t ch, uint8_t id, uint32_t duty, float freq) {
    R_GPT0_Type *gpt_reg = gpt_regs[ch];

    if ((!gpt_reg) || (ch >= GPT_CH_SIZE)) {
        return;
    }

    #ifdef RA4M1
    if (ch <= 1) {
    #elif defined(RA4W1)
    if (ch <= 3) {
    #elif defined(RA6M1) || defined(RA6M2) || defined(RA6M3)
    if (ch <= 7) {
    #elif defined(RA6M5)
    if (ch <= 9) {
    #else
    #error Choose proper clock enable BIT!
        #endif
        #ifdef RA6M5
        // R_MSTP_MSTPCRE_MSTPE22_Msk - R_MSTP_MSTPCRE_MSTPE31_Msk
        ra_mstpcre_start(1UL << (31 - ch));
        #else
        ra_mstpcrd_start(R_MSTP_MSTPCRD_MSTPD5_Msk);
        #endif
    } else {
        ra_mstpcrd_start(R_MSTP_MSTPCRD_MSTPD6_Msk);
    }

    ra_gpt_timer_stop(ch);                          // Stop the timer
    gpt_reg->GTCR_b.MD = 0;                         // Set operating mode
    gpt_reg->GTUDDTYC_b.UD = 1;                     // Set count direction
    gpt_reg->GTBER_b.PR = 1;                        // Set buffer operation for the period
    ra_gpt_timer_set_freq(ch, freq);                // Set count clock & cycle

    gpt_reg->GTCNT = 0;                             // Set initial value for counter
    if (id == 'A') {
        gpt_reg->GTIOR_b.GTIOA = 0x9;               // Set GTIOC pin function (Initial low -> Low at GTCCRA/B compare match -> High at cycle end)
        gpt_reg->GTIOR_b.OAE = 1;                   // Enable GTIOC pin output
        gpt_reg->GTBER_b.CCRA = 1;                  // Set buffer operation for the duty
        gpt_reg->GTCCR[0] = duty? duty - 1 : duty;  // Set compare match value
        gpt_reg->GTCCR[2] = gpt_reg->GTCCR[0];
    } else {
        gpt_reg->GTIOR_b.GTIOB = 0x9;
        gpt_reg->GTIOR_b.OBE = 1;
        gpt_reg->GTBER_b.CCRB = 1;
        gpt_reg->GTCCR[1] = duty? duty - 1 : duty;
        gpt_reg->GTCCR[3] = gpt_reg->GTCCR[1];
    }

    ra_gpt_timer_set_pin(pwm_pin);

    if (gpt_reg->GTPR && gpt_reg->GTCCR[(id == 'A')? 0 : 1]) {
        ra_gpt_timer_start(ch);
    }
}

void ra_gpt_timer_deinit(uint32_t pwm_pin, uint32_t ch, uint8_t id) {
    if (CH_GAP || (ch >= GPT_CH_SIZE)) {
        return;
    }
    ra_gpt_timer_stop(ch);

    ra_gpt_div[ch] = 0;
    ra_gpt_freq[ch] = 0;

    gpt_regs[ch]->GTPR = 0;
    gpt_regs[ch]->GTPBR = 0;

    if (id == 'A') {
        gpt_regs[ch]->GTIOR_b.OAE = 0;
        gpt_regs[ch]->GTCCR[0] = 0;
        gpt_regs[ch]->GTCCR[2] = 0;
    } else {
        gpt_regs[ch]->GTIOR_b.OBE = 0;
        gpt_regs[ch]->GTCCR[1] = 0;
        gpt_regs[ch]->GTCCR[3] = 0;
    }

    #ifdef RA4M1
    if (ch <= 1) {
    #elif defined(RA4W1)
    if (ch <= 3) {
    #elif defined(RA6M1) || defined(RA6M2) || defined(RA6M3)
    if (ch <= 7) {
    #elif defined(RA6M5)
    if (ch <= 9) {
    #else
    #error Choose proper clock enable BIT!
        #endif
        #ifdef RA6M5
        ra_mstpcre_stop(1UL << (31 - ch));
        #else
        ra_mstpcrd_stop(R_MSTP_MSTPCRD_MSTPD5_Msk);
        #endif
    } else {
        ra_mstpcrd_stop(R_MSTP_MSTPCRD_MSTPD6_Msk);
    }

    ra_gpt_timer_release_pin(pwm_pin);
}
