/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 hathach for Adafruit Industries
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


#include "tick.h"
#include "supervisor/usb.h"
#include "lib/utils/interrupt_char.h"
#include "lib/mp-readline/readline.h"
#include "stm32f4xx.h"


void init_usb_hardware(void) {
	// Init the LED on PD14
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
  GPIOD->MODER |= GPIO_MODER_MODE14_0;

  // USB Clock init
  // PLL input- 8 MHz (External oscillator clock; HSI clock tolerance isn't
  // tight enough- 1%, need 0.25%)
  // VCO input- 1 to 2 MHz (2 MHz, M = 4)
  // VCO output- 100 to 432 MHz (144 MHz, N = 72)
  // Main PLL out- <= 180 MHz (18 MHz, P = 3- divides by 8)
  // USB PLL out- 48 MHz (Q = 3)
  RCC->PLLCFGR = RCC_PLLCFGR_PLLSRC_HSE | (3 << RCC_PLLCFGR_PLLQ_Pos) | \
    (3 << RCC_PLLCFGR_PLLP_Pos) | (72 << RCC_PLLCFGR_PLLN_Pos) | \
    (4 << RCC_PLLCFGR_PLLM_Pos);

  // Wait for external clock to become ready
  RCC->CR |= RCC_CR_HSEON;
  while(!(RCC->CR & RCC_CR_HSERDY_Msk));

  // Wait for PLL to become ready
  RCC->CR |= RCC_CR_PLLON;
  while(!(RCC->CR & RCC_CR_PLLRDY_Msk));

  // Switch clocks!
  RCC->CFGR |= RCC_CFGR_SW_1;

  // Notify runtime of frequency change.
  SystemCoreClockUpdate();

  #if CFG_TUSB_OS  == OPT_OS_NONE
  // 1ms tick timer
  SysTick_Config(SystemCoreClock / 1000);
  #endif

  RCC->AHB2ENR |= RCC_AHB2ENR_OTGFSEN;

  // USB Pin Init
  // PA9- VUSB, PA10- ID, PA11- DM, PA12- DP
  // PC0- Power on
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
  GPIOA->MODER |= GPIO_MODER_MODE9_1 | GPIO_MODER_MODE10_1 | \
    GPIO_MODER_MODE11_1 | GPIO_MODER_MODE12_1;
  GPIOA->AFR[1] |= (10 << GPIO_AFRH_AFSEL9_Pos) | \
    (10 << GPIO_AFRH_AFSEL10_Pos) | (10 << GPIO_AFRH_AFSEL11_Pos) | \
    (10 << GPIO_AFRH_AFSEL12_Pos);

  // Pullup required on ID, despite the manual claiming there's an
  // internal pullup already (page 1245, Rev 17)
  GPIOA->PUPDR |= GPIO_PUPDR_PUPD10_0;
}
