/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024-2025 Damien P. George
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

#include "py/mperrno.h"
#include "py/mphal.h"

#if defined(MICROPY_HW_XSPIFLASH_SIZE_BITS_LOG2)

#ifndef MICROPY_HW_XSPI_PRESCALER
#define MICROPY_HW_XSPI_PRESCALER (4) // F_CLK = F_AHB/4
#endif

#ifndef MICROPY_HW_XSPI_CS_HIGH_CYCLES
#define MICROPY_HW_XSPI_CS_HIGH_CYCLES (4) // nCS stays high for 4 cycles
#endif

// For XSPI2, PN0 through PN12.
#define XSPI2_AF (9)

static inline void mp_hal_pin_config_alt_speed(mp_hal_pin_obj_t pin, uint32_t pull, uint32_t alt, uint32_t speed) {
    mp_hal_pin_config(pin, MP_HAL_PIN_MODE_ALT, pull, alt);
    mp_hal_pin_config_speed(pin, speed);
}

static int xspi_read_111(uint8_t cmd, size_t len, uint8_t *dest);
static void xspi_memory_map_111(void);
static void xspi_memory_map_888(void);

void xspi_flash_init(void) {
    // Configure XSPI pins.
    mp_hal_pin_config_alt_speed(pyb_pin_XSPIM_P2_CS, MP_HAL_PIN_PULL_NONE, XSPI2_AF, MP_HAL_PIN_SPEED_VERY_HIGH);
    mp_hal_pin_config_alt_speed(pyb_pin_XSPIM_P2_SCK, MP_HAL_PIN_PULL_NONE, XSPI2_AF, MP_HAL_PIN_SPEED_VERY_HIGH);
    mp_hal_pin_config_alt_speed(pyb_pin_XSPIM_P2_DQS, MP_HAL_PIN_PULL_NONE, XSPI2_AF, MP_HAL_PIN_SPEED_VERY_HIGH);
    mp_hal_pin_config_alt_speed(pyb_pin_XSPIM_P2_IO0, MP_HAL_PIN_PULL_NONE, XSPI2_AF, MP_HAL_PIN_SPEED_VERY_HIGH);
    mp_hal_pin_config_alt_speed(pyb_pin_XSPIM_P2_IO1, MP_HAL_PIN_PULL_NONE, XSPI2_AF, MP_HAL_PIN_SPEED_VERY_HIGH);
    mp_hal_pin_config_alt_speed(pyb_pin_XSPIM_P2_IO2, MP_HAL_PIN_PULL_NONE, XSPI2_AF, MP_HAL_PIN_SPEED_VERY_HIGH);
    mp_hal_pin_config_alt_speed(pyb_pin_XSPIM_P2_IO3, MP_HAL_PIN_PULL_NONE, XSPI2_AF, MP_HAL_PIN_SPEED_VERY_HIGH);
    mp_hal_pin_config_alt_speed(pyb_pin_XSPIM_P2_IO4, MP_HAL_PIN_PULL_NONE, XSPI2_AF, MP_HAL_PIN_SPEED_VERY_HIGH);
    mp_hal_pin_config_alt_speed(pyb_pin_XSPIM_P2_IO5, MP_HAL_PIN_PULL_NONE, XSPI2_AF, MP_HAL_PIN_SPEED_VERY_HIGH);
    mp_hal_pin_config_alt_speed(pyb_pin_XSPIM_P2_IO6, MP_HAL_PIN_PULL_NONE, XSPI2_AF, MP_HAL_PIN_SPEED_VERY_HIGH);
    mp_hal_pin_config_alt_speed(pyb_pin_XSPIM_P2_IO7, MP_HAL_PIN_PULL_NONE, XSPI2_AF, MP_HAL_PIN_SPEED_VERY_HIGH);

    LL_RCC_SetXSPIClockSource(LL_RCC_XSPI2_CLKSOURCE_HCLK);

    __HAL_RCC_XSPIM_CLK_ENABLE();
    __HAL_RCC_XSPI1_CLK_ENABLE();
    __HAL_RCC_XSPI2_CLK_ENABLE();

    // XSPI power enable.
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_PWREx_EnableVddIO3();                     
    HAL_PWREx_ConfigVddIORange(PWR_VDDIO3, PWR_VDDIO_RANGE_1V8);

    // Configure XSPIM in direct mode.
    XSPI1->CR &= ~XSPI_CR_EN;
    XSPI2->CR &= ~XSPI_CR_EN;
    XSPIM->CR = 0;

    // Configure the XSPI2 peripheral.

    XSPI2->CR =
        0 << XSPI_CR_FTHRES_Pos // 1 byte must be available to read/write
            | 0 << XSPI_CR_MSEL_Pos // FLASH 0 selected
            | 0 << XSPI_CR_CSSEL_Pos // use NCS1 as chip select
            | 0 << XSPI_CR_DMM_Pos // dual-memory mode disabled
            | 0 << XSPI_CR_TCEN_Pos // time-out counter enabled
            | 0 << XSPI_CR_DMAEN_Pos // DMA disabled
            | 0 << XSPI_CR_ABORT_Pos // no abort request
            | 0 << XSPI_CR_EN_Pos // disabled
    ;

    XSPI2->DCR1 =
        1 << XSPI_DCR1_MTYP_Pos // Macronix mode
            | (MICROPY_HW_XSPIFLASH_SIZE_BITS_LOG2 - 3 - 1) << XSPI_DCR1_DEVSIZE_Pos
            | (MICROPY_HW_XSPI_CS_HIGH_CYCLES - 1) << XSPI_DCR1_CSHT_Pos
            | 0 << XSPI_DCR1_FRCK_Pos // CLK is not free running
            | 0 << XSPI_DCR1_CKMODE_Pos // CLK idles at low state
    ;

    XSPI2->DCR2 =
        0 << XSPI_DCR2_WRAPSIZE_Pos // separate wrap reads are not supported by the memory
            | (MICROPY_HW_XSPI_PRESCALER - 1) << XSPI_DCR2_PRESCALER_Pos
    ;

    XSPI2->DCR3 =
        0
        //10 << XSPI_DCR3_CSBOUND_Pos // transaction boundary at 1024
    ;

    XSPI2->DCR4 =
        0 << XSPI_DCR4_REFRESH_Pos // refresh disabled (it's non-volatile memory)
    ;

    XSPI2->TCR = 0;

    // Enable the XSPI peripheral.
    XSPI2->CR |= XSPI_CR_EN;

    // XSPIM init
    XSPI1->CR &= ~(1 << XSPI_CR_EN_Pos);
    XSPI2->CR &= ~(1 << XSPI_CR_EN_Pos);
    XSPIM->CR = 0; // can also be (1 << 4) to pass through CS signal
    //mem32[xspi1 + XSPI_CR] |= 1 << XSPI_CR_EN_Pos
    XSPI2->CR |= 1 << XSPI_CR_EN_Pos;

    //////////////////////////

    #if 0
    for (int i = 0; i < 4; ++i) {
        printf("xspim %x %x\n", i, (int)((volatile uint32_t *)XSPIM)[i]);
    }
    for (int i = 0; i < 0x22c/4; ++i) {
        if (((volatile uint32_t *)XSPI1)[i]) {
            printf("xspi1 %x %x\n", i, (int)((volatile uint32_t *)XSPI1)[i]);
        }
    }
    for (int i = 0; i < 0x22c/4; ++i) {
        if (((volatile uint32_t *)XSPI2)[i]) {
            printf("xspi2 %x %x\n", i, (int)((volatile uint32_t *)XSPI2)[i]);
        }
    }
    #endif
                
    //////////////////////////

    uint8_t buf[4];
    xspi_read_111(0x9f, 3, buf);
    //mp_printf(&mp_plat_print, "xspi flash init id=%x:%x:%x\n", buf[0], buf[1], buf[2]);

    if (1) {
        xspi_memory_map_111();
    }
    if (0) {
        xspi_memory_map_888();
    }
}

