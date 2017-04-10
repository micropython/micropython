/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
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

#include "mphalport.h"

#include "shared-bindings/neopixel_write/__init__.h"

#include "asf/common2/services/delay/delay.h"
#include "asf/sam0/drivers/port/port.h"

void common_hal_neopixel_write(const digitalio_digitalinout_obj_t* digitalinout, uint8_t *pixels, uint32_t numBytes) {
    // This is adapted directly from the Adafruit NeoPixel library SAMD21G18A code:
    // https://github.com/adafruit/Adafruit_NeoPixel/blob/master/Adafruit_NeoPixel.cpp
    uint8_t  *ptr, *end, p, bitMask;
    uint32_t  pinMask;
    PortGroup* port;

    // Turn off interrupts of any kind during timing-sensitive code.
    mp_hal_disable_all_interrupts();

    // Make sure the NVM cache is consistently timed.
    NVMCTRL->CTRLB.bit.READMODE = NVMCTRL_CTRLB_READMODE_DETERMINISTIC_Val;

    uint32_t pin = digitalinout->pin->pin;
    port    =  port_get_group_from_gpio_pin(pin);
    pinMask =  (1UL << (pin % 32));  // From port_pin_set_output_level ASF code.
    ptr     =  pixels;
    end     =  ptr + numBytes;
    p       = *ptr++;
    bitMask =  0x80;

    volatile uint32_t *set = &(port->OUTSET.reg),
                      *clr = &(port->OUTCLR.reg);

    for(;;) {
        *set = pinMask;
        asm("nop; nop;");
        if(p & bitMask) {
            asm("nop; nop; nop; nop; nop; nop; nop;");
            *clr = pinMask;
        } else {
            *clr = pinMask;
            asm("nop; nop;");
        }
        if((bitMask >>= 1) != 0) {
            asm("nop; nop; nop; nop; nop;");
        } else {
            if(ptr >= end) break;
            p       = *ptr++;
            bitMask = 0x80;
        }
    }

    // Speed up! (But inconsistent timing.)
    NVMCTRL->CTRLB.bit.READMODE = NVMCTRL_CTRLB_READMODE_NO_MISS_PENALTY_Val;

    // Turn on interrupts after timing-sensitive code.
    mp_hal_enable_all_interrupts();

    // 50us delay to let pixels latch to the data that was just sent.
    // This could be optimized to only occur before pixel writes when necessary,
    // like in the Arduino library.
    delay_us(50);
}
