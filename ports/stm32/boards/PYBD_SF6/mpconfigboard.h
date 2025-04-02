/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018-2019 Damien P. George
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

// Take PYBD_SF2 as base configuration
#include "boards/PYBD_SF2/mpconfigboard.h"

#undef MICROPY_HW_BOARD_NAME
#undef MICROPY_HW_MCU_NAME
#undef MICROPY_HW_CLK_PLLM
#undef MICROPY_HW_CLK_PLLN
#undef MICROPY_HW_CLK_PLLP
#undef MICROPY_HW_CLK_PLLQ
#undef MICROPY_HW_FLASH_LATENCY

#define MICROPY_HW_BOARD_NAME       "PYBD-SF6W"
#define MICROPY_HW_MCU_NAME         "STM32F767IIK"

// HSE is 25MHz, run SYS at 144MHz
#define MICROPY_HW_CLK_PLLM         (25)
#define MICROPY_HW_CLK_PLLN         (288)
#define MICROPY_HW_CLK_PLLP         (RCC_PLLP_DIV2)
#define MICROPY_HW_CLK_PLLQ         (6)
#define MICROPY_HW_FLASH_LATENCY    (FLASH_LATENCY_4)

// ROMFS config
#define MICROPY_HW_ROMFS_ENABLE_EXTERNAL_QSPI (1)
#define MICROPY_HW_ROMFS_QSPI_SPIFLASH_OBJ (&spi_bdev2.spiflash)
#define MICROPY_HW_ROMFS_ENABLE_PART0 (1)

// Extra UART config
#define MICROPY_HW_UART7_TX         (pyb_pin_W16)
#define MICROPY_HW_UART7_RX         (pyb_pin_W22B)

// Extra CAN buses
#define MICROPY_HW_CAN2_NAME        "Y"
#define MICROPY_HW_CAN2_TX          (pyb_pin_Y6)
#define MICROPY_HW_CAN2_RX          (pyb_pin_Y5)

// Ethernet via RMII
#define MICROPY_HW_ETH_MDC          (pyb_pin_W24)
#define MICROPY_HW_ETH_MDIO         (pyb_pin_W15)
#define MICROPY_HW_ETH_RMII_REF_CLK (pyb_pin_W17)
#define MICROPY_HW_ETH_RMII_CRS_DV  (pyb_pin_W14)
#define MICROPY_HW_ETH_RMII_RXD0    (pyb_pin_W51)
#define MICROPY_HW_ETH_RMII_RXD1    (pyb_pin_W47)
#define MICROPY_HW_ETH_RMII_TX_EN   (pyb_pin_W8)
#define MICROPY_HW_ETH_RMII_TXD0    (pyb_pin_W45)
#define MICROPY_HW_ETH_RMII_TXD1    (pyb_pin_W49)

// The below code reconfigures SPI flash for dynamic size detection.

#undef MICROPY_BOARD_EARLY_INIT
#undef MICROPY_HW_SPIFLASH_SIZE_BITS
#undef MICROPY_HW_QSPIFLASH_SIZE_BITS_LOG2
#undef MBOOT_SPIFLASH_BYTE_SIZE
#undef MBOOT_SPIFLASH_LAYOUT
#undef MBOOT_SPIFLASH_ERASE_BLOCKS_PER_PAGE
#undef MBOOT_SPIFLASH2_BYTE_SIZE
#undef MBOOT_SPIFLASH2_LAYOUT
#undef MBOOT_SPIFLASH2_ERASE_BLOCKS_PER_PAGE

// These are convenience macros to refer to the SPI flash chip parameters for the external SPI flash.
#define MICROPY_BOARD_SPIFLASH_CHIP_PARAMS0 (spi_bdev.spiflash.chip_params) // SPI flash #1, R/W storage
#define MICROPY_BOARD_SPIFLASH_CHIP_PARAMS1 (spi_bdev2.spiflash.chip_params) // SPI flash #2, memory mapped

// Early init is needed to initialise the default SPI flash chip parameters.
#define MICROPY_BOARD_EARLY_INIT    board_early_init_sf6

// Enable dynamic detection of SPI flash.
#define MICROPY_HW_SPIFLASH_CHIP_PARAMS (1)
#define MICROPY_HW_SPIFLASH_DETECT_DEVICE (1)

// Settings for SPI flash #1.
#define MICROPY_HW_SPIFLASH_SIZE_BITS (1 << (MICROPY_BOARD_SPIFLASH_CHIP_PARAMS0->memory_size_bytes_log2 + 3))

// Settings for SPI flash #2.
#define MICROPY_HW_QSPI_PRESCALER   (MICROPY_BOARD_SPIFLASH_CHIP_PARAMS1->qspi_prescaler)
#define MICROPY_HW_QSPIFLASH_SIZE_BITS_LOG2 (MICROPY_BOARD_SPIFLASH_CHIP_PARAMS1->memory_size_bytes_log2 + 3)

// ROMFS partition 0 is dynamically sized (SPI flash #2).
#define MICROPY_HW_ROMFS_PART0_START (uintptr_t)(&_micropy_hw_romfs_part0_start)
#define MICROPY_HW_ROMFS_PART0_SIZE (1 << MICROPY_BOARD_SPIFLASH_CHIP_PARAMS1->memory_size_bytes_log2)

// Mboot SPI flash #1 configuration.
#define MBOOT_SPIFLASH_LAYOUT_DYNAMIC_MAX_LEN (20)
#define MBOOT_SPIFLASH_LAYOUT       (MICROPY_BOARD_SPIFLASH_CHIP_PARAMS0->memory_size_bytes_log2 == 21 ? "/0x80000000/512*4Kg" : "/0x80000000/2048*4Kg")
#define MBOOT_SPIFLASH_BYTE_SIZE    (1 << MICROPY_BOARD_SPIFLASH_CHIP_PARAMS0->memory_size_bytes_log2)
#define MBOOT_SPIFLASH_ERASE_BLOCKS_PER_PAGE (1)

// Mboot SPI flash #2 configuration.
#define MBOOT_SPIFLASH2_LAYOUT_DYNAMIC_MAX_LEN (20)
#define MBOOT_SPIFLASH2_LAYOUT      (MICROPY_BOARD_SPIFLASH_CHIP_PARAMS1->memory_size_bytes_log2 == 21 ? "/0x90000000/512*4Kg" : "/0x90000000/2048*4Kg")
#define MBOOT_SPIFLASH2_BYTE_SIZE   (1 << MICROPY_BOARD_SPIFLASH_CHIP_PARAMS1->memory_size_bytes_log2)
#define MBOOT_SPIFLASH2_ERASE_BLOCKS_PER_PAGE (1)

extern unsigned char _micropy_hw_romfs_part0_start;

void board_early_init_sf6(void);
