/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017-2018 Damien P. George
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
#include "extmod/crypto-algorithms/sha256.c"
#include "usbd_core.h"
#include "storage.h"
#include "i2cslave.h"

// Using polling is about 10% faster than not using it (and using IRQ instead)
// This DFU code with polling runs in about 70% of the time of the ST bootloader
#define USE_USB_POLLING (1)

// Using cache probably won't make it faster because we run at 48MHz, and best
// to keep the MCU config as minimal as possible.
#define USE_CACHE (0)

// IRQ priorities (encoded values suitable for NVIC_SetPriority)
#define IRQ_PRI_SYSTICK (NVIC_EncodePriority(NVIC_PRIORITYGROUP_4, 0, 0))
#define IRQ_PRI_I2C (NVIC_EncodePriority(NVIC_PRIORITYGROUP_4, 1, 0))

// Configure PLL to give a 48MHz CPU freq
#define CORE_PLL_FREQ (48000000)
#undef MICROPY_HW_CLK_PLLM
#undef MICROPY_HW_CLK_PLLN
#undef MICROPY_HW_CLK_PLLP
#undef MICROPY_HW_CLK_PLLQ
#undef MICROPY_HW_FLASH_LATENCY
#define MICROPY_HW_CLK_PLLM (HSE_VALUE / 1000000)
#define MICROPY_HW_CLK_PLLN (192)
#define MICROPY_HW_CLK_PLLP (RCC_PLLP_DIV4)
#define MICROPY_HW_CLK_PLLQ (4)
#define MICROPY_HW_FLASH_LATENCY FLASH_LATENCY_1

// Work out which USB device to use for the USB DFU interface
#if !defined(MICROPY_HW_USB_MAIN_DEV)
#if MICROPY_HW_USB_FS
#define MICROPY_HW_USB_MAIN_DEV (USB_PHY_FS_ID)
#elif MICROPY_HW_USB_HS && MICROPY_HW_USB_HS_IN_FS
#define MICROPY_HW_USB_MAIN_DEV (USB_PHY_HS_ID)
#else
#error Unable to determine proper MICROPY_HW_USB_MAIN_DEV to use
#endif
#endif

// These bits are used to detect valid application firmware at APPLICATION_ADDR
#define APP_VALIDITY_BITS (0x00000003)

#define MP_ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

static void do_reset(void);

static uint32_t get_le32(const uint8_t *b) {
    return b[0] | b[1] << 8 | b[2] << 16 | b[3] << 24;
}

void mp_hal_delay_us(mp_uint_t usec) {
    // use a busy loop for the delay
    // sys freq is always a multiple of 2MHz, so division here won't lose precision
    const uint32_t ucount = CORE_PLL_FREQ / 2000000 * usec / 2;
    for (uint32_t count = 0; ++count <= ucount;) {
    }
}

static volatile uint32_t systick_ms;

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

static void __fatal_error(const char *msg) {
    NVIC_SystemReset();
    for (;;) {
    }
}

/******************************************************************************/
// CLOCK

#if defined(STM32F4) || defined(STM32F7)

#define CONFIG_RCC_CR_1ST (RCC_CR_HSION)
#define CONFIG_RCC_CR_2ND (RCC_CR_HSEON || RCC_CR_CSSON || RCC_CR_PLLON)
#define CONFIG_RCC_PLLCFGR (0x24003010)

#else
#error Unknown processor
#endif

void SystemInit(void) {
    // Set HSION bit
    RCC->CR |= CONFIG_RCC_CR_1ST;

    // Reset CFGR register
    RCC->CFGR = 0x00000000;

    // Reset HSEON, CSSON and PLLON bits
    RCC->CR &= ~CONFIG_RCC_CR_2ND;

    // Reset PLLCFGR register
    RCC->PLLCFGR = CONFIG_RCC_PLLCFGR;

    // Reset HSEBYP bit
    RCC->CR &= (uint32_t)0xFFFBFFFF;

    // Disable all interrupts
    RCC->CIR = 0x00000000;

    // Set location of vector table
    SCB->VTOR = FLASH_BASE;

    // Enable 8-byte stack alignment for IRQ handlers, in accord with EABI
    SCB->CCR |= SCB_CCR_STKALIGN_Msk;
}

