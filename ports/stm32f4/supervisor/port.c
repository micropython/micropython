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
#include "tick.h"

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/busio/I2C.h"
#include "common-hal/busio/SPI.h"

#include "stm32f4/clocks.h"
#include "stm32f4/gpio.h"

#include "stm32f4xx_hal.h"

safe_mode_t port_init(void) {
	HAL_Init();
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();

	stm32f4_peripherals_clocks_init();
	stm32f4_peripherals_gpio_init();

    tick_init();
    board_init(); 

    return NO_SAFE_MODE;
}

void reset_port(void) {
	reset_all_pins();
    i2c_reset();
    spi_reset();
}

void reset_to_bootloader(void) {

}

void reset_cpu(void) {
	NVIC_SystemReset();
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
