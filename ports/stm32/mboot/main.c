/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017-2019 Damien P. George
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

#include <stdio.h>
#include <string.h>

#include "py/mphal.h"
#include "lib/crypto-algorithms/sha256.c"
#include "boardctrl.h"
#include "usbd_core.h"
#include "storage.h"
#include "flash.h"
#include "i2cslave.h"
#include "irq.h"
#include "mboot.h"
#include "powerctrl.h"
#include "dfu.h"
#include "pack.h"

// Whether the bootloader will leave via reset, or direct jump to the application.
#ifndef MBOOT_LEAVE_BOOTLOADER_VIA_RESET
#define MBOOT_LEAVE_BOOTLOADER_VIA_RESET (1)
#endif

// This option selects whether to use explicit polling or IRQs for USB events.
// In some test cases polling mode can run slightly faster, but it uses more power.
// Polling mode will also cause failures with the mass-erase command because USB
// events will not be serviced for the duration of the mass erase.
// With STM32WB MCUs only non-polling/IRQ mode is supported.
#define USE_USB_POLLING (0)

// Using cache probably won't make it faster because we run at a low frequency, and best
// to keep the MCU config as minimal as possible.
#define USE_CACHE (0)

// IRQ priorities (encoded values suitable for NVIC_SetPriority)
// Most values are defined in irq.h.
#define IRQ_PRI_I2C (NVIC_EncodePriority(NVIC_PRIORITYGROUP_4, 1, 0))

// Configure PLL to give the desired CPU freq
#undef MICROPY_HW_FLASH_LATENCY
#if defined(STM32F4) || defined(STM32F7)
  #if MBOOT_ENABLE_PACKING
    // With encryption/signing/compression, a faster CPU makes processing much faster.
    #define CORE_PLL_FREQ (96000000)
    #define MICROPY_HW_FLASH_LATENCY FLASH_LATENCY_3
  #else
    #define CORE_PLL_FREQ (48000000)
    #define MICROPY_HW_FLASH_LATENCY FLASH_LATENCY_1
  #endif
#elif defined(STM32H7)
  #define CORE_PLL_FREQ (96000000)
  #define MICROPY_HW_FLASH_LATENCY FLASH_LATENCY_2
#endif
#undef MICROPY_HW_CLK_PLLM
#undef MICROPY_HW_CLK_PLLN
#undef MICROPY_HW_CLK_PLLP
#undef MICROPY_HW_CLK_PLLQ
#undef MICROPY_HW_CLK_PLLR
#define MICROPY_HW_CLK_PLLM (HSE_VALUE / 1000000)
#define MICROPY_HW_CLK_PLLN (192)
#define MICROPY_HW_CLK_PLLP (MICROPY_HW_CLK_PLLN / (CORE_PLL_FREQ / 1000000))
#define MICROPY_HW_CLK_PLLQ (4)
#define MICROPY_HW_CLK_PLLR (2)

// Work out which USB device to use for the USB DFU interface
#if !defined(MICROPY_HW_USB_MAIN_DEV)
#if MICROPY_HW_USB_FS
#define MICROPY_HW_USB_MAIN_DEV (USB_PHY_FS_ID)
#elif MICROPY_HW_USB_HS
#define MICROPY_HW_USB_MAIN_DEV (USB_PHY_HS_ID)
#else
#error Unable to determine proper MICROPY_HW_USB_MAIN_DEV to use
#endif
#endif

// These bits are used to detect valid application firmware at APPLICATION_ADDR
#define APP_VALIDITY_BITS (0x00000003)

// For 1ms system ticker.
static volatile uint32_t systick_ms;

// Global dfu state
dfu_context_t dfu_context SECTION_NOZERO_BSS;

uint32_t get_le32(const uint8_t *b) {
    return b[0] | b[1] << 8 | b[2] << 16 | b[3] << 24;
}

mp_uint_t mp_hal_ticks_ms(void) {
    return systick_ms;
}

void mp_hal_delay_us(mp_uint_t usec) {
    // use a busy loop for the delay
    // sys freq is always a multiple of 2MHz, so division here won't lose precision
    #if defined(CORE_PLL_FREQ)
    const uint32_t ucount = CORE_PLL_FREQ / 2000000 * usec / 2;
    #else
    const uint32_t ucount = SystemCoreClock / 2000000 * usec / 2;
    #endif
    for (uint32_t count = 0; ++count <= ucount;) {
        __NOP();
    }
}

void mp_hal_delay_ms(mp_uint_t ms) {
    if (__get_PRIMASK() == 0) {
        // IRQs enabled, use systick
        if (ms != 0 && ms != (mp_uint_t)-1) {
            ++ms; // account for the fact that systick_ms may roll over immediately
        }
        uint32_t start = systick_ms;
        while (systick_ms - start < ms) {
            __WFI();
        }
    } else {
        // IRQs disabled, so need to use a busy loop for the delay.
        // To prevent possible overflow of the counter we use a double loop.
        const uint32_t count_1ms = 16000000 / 8000;
        for (uint32_t i = 0; i < ms; i++) {
            for (volatile uint32_t count = 0; ++count <= count_1ms;) {
            }
        }
    }
}

// Needed by parts of the HAL
uint32_t HAL_GetTick(void) {
    return systick_ms;
}

// Needed by parts of the HAL
void HAL_Delay(uint32_t ms) {
    mp_hal_delay_ms(ms);
}

NORETURN static void __fatal_error(const char *msg) {
    NVIC_SystemReset();
    for (;;) {
    }
}

/******************************************************************************/
// CLOCK

void systick_init(void) {
    // Configure SysTick as 1ms ticker
    SysTick_Config(SystemCoreClock / 1000);
    NVIC_SetPriority(SysTick_IRQn, IRQ_PRI_SYSTICK);
}

#if defined(STM32F4) || defined(STM32F7)

void SystemClock_Config(void) {
    // This function assumes that HSI is used as the system clock (see RCC->CFGR, SWS bits)

    // Enable Power Control clock
    __HAL_RCC_PWR_CLK_ENABLE();

    // Reduce power consumption
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    // Turn HSE on
    __HAL_RCC_HSE_CONFIG(RCC_HSE_ON);
    while (__HAL_RCC_GET_FLAG(RCC_FLAG_HSERDY) == RESET) {
    }

    // Disable PLL
    __HAL_RCC_PLL_DISABLE();
    while (__HAL_RCC_GET_FLAG(RCC_FLAG_PLLRDY) != RESET) {
    }

    // Configure PLL factors and source
    RCC->PLLCFGR =
        1 << RCC_PLLCFGR_PLLSRC_Pos // HSE selected as PLL source
        | MICROPY_HW_CLK_PLLM << RCC_PLLCFGR_PLLM_Pos
        | MICROPY_HW_CLK_PLLN << RCC_PLLCFGR_PLLN_Pos
        | ((MICROPY_HW_CLK_PLLP >> 1) - 1) << RCC_PLLCFGR_PLLP_Pos
        | MICROPY_HW_CLK_PLLQ << RCC_PLLCFGR_PLLQ_Pos
        #ifdef RCC_PLLCFGR_PLLR
        | 2 << RCC_PLLCFGR_PLLR_Pos // default PLLR value of 2
        #endif
        ;

    // Enable PLL
    __HAL_RCC_PLL_ENABLE();
    while(__HAL_RCC_GET_FLAG(RCC_FLAG_PLLRDY) == RESET) {
    }

    // Increase latency before changing clock
    if (MICROPY_HW_FLASH_LATENCY > (FLASH->ACR & FLASH_ACR_LATENCY)) {
        __HAL_FLASH_SET_LATENCY(MICROPY_HW_FLASH_LATENCY);
    }

    // Configure AHB divider
    MODIFY_REG(RCC->CFGR, RCC_CFGR_HPRE, RCC_SYSCLK_DIV1);

    // Configure SYSCLK source from PLL
    __HAL_RCC_SYSCLK_CONFIG(RCC_SYSCLKSOURCE_PLLCLK);
    while (__HAL_RCC_GET_SYSCLK_SOURCE() != RCC_SYSCLKSOURCE_STATUS_PLLCLK) {
    }

    // Decrease latency after changing clock
    if (MICROPY_HW_FLASH_LATENCY < (FLASH->ACR & FLASH_ACR_LATENCY)) {
        __HAL_FLASH_SET_LATENCY(MICROPY_HW_FLASH_LATENCY);
    }

    // Set APB clock dividers
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1, RCC_HCLK_DIV4);
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2, RCC_HCLK_DIV2 << 3);

    // Update clock value and reconfigure systick now that the frequency changed
    SystemCoreClock = CORE_PLL_FREQ;
    systick_init();

    #if defined(STM32F7)
    // The DFU bootloader changes the clocksource register from its default power
    // on reset value, so we set it back here, so the clocksources are the same
    // whether we were started from DFU or from a power on reset.
    RCC->DCKCFGR2 = 0;
    #endif
}

