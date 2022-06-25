/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2021,2022 Renesas Electronics Corporation
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
#include "hal_data.h"
#include "ra_config.h"
#include "ra_gpio.h"

void ra_gpio_config(uint32_t pin, uint32_t mode, uint32_t pull, uint32_t drive, uint32_t alt) {
    uint32_t port = GPIO_PORT(pin);
    uint32_t bit = GPIO_BIT(pin);
    pwpr_unprotect();
    switch (mode) {
        case GPIO_MODE_INPUT:
            _PXXPFS(port, bit) &= ~(PMR_MASK | ASEL_MASK);  // GPIO
            _PXXPFS(port, bit) &= ~PDR_MASK;    // input
            if (pull != 0) {
                _PXXPFS(port, bit) |= PCR_MASK;     // set pullup
            } else {
                _PXXPFS(port, bit) &= ~PCR_MASK;    // clear pullup
            }
            break;
        case GPIO_MODE_OUTPUT_PP:
            _PXXPFS(port, bit) &= ~(PMR_MASK | ASEL_MASK);  // GPIO
            _PXXPFS(port, bit) |= PDR_MASK;     // output
            _PXXPFS(port, bit) &= ~PCR_MASK;    // pullup clear
            break;
        case GPIO_MODE_OUTPUT_OD:
            _PXXPFS(port, bit) &= ~(PMR_MASK | ASEL_MASK);  // GPIO
            _PXXPFS(port, bit) |= (PDR_MASK | NCODR_MASK);
            break;
        case GPIO_MODE_AF_PP:
            _PXXPFS(port, bit) |= (PMR_MASK | PDR_MASK);
            break;
        case GPIO_MODE_AF_OD:
            _PXXPFS(port, bit) |= (PMR_MASK | PDR_MASK | NCODR_MASK);
            break;
    }
    switch (drive) {
        case GPIO_HIGH_POWER:
            _PXXPFS(port, bit) |= (DSCR1_MASK | DSCR_MASK);
            break;
        case GPIO_MED_POWER:
            _PXXPFS(port, bit) &= ~DSCR1_MASK;
            _PXXPFS(port, bit) |= DSCR_MASK;
            break;
        case GPIO_LOW_POWER:
            _PXXPFS(port, bit) &= ~(DSCR1_MASK | DSCR_MASK);
            break;
        default: /* GPIO_NOTOUCH_POWER */
            /* do not modify */
            break;
    }
    _PXXPFS(port, bit) &= ~(uint32_t)(0x1f000000);
    if (alt != 0) {
        _PXXPFS(port, bit) |= (alt << 24); // Must set PSEL when PMR is 0
        _PXXPFS(port, bit) |= PMR_MASK;
    }
    pwpr_protect();
}

void ra_gpio_mode_output(uint32_t pin) {
    uint32_t port = GPIO_PORT(pin);
    uint32_t bit = GPIO_BIT(pin);
    pwpr_unprotect();
    _PXXPFS(port, bit) &= ~(PMR_MASK | ASEL_MASK | PCR_MASK);   // GPIO
    _PXXPFS(port, bit) |= PDR_MASK;     // output
    pwpr_protect();
}

void ra_gpio_mode_input(uint32_t pin) {
    uint32_t port = GPIO_PORT(pin);
    uint32_t bit = GPIO_BIT(pin);
    pwpr_unprotect();
    _PXXPFS(port, bit) &= ~(PMR_MASK | ASEL_MASK);  // GPIO
    _PXXPFS(port, bit) &= ~PDR_MASK;    // input
    pwpr_protect();
}

void ra_gpio_toggle(uint32_t pin) {
    uint32_t port = GPIO_PORT(pin);
    uint32_t bit = GPIO_BIT(pin);
    pwpr_unprotect();
    _PXXPFS(port, bit) &= ~(PMR_MASK | ASEL_MASK);  // GPIO
    _PXXPFS(port, bit) ^= 1;
    pwpr_protect();
}

void ra_gpio_write(uint32_t pin, uint32_t value) {
    uint32_t port = GPIO_PORT(pin);
    uint32_t bit = GPIO_BIT(pin);
    pwpr_unprotect();
    _PXXPFS(port, bit) &= ~(PMR_MASK | ASEL_MASK);  // GPIO
    if (value != 0) {
        _PXXPFS(port, bit) |= 1;
    } else {
        _PXXPFS(port, bit) &= ~(uint32_t)1;
    }
    pwpr_protect();
}

uint32_t ra_gpio_read(uint32_t pin) {
    uint32_t port = GPIO_PORT(pin);
    uint32_t bit = GPIO_BIT(pin);
    return ((_PXXPFS(port, bit) &= PIDR_MASK) != 0) ? 1 : 0;
}

uint32_t ra_gpio_get_mode(uint32_t pin) {
    uint8_t mode = 0;
    uint32_t port = GPIO_PORT(pin);
    uint32_t bit = GPIO_BIT(pin);
    if ((_PXXPFS(port, bit) &= PDR_MASK) != 0) {
        mode = GPIO_MODE_OUTPUT_PP;
    } else {
        mode = GPIO_MODE_INPUT;
    }
    return mode;
}

uint32_t ra_gpio_get_pull(uint32_t pin) {
    uint8_t pull = 0;
    uint32_t port = GPIO_PORT(pin);
    uint32_t bit = GPIO_BIT(pin);
    if ((_PXXPFS(port, bit) &= PCR_MASK) != 0) {
        pull = GPIO_PULLUP;
    } else {
        pull = GPIO_NOPULL;
    }
    return pull;
}

uint32_t ra_gpio_get_af(uint32_t pin) {
    uint32_t port = GPIO_PORT(pin);
    uint32_t bit = GPIO_BIT(pin);
    return (_PXXPFS(port, bit) &= PMR_MASK) != 0;
}

uint32_t ra_gpio_get_drive(uint32_t pin) {
    uint8_t drive = 0;
    uint32_t port = GPIO_PORT(pin);
    uint32_t bit = GPIO_BIT(pin);
    switch (_PXXPFS(port, bit) &= (DSCR1_MASK | DSCR_MASK)) {
        case (DSCR1_MASK | DSCR_MASK):
            drive = GPIO_HIGH_POWER;
            break;
        case DSCR_MASK:
            drive = GPIO_MED_POWER;
            break;
        case 0:
        default:
            drive = GPIO_LOW_POWER;
            break;
    }
    return drive;
}