void systick_init(void) {
    // Configure SysTick as 1ms ticker
    SysTick_Config(SystemCoreClock / 1000);
    NVIC_SetPriority(SysTick_IRQn, IRQ_PRI_SYSTICK);
}

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

// Needed by HAL_PCD_IRQHandler
uint32_t HAL_RCC_GetHCLKFreq(void) {
    return SystemCoreClock;
}

/******************************************************************************/
// GPIO

void mp_hal_pin_config(mp_hal_pin_obj_t port_pin, uint32_t mode, uint32_t pull, uint32_t alt) {
    GPIO_TypeDef *gpio = (GPIO_TypeDef*)(port_pin & ~0xf);

    // Enable the GPIO peripheral clock
    uint32_t en_bit = RCC_AHB1ENR_GPIOAEN_Pos + ((uintptr_t)gpio - GPIOA_BASE) / (GPIOB_BASE - GPIOA_BASE);
    RCC->AHB1ENR |= 1 << en_bit;
    volatile uint32_t tmp = RCC->AHB1ENR; // Delay after enabling clock
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

#define LED0 MICROPY_HW_LED1
#define LED1 MICROPY_HW_LED2
#define LED2 MICROPY_HW_LED3

void led_init(void) {
    mp_hal_pin_output(LED0);
    mp_hal_pin_output(LED1);
    mp_hal_pin_output(LED2);
}

void led_state(int led, int val) {
    if (led == 1) {
        led = LED0;
    }
    if (val) {
        MICROPY_HW_LED_ON(led);
    } else {
        MICROPY_HW_LED_OFF(led);
    }
}

/******************************************************************************/
// USR BUTTON

static void usrbtn_init(void) {
    mp_hal_pin_config(MICROPY_HW_USRSW_PIN, MP_HAL_PIN_MODE_INPUT, MICROPY_HW_USRSW_PULL, 0);
}

static int usrbtn_state(void) {
    return mp_hal_pin_read(MICROPY_HW_USRSW_PIN) == MICROPY_HW_USRSW_PRESSED;
}

/******************************************************************************/
// FLASH

#ifndef MBOOT_SPIFLASH_LAYOUT
#define MBOOT_SPIFLASH_LAYOUT ""
#endif

#ifndef MBOOT_SPIFLASH2_LAYOUT
#define MBOOT_SPIFLASH2_LAYOUT ""
#endif

typedef struct {
    uint32_t base_address;
    uint32_t sector_size;
    uint32_t sector_count;
} flash_layout_t;

#if defined(STM32F7)
// FLASH_FLAG_PGSERR (Programming Sequence Error) was renamed to
// FLASH_FLAG_ERSERR (Erasing Sequence Error) in STM32F7
#define FLASH_FLAG_PGSERR FLASH_FLAG_ERSERR
#endif

#if defined(STM32F4) \
    || defined(STM32F722xx) \
    || defined(STM32F723xx) \
    || defined(STM32F732xx) \
    || defined(STM32F733xx)

#define FLASH_LAYOUT_STR "@Internal Flash  /0x08000000/04*016Kg,01*064Kg,07*128Kg" MBOOT_SPIFLASH_LAYOUT MBOOT_SPIFLASH2_LAYOUT

static const flash_layout_t flash_layout[] = {
    { 0x08000000, 0x04000, 4 },
    { 0x08010000, 0x10000, 1 },
    { 0x08020000, 0x20000, 3 },
    #if defined(FLASH_SECTOR_8)
    { 0x08080000, 0x20000, 4 },
    #endif
    #if defined(FLASH_SECTOR_12)
    { 0x08100000, 0x04000, 4 },
    { 0x08110000, 0x10000, 1 },
    { 0x08120000, 0x20000, 7 },
    #endif
};

#elif defined(STM32F767xx)

#define FLASH_LAYOUT_STR "@Internal Flash  /0x08000000/04*032Kg,01*128Kg,07*256Kg" MBOOT_SPIFLASH_LAYOUT MBOOT_SPIFLASH2_LAYOUT

// This is for dual-bank mode disabled
static const flash_layout_t flash_layout[] = {
    { 0x08000000, 0x08000, 4 },
    { 0x08020000, 0x20000, 1 },
    { 0x08040000, 0x40000, 7 },
};

#endif

static uint32_t flash_get_sector_index(uint32_t addr) {
    if (addr >= flash_layout[0].base_address) {
        uint32_t sector_index = 0;
        for (int i = 0; i < MP_ARRAY_SIZE(flash_layout); ++i) {
            for (int j = 0; j < flash_layout[i].sector_count; ++j) {
                uint32_t sector_start_next = flash_layout[i].base_address
                    + (j + 1) * flash_layout[i].sector_size;
                if (addr < sector_start_next) {
                    return sector_index;
                }
                ++sector_index;
            }
        }
    }
    return 0;
}

static int flash_mass_erase(void) {
    // TODO
    return -1;
}

static int flash_page_erase(uint32_t addr) {
    uint32_t sector = flash_get_sector_index(addr);
    if (sector == 0) {
        // Don't allow to erase the sector with this bootloader in it
        return -1;
    }

    HAL_FLASH_Unlock();

    // Clear pending flags (if any)
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                           FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

    // erase the sector(s)
    FLASH_EraseInitTypeDef EraseInitStruct;
    EraseInitStruct.TypeErase = TYPEERASE_SECTORS;
    EraseInitStruct.VoltageRange = VOLTAGE_RANGE_3; // voltage range needs to be 2.7V to 3.6V
    EraseInitStruct.Sector = sector;
    EraseInitStruct.NbSectors = 1;

    uint32_t SectorError = 0;
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK) {
        // error occurred during sector erase
        return -1;
    }

    // Check the erase set bits to 1, at least for the first 256 bytes
    for (int i = 0; i < 64; ++i) {
        if (((volatile uint32_t*)addr)[i] != 0xffffffff) {
            return -2;
        }
    }

    return 0;
}

