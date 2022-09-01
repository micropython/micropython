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

#ifndef RA_RA_GPIO_H_
#define RA_RA_GPIO_H_

// clang-format off

#include <stdint.h>

typedef struct ra_af_pin {
    uint8_t af;
    uint8_t ch;
    uint32_t pin;
} ra_af_pin_t;

enum CPU_PIN {
    P000 = 0x00, P001, P002, P003, P004, P005, P006, P007, P008, P009, P010, P011, P012, P013, P014, P015,
    P100 = 0x10, P101, P102, P103, P104, P105, P106, P107, P108, P109, P110, P111, P112, P113, P114, P115,
    P200 = 0x20, P201, P202, P203, P204, P205, P206, P207, P208, P209, P210, P211, P212, P213, P214, P215,
    P300 = 0x30, P301, P302, P303, P304, P305, P306, P307, P308, P309, P310, P311, P312, P313, P314, P315,
    P400 = 0x40, P401, P402, P403, P404, P405, P406, P407, P408, P409, P410, P411, P412, P413, P414, P415,
    P500 = 0x50, P501, P502, P503, P504, P505, P506, P507, P508, P509, P510, P511, P512, P513, P514, P515,
    P600 = 0x90, P601, P602, P603, P604, P605, P606, P607, P608, P609, P610, P611, P612, P613, P614, P615,
    P700 = 0x70, P701, P702, P703, P704, P705, P706, P707, P708, P709, P710, P711, P712, P713, P714, P715,
    P800 = 0x80, P801, P802, P803, P804, P805, P806, P807, P808, P809, P810, P811, P812, P813, P814, P815,
    P900 = 0x90, P901, P902, P903, P904, P905, P906, P907, P908, P909, P910, P911, P912, P913, P914, P915,
    PIN_END = 0xff,
};

enum AF_INDEX {
    AF_GPIO = 0,
    AF_AGT = 1,
    AF_GPT1 = 2,
    AF_GPT2 = 3,
    AF_SCI1 = 4,
    AF_SCI2 = 5,
    AF_SPI = 6,
    AF_I2C = 7,
    AF_KINT = 8,
    AF_RTC = 9,
    AF_CAC = 10,
    AF_CTSU = 12,
    AF_SLCDC = 13,
    AF_CAN = 16,
    AF_SSIE = 18,
    AF_USBFS = 19,
    AF_END = 0xff,
};

#define  GPIO_MODE_INPUT        0
#define  GPIO_MODE_OUTPUT_PP    1
#define  GPIO_MODE_OUTPUT_OD    2
#define  GPIO_MODE_AF_PP        3
#define  GPIO_MODE_AF_OD        4
#define  GPIO_MODE_ANALOG       5

#define  GPIO_IRQ_FALLING       0x1
#define  GPIO_IRQ_RISING        0x2
#define  GPIO_IRQ_LOWLEVEL      0x4
#define  GPIO_IRQ_HIGHLEVEL     0x8

#define  GPIO_NOPULL            0
#define  GPIO_PULLUP            1
#define  GPIO_PULLDOWN          2

#define  GPIO_LOW_POWER         0
#define  GPIO_MID_POWER         1
#define  GPIO_MID_FAST_POWER    2
#define  GPIO_HIGH_POWER        3

#define IS_GPIO_MODE(MODE) (((MODE) == GPIO_MODE_INPUT) || \
    ((MODE) == GPIO_MODE_OUTPUT_PP) || \
    ((MODE) == GPIO_MODE_OUTPUT_OD) || \
    ((MODE) == GPIO_MODE_AF_PP) || \
    ((MODE) == GPIO_MODE_AF_OD) || \
    ((MODE) == GPIO_MODE_ANALOG))

#define IS_GPIO_DRIVE(DRIVE) (((DRIVE) == GPIO_LOW_POWER) || \
    ((DRIVE) == GPIO_MID_POWER) || \
    ((DRIVE) == GPIO_MID_FAST_POWER) || \
    ((DRIVE) == GPIO_HIGH_POWER))

#define IS_GPIO_PULL(PULL) (((PULL) == GPIO_NOPULL) || \
    ((PULL) == GPIO_PULLUP) || \
    ((PULL) == GPIO_PULLDOWN))

#define IS_GPIO_AF(AF)   ((AF) <= (uint8_t)0x1F)

#define GPIO_PORT(pin)  ((pin) >> 4)
#define GPIO_MASK(pin)  (1 << ((pin) & 0xf))
#define GPIO_BIT(pin)   ((pin) & 0xf)