static int xspi_read_111(uint8_t cmd, size_t len, uint8_t *dest) {
    XSPI2->FCR = XSPI_FCR_CTCF; // clear TC flag

    XSPI2->DLR = len - 1; // number of bytes to read

    XSPI2->CR = (XSPI2->CR & ~XSPI_CR_FMODE_Msk) | 1 << XSPI_CR_FMODE_Pos; // indirect read mode

    XSPI2->CCR =
        0 << XSPI_CCR_DDTR_Pos // data DTR disabled
            | 1 << XSPI_CCR_DMODE_Pos // data on 1 line
            | 0 << XSPI_CCR_ABMODE_Pos // no alternate byte
            | 0 << XSPI_CCR_ADMODE_Pos // no address
            | 1 << XSPI_CCR_IMODE_Pos // instruction on 1 line
    ;

    XSPI2->TCR = 0 << XSPI_TCR_DCYC_Pos; // 0 dummy cycles

    // This triggers the start of the operation.
    XSPI2->IR = cmd; // read opcode

    // Read in remaining data 1 byte at a time
    while (len) {
        while (!((XSPI2->SR >> XSPI_SR_FLEVEL_Pos) & 0x3f)) {
            if (XSPI2->SR & XSPI_SR_TEF) {
                return -MP_EIO;
            }
        }
        *dest++ = *(volatile uint8_t *)&XSPI2->DR;
        --len;
    }

    XSPI2->FCR = XSPI_FCR_CTCF; // clear TC flag

    return 0;
}