#elif defined(STM32H7)

void SystemClock_Config(void) {
    // This function assumes that HSI is used as the system clock (see RCC->CFGR, SWS bits)

    // Select VOS level as high voltage to give reliable operation
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    while (__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY) == RESET) {
    }

    // Turn HSE on
    __HAL_RCC_HSE_CONFIG(RCC_HSE_ON);
    while (__HAL_RCC_GET_FLAG(RCC_FLAG_HSERDY) == RESET) {
    }

    // Disable PLL1
    __HAL_RCC_PLL_DISABLE();
    while (__HAL_RCC_GET_FLAG(RCC_FLAG_PLLRDY) != RESET) {
    }

    // Configure PLL1 factors and source
    RCC->PLLCKSELR =
        MICROPY_HW_CLK_PLLM << RCC_PLLCKSELR_DIVM1_Pos
        | 2 << RCC_PLLCKSELR_PLLSRC_Pos; // HSE selected as PLL source
    RCC->PLL1DIVR =
        (MICROPY_HW_CLK_PLLN - 1) << RCC_PLL1DIVR_N1_Pos
        | (MICROPY_HW_CLK_PLLP - 1) << RCC_PLL1DIVR_P1_Pos // only even P allowed
        | (MICROPY_HW_CLK_PLLQ - 1) << RCC_PLL1DIVR_Q1_Pos
        | (MICROPY_HW_CLK_PLLR - 1) << RCC_PLL1DIVR_R1_Pos;

    // Enable PLL1 outputs for SYSCLK and USB
    RCC->PLLCFGR = RCC_PLLCFGR_DIVP1EN | RCC_PLLCFGR_DIVQ1EN;

    // Select PLL1-Q for USB clock source
    RCC->D2CCIP2R |= 1 << RCC_D2CCIP2R_USBSEL_Pos;

    // Enable PLL1
    __HAL_RCC_PLL_ENABLE();
    while(__HAL_RCC_GET_FLAG(RCC_FLAG_PLLRDY) == RESET) {
    }

    // Increase latency before changing SYSCLK
    if (MICROPY_HW_FLASH_LATENCY > (FLASH->ACR & FLASH_ACR_LATENCY)) {
        __HAL_FLASH_SET_LATENCY(MICROPY_HW_FLASH_LATENCY);
    }

    // Configure AHB divider
    RCC->D1CFGR =
        0 << RCC_D1CFGR_D1CPRE_Pos // SYSCLK prescaler of 1
        | 8 << RCC_D1CFGR_HPRE_Pos // AHB prescaler of 2
        ;

    // Configure SYSCLK source from PLL
    __HAL_RCC_SYSCLK_CONFIG(RCC_SYSCLKSOURCE_PLLCLK);
    while (__HAL_RCC_GET_SYSCLK_SOURCE() != RCC_CFGR_SWS_PLL1) {
    }

    // Decrease latency after changing clock
    if (MICROPY_HW_FLASH_LATENCY < (FLASH->ACR & FLASH_ACR_LATENCY)) {
        __HAL_FLASH_SET_LATENCY(MICROPY_HW_FLASH_LATENCY);
    }

    // Set APB clock dividers
    RCC->D1CFGR |=
        4 << RCC_D1CFGR_D1PPRE_Pos // APB3 prescaler of 2
        ;
    RCC->D2CFGR =
        4 << RCC_D2CFGR_D2PPRE2_Pos // APB2 prescaler of 2
        | 4 << RCC_D2CFGR_D2PPRE1_Pos // APB1 prescaler of 2
        ;
    RCC->D3CFGR =
        4 << RCC_D3CFGR_D3PPRE_Pos // APB4 prescaler of 2
        ;

    // Update clock value and reconfigure systick now that the frequency changed
    SystemCoreClock = CORE_PLL_FREQ;
    systick_init();
}

#endif

// Needed by HAL_PCD_IRQHandler
uint32_t HAL_RCC_GetHCLKFreq(void) {
    return SystemCoreClock;
}

/******************************************************************************/
// GPIO

#if defined(STM32F4) || defined(STM32F7)
#define AHBxENR AHB1ENR
#define AHBxENR_GPIOAEN_Pos RCC_AHB1ENR_GPIOAEN_Pos
#elif defined(STM32H7)
#define AHBxENR AHB4ENR
#define AHBxENR_GPIOAEN_Pos RCC_AHB4ENR_GPIOAEN_Pos
#elif defined(STM32WB)
#define AHBxENR AHB2ENR
#define AHBxENR_GPIOAEN_Pos RCC_AHB2ENR_GPIOAEN_Pos
#endif

void mp_hal_pin_config(mp_hal_pin_obj_t port_pin, uint32_t mode, uint32_t pull, uint32_t alt) {
    GPIO_TypeDef *gpio = (GPIO_TypeDef*)(port_pin & ~0xf);

    // Enable the GPIO peripheral clock
    uint32_t gpio_idx = ((uintptr_t)gpio - GPIOA_BASE) / (GPIOB_BASE - GPIOA_BASE);
    RCC->AHBxENR |= 1 << (AHBxENR_GPIOAEN_Pos + gpio_idx);
    volatile uint32_t tmp = RCC->AHBxENR; // Delay after enabling clock
    (void)tmp;

    // Configure the pin
    uint32_t pin = port_pin & 0xf;
    gpio->MODER = (gpio->MODER & ~(3 << (2 * pin))) | ((mode & 3) << (2 * pin));
    gpio->OTYPER = (gpio->OTYPER & ~(1 << pin)) | ((mode >> 2) << pin);
    gpio->OSPEEDR = (gpio->OSPEEDR & ~(3 << (2 * pin))) | (2 << (2 * pin)); // full speed
    gpio->PUPDR = (gpio->PUPDR & ~(3 << (2 * pin))) | (pull << (2 * pin));
    gpio->AFR[pin >> 3] = (gpio->AFR[pin >> 3] & ~(15 << (4 * (pin & 7)))) | (alt << (4 * (pin & 7)));
}

void mp_hal_pin_config_speed(uint32_t port_pin, uint32_t speed) {
    GPIO_TypeDef *gpio = (GPIO_TypeDef*)(port_pin & ~0xf);
    uint32_t pin = port_pin & 0xf;
    gpio->OSPEEDR = (gpio->OSPEEDR & ~(3 << (2 * pin))) | (speed << (2 * pin));
}

/******************************************************************************/
// LED

#if defined(MBOOT_LED1)
#define LED0 MBOOT_LED1
#elif defined(MICROPY_HW_LED1)
#define LED0 MICROPY_HW_LED1
#endif

#if defined(MBOOT_LED2)
#define LED1 MBOOT_LED2
#elif defined(MICROPY_HW_LED2)
#define LED1 MICROPY_HW_LED2
#endif

#if defined(MBOOT_LED3)
#define LED2 MBOOT_LED3
#elif defined(MICROPY_HW_LED3)
#define LED2 MICROPY_HW_LED3
#endif

#if defined(MBOOT_LED4)
#define LED3 MBOOT_LED4
#elif defined(MICROPY_HW_LED4)
#define LED3 MICROPY_HW_LED4
#endif

// For flashing states: bit 0 is "active", bit 1 is "inactive", bits 2-6 are flash rate.
typedef enum {
    LED0_STATE_OFF = 0,
    LED0_STATE_ON = 1,
    LED0_STATE_SLOW_FLASH = (20 << 2) | 1,
    LED0_STATE_FAST_FLASH = (2 << 2) | 1,
    LED0_STATE_SLOW_INVERTED_FLASH = (20 << 2) | 2,
} led0_state_t;

static led0_state_t led0_cur_state = LED0_STATE_OFF;
static uint32_t led0_ms_interval = 0;
static int led0_toggle_count = 0;

MP_WEAK void led_init(void) {
    #if defined(MBOOT_BOARD_LED_INIT)
    // Custom LED init function provided by the board.
    MBOOT_BOARD_LED_INIT();
    #else
    // Init LEDs using GPIO calls.
    mp_hal_pin_output(LED0);
    #ifdef LED1
    mp_hal_pin_output(LED1);
    #endif
    #ifdef LED2
    mp_hal_pin_output(LED2);
    #endif
    #ifdef LED3
    mp_hal_pin_output(LED3);
    #endif
    #endif

    led0_cur_state = LED0_STATE_OFF;
}

