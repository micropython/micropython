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

#include "supervisor/port.h"

#ifdef SAMD51
#include "hri/hri_cmcc_d51.h"
#include "hri/hri_nvmctrl_d51.h"
#endif

__attribute__((naked,noinline,aligned(16)))
static void neopixel_send_buffer_core(volatile uint32_t *clraddr, uint32_t pinMask,
                                      const uint8_t *ptr, int numBytes);

static void neopixel_send_buffer_core(volatile uint32_t *clraddr, uint32_t pinMask,
                                      const uint8_t *ptr, int numBytes) {
    asm volatile("        push    {r4, r5, r6, lr};"
                 "        add     r3, r2, r3;"
                 "loopLoad:"
                 "        ldrb r5, [r2, #0];" // r5 := *ptr
                 "        add  r2, #1;"       // ptr++
                 "        movs    r4, #128;"  // r4-mask, 0x80
                 "loopBit:"
                 "        str r1, [r0, #4];"                    // set
                 #ifdef SAMD21
                 "        movs r6, #3; d2: sub r6, #1; bne d2;" // delay 3
                 #endif
                 #ifdef SAMD51
                 "        movs r6, #3; d2: subs r6, #1; bne d2;" // delay 3
                 #endif
                 "        tst r4, r5;"                          // mask&r5
                 "        bne skipclr;"
                 "        str r1, [r0, #0];" // clr
                 "skipclr:"
                 #ifdef SAMD21
                 "        movs r6, #6; d0: sub r6, #1; bne d0;" // delay 6
                 #endif
                 #ifdef SAMD51
                 "        movs r6, #6; d0: subs r6, #1; bne d0;" // delay 6
                 #endif
                 "        str r1, [r0, #0];"   // clr (possibly again, doesn't matter)
                 #ifdef SAMD21
                 "        asr     r4, r4, #1;" // mask >>= 1
                 #endif
                 #ifdef SAMD51
                 "        asrs     r4, r4, #1;" // mask >>= 1
                 #endif
                 "        beq     nextbyte;"
                 "        uxtb    r4, r4;"
                 #ifdef SAMD21
                 "        movs r6, #2; d1: sub r6, #1; bne d1;" // delay 2
                 #endif
                 #ifdef SAMD51
                 "        movs r6, #2; d1: subs r6, #1; bne d1;" // delay 2
                 #endif
                 "        b       loopBit;"
                 "nextbyte:"
                 "        cmp r2, r3;"
                 "        bcs neopixel_stop;"
                 "        b loopLoad;"
                 "neopixel_stop:"
                 "        pop {r4, r5, r6, pc};"
                 "");
}

uint64_t next_start_raw_ticks = 0;

void common_hal_neopixel_write(const digitalio_digitalinout_obj_t* digitalinout, uint8_t *pixels, uint32_t numBytes) {
    // This is adapted directly from the Adafruit NeoPixel library SAMD21G18A code:
    // https://github.com/adafruit/Adafruit_NeoPixel/blob/master/Adafruit_NeoPixel.cpp
    // and the asm version from https://github.com/microsoft/uf2-samdx1/blob/master/inc/neopixel.h
    uint32_t  pinMask;
    PortGroup* port;

    // Wait to make sure we don't append onto the last transmission. This should only be a tick or
    // two.
    while (port_get_raw_ticks(NULL) < next_start_raw_ticks) {}

    // Turn off interrupts of any kind during timing-sensitive code.
    mp_hal_disable_all_interrupts();


    #ifdef SAMD51
    // When this routine is positioned at certain addresses, the timing logic
    // below can be too fast by about 2.5x. This is some kind of (un)fortunate code
    // positioning with respect to a cache line.
    // Theoretically we should turn on off the CMCC caches and the
    // NVM caches to ensure consistent timing. Testing shows the the NVMCTRL
    // cache disabling seems to make the difference. But turn both off to make sure.
    // It's difficult to test because additions to the code before the timing loop
    // below change instruction placement. (though this should be less true now that
    // the main code is in the cache-aligned function neopixel_send_buffer_core)
    // Testing was done by adding cache changes below the loop (so only the
    // first time through is wrong).
    //
    // Turn off instruction, data, and NVM caches to force consistent timing.
    // Invalidate existing cache entries.
    hri_cmcc_set_CFG_reg(CMCC, CMCC_CFG_DCDIS | CMCC_CFG_ICDIS);
    hri_cmcc_write_MAINT0_reg(CMCC, CMCC_MAINT0_INVALL);
    hri_nvmctrl_set_CTRLA_CACHEDIS0_bit(NVMCTRL);
    hri_nvmctrl_set_CTRLA_CACHEDIS1_bit(NVMCTRL);
    #endif

    uint32_t pin = digitalinout->pin->number;
    port    =  &PORT->Group[GPIO_PORT(pin)];  // Convert GPIO # to port register
    pinMask =  (1UL << (pin % 32));  // From port_pin_set_output_level ASF code.
    volatile uint32_t *clr = &(port->OUTCLR.reg);
    neopixel_send_buffer_core(clr, pinMask, pixels, numBytes);

    #ifdef SAMD51
    // Turn instruction, data, and NVM caches back on.
    hri_cmcc_clear_CFG_reg(CMCC, CMCC_CFG_DCDIS | CMCC_CFG_ICDIS);
    hri_nvmctrl_clear_CTRLA_CACHEDIS0_bit(NVMCTRL);
    hri_nvmctrl_clear_CTRLA_CACHEDIS1_bit(NVMCTRL);

    #endif

    // Update the next start.
    next_start_raw_ticks = port_get_raw_ticks(NULL) + 4;

    // Turn on interrupts after timing-sensitive code.
    mp_hal_enable_all_interrupts();

}