static void xspi_memory_map_111(void) {
    XSPI2->CCR =
        1 << XSPI_CCR_DMODE_Pos // data on 1 line
            | 3 << XSPI_CCR_ADSIZE_Pos // 32-bit address
            | 1 << XSPI_CCR_ADMODE_Pos // address on 1 line
            | 1 << XSPI_CCR_IMODE_Pos // instruction on 1 line
    ;

    XSPI2->TCR = 0 << XSPI_TCR_DCYC_Pos; // no dummy cycles
    XSPI2->IR = 0x13; // READ4B
    XSPI2->LPTR = 1024; // timeout period in number of CLK cycles

    // Enable the XSPI peripheral in memory-mapped mode.
    XSPI2->CR = (XSPI2->CR & ~XSPI_CR_FMODE_Msk) | 3 << XSPI_CR_FMODE_Pos;

    // TODO: enable MPU?
}

static void xspi_memory_map_888(void) {
    XSPI2->CCR =
        1 << XSPI_CCR_DQSE_Pos // DQS enabled
            | 1 << XSPI_CCR_DDTR_Pos // data DTR enabled
            | 4 << XSPI_CCR_DMODE_Pos // data on 8 lines
            | 0 << XSPI_CCR_ABSIZE_Pos // 8-bit alternate bytes
            | 0 << XSPI_CCR_ABDTR_Pos // alternate bytes DTR disabled
            | 0 << XSPI_CCR_ABMODE_Pos // no alternate bytes
            | 3 << XSPI_CCR_ADSIZE_Pos // 32-bit address
            | 1 << XSPI_CCR_ADDTR_Pos // address DTR enabled
            | 4 << XSPI_CCR_ADMODE_Pos // address on 8 lines
            | 1 << XSPI_CCR_ISIZE_Pos // 16-bit instruction
            | 1 << XSPI_CCR_IDTR_Pos // instruction DTR enabled
            | 4 << XSPI_CCR_IMODE_Pos // instruction on 8 lines
    ;

    XSPI2->TCR =
        0 << XSPI_TCR_SSHIFT_Pos // no sample shift
            //| 1 << XSPI_TCR_DHQC_Pos // 1/4 cycle hold enabled
            | 6 << XSPI_TCR_DCYC_Pos // 6 dummy cycles for reading (minimum, flash may insert more by holding DQS low)
    ;

    XSPI2->IR = 0xee11; // OCTA DTR read mode (8DTRD)

    XSPI2->LPTR = 1024; // timeout period in number of CLK cycles

    // Enable the XSPI peripheral in memory-mapped mode.
    XSPI2->CR = (XSPI2->CR & ~XSPI_CR_FMODE_Msk) | 3 << XSPI_CR_FMODE_Pos;  // memory-mapped mode

    // TODO: enable MPU?
}

#endif // defined(MICROPY_HW_XSPIFLASH_SIZE_BITS_LOG2)