MP_WEAK void led_state(uint32_t led, int val) {
    #if defined(MBOOT_BOARD_LED_STATE)
    // Custom LED state function provided by the board.
    return MBOOT_BOARD_LED_STATE(led, val);
    #else
    // Set LEDs using GPIO calls.
    if (val) {
        MICROPY_HW_LED_ON(led);
    } else {
        MICROPY_HW_LED_OFF(led);
    }
    #endif
}

void led_state_all(unsigned int mask) {
    led_state(LED0, mask & 1);
    #ifdef LED1
    led_state(LED1, mask & 2);
    #endif
    #ifdef LED2
    led_state(LED2, mask & 4);
    #endif
    #ifdef LED3
    led_state(LED3, mask & 8);
    #endif
}

void led0_state(led0_state_t state) {
    led0_cur_state = state;
    if (state == LED0_STATE_OFF || state == LED0_STATE_ON) {
        led_state(LED0, state);
    }
}

void led0_update() {
    if (led0_cur_state != LED0_STATE_OFF && systick_ms - led0_ms_interval > 50) {
        uint8_t rate = (led0_cur_state >> 2) & 0x1f;
        led0_ms_interval += 50;
        if (++led0_toggle_count >= rate) {
            led0_toggle_count = 0;
        }
        led_state(LED0, (led0_cur_state & (led0_toggle_count == 0 ? 1 : 2)));
    }
}

/******************************************************************************/
// FLASH

#if defined(STM32WB)
#define FLASH_END FLASH_END_ADDR
#endif
#define APPLICATION_FLASH_LENGTH (FLASH_END + 1 - APPLICATION_ADDR)

#ifndef MBOOT_SPIFLASH_LAYOUT
#define MBOOT_SPIFLASH_LAYOUT ""
#endif

#ifndef MBOOT_SPIFLASH2_LAYOUT
#define MBOOT_SPIFLASH2_LAYOUT ""
#endif

#if defined(STM32F4) \
    || defined(STM32F722xx) \
    || defined(STM32F723xx) \
    || defined(STM32F732xx) \
    || defined(STM32F733xx)
#define FLASH_LAYOUT_STR "@Internal Flash  /0x08000000/04*016Kg,01*064Kg,07*128Kg" MBOOT_SPIFLASH_LAYOUT MBOOT_SPIFLASH2_LAYOUT
#elif defined(STM32F765xx) || defined(STM32F767xx) || defined(STM32F769xx)
#define FLASH_LAYOUT_STR "@Internal Flash  /0x08000000/04*032Kg,01*128Kg,07*256Kg" MBOOT_SPIFLASH_LAYOUT MBOOT_SPIFLASH2_LAYOUT
#elif defined(STM32H743xx)
#define FLASH_LAYOUT_STR "@Internal Flash  /0x08000000/16*128Kg" MBOOT_SPIFLASH_LAYOUT MBOOT_SPIFLASH2_LAYOUT
#elif defined(STM32H750xx)
#define FLASH_LAYOUT_STR "@Internal Flash  /0x08000000/01*128Kg" MBOOT_SPIFLASH_LAYOUT MBOOT_SPIFLASH2_LAYOUT
#elif defined(STM32WB)
#define FLASH_LAYOUT_STR "@Internal Flash  /0x08000000/256*04Kg" MBOOT_SPIFLASH_LAYOUT MBOOT_SPIFLASH2_LAYOUT
#endif

static int mboot_flash_mass_erase(void) {
    // Erase all flash pages after mboot.
    int ret = flash_erase(APPLICATION_ADDR, APPLICATION_FLASH_LENGTH / sizeof(uint32_t));
    return ret;
}

static int mboot_flash_page_erase(uint32_t addr, uint32_t *next_addr) {
    uint32_t sector_size = 0;
    uint32_t sector_start = 0;
    int32_t sector = flash_get_sector_info(addr, &sector_start, &sector_size);
    if (sector <= 0) {
        // Don't allow to erase the sector with this bootloader in it, or invalid sectors
        dfu_context.status = DFU_STATUS_ERROR_ADDRESS;
        dfu_context.error = (sector == 0) ? MBOOT_ERROR_STR_OVERWRITE_BOOTLOADER_IDX
                                          : MBOOT_ERROR_STR_INVALID_ADDRESS_IDX;
        return -MBOOT_ERRNO_FLASH_ERASE_DISALLOWED;
    }

    *next_addr = sector_start + sector_size;

    // Erase the flash page.
    int ret = flash_erase(sector_start, sector_size / sizeof(uint32_t));
    if (ret != 0) {
        return ret;
    }

    // Check the erase set bits to 1, at least for the first 256 bytes
    for (int i = 0; i < 64; ++i) {
        if (((volatile uint32_t*)sector_start)[i] != 0xffffffff) {
            return -MBOOT_ERRNO_FLASH_ERASE_FAILED;
        }
    }

    return 0;
}

static int mboot_flash_write(uint32_t addr, const uint8_t *src8, size_t len) {
    int32_t sector = flash_get_sector_info(addr, NULL, NULL);
    if (sector <= 0) {
        // Don't allow to write the sector with this bootloader in it
        dfu_context.status = DFU_STATUS_ERROR_ADDRESS;
        dfu_context.error = (sector == 0) ? MBOOT_ERROR_STR_OVERWRITE_BOOTLOADER_IDX
                                          : MBOOT_ERROR_STR_INVALID_ADDRESS_IDX;
        return -MBOOT_ERRNO_FLASH_WRITE_DISALLOWED;
    }

    const uint32_t *src = (const uint32_t*)src8;
    size_t num_word32 = (len + 3) / 4;

    // Write the data to flash.
    int ret = flash_write(addr, src, num_word32);
    if (ret != 0) {
        return ret;
    }

    // TODO verify data

    return 0;
}

/******************************************************************************/
// Writable address space interface

static int do_mass_erase(void) {
    // TODO spiflash erase ?
    return mboot_flash_mass_erase();
}

#if defined(MBOOT_SPIFLASH_ADDR) || defined(MBOOT_SPIFLASH2_ADDR)
static int spiflash_page_erase(mp_spiflash_t *spif, uint32_t addr, uint32_t n_blocks) {
    for (int i = 0; i < n_blocks; ++i) {
        int ret = mp_spiflash_erase_block(spif, addr);
        if (ret != 0) {
            return ret;
        }
        addr += MP_SPIFLASH_ERASE_BLOCK_SIZE;
    }
    return 0;
}
#endif

int hw_page_erase(uint32_t addr, uint32_t *next_addr) {
    int ret = -1;
    led0_state(LED0_STATE_ON);

    #if defined(MBOOT_SPIFLASH_ADDR)
    if (MBOOT_SPIFLASH_ADDR <= addr && addr < MBOOT_SPIFLASH_ADDR + MBOOT_SPIFLASH_BYTE_SIZE) {
        *next_addr = addr + MBOOT_SPIFLASH_ERASE_BLOCKS_PER_PAGE * MP_SPIFLASH_ERASE_BLOCK_SIZE;
        ret = spiflash_page_erase(MBOOT_SPIFLASH_SPIFLASH,
            addr - MBOOT_SPIFLASH_ADDR, MBOOT_SPIFLASH_ERASE_BLOCKS_PER_PAGE);
    } else
    #endif
    #if defined(MBOOT_SPIFLASH2_ADDR)
    if (MBOOT_SPIFLASH2_ADDR <= addr && addr < MBOOT_SPIFLASH2_ADDR + MBOOT_SPIFLASH2_BYTE_SIZE) {
        *next_addr = addr + MBOOT_SPIFLASH2_ERASE_BLOCKS_PER_PAGE * MP_SPIFLASH_ERASE_BLOCK_SIZE;
        ret = spiflash_page_erase(MBOOT_SPIFLASH2_SPIFLASH,
            addr - MBOOT_SPIFLASH2_ADDR, MBOOT_SPIFLASH2_ERASE_BLOCKS_PER_PAGE);
    } else
    #endif
    {
        ret = mboot_flash_page_erase(addr, next_addr);
    }

    led0_state((ret == 0) ? LED0_STATE_SLOW_FLASH : LED0_STATE_SLOW_INVERTED_FLASH);
    return ret;
}

