// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <stdio.h>
#include "sdmmc_defs.h"
#include "sdmmc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Probe and initialize SD/MMC card using given host
 *
 * @note Only SD cards (SDSC and SDHC/SDXC) are supported now.
 *       Support for MMC/eMMC cards will be added later.
 *
 * @param host  pointer to structure defining host controller
 * @param out_card  pointer to structure which will receive information
 *                  about the card when the function completes
 * @return
 *      - SDMMC_OK on success
 *      - One of the error codes from SDMMC host controller
 */
sdmmc_err_t sdmmc_card_init(const sdmmc_host_t* host,
        sdmmc_card_t* out_card);

/**
 * @brief Print information about the card to a stream
 * @param stream  stream obtained using fopen or fdopen
 * @param card  card information structure initialized using sdmmc_card_init
 */
void sdmmc_card_print_info(FILE* stream, const sdmmc_card_t* card);

/**
 * Write given number of sectors to SD/MMC card
 *
 * @param card  pointer to card information structure previously initialized
 *              using sdmmc_card_init
 * @param src   pointer to data buffer to read data from; data size must be
 *              equal to sector_count * card->csd.sector_size
 * @param start_sector  sector where to start writing
 * @param sector_count  number of sectors to write
 * @return
 *      - SDMMC_OK on success
 *      - One of the error codes from SDMMC host controller
 */
sdmmc_err_t sdmmc_write_sectors(sdmmc_card_t* card, const void* src,
        size_t start_sector, size_t sector_count);

/**
 * Read given number of sectors from the SD/MMC card
 *
 * @param card  pointer to card information structure previously initialized
 *              using sdmmc_card_init
 * @param dst   pointer to data buffer to write into; buffer size must be
 *              at least sector_count * card->csd.sector_size
 * @param start_sector  sector where to start reading
 * @param sector_count  number of sectors to read
 * @return
 *      - SDMMC_OK on success
 *      - One of the error codes from SDMMC host controller
 */
sdmmc_err_t sdmmc_read_sectors(sdmmc_card_t* card, void* dst,
        size_t start_sector, size_t sector_count);

/**
 * Read one byte from an SDIO card using IO_RW_DIRECT (CMD52)
 *
 * @param card  pointer to card information structure previously initialized
 *              using sdmmc_card_init
 * @param function  IO function number
 * @param reg  byte address within IO function
 * @param[out] out_byte  output, receives the value read from the card
 * @return
 *      - SDMMC_OK on success
 *      - One of the error codes from SDMMC host controller
 */
sdmmc_err_t sdmmc_io_read_byte(sdmmc_card_t* card, uint32_t function,
        uint32_t reg, uint8_t *out_byte);

/**
 * Write one byte to an SDIO card using IO_RW_DIRECT (CMD52)
 *
 * @param card  pointer to card information structure previously initialized
 *              using sdmmc_card_init
 * @param function  IO function number
 * @param reg  byte address within IO function
 * @param in_byte  value to be written
 * @param[out] out_byte  if not NULL, receives new byte value read
 *                       from the card (read-after-write).
 * @return
 *      - SDMMC_OK on success
 *      - One of the error codes from SDMMC host controller
 */
sdmmc_err_t sdmmc_io_write_byte(sdmmc_card_t* card, uint32_t function,
        uint32_t reg, uint8_t in_byte, uint8_t* out_byte);

/**
 * Read multiple bytes from an SDIO card using IO_RW_EXTENDED (CMD53)
 *
 * This function performs read operation using CMD53 in byte mode.
 * For block mode, see sdmmc_io_read_blocks.
 *
 * @param card  pointer to card information structure previously initialized
 *              using sdmmc_card_init
 * @param function  IO function number
 * @param addr  byte address within IO function where reading starts
 * @param dst  buffer which receives the data read from card
 * @param size  number of bytes to read
 * @return
 *      - SDMMC_OK on success
 *      - SDMMC_ERR_INVALID_SIZE if size exceeds 512 bytes
 *      - One of the error codes from SDMMC host controller
 */
sdmmc_err_t sdmmc_io_read_bytes(sdmmc_card_t* card, uint32_t function,
        uint32_t addr, void* dst, size_t size);

/**
 * Write multiple bytes to an SDIO card using IO_RW_EXTENDED (CMD53)
 *
 * This function performs write operation using CMD53 in byte mode.
 * For block mode, see sdmmc_io_write_blocks.
 *
 * @param card  pointer to card information structure previously initialized
 *              using sdmmc_card_init
 * @param function  IO function number
 * @param addr  byte address within IO function where writing starts
 * @param src  data to be written
 * @param size  number of bytes to write
 * @return
 *      - SDMMC_OK on success
 *      - SDMMC_ERR_INVALID_SIZE if size exceeds 512 bytes
 *      - One of the error codes from SDMMC host controller
 */
sdmmc_err_t sdmmc_io_write_bytes(sdmmc_card_t* card, uint32_t function,
        uint32_t addr, const void* src, size_t size);

