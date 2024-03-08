/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Arduino SA
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

#include "esp_attr.h"
#include "hal/gpio_hal.h"

static const gpio_num_t PWDN = GPIO_NUM_42;
static const gpio_num_t LEDs[] = { 17, 18, 43, 44, 45, 46};

void setLEDs(){
	for(int i = 0; i < sizeof(LEDs)/sizeof(LEDs[0]); i++){
		const uint8_t pin = LEDs[i];
		gpio_ll_pullup_dis(&GPIO, pin);
		gpio_ll_pulldown_dis(&GPIO, pin);
		gpio_ll_input_disable(&GPIO, pin);

		gpio_ll_output_enable(&GPIO, pin);
		gpio_ll_set_level(&GPIO, pin, 0);
	}
}

void ARTEMIS_ESP32_enter_bootloader(void){
	/* Keep in my mind that a lot of functions cannot be called from here
	 * as system initialization has not been performed yet, including
	 * BSS, SPI flash, or memory protection. */
//	ESP_LOGW("HOOK", "This hook is called BEFORE bootloader initialization");

	gpio_ll_input_enable(&GPIO, PWDN);
	gpio_ll_pulldown_dis(&GPIO, PWDN);
	gpio_ll_pullup_dis(&GPIO, PWDN);

	setLEDs();
}