void hw_read(uint32_t addr, int len, uint8_t *buf) {
    led0_state(LED0_STATE_FAST_FLASH);
    #if defined(MBOOT_SPIFLASH_ADDR)
    if (MBOOT_SPIFLASH_ADDR <= addr && addr < MBOOT_SPIFLASH_ADDR + MBOOT_SPIFLASH_BYTE_SIZE) {
        mp_spiflash_read(MBOOT_SPIFLASH_SPIFLASH, addr - MBOOT_SPIFLASH_ADDR, len, buf);
    } else
    #endif
    #if defined(MBOOT_SPIFLASH2_ADDR)
    if (MBOOT_SPIFLASH2_ADDR <= addr && addr < MBOOT_SPIFLASH2_ADDR + MBOOT_SPIFLASH2_BYTE_SIZE) {
        mp_spiflash_read(MBOOT_SPIFLASH2_SPIFLASH, addr - MBOOT_SPIFLASH2_ADDR, len, buf);
    } else
    #endif
    {
        // Other addresses, just read directly from memory
        memcpy(buf, (void*)addr, len);
    }
    led0_state(LED0_STATE_SLOW_FLASH);
}

int hw_write(uint32_t addr, const uint8_t *src8, size_t len) {
    int ret = -1;
    led0_state(LED0_STATE_FAST_FLASH);
    #if defined(MBOOT_SPIFLASH_ADDR)
    if (MBOOT_SPIFLASH_ADDR <= addr && addr < MBOOT_SPIFLASH_ADDR + MBOOT_SPIFLASH_BYTE_SIZE) {
        ret = mp_spiflash_write(MBOOT_SPIFLASH_SPIFLASH, addr - MBOOT_SPIFLASH_ADDR, len, src8);
    } else
    #endif
    #if defined(MBOOT_SPIFLASH2_ADDR)
    if (MBOOT_SPIFLASH2_ADDR <= addr && addr < MBOOT_SPIFLASH2_ADDR + MBOOT_SPIFLASH2_BYTE_SIZE) {
        ret = mp_spiflash_write(MBOOT_SPIFLASH2_SPIFLASH, addr - MBOOT_SPIFLASH2_ADDR, len, src8);
    } else
    #endif
    if (flash_is_valid_addr(addr)) {
        ret = mboot_flash_write(addr, src8, len);
    } else {
        dfu_context.status = DFU_STATUS_ERROR_ADDRESS;
        dfu_context.error = MBOOT_ERROR_STR_INVALID_ADDRESS_IDX;
    }

    led0_state((ret == 0) ? LED0_STATE_SLOW_FLASH : LED0_STATE_SLOW_INVERTED_FLASH);
    return ret;
}

int do_page_erase(uint32_t addr, uint32_t *next_addr) {
    #if MBOOT_ENABLE_PACKING
    // Erase handled automatically for packed mode.
    return 0;
    #else
    return hw_page_erase(addr, next_addr);
    #endif
}

void do_read(uint32_t addr, int len, uint8_t *buf) {
    #if MBOOT_ENABLE_PACKING
    // Read disabled on packed (encrypted) mode.
    dfu_context.status = DFU_STATUS_ERROR_FILE;
    dfu_context.error = MBOOT_ERROR_STR_INVALID_READ_IDX;
    led0_state(LED0_STATE_SLOW_INVERTED_FLASH);
    #else
    hw_read(addr, len, buf);
    #endif
}

int do_write(uint32_t addr, const uint8_t *src8, size_t len) {
    #if MBOOT_ENABLE_PACKING
    return mboot_pack_write(addr, src8, len);
    #else
    return hw_write(addr, src8, len);
    #endif
}

/******************************************************************************/
// I2C slave interface

#if defined(MBOOT_I2C_SCL)

#define PASTE2(a, b) a ## b
#define PASTE3(a, b, c) a ## b ## c
#define EVAL_PASTE2(a, b) PASTE2(a, b)
#define EVAL_PASTE3(a, b, c) PASTE3(a, b, c)

#define MBOOT_I2Cx EVAL_PASTE2(I2C, MBOOT_I2C_PERIPH_ID)
#define I2Cx_EV_IRQn EVAL_PASTE3(I2C, MBOOT_I2C_PERIPH_ID, _EV_IRQn)
#define I2Cx_EV_IRQHandler EVAL_PASTE3(I2C, MBOOT_I2C_PERIPH_ID, _EV_IRQHandler)

#define I2C_CMD_BUF_LEN (129)

enum {
    I2C_CMD_ECHO = 1,
    I2C_CMD_GETID,      // () -> u8*12 unique id, ASCIIZ mcu name, ASCIIZ board name
    I2C_CMD_GETCAPS,    // not implemented
    I2C_CMD_RESET,      // () -> ()
    I2C_CMD_CONFIG,     // not implemented
    I2C_CMD_GETLAYOUT,  // () -> ASCII string
    I2C_CMD_MASSERASE,  // () -> ()
    I2C_CMD_PAGEERASE,  // le32 -> ()
    I2C_CMD_SETRDADDR,  // le32 -> ()
    I2C_CMD_SETWRADDR,  // le32 -> ()
    I2C_CMD_READ,       // u8 -> bytes
    I2C_CMD_WRITE,      // bytes -> ()
    I2C_CMD_COPY,       // not implemented
    I2C_CMD_CALCHASH,   // le32 -> u8*32
    I2C_CMD_MARKVALID,  // () -> ()
};

typedef struct _i2c_obj_t {
    volatile bool cmd_send_arg;
    volatile bool cmd_arg_sent;
    volatile int cmd_arg;
    volatile uint32_t cmd_rdaddr;
    volatile uint32_t cmd_wraddr;
    volatile uint16_t cmd_buf_pos;
    uint8_t cmd_buf[I2C_CMD_BUF_LEN];
} i2c_obj_t;

static i2c_obj_t i2c_obj;

void i2c_init(int addr) {
    i2c_obj.cmd_send_arg = false;

    mp_hal_pin_config(MBOOT_I2C_SCL, MP_HAL_PIN_MODE_ALT_OPEN_DRAIN, MP_HAL_PIN_PULL_NONE, MBOOT_I2C_ALTFUNC);
    mp_hal_pin_config(MBOOT_I2C_SDA, MP_HAL_PIN_MODE_ALT_OPEN_DRAIN, MP_HAL_PIN_PULL_NONE, MBOOT_I2C_ALTFUNC);

    i2c_slave_init(MBOOT_I2Cx, I2Cx_EV_IRQn, IRQ_PRI_I2C, addr);
}

int i2c_slave_process_addr_match(i2c_slave_t *i2c, int rw) {
    if (i2c_obj.cmd_arg_sent) {
        i2c_obj.cmd_send_arg = false;
    }
    i2c_obj.cmd_buf_pos = 0;
    return 0; // ACK
}

int i2c_slave_process_rx_byte(i2c_slave_t *i2c, uint8_t val) {
    if (i2c_obj.cmd_buf_pos < sizeof(i2c_obj.cmd_buf)) {
        i2c_obj.cmd_buf[i2c_obj.cmd_buf_pos++] = val;
    }
    return 0; // ACK
}