#define PODR_MASK   (uint32_t)0x00000001
#define PIDR_MASK   (uint32_t)0x00000002
#define PDR_MASK    (uint32_t)0x00000004
#define PCR_MASK    (uint32_t)0x00000010
#define NCODR_MASK  (uint32_t)0x00000040
#define DSCR_MASK   (uint32_t)0x00000400
#define DSCR1_MASK  (uint32_t)0x00000800
#define EOR_MASK    (uint32_t)0x00001000
#define EOF_MASK    (uint32_t)0x00002000
#define ISEL_MASK   (uint32_t)0x00004000
#define ASEL_MASK   (uint32_t)0x00008000
#define PMR_MASK    (uint32_t)0x00010000
#define PSEL_MASK   (uint32_t)0x1f000000

#define _PWPR               (*(volatile uint8_t *)(0x40040D03))

#define _PXXPFS(port, bit)  (*(volatile uint32_t *)(0x40040800 + (0x40 * ((uint32_t)port)) + (0x4 * ((uint32_t)bit))))
#define _PCNTR1(port)       (*(volatile uint32_t *)(0x40040000 + (0x20 * (port))))
#define _PODR(port)         (*(volatile uint16_t *)(0x40040000 + (0x20 * (port))))
#define _PDR(port)          (*(volatile uint16_t *)(0x40040002 + (0x20 * (port))))
#define _PCNTR2(port)       (*(volatile uint32_t *)(0x40040004 + (0x20 * (port))))
#define _EIDR(port)         (*(volatile uint16_t *)(0x40040004 + (0x20 * (port))))
#define _PIDR(port)         (*(volatile uint16_t *)(0x40040006 + (0x20 * (port))))
#define _PCNTR3(port)       (*(volatile uint32_t *)(0x40040008 + (0x20 * (port))))
#define _PORR(port)         (*(volatile uint16_t *)(0x40040008 + (0x20 * (port))))
#define _POSR(port)         (*(volatile uint16_t *)(0x4004000a + (0x20 * (port))))
#define _PCNTR4(port)       (*(volatile uint32_t *)(0x4004000c + (0x20 * (port))))
#define _EORR(port)         (*(volatile uint16_t *)(0x4004000c + (0x20 * (port))))
#define _EOSR(port)         (*(volatile uint16_t *)(0x4004000e + (0x20 * (port))))

#define _PPXXPFS(port, bit)  ((volatile uint32_t *)(0x40040800 + (0x40 * (port)) + (0x4 * (bit))))
#define _PPCNTR1(port)       ((volatile uint32_t *)(0x40040000 + (0x20 * (port))))
#define _PPODR(port)         ((volatile uint16_t *)(0x40040000 + (0x20 * (port))))
#define _PPDR(port)          ((volatile uint16_t *)(0x40040002 + (0x20 * (port))))
#define _PPCNTR2(port)       ((volatile uint32_t *)(0x40040004 + (0x20 * (port))))
#define _PEIDR(port)         ((volatile uint16_t *)(0x40040004 + (0x20 * (port))))
#define _PPIDR(port)         ((volatile uint16_t *)(0x40040006 + (0x20 * (port))))
#define _PPCNTR3(port)       ((volatile uint32_t *)(0x40040008 + (0x20 * (port))))
#define _PPORR(port)         ((volatile uint16_t *)(0x40040008 + (0x20 * (port))))
#define _PPOSR(port)         ((volatile uint16_t *)(0x4004000a + (0x20 * (port))))
#define _PPCNTR4(port)       ((volatile uint32_t *)(0x4004000c + (0x20 * (port))))
#define _PEORR(port)         ((volatile uint16_t *)(0x4004000c + (0x20 * (port))))
#define _PEOSR(port)         ((volatile uint16_t *)(0x4004000e + (0x20 * (port))))

void ra_gpio_config(uint32_t pin, uint32_t mode, uint32_t pull, uint32_t drive, uint32_t alt);
void ra_gpio_mode_output(uint32_t pin);
void ra_gpio_mode_input(uint32_t pin);
void ra_gpio_set(uint32_t pin, uint32_t value);
uint32_t ra_gpio_get(uint32_t pin);
void ra_gpio_toggle(uint32_t pin);
void ra_gpio_write(uint32_t pin, uint32_t value);
uint32_t ra_gpio_read(uint32_t pin);
uint32_t ra_gpio_get_mode(uint32_t pin);
uint32_t ra_gpio_get_pull(uint32_t pin);
uint32_t ra_gpio_get_af(uint32_t pin);
uint32_t ra_gpio_get_drive(uint32_t pin);

inline static void pwpr_unprotect(void) {
    _PWPR &= (uint8_t) ~0x80;
    _PWPR |= (uint8_t)0x40;
}

inline static void pwpr_protect(void) {
    _PWPR &= (uint8_t) ~0x40;
    _PWPR |= (uint8_t)0x80;
}

#endif /* RA_RA_GPIO_H_ */
