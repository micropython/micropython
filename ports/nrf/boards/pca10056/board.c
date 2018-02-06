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
#include <stdbool.h>

#include "nrf.h"

#include "boards/board.h"

volatile uint32_t ticks_ms = 0;

#define HAL_LFCLK_FREQ (32768UL)
#define HAL_RTC_FREQ   (1024UL)
#define HAL_RTC_COUNTER_PRESCALER ((HAL_LFCLK_FREQ/HAL_RTC_FREQ)-1)

/* Maximum RTC ticks */
#define portNRF_RTC_MAXTICKS   ((1U<<24)-1U)

void board_init(void)
{
  // 32Khz XTAL
  NRF_CLOCK->LFCLKSRC = (uint32_t)((CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos) & CLOCK_LFCLKSRC_SRC_Msk);
  NRF_CLOCK->TASKS_LFCLKSTART = 1UL;

  // Set up RTC1 as tick timer
  NVIC_DisableIRQ(RTC1_IRQn);
  NRF_RTC1->EVTENCLR    = RTC_EVTEN_COMPARE0_Msk;
  NRF_RTC1->INTENCLR    = RTC_INTENSET_COMPARE0_Msk;
  NRF_RTC1->TASKS_STOP  = 1;
  NRF_RTC1->TASKS_CLEAR = 1;

  ticks_ms = 0;

  NRF_RTC1->PRESCALER = HAL_RTC_COUNTER_PRESCALER;
  NRF_RTC1->INTENSET = RTC_INTENSET_TICK_Msk;
  NRF_RTC1->TASKS_START = 1;
  NRF_RTC1->EVTENSET = RTC_EVTEN_OVRFLW_Msk;
  NVIC_SetPriority(RTC1_IRQn, 0xf); // lowest priority
  NVIC_EnableIRQ(RTC1_IRQn);
}

void RTC1_IRQHandler(void)
{
  // Clear event
  NRF_RTC1->EVENTS_TICK = 0;
  volatile uint32_t dummy = NRF_RTC1->EVENTS_TICK;
  (void) dummy;
  //
  //// Tick correction
  uint32_t systick_counter = NRF_RTC1->COUNTER;
  uint32_t diff = (systick_counter - ticks_ms) & portNRF_RTC_MAXTICKS;
  ticks_ms += diff;
}

bool board_requests_safe_mode(void) {
  return false;
}

void reset_board(void) {

}