void i2c_slave_process_rx_end(i2c_slave_t *i2c) {
    if (i2c_obj.cmd_buf_pos == 0) {
        return;
    }

    int len = i2c_obj.cmd_buf_pos - 1;
    uint8_t *buf = i2c_obj.cmd_buf;

    if (buf[0] == I2C_CMD_ECHO) {
        ++len;
    } else if (buf[0] == I2C_CMD_GETID && len == 0) {
        memcpy(buf, (uint8_t*)MP_HAL_UNIQUE_ID_ADDRESS, 12);
        memcpy(buf + 12, MICROPY_HW_MCU_NAME, sizeof(MICROPY_HW_MCU_NAME));
        memcpy(buf + 12 + sizeof(MICROPY_HW_MCU_NAME), MICROPY_HW_BOARD_NAME, sizeof(MICROPY_HW_BOARD_NAME) - 1);
        len = 12 + sizeof(MICROPY_HW_MCU_NAME) + sizeof(MICROPY_HW_BOARD_NAME) - 1;
    } else if (buf[0] == I2C_CMD_RESET && len == 0) {
        dfu_context.leave_dfu = true;
    } else if (buf[0] == I2C_CMD_GETLAYOUT && len == 0) {
        len = strlen(FLASH_LAYOUT_STR);
        memcpy(buf, FLASH_LAYOUT_STR, len);
    } else if (buf[0] == I2C_CMD_MASSERASE && len == 0) {
        len = do_mass_erase();
    } else if (buf[0] == I2C_CMD_PAGEERASE && len == 4) {
        uint32_t next_addr;
        len = do_page_erase(get_le32(buf + 1), &next_addr);
    } else if (buf[0] == I2C_CMD_SETRDADDR && len == 4) {
        i2c_obj.cmd_rdaddr = get_le32(buf + 1);
        len = 0;
    } else if (buf[0] == I2C_CMD_SETWRADDR && len == 4) {
        i2c_obj.cmd_wraddr = get_le32(buf + 1);
        len = 0;
    } else if (buf[0] == I2C_CMD_READ && len == 1) {
        len = buf[1];
        if (len > I2C_CMD_BUF_LEN) {
            len = I2C_CMD_BUF_LEN;
        }
        do_read(i2c_obj.cmd_rdaddr, len, buf);
        i2c_obj.cmd_rdaddr += len;
    } else if (buf[0] == I2C_CMD_WRITE) {
        if (i2c_obj.cmd_wraddr == APPLICATION_ADDR) {
            // Mark the 2 lower bits to indicate invalid app firmware
            buf[1] |= APP_VALIDITY_BITS;
        }
        int ret = do_write(i2c_obj.cmd_wraddr, buf + 1, len);
        if (ret < 0) {
            len = ret;
        } else {
            i2c_obj.cmd_wraddr += len;
            len = 0;
        }
    } else if (buf[0] == I2C_CMD_CALCHASH && len == 4) {
        uint32_t hashlen = get_le32(buf + 1);
        static CRYAL_SHA256_CTX ctx;
        sha256_init(&ctx);
        sha256_update(&ctx, (const void*)i2c_obj.cmd_rdaddr, hashlen);
        i2c_obj.cmd_rdaddr += hashlen;
        sha256_final(&ctx, buf);
        len = 32;
    } else if (buf[0] == I2C_CMD_MARKVALID && len == 0) {
        uint32_t buf;
        buf = *(volatile uint32_t*)APPLICATION_ADDR;
        if ((buf & APP_VALIDITY_BITS) != APP_VALIDITY_BITS) {
            len = -1;
        } else {
            buf &= ~APP_VALIDITY_BITS;
            int ret = do_write(APPLICATION_ADDR, (void*)&buf, 4);
            if (ret < 0) {
                len = ret;
            } else {
                buf = *(volatile uint32_t*)APPLICATION_ADDR;
                if ((buf & APP_VALIDITY_BITS) != 0) {
                    len = -2;
                } else {
                    len = 0;
                }
            }
        }
    } else {
        len = -127;
    }
    i2c_obj.cmd_arg = len;
    i2c_obj.cmd_send_arg = true;
    i2c_obj.cmd_arg_sent = false;
}

uint8_t i2c_slave_process_tx_byte(i2c_slave_t *i2c) {
    if (i2c_obj.cmd_send_arg) {
        i2c_obj.cmd_arg_sent = true;
        return i2c_obj.cmd_arg;
    } else if (i2c_obj.cmd_buf_pos < sizeof(i2c_obj.cmd_buf)) {
        return i2c_obj.cmd_buf[i2c_obj.cmd_buf_pos++];
    } else {
        return 0;
    }
}

void i2c_slave_process_tx_end(i2c_slave_t *i2c) {
}

#endif // defined(MBOOT_I2C_SCL)

/******************************************************************************/
// DFU

static void dfu_init(void) {
    dfu_context.state = DFU_STATE_IDLE;
    dfu_context.cmd = DFU_CMD_NONE;
    dfu_context.status = DFU_STATUS_OK;
    dfu_context.error = 0;
    dfu_context.leave_dfu = false;
    dfu_context.addr = 0x08000000;
}

static int dfu_process_dnload(void) {
    int ret = -1;
    if (dfu_context.wBlockNum == 0) {
        // download control commands
        if (dfu_context.wLength >= 1 && dfu_context.buf[0] == DFU_CMD_DNLOAD_ERASE) {
            if (dfu_context.wLength == 1) {
                // mass erase
                ret = do_mass_erase();
                if (ret != 0) {
                    dfu_context.cmd = DFU_CMD_NONE;
                }
            } else if (dfu_context.wLength == 5) {
                // erase page
                uint32_t next_addr;
                ret = do_page_erase(get_le32(&dfu_context.buf[1]), &next_addr);
            }
        } else if (dfu_context.wLength >= 1 && dfu_context.buf[0] == DFU_CMD_DNLOAD_SET_ADDRESS) {
            if (dfu_context.wLength == 5) {
                // set address
                dfu_context.addr = get_le32(&dfu_context.buf[1]);
                ret = 0;
            }
        }
    } else if (dfu_context.wBlockNum > 1) {
        // write data to memory
        uint32_t addr = (dfu_context.wBlockNum - 2) * DFU_XFER_SIZE + dfu_context.addr;
        ret = do_write(addr, dfu_context.buf, dfu_context.wLength);
    }
    if (ret == 0) {
        return DFU_STATE_DNLOAD_IDLE;
    } else {
        return DFU_STATE_ERROR;
    }
}

static void dfu_handle_rx(int cmd, int arg, int len, const void *buf) {
    if (cmd == DFU_CLRSTATUS) {
        // clear status
        dfu_context.state = DFU_STATE_IDLE;
        dfu_context.cmd = DFU_CMD_NONE;
        dfu_context.status = DFU_STATUS_OK;
        dfu_context.error = 0;
    } else if (cmd == DFU_ABORT) {
        // clear status
        dfu_context.state = DFU_STATE_IDLE;
        dfu_context.cmd = DFU_CMD_NONE;
        dfu_context.status = DFU_STATUS_OK;
        dfu_context.error = 0;
    } else if (cmd == DFU_DNLOAD) {
        if (len == 0) {
            // exit DFU
            dfu_context.cmd = DFU_CMD_EXIT;
        } else {
            // download
            dfu_context.cmd = DFU_CMD_DNLOAD;
            dfu_context.wBlockNum = arg;
            dfu_context.wLength = len;
            memcpy(dfu_context.buf, buf, len);
        }
    }
}

static void dfu_process(void) {
    if (dfu_context.state == DFU_STATE_MANIFEST) {
        // Set a flag to leave DFU mode from the main thread (here we are in an IRQ handler).
        dfu_context.leave_dfu = true;
    }

    if (dfu_context.state == DFU_STATE_BUSY) {
        if (dfu_context.cmd == DFU_CMD_DNLOAD) {
            dfu_context.cmd = DFU_CMD_NONE;
            dfu_context.state = dfu_process_dnload();
        }
    }
}

static int dfu_handle_tx(int cmd, int arg, int len, uint8_t *buf, int max_len) {
    if (cmd == DFU_UPLOAD) {
        if (arg >= 2) {
            dfu_context.cmd = DFU_CMD_UPLOAD;
            uint32_t addr = (arg - 2) * max_len + dfu_context.addr;
            do_read(addr, len, buf);
            return len;
        }
    } else if (cmd == DFU_GETSTATUS && len == 6) {
        // execute command and get status
        switch (dfu_context.cmd) {
            case DFU_CMD_NONE:
                break;
            case DFU_CMD_EXIT:
                dfu_context.state = DFU_STATE_MANIFEST;
                break;
            case DFU_CMD_UPLOAD:
                dfu_context.state = DFU_STATE_UPLOAD_IDLE;
                break;
            case DFU_CMD_DNLOAD:
                dfu_context.state = DFU_STATE_BUSY;
                break;
            default:
                dfu_context.state = DFU_STATE_BUSY;
        }
        buf[0] = dfu_context.status;          // bStatus
        buf[1] = 0;                           // bwPollTimeout_lsb (ms)
        buf[2] = 0;                           // bwPollTimeout     (ms)
        buf[3] = 0;                           // bwPollTimeout_msb (ms)
        buf[4] = dfu_context.state;           // bState
        buf[5] = dfu_context.error;           // iString
        // Clear errors now they've been sent
        dfu_context.status = DFU_STATUS_OK;
        dfu_context.error = 0;
        return 6;
    } else if (cmd == DFU_GETSTATE && len == 1) {
        buf[0] = dfu_context.state; // bState
        return 1;
    }
    return -1;
}

/******************************************************************************/
// USB

#define USB_XFER_SIZE (DFU_XFER_SIZE)

#define USB_PHY_FS_ID (0)
#define USB_PHY_HS_ID (1)

typedef struct _pyb_usbdd_obj_t {
    bool started;
    bool tx_pending;
    USBD_HandleTypeDef hUSBDDevice;

    uint8_t bRequest;
    uint16_t wValue;
    uint16_t wLength;
    __ALIGN_BEGIN uint8_t rx_buf[USB_XFER_SIZE] __ALIGN_END;
    __ALIGN_BEGIN uint8_t tx_buf[USB_XFER_SIZE] __ALIGN_END;

    // RAM to hold the current descriptors, which we configure on the fly
    __ALIGN_BEGIN uint8_t usbd_device_desc[USB_LEN_DEV_DESC] __ALIGN_END;
    __ALIGN_BEGIN uint8_t usbd_str_desc[USBD_MAX_STR_DESC_SIZ] __ALIGN_END;
} pyb_usbdd_obj_t;