/**
 * Read blocks of data from an SDIO card using IO_RW_EXTENDED (CMD53)
 *
 * This function performs read operation using CMD53 in block mode.
 * For byte mode, see sdmmc_io_read_bytes.
 *
 * @param card  pointer to card information structure previously initialized
 *              using sdmmc_card_init
 * @param function  IO function number
 * @param addr  byte address within IO function where writing starts
 * @param dst  buffer which receives the data read from card
 * @param size  number of bytes to read, must be divisible by the card block
 *              size.
 * @return
 *      - SDMMC_OK on success
 *      - SDMMC_ERR_INVALID_SIZE if size is not divisible by 512 bytes
 *      - One of the error codes from SDMMC host controller
 */
sdmmc_err_t sdmmc_io_read_blocks(sdmmc_card_t* card, uint32_t function,
        uint32_t addr, void* dst, size_t size);

/**
 * Write blocks of data to an SDIO card using IO_RW_EXTENDED (CMD53)
 *
 * This function performs write operation using CMD53 in block mode.
 * For byte mode, see sdmmc_io_write_bytes.
 *
 * @param card  pointer to card information structure previously initialized
 *              using sdmmc_card_init
 * @param function  IO function number
 * @param addr  byte address within IO function where writing starts
 * @param src  data to be written
 * @param size  number of bytes to read, must be divisible by the card block
 *              size.
 * @return
 *      - SDMMC_OK on success
 *      - SDMMC_ERR_INVALID_SIZE if size is not divisible by 512 bytes
 *      - One of the error codes from SDMMC host controller
 */
sdmmc_err_t sdmmc_io_write_blocks(sdmmc_card_t* card, uint32_t function,
        uint32_t addr, const void* src, size_t size);

/**
 * Enable SDIO interrupt in the SDMMC host
 *
 * @param card  pointer to card information structure previously initialized
 *              using sdmmc_card_init
 * @return
 *      - SDMMC_OK on success
 *      - SDMMC_ERR_NOT_SUPPORTED if the host controller does not support
 *        IO interrupts
 */
sdmmc_err_t sdmmc_io_enable_int(sdmmc_card_t* card);

/**
 * Block until an SDIO interrupt is received
 *
 * Slave uses D1 line to signal interrupt condition to the host.
 * This function can be used to wait for the interrupt.
 *
 * @param card  pointer to card information structure previously initialized
 *              using sdmmc_card_init
 * @param timeout_ticks  time to wait for the interrupt, in RTOS ticks
 * @return
 *      - SDMMC_OK if the interrupt is received
 *      - SDMMC_ERR_NOT_SUPPORTED if the host controller does not support
 *        IO interrupts
 *      - SDMMC_ERR_TIMEOUT if the interrupt does not happen in timeout_ticks
 */
sdmmc_err_t sdmmc_io_wait_int(sdmmc_card_t* card, int timeout_ms);

/**
 * Get the data of CIS region of a SDIO card.
 *
 * You may provide a buffer not sufficient to store all the CIS data. In this
 * case, this functions store as much data into your buffer as possible. Also,
 * this function will try to get and return the size required for you.
 *
 * @param card  pointer to card information structure previously initialized
 *              using sdmmc_card_init
 * @param out_buffer Output buffer of the CIS data
 * @param buffer_size Size of the buffer.
 * @param inout_cis_size Mandatory, pointer to a size, input and output.
 *              - input: Limitation of maximum searching range, should be 0 or larger than
 *                      buffer_size. The function searches for CIS_CODE_END until this range. Set to
 *                      0 to search infinitely.
 *              - output: The size required to store all the CIS data, if CIS_CODE_END is found.
 *
 * @return
 *      - SDMMC_OK: on success
 *      - SDMMC_ERR_INVALID_RESPONSE: if the card does not (correctly) support CIS.
 *      - SDMMC_ERR_INVALID_SIZE: CIS_CODE_END found, but buffer_size is less than
 *              required size, which is stored in the inout_cis_size then.
 *      - SDMMC_ERR_NOT_FOUND: if the CIS_CODE_END not found. Increase input value of
 *              inout_cis_size or set it to 0, if you still want to search for the end;
 *              output value of inout_cis_size is invalid in this case.
 *      - and other error code return from sdmmc_io_read_bytes
 */
sdmmc_err_t sdmmc_io_get_cis_data(sdmmc_card_t* card, uint8_t* out_buffer, size_t buffer_size, size_t* inout_cis_size);

/**
 * Parse and print the CIS information of a SDIO card.
 *
 * @note Not all the CIS codes and all kinds of tuples are supported. If you
 * see some unresolved code, you can add the parsing of these code in
 * sdmmc_io.c and contribute to the IDF through the Github repository.
 *
 *              using sdmmc_card_init
 * @param buffer Buffer to parse
 * @param buffer_size Size of the buffer.
 * @param fp File pointer to print to, set to NULL to print to stdout.
 *
 * @return
 *      - SDMMC_OK: on success
 *      - SDMMC_ERR_NOT_SUPPORTED: if the value from the card is not supported to be parsed.
 *      - SDMMC_ERR_INVALID_SIZE: if the CIS size fields are not correct.
 */
sdmmc_err_t sdmmc_io_print_cis_info(uint8_t* buffer, size_t buffer_size, FILE* fp);


#ifdef __cplusplus
}
#endif
