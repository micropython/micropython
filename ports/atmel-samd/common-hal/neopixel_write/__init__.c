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
#include "hpl_gpio.h"

#include "py/mphal.h"

#include "shared-bindings/neopixel_write/__init__.h"

#include "tick.h"

#ifdef SAMD51
static inline void delay_cycles(uint8_t cycles) {
    uint32_t start = SysTick->VAL;
    uint32_t stop = start - cycles;
    if (start < cycles) {
        stop = 0xffffff + start - cycles;
        while (SysTick->VAL < start || SysTick->VAL > stop) {}
    } else {
        // Make sure the systick value is between start and stop in case it
        // wraps around before we read its value less than stop.
        while (SysTick->VAL > stop && SysTick->VAL <= start) {}
    }
}
#endif

uint64_t next_start_tick_ms = 0;
uint32_t next_start_tick_us = 1000;

void common_hal_neopixel_write(const digitalio_digitalinout_obj_t* digitalinout, uint8_t *pixels, uint32_t numBytes) {
    // This is adapted directly from the Adafruit NeoPixel library SAMD21G18A code:
    // https://github.com/adafruit/Adafruit_NeoPixel/blob/master/Adafruit_NeoPixel.cpp
    uint8_t  *ptr, *end, p, bitMask;
    uint32_t  pinMask;
    PortGroup* port;

    // This must be called while interrupts are on in case we're waiting for a
    // future ms tick.
    wait_until(next_start_tick_ms, next_start_tick_us);

    // Turn off interrupts of any kind during timing-sensitive code.
    mp_hal_disable_all_interrupts();

    #ifdef SAMD21
    // Make sure the NVM cache is consistently timed.
    NVMCTRL->CTRLB.bit.READMODE = NVMCTRL_CTRLB_READMODE_DETERMINISTIC_Val;
    #endif


    uint32_t pin = digitalinout->pin->pin;
    port    =  &PORT->Group[GPIO_PORT(pin)];  // Convert GPIO # to port register
    pinMask =  (1UL << (pin % 32));  // From port_pin_set_output_level ASF code.
    ptr     =  pixels;
    end     =  ptr + numBytes;
    p       = *ptr++;
    bitMask =  0x80;

    volatile uint32_t *set = &(port->OUTSET.reg),
                      *clr = &(port->OUTCLR.reg);

    for(;;) {
        *set = pinMask;
        // This is the time where the line is always high regardless of the bit.
        // For the SK6812 its 0.3us +- 0.15us
        #ifdef SAMD21
        asm("nop; nop;");
        #endif
        #ifdef SAMD51
        delay_cycles(18);
        #endif
        if(p & bitMask) {
            // This is the high delay unique to a one bit.
            // For the SK6812 its 0.3us
            #ifdef SAMD21
            asm("nop; nop; nop; nop; nop; nop; nop;");
            #endif
            #ifdef SAMD51
            delay_cycles(25);
            #endif
            *clr = pinMask;
        } else {
            *clr = pinMask;
            // This is the low delay unique to a zero bit.
            // For the SK6812 its 0.3us
            #ifdef SAMD21
            asm("nop; nop;");
            #endif
            #ifdef SAMD51
            delay_cycles(25);
            #endif
        }
        if((bitMask >>= 1) != 0) {
            // This is the delay between bits in a byte and is the 1 code low
            // level time from the datasheet.
            // For the SK6812 its 0.6us +- 0.15us
            #ifdef SAMD21
            asm("nop; nop; nop; nop; nop;");
            #endif
            #ifdef SAMD51
            delay_cycles(44);
            #endif
        } else {
            if(ptr >= end) break;
            p       = *ptr++;
            bitMask = 0x80;
            // This is the delay between bytes. Its similar to the other branch
            // in the if statement except its tuned to account for the time the
            // above operations take.
            // For the SK6812 its 0.6us +- 0.15us
            #ifdef SAMD51
            delay_cycles(50);
            #endif
        }
    }

    #ifdef SAMD21
    // Speed up! (But inconsistent timing.)
    NVMCTRL->CTRLB.bit.READMODE = NVMCTRL_CTRLB_READMODE_NO_MISS_PENALTY_Val;
    #endif

    // ticks_ms may be out of date at this point because we stopped the
    // interrupt. We'll risk it anyway.
    current_tick(&next_start_tick_ms, &next_start_tick_us);
    if (next_start_tick_us < 100) {
        next_start_tick_ms += 1;
        next_start_tick_us = 100 - next_start_tick_us;
    } else {
        next_start_tick_us -= 100;
    }

    // Turn on interrupts after timing-sensitive code.
    mp_hal_enable_all_interrupts();

}