#ifndef MBOOT_USBD_LANGID_STRING
#define MBOOT_USBD_LANGID_STRING (0x409)
#endif

#ifndef MBOOT_USBD_MANUFACTURER_STRING
#define MBOOT_USBD_MANUFACTURER_STRING      "MicroPython"
#endif

#ifndef MBOOT_USBD_PRODUCT_STRING
#define MBOOT_USBD_PRODUCT_STRING        "Pyboard DFU"
#endif

#ifndef MBOOT_USB_VID
#define MBOOT_USB_VID BOOTLOADER_DFU_USB_VID
#endif

#ifndef MBOOT_USB_PID
#define MBOOT_USB_PID BOOTLOADER_DFU_USB_PID
#endif

#if !MICROPY_HW_USB_IS_MULTI_OTG
STATIC const uint8_t usbd_fifo_size[USBD_PMA_NUM_FIFO] = {
    32, 32, // EP0(out), EP0(in)
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 14x unused
};
#else
static const uint8_t usbd_fifo_size[] = {
    32, 8, 16, 8, 16, 0, 0, // FS: RX, EP0(in), 5x IN endpoints
    #if MICROPY_HW_USB_HS
    116, 8, 64, 4, 64, 0, 0, 0, 0, 0, // HS: RX, EP0(in), 8x IN endpoints
    #endif
};
#endif

__ALIGN_BEGIN static const uint8_t USBD_LangIDDesc[USB_LEN_LANGID_STR_DESC] __ALIGN_END = {
    USB_LEN_LANGID_STR_DESC,
    USB_DESC_TYPE_STRING,
    LOBYTE(MBOOT_USBD_LANGID_STRING),
    HIBYTE(MBOOT_USBD_LANGID_STRING),
};

static const uint8_t dev_descr[0x12] = {
    0x12, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x40,
    LOBYTE(MBOOT_USB_VID), HIBYTE(MBOOT_USB_VID),
    LOBYTE(MBOOT_USB_PID), HIBYTE(MBOOT_USB_PID),
    0x00, 0x22, 0x01, 0x02, 0x03, 0x01
};

// This may be modified by USBD_GetDescriptor
static uint8_t cfg_descr[9 + 9 + 9] =
    "\x09\x02\x1b\x00\x01\x01\x00\xc0\x32"
    "\x09\x04\x00\x00\x00\xfe\x01\x02\x04"
    "\x09\x21\x0b\xff\x00\x00\x08\x1a\x01" // \x00\x08 goes with USB_XFER_SIZE
;

static uint8_t *pyb_usbdd_DeviceDescriptor(USBD_HandleTypeDef *pdev, uint16_t *length) {
    *length = USB_LEN_DEV_DESC;
    return (uint8_t*)dev_descr;
}

static char get_hex_char(int val) {
    val &= 0xf;
    if (val <= 9) {
        return '0' + val;
    } else {
        return 'A' + val - 10;
    }
}

static void format_hex(char *buf, int val) {
    buf[0] = get_hex_char(val >> 4);
    buf[1] = get_hex_char(val);
}

static uint8_t *pyb_usbdd_StrDescriptor(USBD_HandleTypeDef *pdev, uint8_t idx, uint16_t *length) {
    pyb_usbdd_obj_t *self = (pyb_usbdd_obj_t*)pdev->pClassData;
    uint8_t *str_desc = self->usbd_str_desc;
    switch (idx) {
        case USBD_IDX_LANGID_STR:
            *length = sizeof(USBD_LangIDDesc);
            return (uint8_t*)USBD_LangIDDesc; // the data should only be read from this buf

        case USBD_IDX_MFC_STR:
            USBD_GetString((uint8_t*)MBOOT_USBD_MANUFACTURER_STRING, str_desc, length);
            return str_desc;

        case USBD_IDX_PRODUCT_STR:
            USBD_GetString((uint8_t*)MBOOT_USBD_PRODUCT_STRING, str_desc, length);
            return str_desc;

        case USBD_IDX_SERIAL_STR: {
            // This document: http://www.usb.org/developers/docs/devclass_docs/usbmassbulk_10.pdf
            // says that the serial number has to be at least 12 digits long and that
            // the last 12 digits need to be unique. It also stipulates that the valid
            // character set is that of upper-case hexadecimal digits.
            //
            // The onboard DFU bootloader produces a 12-digit serial number based on
            // the 96-bit unique ID, so for consistency we go with this algorithm.
            // You can see the serial number if you do:
            //
            //     dfu-util -l
            //
            // See: https://my.st.com/52d187b7 for the algorithim used.
            uint8_t *id = (uint8_t*)MP_HAL_UNIQUE_ID_ADDRESS;
            char serial_buf[16];
            format_hex(&serial_buf[0], id[11]);
            format_hex(&serial_buf[2], id[10] + id[2]);
            format_hex(&serial_buf[4], id[9]);
            format_hex(&serial_buf[6], id[8] + id[0]);
            format_hex(&serial_buf[8], id[7]);
            format_hex(&serial_buf[10], id[6]);
            serial_buf[12] = '\0';

            USBD_GetString((uint8_t*)serial_buf, str_desc, length);
            return str_desc;
        }

        case USBD_IDX_CONFIG_STR:
            USBD_GetString((uint8_t*)FLASH_LAYOUT_STR, str_desc, length);
            return str_desc;

        case MBOOT_ERROR_STR_OVERWRITE_BOOTLOADER_IDX:
            USBD_GetString((uint8_t*)MBOOT_ERROR_STR_OVERWRITE_BOOTLOADER, str_desc, length);
            return str_desc;

        case MBOOT_ERROR_STR_INVALID_ADDRESS_IDX:
            USBD_GetString((uint8_t*)MBOOT_ERROR_STR_INVALID_ADDRESS, str_desc, length);
            return str_desc;

        #if MBOOT_ENABLE_PACKING
        case MBOOT_ERROR_STR_INVALID_SIG_IDX:
            USBD_GetString((uint8_t*)MBOOT_ERROR_STR_INVALID_SIG, str_desc, length);
            return str_desc;

        case MBOOT_ERROR_STR_INVALID_READ_IDX:
            USBD_GetString((uint8_t*)MBOOT_ERROR_STR_INVALID_READ, str_desc, length);
            return str_desc;
        #endif

        default:
            return NULL;
    }
}

static const USBD_DescriptorsTypeDef pyb_usbdd_descriptors = {
    pyb_usbdd_DeviceDescriptor,
    pyb_usbdd_StrDescriptor,
};

static uint8_t pyb_usbdd_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx) {
    pyb_usbdd_obj_t *self = (pyb_usbdd_obj_t*)pdev->pClassData;
    (void)self;
    return USBD_OK;
}

static uint8_t pyb_usbdd_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx) {
    pyb_usbdd_obj_t *self = (pyb_usbdd_obj_t*)pdev->pClassData;
    (void)self;
    return USBD_OK;
}

static uint8_t pyb_usbdd_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req) {
    pyb_usbdd_obj_t *self = (pyb_usbdd_obj_t*)pdev->pClassData;
    (void)self;
    self->bRequest = req->bRequest;
    self->wValue = req->wValue;
    self->wLength = req->wLength;
    if (req->bmRequest == 0x21) {
        // host-to-device request
        if (req->wLength == 0) {
            // no data, process command straightaway
            dfu_handle_rx(self->bRequest, self->wValue, 0, NULL);
        } else {
            // have data, prepare to receive it
            USBD_CtlPrepareRx(pdev, self->rx_buf, req->wLength);
        }
    } else if (req->bmRequest == 0xa1) {
        // device-to-host request
        int len = dfu_handle_tx(self->bRequest, self->wValue, self->wLength, self->tx_buf, USB_XFER_SIZE);
        if (len >= 0) {
            self->tx_pending = true;
            USBD_CtlSendData(&self->hUSBDDevice, self->tx_buf, len);
        }
    }
    return USBD_OK;
}

static uint8_t pyb_usbdd_EP0_TxSent(USBD_HandleTypeDef *pdev) {
    pyb_usbdd_obj_t *self = (pyb_usbdd_obj_t*)pdev->pClassData;
    self->tx_pending = false;
    #if !USE_USB_POLLING
    // Process now that we have sent a response
    dfu_process();
    #endif
    return USBD_OK;
}

static uint8_t pyb_usbdd_EP0_RxReady(USBD_HandleTypeDef *pdev) {
    pyb_usbdd_obj_t *self = (pyb_usbdd_obj_t*)pdev->pClassData;
    dfu_handle_rx(self->bRequest, self->wValue, self->wLength, self->rx_buf);
    return USBD_OK;
}

