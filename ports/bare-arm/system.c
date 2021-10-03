/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Damien P. George
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
#include <string.h>

#define RCC     ((periph_rcc_t *)0x40023800)
#define GPIOA   ((periph_gpio_t *)0x40020000)
#define UART4   ((periph_uart_t *)0x40004C00)

typedef struct {
    volatile uint32_t CR;
    volatile uint32_t PLLCFGR;
    volatile uint32_t CFGR;
    volatile uint32_t CIR;
    uint32_t _1[8];
    volatile uint32_t AHB1ENR;
    volatile uint32_t AHB2ENR;
    volatile uint32_t AHB3ENR;
    uint32_t _2;
    volatile uint32_t APB1ENR;
    volatile uint32_t APB2ENR;
} periph_rcc_t;

typedef struct {
    volatile uint32_t MODER;
    volatile uint32_t OTYPER;
    volatile uint32_t OSPEEDR;
    volatile uint32_t PUPDR;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint16_t BSRRL;
    volatile uint16_t BSRRH;
    volatile uint32_t LCKR;
    volatile uint32_t AFR[2];
} periph_gpio_t;

typedef struct {
    volatile uint32_t SR;
    volatile uint32_t DR;
    volatile uint32_t BRR;
    volatile uint32_t CR1;
} periph_uart_t;

extern uint32_t _estack, _sidata, _sdata, _edata, _sbss, _ebss;

void Reset_Handler(void) __attribute__((naked));
void bare_main(void);

static void stm32_init(void);
static void gpio_init_alt(periph_gpio_t *gpio, int pin, int alt);

// Very simple ARM vector table.
const uint32_t isr_vector[] __attribute__((section(".isr_vector"))) = {
    (uint32_t)&_estack,
    (uint32_t)&Reset_Handler,
};

// The CPU runs this function after a reset.
void Reset_Handler(void) {
    // Set stack pointer.
    __asm volatile ("ldr sp, =_estack");

    // Copy .data section from flash to RAM.
    memcpy(&_sdata, &_sidata, (char *)&_edata - (char *)&_sdata);

    // Zero out .bss section.
    memset(&_sbss, 0, (char *)&_ebss - (char *)&_sbss);

    // SCB->CCR: enable 8-byte stack alignment for IRQ handlers, in accord with EABI.
    *((volatile uint32_t *)0xe000ed14) |= 1 << 9;

    // Initialise the cpu and peripherals.
    stm32_init();

    // Now that there is a basic system up and running, call the main application code.
    bare_main();

    // This function must not return.
    for (;;) {
    }
}

// Set up the STM32 MCU.
static void stm32_init(void) {
    // Note: default clock is internal 16MHz.
    RCC->AHB1ENR |= 1 << 0; // GPIOAEN
    RCC->APB1ENR |= 1 << 19; // UART4EN
    gpio_init_alt(GPIOA, 0, 8);
    UART4->BRR = (8 << 4) | 11; // 16MHz/(16*8.6875) = 115107 baud
    UART4->CR1 = 0x00002008; // USART enable, tx enable, rx enable
}

// Configure a GPIO pin in alternate-function mode.
static void gpio_init_alt(periph_gpio_t *gpio, int pin, int alt) {
    gpio->MODER = (gpio->MODER & ~(3 << (2 * pin))) | (2 << (2 * pin));
    // OTYPER is left as default push-pull
    // OSPEEDR is left as default low speed
    // PUPDR is left as default no-pull
    gpio->AFR[pin >> 3] = (gpio->AFR[pin >> 3] & ~(15 << (4 * (pin & 7)))) | (alt << (4 * (pin & 7)));
}

// Write a character out to the UART.
static inline void uart_write_char(int c) {
    // Wait for TXE, then write the character.
    while ((UART4->SR & (1 << 7)) == 0) {
    }
    UART4->DR = c;
}

// Send string of given length to stdout, converting \n to \r\n.
void mp_hal_stdout_tx_strn_cooked(const char *str, size_t len) {
    while (len--) {
        if (*str == '\n') {
            uart_write_char('\r');
        }
        uart_write_char(*str++);
    }
}