static int flash_write(uint32_t addr, const uint8_t *src8, size_t len) {
    if (addr >= flash_layout[0].base_address && addr < flash_layout[0].base_address + flash_layout[0].sector_size) {
        // Don't allow to write the sector with this bootloader in it
        return -1;
    }

    const uint32_t *src = (const uint32_t*)src8;
    size_t num_word32 = (len + 3) / 4;
    HAL_FLASH_Unlock();
    // program the flash word by word
    for (size_t i = 0; i < num_word32; i++) {
        if (HAL_FLASH_Program(TYPEPROGRAM_WORD, addr, *src) != HAL_OK) {
            return -1;
        }
        addr += 4;
        src += 1;
    }

    // TODO verify data

    return 0;
}

/******************************************************************************/
// Writable address space interface

static int do_mass_erase(void) {
    // TODO
    return flash_mass_erase();
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

static int do_page_erase(uint32_t addr) {
    led_state(LED0, 1);

    #if defined(MBOOT_SPIFLASH_ADDR)
    if (MBOOT_SPIFLASH_ADDR <= addr && addr < MBOOT_SPIFLASH_ADDR + MBOOT_SPIFLASH_BYTE_SIZE) {
        return spiflash_page_erase(MBOOT_SPIFLASH_SPIFLASH,
            addr - MBOOT_SPIFLASH_ADDR, MBOOT_SPIFLASH_ERASE_BLOCKS_PER_PAGE);
    }
    #endif

    #if defined(MBOOT_SPIFLASH2_ADDR)
    if (MBOOT_SPIFLASH2_ADDR <= addr && addr < MBOOT_SPIFLASH2_ADDR + MBOOT_SPIFLASH2_BYTE_SIZE) {
        return spiflash_page_erase(MBOOT_SPIFLASH2_SPIFLASH,
            addr - MBOOT_SPIFLASH2_ADDR, MBOOT_SPIFLASH2_ERASE_BLOCKS_PER_PAGE);
    }
    #endif

    return flash_page_erase(addr);
}

static void do_read(uint32_t addr, int len, uint8_t *buf) {
    #if defined(MBOOT_SPIFLASH_ADDR)
    if (MBOOT_SPIFLASH_ADDR <= addr && addr < MBOOT_SPIFLASH_ADDR + MBOOT_SPIFLASH_BYTE_SIZE) {
        mp_spiflash_read(MBOOT_SPIFLASH_SPIFLASH, addr - MBOOT_SPIFLASH_ADDR, len, buf);
        return;
    }
    #endif
    #if defined(MBOOT_SPIFLASH2_ADDR)
    if (MBOOT_SPIFLASH2_ADDR <= addr && addr < MBOOT_SPIFLASH2_ADDR + MBOOT_SPIFLASH2_BYTE_SIZE) {
        mp_spiflash_read(MBOOT_SPIFLASH2_SPIFLASH, addr - MBOOT_SPIFLASH2_ADDR, len, buf);
        return;
    }
    #endif

    // Other addresses, just read directly from memory
    memcpy(buf, (void*)addr, len);
}

static int do_write(uint32_t addr, const uint8_t *src8, size_t len) {
    static uint32_t led_tog = 0;
    led_state(LED0, (led_tog++) & 4);

    #if defined(MBOOT_SPIFLASH_ADDR)
    if (MBOOT_SPIFLASH_ADDR <= addr && addr < MBOOT_SPIFLASH_ADDR + MBOOT_SPIFLASH_BYTE_SIZE) {
        return mp_spiflash_write(MBOOT_SPIFLASH_SPIFLASH, addr - MBOOT_SPIFLASH_ADDR, len, src8);
    }
    #endif

    #if defined(MBOOT_SPIFLASH2_ADDR)
    if (MBOOT_SPIFLASH2_ADDR <= addr && addr < MBOOT_SPIFLASH2_ADDR + MBOOT_SPIFLASH2_BYTE_SIZE) {
        return mp_spiflash_write(MBOOT_SPIFLASH2_SPIFLASH, addr - MBOOT_SPIFLASH2_ADDR, len, src8);
    }
    #endif

    return flash_write(addr, src8, len);
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

int i2c_slave_process_addr_match(int rw) {
    if (i2c_obj.cmd_arg_sent) {
        i2c_obj.cmd_send_arg = false;
    }
    i2c_obj.cmd_buf_pos = 0;
    return 0; // ACK
}

int i2c_slave_process_rx_byte(uint8_t val) {
    if (i2c_obj.cmd_buf_pos < sizeof(i2c_obj.cmd_buf)) {
        i2c_obj.cmd_buf[i2c_obj.cmd_buf_pos++] = val;
    }
    return 0; // ACK
}

void i2c_slave_process_rx_end(void) {
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
        do_reset();
    } else if (buf[0] == I2C_CMD_GETLAYOUT && len == 0) {
        len = strlen(FLASH_LAYOUT_STR);
        memcpy(buf, FLASH_LAYOUT_STR, len);
    } else if (buf[0] == I2C_CMD_MASSERASE && len == 0) {
        len = do_mass_erase();
    } else if (buf[0] == I2C_CMD_PAGEERASE && len == 4) {
        len = do_page_erase(get_le32(buf + 1));
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

uint8_t i2c_slave_process_tx_byte(void) {
    if (i2c_obj.cmd_send_arg) {
        i2c_obj.cmd_arg_sent = true;
        return i2c_obj.cmd_arg;
    } else if (i2c_obj.cmd_buf_pos < sizeof(i2c_obj.cmd_buf)) {
        return i2c_obj.cmd_buf[i2c_obj.cmd_buf_pos++];
    } else {
        return 0;
    }
}

#endif // defined(MBOOT_I2C_SCL)

/******************************************************************************/
// DFU

#define DFU_XFER_SIZE (2048)

enum {
    DFU_DNLOAD = 1,
    DFU_UPLOAD = 2,
    DFU_GETSTATUS = 3,
    DFU_CLRSTATUS = 4,
    DFU_ABORT = 6,
};

enum {
    DFU_STATUS_IDLE = 2,
    DFU_STATUS_BUSY = 4,
    DFU_STATUS_DNLOAD_IDLE = 5,
    DFU_STATUS_MANIFEST = 7,
    DFU_STATUS_UPLOAD_IDLE = 9,
    DFU_STATUS_ERROR = 0xa,
};

enum {
    DFU_CMD_NONE = 0,
    DFU_CMD_EXIT = 1,
    DFU_CMD_UPLOAD = 7,
    DFU_CMD_DNLOAD = 8,
};

typedef struct _dfu_state_t {
    int status;
    int cmd;
    uint16_t wBlockNum;
    uint16_t wLength;
    uint32_t addr;
    uint8_t buf[DFU_XFER_SIZE] __attribute__((aligned(4)));
} dfu_state_t;

static dfu_state_t dfu_state;

static void dfu_init(void) {
    dfu_state.status = DFU_STATUS_IDLE;
    dfu_state.cmd = DFU_CMD_NONE;
    dfu_state.addr = 0x08000000;
}

static int dfu_process_dnload(void) {
    int ret = -1;
    if (dfu_state.wBlockNum == 0) {
        // download control commands
        if (dfu_state.wLength >= 1 && dfu_state.buf[0] == 0x41) {
            if (dfu_state.wLength == 1) {
                // mass erase
                ret = do_mass_erase();
            } else if (dfu_state.wLength == 5) {
                // erase page
                ret = do_page_erase(get_le32(&dfu_state.buf[1]));
            }
        } else if (dfu_state.wLength >= 1 && dfu_state.buf[0] == 0x21) {
            if (dfu_state.wLength == 5) {
                // set address
                dfu_state.addr = get_le32(&dfu_state.buf[1]);
                ret = 0;
            }
        }
    } else if (dfu_state.wBlockNum > 1) {
        // write data to memory
        uint32_t addr = (dfu_state.wBlockNum - 2) * DFU_XFER_SIZE + dfu_state.addr;
        ret = do_write(addr, dfu_state.buf, dfu_state.wLength);
    }
    if (ret == 0) {
        return DFU_STATUS_DNLOAD_IDLE;
    } else {
        return DFU_STATUS_ERROR;
    }
}

static void dfu_handle_rx(int cmd, int arg, int len, const void *buf) {
    if (cmd == DFU_CLRSTATUS) {
        // clear status
        dfu_state.status = DFU_STATUS_IDLE;
        dfu_state.cmd = DFU_CMD_NONE;
    } else if (cmd == DFU_ABORT) {
        // clear status
        dfu_state.status = DFU_STATUS_IDLE;
        dfu_state.cmd = DFU_CMD_NONE;
    } else if (cmd == DFU_DNLOAD) {
        if (len == 0) {
            // exit DFU
            dfu_state.cmd = DFU_CMD_EXIT;
        } else {
            // download
            dfu_state.cmd = DFU_CMD_DNLOAD;
            dfu_state.wBlockNum = arg;
            dfu_state.wLength = len;
            memcpy(dfu_state.buf, buf, len);
        }
    }
}

static void dfu_process(void) {
    if (dfu_state.status == DFU_STATUS_MANIFEST) {
        do_reset();
    }

    if (dfu_state.status == DFU_STATUS_BUSY) {
        if (dfu_state.cmd == DFU_CMD_DNLOAD) {
            dfu_state.cmd = DFU_CMD_NONE;
            dfu_state.status = dfu_process_dnload();
        }
    }
}

static int dfu_handle_tx(int cmd, int arg, int len, uint8_t *buf, int max_len) {
    if (cmd == DFU_UPLOAD) {
        if (arg >= 2) {
            dfu_state.cmd = DFU_CMD_UPLOAD;
            uint32_t addr = (arg - 2) * max_len + dfu_state.addr;
            do_read(addr, len, buf);
            return len;
        }
    } else if (cmd == DFU_GETSTATUS && len == 6) {
        // execute command and get status
        switch (dfu_state.cmd) {
            case DFU_CMD_NONE:
                break;
            case DFU_CMD_EXIT:
                dfu_state.status = DFU_STATUS_MANIFEST;
                break;
            case DFU_CMD_UPLOAD:
                dfu_state.status = DFU_STATUS_UPLOAD_IDLE;
                break;
            case DFU_CMD_DNLOAD:
                dfu_state.status = DFU_STATUS_BUSY;
                break;
        }
        buf[0] = 0;
        buf[1] = dfu_state.cmd; // TODO is this correct?
        buf[2] = 0;
        buf[3] = 0;
        buf[4] = dfu_state.status;
        buf[5] = 0;
        return 6;
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

#define USBD_LANGID_STRING (0x409)

__ALIGN_BEGIN static const uint8_t USBD_LangIDDesc[USB_LEN_LANGID_STR_DESC] __ALIGN_END = {
    USB_LEN_LANGID_STR_DESC,
    USB_DESC_TYPE_STRING,
    LOBYTE(USBD_LANGID_STRING),
    HIBYTE(USBD_LANGID_STRING),
};

static const uint8_t dev_descr[0x12] = "\x12\x01\x00\x01\x00\x00\x00\x40\x83\x04\x11\xdf\x00\x22\x01\x02\x03\x01";

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
            USBD_GetString((uint8_t*)"USBDevice Manuf", str_desc, length);
            return str_desc;

        case USBD_IDX_PRODUCT_STR:
            USBD_GetString((uint8_t*)"USBDevice Product", str_desc, length);
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

static pyb_usbdd_obj_t pyb_usbdd;

static void pyb_usbdd_init(pyb_usbdd_obj_t *self, int phy_id) {
    self->started = false;
    USBD_HandleTypeDef *usbd = &self->hUSBDDevice;
    usbd->id = phy_id;
    usbd->dev_state = USBD_STATE_DEFAULT;
    usbd->pDesc = (USBD_DescriptorsTypeDef*)&pyb_usbdd_descriptors;
    usbd->pClass = &pyb_usbdd_class;
    usbd->pClassData = self;
}

static void pyb_usbdd_start(pyb_usbdd_obj_t *self) {
    if (!self->started) {
        USBD_LL_Init(&self->hUSBDDevice, 0);
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

#define RESET_MODE_NUM_STATES (4)
#define RESET_MODE_TIMEOUT_CYCLES (8)
#define RESET_MODE_LED_STATES 0x7421

static int get_reset_mode(void) {
    usrbtn_init();
    int reset_mode = 1;
    if (usrbtn_state()) {
        // Cycle through reset modes while USR is held
        // Timeout is roughly 20s, where reset_mode=1
        systick_init();
        led_init();
        reset_mode = 0;
        for (int i = 0; i < (RESET_MODE_NUM_STATES * RESET_MODE_TIMEOUT_CYCLES + 1) * 32; i++) {
            if (i % 32 == 0) {
                if (++reset_mode > RESET_MODE_NUM_STATES) {
                    reset_mode = 1;
                }
                uint8_t l = RESET_MODE_LED_STATES >> ((reset_mode - 1) * 4);
                led_state(LED0, l & 1);
                led_state(LED1, l & 2);
                led_state(LED2, l & 4);
            }
            if (!usrbtn_state()) {
                break;
            }
            mp_hal_delay_ms(19);
        }
        // Flash the selected reset mode
        for (int i = 0; i < 6; i++) {
            led_state(LED0, 0);
            led_state(LED1, 0);
            led_state(LED2, 0);
            mp_hal_delay_ms(50);
            uint8_t l = RESET_MODE_LED_STATES >> ((reset_mode - 1) * 4);
            led_state(LED0, l & 1);
            led_state(LED1, l & 2);
            led_state(LED2, l & 4);
            mp_hal_delay_ms(50);
        }
        mp_hal_delay_ms(300);
    }
    return reset_mode;
}

static void do_reset(void) {
    led_state(LED0, 0);
    led_state(LED1, 0);
    led_state(LED2, 0);
    mp_hal_delay_ms(50);
    pyb_usbdd_shutdown();
    #if defined(MBOOT_I2C_SCL)
    i2c_slave_shutdown(MBOOT_I2Cx, I2Cx_EV_IRQn);
    #endif
    mp_hal_delay_ms(50);
    NVIC_SystemReset();
}

uint32_t SystemCoreClock;

extern PCD_HandleTypeDef pcd_fs_handle;
extern PCD_HandleTypeDef pcd_hs_handle;

void stm32_main(int initial_r0) {
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

    #ifdef MBOOT_BOOTPIN_PIN
    mp_hal_pin_config(MBOOT_BOOTPIN_PIN, MP_HAL_PIN_MODE_INPUT, MBOOT_BOOTPIN_PULL, 0);
    if (mp_hal_pin_read(MBOOT_BOOTPIN_PIN) == MBOOT_BOOTPIN_ACTIVE) {
        goto enter_bootloader;
    }
    #endif

    if ((initial_r0 & 0xffffff00) == 0x70ad0000) {
        goto enter_bootloader;
    }

    // MCU starts up with 16MHz HSI
    SystemCoreClock = 16000000;

    int reset_mode = get_reset_mode();
    uint32_t msp = *(volatile uint32_t*)APPLICATION_ADDR;
    if (reset_mode != 4 && (msp & APP_VALIDITY_BITS) == 0) {
        // not DFU mode so jump to application, passing through reset_mode
        // undo our DFU settings
        // TODO probably should disable all IRQ sources first
        #if USE_CACHE && defined(STM32F7)
        SCB_DisableICache();
        SCB_DisableDCache();
        #endif
        __set_MSP(msp);
        ((void (*)(uint32_t)) *((volatile uint32_t*)(APPLICATION_ADDR + 4)))(reset_mode);
    }

enter_bootloader:

    // Init subsystems (get_reset_mode() may call these, calling them again is ok)
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

    dfu_init();

    pyb_usbdd_init(&pyb_usbdd, MICROPY_HW_USB_MAIN_DEV);
    pyb_usbdd_start(&pyb_usbdd);

    #if defined(MBOOT_I2C_SCL)
    initial_r0 &= 0x7f;
    if (initial_r0 == 0) {
        initial_r0 = 0x23; // Default I2C address
    }
    i2c_init(initial_r0);
    #endif

    led_state(LED0, 0);
    led_state(LED1, 0);
    led_state(LED2, 0);

    #if USE_USB_POLLING
    uint32_t ss = systick_ms;
    int ss2 = -1;
    #endif
    for (;;) {
        #if USE_USB_POLLING
        #if MICROPY_HW_USB_MAIN_DEV == USB_PHY_FS_ID
        if (USB_OTG_FS->GINTSTS & USB_OTG_FS->GINTMSK) {
            HAL_PCD_IRQHandler(&pcd_fs_handle);
        }
        #else
        if (USB_OTG_HS->GINTSTS & USB_OTG_HS->GINTMSK) {
            HAL_PCD_IRQHandler(&pcd_hs_handle);
        }
        #endif
        if (!pyb_usbdd.tx_pending) {
            dfu_process();
        }
        #endif

        #if USE_USB_POLLING
        //__WFI(); // slows it down way too much; might work with 10x faster systick
        if (systick_ms - ss > 50) {
            ss += 50;
            ss2 = (ss2 + 1) % 20;
            switch (ss2) {
                case 0: led_state(LED0, 1); break;
                case 1: led_state(LED0, 0); break;
            }
        }
        #else
        led_state(LED0, 1);
        mp_hal_delay_ms(50);
        led_state(LED0, 0);
        mp_hal_delay_ms(950);
        #endif
    }
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
}

#if defined(MBOOT_I2C_SCL)
void I2Cx_EV_IRQHandler(void) {
    i2c_slave_ev_irq_handler(MBOOT_I2Cx);
}
#endif

#if !USE_USB_POLLING
#if MICROPY_HW_USB_MAIN_DEV == USB_PHY_FS_ID
void OTG_FS_IRQHandler(void) {
    HAL_PCD_IRQHandler(&pcd_fs_handle);
}
#else
void OTG_HS_IRQHandler(void) {
    HAL_PCD_IRQHandler(&pcd_hs_handle);
}
#endif
#endif