static uint8_t *pyb_usbdd_GetCfgDesc(USBD_HandleTypeDef *pdev, uint16_t *length) {
    *length = sizeof(cfg_descr);
    return (uint8_t*)cfg_descr;
}

// this is used only in high-speed mode, which we don't support
static uint8_t *pyb_usbdd_GetDeviceQualifierDescriptor(USBD_HandleTypeDef *pdev, uint16_t *length) {
    pyb_usbdd_obj_t *self = (pyb_usbdd_obj_t*)pdev->pClassData;
    (void)self;
    /*
    *length = sizeof(USBD_CDC_MSC_HID_DeviceQualifierDesc);
    return USBD_CDC_MSC_HID_DeviceQualifierDesc;
    */
    *length = 0;
    return NULL;
}

static const USBD_ClassTypeDef pyb_usbdd_class = {
    pyb_usbdd_Init,
    pyb_usbdd_DeInit,
    pyb_usbdd_Setup,
    pyb_usbdd_EP0_TxSent,
    pyb_usbdd_EP0_RxReady,
    NULL, // pyb_usbdd_DataIn,
    NULL, // pyb_usbdd_DataOut,
    NULL, // SOF
    NULL, // IsoINIncomplete
    NULL, // IsoOUTIncomplete
    pyb_usbdd_GetCfgDesc,
    pyb_usbdd_GetCfgDesc,
    pyb_usbdd_GetCfgDesc,
    pyb_usbdd_GetDeviceQualifierDescriptor,
};

static pyb_usbdd_obj_t pyb_usbdd SECTION_NOZERO_BSS;

static int pyb_usbdd_detect_port(void) {
    #if MBOOT_USB_AUTODETECT_PORT
    mp_hal_pin_config(pin_A11, MP_HAL_PIN_MODE_INPUT, MP_HAL_PIN_PULL_UP, 0);
    mp_hal_pin_config(pin_A12, MP_HAL_PIN_MODE_INPUT, MP_HAL_PIN_PULL_UP, 0);
    int state = mp_hal_pin_read(pin_A11) == 0 && mp_hal_pin_read(pin_A12) == 0;
    mp_hal_pin_config(pin_A11, MP_HAL_PIN_MODE_INPUT, MP_HAL_PIN_PULL_NONE, 0);
    mp_hal_pin_config(pin_A12, MP_HAL_PIN_MODE_INPUT, MP_HAL_PIN_PULL_NONE, 0);
    if (state) {
        return USB_PHY_FS_ID;
    }
    mp_hal_pin_config(pin_B14, MP_HAL_PIN_MODE_INPUT, MP_HAL_PIN_PULL_UP, 0);
    mp_hal_pin_config(pin_B15, MP_HAL_PIN_MODE_INPUT, MP_HAL_PIN_PULL_UP, 0);
    state = mp_hal_pin_read(pin_B14) == 0 && mp_hal_pin_read(pin_B15) == 0;
    mp_hal_pin_config(pin_B14, MP_HAL_PIN_MODE_INPUT, MP_HAL_PIN_PULL_NONE, 0);
    mp_hal_pin_config(pin_B15, MP_HAL_PIN_MODE_INPUT, MP_HAL_PIN_PULL_NONE, 0);
    if (state) {
        return USB_PHY_HS_ID;
    }
    #endif
    return MICROPY_HW_USB_MAIN_DEV;
}

static void pyb_usbdd_init(pyb_usbdd_obj_t *self, int phy_id) {
    self->started = false;
    self->tx_pending = false;
    USBD_HandleTypeDef *usbd = &self->hUSBDDevice;
    usbd->id = phy_id;
    usbd->dev_state = USBD_STATE_DEFAULT;
    usbd->pDesc = (USBD_DescriptorsTypeDef*)&pyb_usbdd_descriptors;
    usbd->pClass = &pyb_usbdd_class;
    usbd->pClassData = self;
}

static void pyb_usbdd_start(pyb_usbdd_obj_t *self) {
    if (!self->started) {
        #if defined(STM32H7)
        PWR->CR3 |= PWR_CR3_USB33DEN;
        while (!(PWR->CR3 & PWR_CR3_USB33RDY)) {
        }
        #endif
        USBD_LL_Init(&self->hUSBDDevice, 0, usbd_fifo_size);
        USBD_LL_Start(&self->hUSBDDevice);
        self->started = true;
    }
}

static void pyb_usbdd_stop(pyb_usbdd_obj_t *self) {
    if (self->started) {
        USBD_Stop(&self->hUSBDDevice);
        self->started = false;
    }
}

static int pyb_usbdd_shutdown(void) {
    pyb_usbdd_stop(&pyb_usbdd);
    return 0;
}

/******************************************************************************/
// main

#if defined(MBOOT_BOARD_GET_RESET_MODE)

static inline int mboot_get_reset_mode(void) {
    return MBOOT_BOARD_GET_RESET_MODE();
}

#else

#define RESET_MODE_NUM_STATES (4)
#define RESET_MODE_TIMEOUT_CYCLES (8)
#ifdef LED2
#ifdef LED3
#define RESET_MODE_LED_STATES 0x8421
#else
#define RESET_MODE_LED_STATES 0x7421
#endif
#else
#define RESET_MODE_LED_STATES 0x3210
#endif

static void usrbtn_init(void) {
    mp_hal_pin_config(MICROPY_HW_USRSW_PIN, MP_HAL_PIN_MODE_INPUT, MICROPY_HW_USRSW_PULL, 0);
}

static int usrbtn_state(void) {
    return mp_hal_pin_read(MICROPY_HW_USRSW_PIN) == MICROPY_HW_USRSW_PRESSED;
}

static int mboot_get_reset_mode(void) {
    usrbtn_init();
    int reset_mode = BOARDCTRL_RESET_MODE_NORMAL;
    if (usrbtn_state()) {
        // Cycle through reset modes while USR is held
        // Timeout is roughly 20s, where reset_mode=1
        systick_init();
        led_init();
        reset_mode = 0;
        for (int i = 0; i < (RESET_MODE_NUM_STATES * RESET_MODE_TIMEOUT_CYCLES + 1) * 32; i++) {
            if (i % 32 == 0) {
                if (++reset_mode > RESET_MODE_NUM_STATES) {
                    reset_mode = BOARDCTRL_RESET_MODE_NORMAL;
                }
                uint8_t l = RESET_MODE_LED_STATES >> ((reset_mode - 1) * 4);
                led_state_all(l);
            }
            if (!usrbtn_state()) {
                break;
            }
            mp_hal_delay_ms(19);
        }
        // Flash the selected reset mode
        for (int i = 0; i < 6; i++) {
            led_state_all(0);
            mp_hal_delay_ms(50);
            uint8_t l = RESET_MODE_LED_STATES >> ((reset_mode - 1) * 4);
            led_state_all(l);
            mp_hal_delay_ms(50);
        }
        mp_hal_delay_ms(300);
    }
    return reset_mode;
}

#endif

NORETURN static __attribute__((naked)) void branch_to_application(uint32_t r0, uint32_t bl_addr) {
    __asm volatile (
        "ldr r2, [r1, #0]\n"    // get address of stack pointer
        "msr msp, r2\n"         // set stack pointer
        "ldr r2, [r1, #4]\n"    // get address of destination
        "bx r2\n"               // branch to application
        );
    MP_UNREACHABLE;
}

static void try_enter_application(int reset_mode) {
    uint32_t msp = *(volatile uint32_t*)APPLICATION_ADDR;
    if ((msp & APP_VALIDITY_BITS) != 0) {
        // Application is invalid.
        return;
    }

    // undo our DFU settings
    // TODO probably should disable all IRQ sources first
    #if defined(MBOOT_BOARD_CLEANUP)
    MBOOT_BOARD_CLEANUP(reset_mode);
    #endif
    #if USE_CACHE && defined(STM32F7)
    SCB_DisableICache();
    SCB_DisableDCache();
    #endif

    // Jump to the application.
    branch_to_application(reset_mode, APPLICATION_ADDR);
}

static void leave_bootloader(void) {
    #if !MBOOT_LEAVE_BOOTLOADER_VIA_RESET
    // Try to enter the application via a jump, if it's valid.
    try_enter_application(BOARDCTRL_RESET_MODE_BOOTLOADER);
    #endif
    NVIC_SystemReset();
}

extern PCD_HandleTypeDef pcd_fs_handle;
extern PCD_HandleTypeDef pcd_hs_handle;

