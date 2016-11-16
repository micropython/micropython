/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Glenn Ruben Bakke
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

#ifndef __NRF52_HAL
#define __NRF52_HAL

#include <stdbool.h>

#include "py/mpconfig.h"
#include "nrf.h"

typedef enum
{
  HAL_OK       = 0x00,
  HAL_ERROR    = 0x01,
  HAL_BUSY     = 0x02,
  HAL_TIMEOUT  = 0x03
} HAL_StatusTypeDef;


#ifdef NRF51
#define GPIO_BASE ((NRF_GPIO_Type *)NRF_GPIO_BASE)
#else
#define GPIO_BASE ((NRF_GPIO_Type *)NRF_P0_BASE)
#endif

typedef enum {
	HAL_GPIO_PULL_DISABLED = (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos),
	HAL_GPIO_PULL_DOWN = (GPIO_PIN_CNF_PULL_Pulldown << GPIO_PIN_CNF_PULL_Pos),
	HAL_GPIO_PULL_UP = (GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos)
} hal_gpio_pull_t;

static inline void hal_gpio_cfg_pin_output(uint32_t pin_number) {
    GPIO_BASE->PIN_CNF[pin_number] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
                                   | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
                                   | (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
                                   | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
                                   | (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
}

static inline void hal_gpio_cfg_pin_input(uint32_t pin_number, hal_gpio_pull_t pull) {
    GPIO_BASE->PIN_CNF[pin_number] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
                                   | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
								   | pull
                                   | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
								   | (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);
}


static inline void hal_gpio_out_set(uint32_t pin_mask) {
	GPIO_BASE->OUTSET = pin_mask;
}

static inline void hal_gpio_pin_set(uint32_t pin) {
	GPIO_BASE->OUTSET = (1 << pin);
}

static inline void hal_gpio_pin_clear(uint32_t pin) {
	GPIO_BASE->OUTCLR = (1 << pin);
}

static inline void hal_gpio_pin_toggle(uint32_t pin) {
	uint32_t pin_mask = (1 << pin);

	if (GPIO_BASE->OUT ^ pin_mask) {
		GPIO_BASE->OUTSET = pin_mask;
	} else {
		GPIO_BASE->OUTCLR = pin_mask;
	}
}

static inline uint32_t hal_tick_fake(void) {
	return 0;
}

#define mp_hal_ticks_ms hal_tick_fake // TODO: implement. Right now, return 0 always

extern const unsigned char mp_hal_status_to_errno_table[4];

NORETURN void mp_hal_raise(HAL_StatusTypeDef status);
void mp_hal_set_interrupt_char(int c); // -1 to disable

int mp_hal_stdin_rx_chr(void);
void mp_hal_stdout_tx_str(const char *str);

#endif