void stm32_main(int initial_r0) {
    #if defined(STM32H7)
    // Configure write-once power options, and wait for voltage levels to be ready
    PWR->CR3 = PWR_CR3_LDOEN;
    while (!(PWR->CSR1 & PWR_CSR1_ACTVOSRDY)) {
    }

    // Reset the kernel clock configuration registers for all domains.
    RCC->D1CCIPR = 0x00000000;
    RCC->D2CCIP1R = 0x00000000;
    RCC->D2CCIP2R = 0x00000000;
    RCC->D3CCIPR = 0x00000000;
    #endif

    // Make sure IRQ vector table points to flash where this bootloader lives.
    SCB->VTOR = MBOOT_VTOR;

    // Enable 8-byte stack alignment for IRQ handlers, in accord with EABI
    SCB->CCR |= SCB_CCR_STKALIGN_Msk;

    #if defined(STM32F4)
    #if INSTRUCTION_CACHE_ENABLE
    __HAL_FLASH_INSTRUCTION_CACHE_ENABLE();
    #endif
    #if DATA_CACHE_ENABLE
    __HAL_FLASH_DATA_CACHE_ENABLE();
    #endif
    #if PREFETCH_ENABLE
    __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
    #endif
    #elif defined(STM32F7)
    #if ART_ACCLERATOR_ENABLE
    __HAL_FLASH_ART_ENABLE();
    #endif
    #endif

    NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

    #if USE_CACHE && defined(STM32F7)
    SCB_EnableICache();
    SCB_EnableDCache();
    #endif

    #if defined(MBOOT_BOARD_EARLY_INIT)
    MBOOT_BOARD_EARLY_INIT();
    #endif

    #ifdef MBOOT_BOOTPIN_PIN
    mp_hal_pin_config(MBOOT_BOOTPIN_PIN, MP_HAL_PIN_MODE_INPUT, MBOOT_BOOTPIN_PULL, 0);
    if (mp_hal_pin_read(MBOOT_BOOTPIN_PIN) == MBOOT_BOOTPIN_ACTIVE) {
        goto enter_bootloader;
    }
    #endif

    if ((initial_r0 & 0xffffff00) == 0x70ad0000) {
        goto enter_bootloader;
    }

    int reset_mode = mboot_get_reset_mode();
    if (reset_mode != BOARDCTRL_RESET_MODE_BOOTLOADER) {
        // Bootloader mode was not selected so try to enter the application,
        // passing through the reset_mode.  This will return if the application
        // is invalid.
        try_enter_application(reset_mode);
    }

enter_bootloader:

    // Init subsystems (mboot_get_reset_mode() may call these, calling them again is ok)
    led_init();

    // set the system clock to be HSE
    SystemClock_Config();

    #if USE_USB_POLLING
    // irqs with a priority value greater or equal to "pri" will be disabled
    // "pri" should be between 1 and 15 inclusive
    uint32_t pri = 2;
    pri <<= (8 - __NVIC_PRIO_BITS);
    __ASM volatile ("msr basepri_max, %0" : : "r" (pri) : "memory");
    #endif

    #if defined(MBOOT_SPIFLASH_ADDR)
    MBOOT_SPIFLASH_SPIFLASH->config = MBOOT_SPIFLASH_CONFIG;
    mp_spiflash_init(MBOOT_SPIFLASH_SPIFLASH);
    #endif

    #if defined(MBOOT_SPIFLASH2_ADDR)
    MBOOT_SPIFLASH2_SPIFLASH->config = MBOOT_SPIFLASH2_CONFIG;
    mp_spiflash_init(MBOOT_SPIFLASH2_SPIFLASH);
    #endif

    #if MBOOT_ENABLE_PACKING
    mboot_pack_init();
    #endif

    #if MBOOT_FSLOAD
    if ((initial_r0 & 0xffffff80) == 0x70ad0080) {
        // Application passed through elements, validate then process them
        const uint8_t *elem_end = elem_search(ELEM_DATA_START, ELEM_TYPE_END);
        if (elem_end != NULL && elem_end[-1] == 0) {
            int ret = fsload_process();
            // If there is a valid ELEM_TYPE_STATUS element then store the status in the given location.
            const uint8_t *elem_status = elem_search(ELEM_DATA_START, ELEM_TYPE_STATUS);
            if (elem_status != NULL && elem_status[-1] == 4) {
                uint32_t *status_ptr = (uint32_t *)get_le32(&elem_status[0]);
                LL_PWR_EnableBkUpAccess(); // In case status_ptr points to backup registers
                *status_ptr = ret;
            }
        }
        // Always reset because the application is expecting to resume
        led_state_all(0);
        leave_bootloader();
    }
    #endif

    dfu_init();

    pyb_usbdd_init(&pyb_usbdd, pyb_usbdd_detect_port());
    pyb_usbdd_start(&pyb_usbdd);

    #if defined(MBOOT_I2C_SCL)
    initial_r0 &= 0x7f;
    if (initial_r0 == 0) {
        initial_r0 = 0x23; // Default I2C address
    }
    i2c_init(initial_r0);
    #endif

    led_state_all(0);
    led0_state(LED0_STATE_SLOW_FLASH);

    #if MBOOT_USB_RESET_ON_DISCONNECT
    bool has_connected = false;
    #endif
    while (!dfu_context.leave_dfu) {
        #if USE_USB_POLLING
        #if MBOOT_USB_AUTODETECT_PORT || MICROPY_HW_USB_MAIN_DEV == USB_PHY_FS_ID
        if (USB_OTG_FS->GINTSTS & USB_OTG_FS->GINTMSK) {
            HAL_PCD_IRQHandler(&pcd_fs_handle);
        }
        #endif
        #if MBOOT_USB_AUTODETECT_PORT || MICROPY_HW_USB_MAIN_DEV == USB_PHY_HS_ID
        if (USB_OTG_HS->GINTSTS & USB_OTG_HS->GINTMSK) {
            HAL_PCD_IRQHandler(&pcd_hs_handle);
        }
        #endif
        if (!pyb_usbdd.tx_pending) {
            dfu_process();
        }
        #else // !USE_USB_POLLING
        __WFI();
        #endif

        #if MBOOT_USB_RESET_ON_DISCONNECT
        if (pyb_usbdd.hUSBDDevice.dev_state == USBD_STATE_CONFIGURED) {
            has_connected = true;
        }
        if (has_connected && pyb_usbdd.hUSBDDevice.dev_state == USBD_STATE_SUSPENDED) {
            break;
        }
        #endif
    }

    // Shutdown and leave the bootloader.
    led_state_all(0);
    mp_hal_delay_ms(50);
    pyb_usbdd_shutdown();
    #if defined(MBOOT_I2C_SCL)
    i2c_slave_shutdown(MBOOT_I2Cx, I2Cx_EV_IRQn);
    #endif
    mp_hal_delay_ms(50);
    leave_bootloader();
}

void NMI_Handler(void) {
}

void MemManage_Handler(void) {
    while (1) {
        __fatal_error("MemManage");
    }
}

void BusFault_Handler(void) {
    while (1) {
        __fatal_error("BusFault");
    }
}

void UsageFault_Handler(void) {
    while (1) {
        __fatal_error("UsageFault");
    }
}

void SVC_Handler(void) {
}

void DebugMon_Handler(void) {
}

void PendSV_Handler(void) {
}

void SysTick_Handler(void) {
    systick_ms += 1;

    // Read the systick control regster. This has the side effect of clearing
    // the COUNTFLAG bit, which makes the logic in mp_hal_ticks_us
    // work properly.
    SysTick->CTRL;

    // Update the LED0 state from here to ensure it's consistent regardless of
    // other processing going on in interrupts or main.
    led0_update();
}

#if defined(MBOOT_I2C_SCL)
void I2Cx_EV_IRQHandler(void) {
    i2c_slave_ev_irq_handler(MBOOT_I2Cx);
}
#endif

#if !USE_USB_POLLING

#if defined(STM32WB)

void USB_LP_IRQHandler(void) {
    HAL_PCD_IRQHandler(&pcd_fs_handle);
}

#else

#if MBOOT_USB_AUTODETECT_PORT || MICROPY_HW_USB_MAIN_DEV == USB_PHY_FS_ID
void OTG_FS_IRQHandler(void) {
    HAL_PCD_IRQHandler(&pcd_fs_handle);
}
#endif

#if MBOOT_USB_AUTODETECT_PORT || MICROPY_HW_USB_MAIN_DEV == USB_PHY_HS_ID
void OTG_HS_IRQHandler(void) {
    HAL_PCD_IRQHandler(&pcd_hs_handle);
}
#endif
#endif

#endif
