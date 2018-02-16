/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016, 2017 Scott Shawcroft for Adafruit Industries
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
#include "spi_flash.h"

#include <stdint.h>
#include <string.h>

#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"
#include "py/misc.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "lib/oofatfs/ff.h"
#include "peripherals.h"
#include "supervisor/shared/rgb_led_status.h"

//#include "shared_dma.h"

#include "hal_gpio.h"
#include "hal_spi_m_sync.h"

#define SPI_FLASH_PART1_START_BLOCK (0x1)

#define NO_SECTOR_LOADED 0xFFFFFFFF

#define CMD_READ_JEDEC_ID 0x9f
#define CMD_READ_DATA 0x03
#define CMD_SECTOR_ERASE 0x20
// #define CMD_SECTOR_ERASE CMD_READ_JEDEC_ID
#define CMD_DISABLE_WRITE 0x04
#define CMD_ENABLE_WRITE 0x06
#define CMD_PAGE_PROGRAM 0x02
// #define CMD_PAGE_PROGRAM CMD_READ_JEDEC_ID
#define CMD_READ_STATUS 0x05
#define CMD_WRITE_STATUS_BYTE1 0x01

static bool spi_flash_is_initialised = false;

struct spi_m_sync_descriptor spi_flash_desc;

// The currently cached sector in the cache, ram or flash based.
static uint32_t current_sector;

// Track which blocks (up to 32) in the current sector currently live in the
// cache.
static uint32_t dirty_mask;

// Address of the scratch flash sector.
#define SCRATCH_SECTOR (SPI_FLASH_TOTAL_SIZE - SPI_FLASH_ERASE_SIZE)

// Enable the flash over SPI.
static void flash_enable(void) {
    gpio_set_pin_level(SPI_FLASH_CS_PIN, false);
}

// Disable the flash over SPI.
static void flash_disable(void) {
    gpio_set_pin_level(SPI_FLASH_CS_PIN, true);
}

static void spi_flash_command(uint8_t* request, uint8_t* response, uint32_t length) {
    struct spi_xfer xfer = { request, response, length };
    flash_enable();
    spi_m_sync_transfer(&spi_flash_desc, &jedec_id_xfer);
    flash_disable();
}

static void qspi_flash_command(uint8_t* request, uint8_t* response, uint32_t length) {

}

static void flash_command(uint8_t* request, uint8_t* response, uint32_t length) {
    #ifdef CIRCUITPY_QSPI
    qspi_flash_command(request, response, length);
    #else
    spi_flash_command(request, response, length);
    #endif
}

static bool spi_flash_write_data(uint8_t* request, uint32_t request_length, uint8_t* data, uint32_t data_length) {
    flash_enable();
    uint8_t page_program_request[4] = {CMD_PAGE_PROGRAM, 0x00, 0x00, 0x00};
    // Write the SPI flash write address into the bytes following the command byte.
    address_to_bytes(address + bytes_written, page_program_request + 1);
    struct spi_xfer page_program_xfer = {request, 0, request_length};
    int32_t status = spi_m_sync_transfer(&spi_flash_desc, &page_program_xfer);
    if (status >= 0) {
        struct spi_xfer write_data_buffer_xfer = {data, 0, data_length};
        status = spi_m_sync_transfer(&spi_flash_desc, &write_data_buffer_xfer);
    }
    flash_disable();
    return status >= 0;
}

static bool qspi_flash_write_data(uint8_t* request, uint32_t request_length, uint8_t* data, uint32_t data_length) {
    return true;
}

static bool flash_write_data(uint8_t* request, uint32_t request_length, uint8_t* data, uint32_t data_length) {
    #ifdef CIRCUITPY_QSPI
    return qspi_flash_write_data(request, request_length, data, data_length);
    #else
    return spi_flash_write_data(request, request_length, data, data_length);
    #endif
}

static bool spi_flash_read_data(uint8_t* request, uint32_t request_length, uint8_t* data, uint32_t data_length) {

}

static bool qspi_flash_read_data(uint8_t* request, uint32_t request_length, uint8_t* data, uint32_t data_length) {
    return true;
}

static bool flash_read_data(uint8_t* request, uint32_t request_length, uint8_t* data, uint32_t data_length) {
    #ifdef CIRCUITPY_QSPI
    return qspi_flash_write_data(request, request_length, data, data_length);
    #else
    return spi_flash_write_data(request, request_length, data, data_length);
    #endif
}

// Wait until both the write enable and write in progress bits have cleared.
static bool wait_for_flash_ready(void) {
    uint8_t read_status_request[2] = {CMD_READ_STATUS, 0x00};
    uint8_t read_status_response[2] = {0x00, 0x00};
    bool ok = true;
    // Both the write enable and write in progress bits should be low.
    do {
        ok = flash_command(read_status_request, read_status_response, 2);
    } while (ok && (read_status_response[1] & 0x3) != 0);
    return ok;
}

// Turn on the write enable bit so we can program and erase the flash.
static bool write_enable(void) {
    uint8_t enable_write_request[1] = {CMD_ENABLE_WRITE};
    return flash_command(enable_write_request, 0, 1);
}

// Pack the low 24 bits of the address into a uint8_t array.
static void address_to_bytes(uint32_t address, uint8_t* bytes) {
    bytes[0] = (address >> 16) & 0xff;
    bytes[1] = (address >> 8) & 0xff;
    bytes[2] = address & 0xff;
}

// Read data_length's worth of bytes starting at address into data.
static bool read_flash(uint32_t address, uint8_t* data, uint32_t data_length) {
    if (!spi_flash_is_initialised) {
        return false;
    }
    if (!wait_for_flash_ready()) {
        return false;
    }
    // We can read as much as we want sequentially.
    uint8_t read_data_request[4] = {CMD_READ_DATA, 0x00, 0x00, 0x00};
    // Write the SPI flash read address into the bytes following the command byte.
    address_to_bytes(address, read_data_request + 1);
    return flash_read_data(read_data_request, 4, data, data_length);
}

// Writes data_length's worth of bytes starting at address from data. Assumes
// that the sector that address resides in has already been erased. So make sure
// to run erase_sector.
static bool write_flash(uint32_t address, const uint8_t* data, uint32_t data_length) {
    if (!spi_flash_is_initialised) {
        return false;
    }
    // Don't bother writing if the data is all 1s. Thats equivalent to the flash
    // state after an erase.
    bool all_ones = true;
    for (uint16_t i = 0; i < data_length; i++) {
        if (data[i] != 0xff) {
            all_ones = false;
            break;
        }
    }
    if (all_ones) {
        return true;
    }

    for (uint32_t bytes_written = 0;
        bytes_written < data_length;
        bytes_written += SPI_FLASH_PAGE_SIZE) {
        if (!wait_for_flash_ready() || !write_enable()) {
            return false;
        }
        int32_t status;

        #ifdef SPI_FLASH_SECTOR_PROTECTION
        // Print out the protection status.
        // uint8_t protect_check[5] = {0x3C, 0x00, 0x00, 0x00, 0x00};
        // address_to_bytes(address + bytes_written, protect_check + 1);
        // flash_enable();
        // status = spi_write_buffer_wait(&spi_flash_desc, protect_check, 5);
        // flash_disable();
        #endif

        uint8_t page_program_request[4] = {CMD_PAGE_PROGRAM, 0x00, 0x00, 0x00};
        // Write the SPI flash write address into the bytes following the command byte.
        address_to_bytes(address + bytes_written, page_program_request + 1);
        if (!flash_write_data(page_program_request, 4, (uint8_t*) data + bytes_written,
                              SPI_FLASH_PAGE_SIZE)) {
            return false;
        }
    }
    return true;
}

static bool page_erased(uint32_t sector_address) {
    // Check the first few bytes to catch the common case where there is data
    // without using a bunch of memory.
    uint8_t short_buffer[4];
    if (read_flash(sector_address, short_buffer, 4)) {
        for (uint16_t i = 0; i < 4; i++) {
            if (short_buffer[i] != 0xff) {
                return false;
            }
        }
    } else {
        return false;
    }

    // Now check the full length.
    uint8_t full_buffer[FILESYSTEM_BLOCK_SIZE];
    if (read_flash(sector_address, full_buffer, FILESYSTEM_BLOCK_SIZE)) {
        for (uint16_t i = 0; i < FILESYSTEM_BLOCK_SIZE; i++) {
            if (short_buffer[i] != 0xff) {
                return false;
            }
        }
    } else {
        return false;
    }
    return true;
}

// Erases the given sector. Make sure you copied all of the data out of it you
// need! Also note, sector_address is really 24 bits.
static bool erase_sector(uint32_t sector_address) {
    // Before we erase the sector we need to wait for any writes to finish and
    // and then enable the write again.
    if (!wait_for_flash_ready() || !write_enable()) {
        return false;
    }

    uint8_t erase_request[4] = {CMD_SECTOR_ERASE, 0x00, 0x00, 0x00};
    address_to_bytes(sector_address, erase_request + 1);
    flash_command(erase_request, NULL, 4);
    return true;
}

// Sector is really 24 bits.
static bool copy_block(uint32_t src_address, uint32_t dest_address) {
    // Copy page by page to minimize RAM buffer.
    uint8_t buffer[SPI_FLASH_PAGE_SIZE];
    for (uint32_t i = 0; i < FILESYSTEM_BLOCK_SIZE / SPI_FLASH_PAGE_SIZE; i++) {
        if (!read_flash(src_address + i * SPI_FLASH_PAGE_SIZE, buffer, SPI_FLASH_PAGE_SIZE)) {
            return false;
        }
        if (!write_flash(dest_address + i * SPI_FLASH_PAGE_SIZE, buffer, SPI_FLASH_PAGE_SIZE)) {
            return false;
        }
    }
    return true;
}

void init_spi_peripheral(void) {
    samd_peripherals_sercom_clock_init(SPI_FLASH_SERCOM, SPI_FLASH_SERCOM_INDEX);

    // Set up with defaults, then change.
    spi_m_sync_init(&spi_flash_desc, SPI_FLASH_SERCOM);

    hri_sercomspi_write_CTRLA_DOPO_bf(SPI_FLASH_SERCOM, SPI_FLASH_DOPO);
    hri_sercomspi_write_CTRLA_DIPO_bf(SPI_FLASH_SERCOM, SPI_FLASH_DIPO);

    gpio_set_pin_direction(SPI_FLASH_SCK_PIN, GPIO_DIRECTION_OUT);
    gpio_set_pin_pull_mode(SPI_FLASH_SCK_PIN, GPIO_PULL_OFF);
    gpio_set_pin_function(SPI_FLASH_SCK_PIN, SPI_FLASH_SCK_PIN_FUNCTION);

    gpio_set_pin_direction(SPI_FLASH_MOSI_PIN, GPIO_DIRECTION_OUT);
    gpio_set_pin_pull_mode(SPI_FLASH_MOSI_PIN, GPIO_PULL_OFF);
    gpio_set_pin_function(SPI_FLASH_MOSI_PIN, SPI_FLASH_MOSI_PIN_FUNCTION);

    gpio_set_pin_direction(SPI_FLASH_MISO_PIN, GPIO_DIRECTION_IN);
    gpio_set_pin_pull_mode(SPI_FLASH_MISO_PIN, GPIO_PULL_OFF);
    gpio_set_pin_function(SPI_FLASH_MISO_PIN, SPI_FLASH_MISO_PIN_FUNCTION);

    hri_sercomspi_write_CTRLA_DOPO_bf(SPI_FLASH_SERCOM, SPI_FLASH_DOPO);
    hri_sercomspi_write_CTRLA_DIPO_bf(SPI_FLASH_SERCOM, SPI_FLASH_DIPO);

    spi_m_sync_set_baudrate(&spi_flash_desc, samd_peripherals_spi_baudrate_to_baud_reg_value(SPI_FLASH_BAUDRATE));

    gpio_set_pin_direction(SPI_FLASH_CS_PIN, GPIO_DIRECTION_OUT);
    // There's already a pull-up on the board.
    gpio_set_pin_pull_mode(SPI_FLASH_CS_PIN, GPIO_PULL_OFF);
    gpio_set_pin_function(SPI_FLASH_CS_PIN, GPIO_PIN_FUNCTION_OFF);

    // Set CS high (disabled).
    flash_disable();

    spi_m_sync_enable(&spi_flash_desc);
}

static void init_qspi_peripheral(void) {
    MCLK->APBCMASK.bit.QSPI_ = true;
    MCLK->AHBMASK.bit.QSPI_ = true;
    MCLK->AHBMASK.bit.QSPI_2X_ = false; // Only true if we are doing DDR.

    QSPI->CTRLA.reg = QSPI_CTRLA_SWRST;
    // We don't need to wait because we're running as fast as the CPU.

    QSPI->BAUD.bit.BAUD = 2;
    QSPI->CTRLB.reg = QSPI_CTRLB_MODE_MEMORY |
                      QSPI_CTRLB_CSMODE_NORELOAD |
                      QSPI_CTRLB_DATALEN_8BITS |
                      QSPI_CTRLB_CSMODE_LASTXFER;

    QSPI->CTRLA.bit.ENABLE = 1;

    // The QSPI is only connected to one set of pins in the SAMD51 so we can hard code it.
    uint32_t pins = {PIN_PA08, PIN_PA09, PIN_PA10, PIN_PA11, PIN_PB10, PIN_PB11};
    for (uint8_t i = 0; i < sizeof(pins); i++) {
        gpio_set_pin_direction(SPI_FLASH_SCK_PIN, GPIO_DIRECTION_IN);
        gpio_set_pin_pull_mode(SPI_FLASH_SCK_PIN, GPIO_PULL_OFF);
        gpio_set_pin_function(SPI_FLASH_SCK_PIN, GPIO_PIN_FUNCTION_H);
    }
}

void init_peripherals(void) {
    #ifdef CIRCUITPY_QSPI
    init_qspi_peripheral();
    #else
    init_spi_peripheral();
    #endif
}

void spi_flash_init(void) {
    if (spi_flash_is_initialised) {
        return;
    }

    init_peripherals();

    // Activity LED for flash writes.
#ifdef MICROPY_HW_LED_MSC
    gpio_set_pin_function(SPI_FLASH_CS_PIN, GPIO_PIN_FUNCTION_OFF);
    gpio_set_pin_direction(MICROPY_HW_LED_MSC, GPIO_DIRECTION_OUT);
    // There's already a pull-up on the board.
    gpio_set_pin_level(MICROPY_HW_LED_MSC, false);
#endif

    uint8_t jedec_id_request[4] = {CMD_READ_JEDEC_ID, 0x00, 0x00, 0x00};
    uint8_t jedec_id_response[4] = {0x00, 0x00, 0x00, 0x00};
    flash_command(jedec_id_request, jedec_id_response, 4);

    uint8_t manufacturer = jedec_id_response[1];
    if ((jedec_id_response[1] == SPI_FLASH_JEDEC_MANUFACTURER
#ifdef SPI_FLASH_JEDEC_MANUFACTURER_2
         || jedec_id_response[1] == SPI_FLASH_JEDEC_MANUFACTURER_2
#endif
         ) &&
        jedec_id_response[2] == SPI_FLASH_JEDEC_MEMORY_TYPE &&
        jedec_id_response[3] == SPI_FLASH_JEDEC_CAPACITY) {
        spi_flash_is_initialised = true;
    } else {
        // Unknown flash chip!
        spi_flash_is_initialised = false;
        return;
    }

    if ((manufacturer == SPI_FLASH_JEDEC_MANUFACTURER && SPI_FLASH_SECTOR_PROTECTION)
#ifdef SPI_FLASH_JEDEC_MANUFACTURER_2
        || (manufacturer == SPI_FLASH_JEDEC_MANUFACTURER_2 && SPI_FLASH_SECTOR_PROTECTION_2)
#endif
        )  {
        write_enable();

        // Turn off sector protection
        uint8_t disable_protect_request[2] = {CMD_WRITE_STATUS_BYTE1, 0x00};
        flash_command(disable_protect_request, NULL, 2);
    }

    // Turn off writes in case this is a microcontroller only reset.
    uint8_t disable_write_request[1] = {CMD_DISABLE_WRITE};
    flash_command(disable_write_request, NULL, 1);

    wait_for_flash_ready();

    current_sector = NO_SECTOR_LOADED;
    dirty_mask = 0;
    MP_STATE_VM(flash_ram_cache) = NULL;

    spi_flash_is_initialised = true;
}

// The size of each individual block.
uint32_t spi_flash_get_block_size(void) {
    return FILESYSTEM_BLOCK_SIZE;
}

// The total number of available blocks.
uint32_t spi_flash_get_block_count(void) {
    // We subtract one erase sector size because we may use it as a staging area
    // for writes.
    return SPI_FLASH_PART1_START_BLOCK + (SPI_FLASH_TOTAL_SIZE - SPI_FLASH_ERASE_SIZE) / FILESYSTEM_BLOCK_SIZE;
}

// Flush the cache that was written to the scratch portion of flash. Only used
// when ram is tight.
static bool flush_scratch_flash(void) {
    // First, copy out any blocks that we haven't touched from the sector we've
    // cached.
    bool copy_to_scratch_ok = true;
    for (uint8_t i = 0; i < SPI_FLASH_ERASE_SIZE / FILESYSTEM_BLOCK_SIZE; i++) {
        if ((dirty_mask & (1 << i)) == 0) {
            copy_to_scratch_ok = copy_to_scratch_ok &&
                copy_block(current_sector + i * FILESYSTEM_BLOCK_SIZE,
                           SCRATCH_SECTOR + i * FILESYSTEM_BLOCK_SIZE);
        }
    }
    if (!copy_to_scratch_ok) {
        // TODO(tannewt): Do more here. We opted to not erase and copy bad data
        // in. We still risk losing the data written to the scratch sector.
        return false;
    }
    // Second, erase the current sector.
    erase_sector(current_sector);
    // Finally, copy the new version into it.
    for (uint8_t i = 0; i < SPI_FLASH_ERASE_SIZE / FILESYSTEM_BLOCK_SIZE; i++) {
        copy_block(SCRATCH_SECTOR + i * FILESYSTEM_BLOCK_SIZE,
                   current_sector + i * FILESYSTEM_BLOCK_SIZE);
    }
    return true;
}

// Attempts to allocate a new set of page buffers for caching a full sector in
// ram. Each page is allocated separately so that the GC doesn't need to provide
// one huge block. We can free it as we write if we want to also.
static bool allocate_ram_cache(void) {
    uint8_t blocks_per_sector = SPI_FLASH_ERASE_SIZE / FILESYSTEM_BLOCK_SIZE;
    uint8_t pages_per_block = FILESYSTEM_BLOCK_SIZE / SPI_FLASH_PAGE_SIZE;
    MP_STATE_VM(flash_ram_cache) = m_malloc_maybe(blocks_per_sector * pages_per_block * sizeof(uint32_t), false);
    if (MP_STATE_VM(flash_ram_cache) == NULL) {
        return false;
    }
    // Declare i and j outside the loops in case we fail to allocate everything
    // we need. In that case we'll give it back.
    uint8_t i = 0;
    uint8_t j = 0;
    bool success = true;
    for (i = 0; i < blocks_per_sector; i++) {
        for (j = 0; j < pages_per_block; j++) {
            uint8_t *page_cache = m_malloc_maybe(SPI_FLASH_PAGE_SIZE, false);
            if (page_cache == NULL) {
                success = false;
                break;
            }
            MP_STATE_VM(flash_ram_cache)[i * pages_per_block + j] = page_cache;
        }
        if (!success) {
            break;
        }
    }
    // We couldn't allocate enough so give back what we got.
    if (!success) {
        // We add 1 so that we delete 0 when i is 1. Going to zero (i >= 0)
        // would never stop because i is unsigned.
        i++;
        for (; i > 0; i--) {
            for (; j > 0; j--) {
                m_free(MP_STATE_VM(flash_ram_cache)[(i - 1) * pages_per_block + (j - 1)]);
            }
            j = pages_per_block;
        }
        m_free(MP_STATE_VM(flash_ram_cache));
        MP_STATE_VM(flash_ram_cache) = NULL;
    }
    return success;
}

// Flush the cached sector from ram onto the flash. We'll free the cache unless
// keep_cache is true.
static bool flush_ram_cache(bool keep_cache) {
    // First, copy out any blocks that we haven't touched from the sector
    // we've cached. If we don't do this we'll erase the data during the sector
    // erase below.
    bool copy_to_ram_ok = true;
    uint8_t pages_per_block = FILESYSTEM_BLOCK_SIZE / SPI_FLASH_PAGE_SIZE;
    for (uint8_t i = 0; i < SPI_FLASH_ERASE_SIZE / FILESYSTEM_BLOCK_SIZE; i++) {
        if ((dirty_mask & (1 << i)) == 0) {
            for (uint8_t j = 0; j < pages_per_block; j++) {
                copy_to_ram_ok = read_flash(
                    current_sector + (i * pages_per_block + j) * SPI_FLASH_PAGE_SIZE,
                    MP_STATE_VM(flash_ram_cache)[i * pages_per_block + j],
                    SPI_FLASH_PAGE_SIZE);
                if (!copy_to_ram_ok) {
                    break;
                }
            }
        }
        if (!copy_to_ram_ok) {
            break;
        }
    }

    if (!copy_to_ram_ok) {
        return false;
    }
    // Second, erase the current sector.
    erase_sector(current_sector);
    // Lastly, write all the data in ram that we've cached.
    for (uint8_t i = 0; i < SPI_FLASH_ERASE_SIZE / FILESYSTEM_BLOCK_SIZE; i++) {
        for (uint8_t j = 0; j < pages_per_block; j++) {
            write_flash(current_sector + (i * pages_per_block + j) * SPI_FLASH_PAGE_SIZE,
                        MP_STATE_VM(flash_ram_cache)[i * pages_per_block + j],
                        SPI_FLASH_PAGE_SIZE);
            if (!keep_cache) {
                m_free(MP_STATE_VM(flash_ram_cache)[i * pages_per_block + j]);
            }
        }
    }
    // We're done with the cache for now so give it back.
    if (!keep_cache) {
        m_free(MP_STATE_VM(flash_ram_cache));
        MP_STATE_VM(flash_ram_cache) = NULL;
    }
    return true;
}

// Delegates to the correct flash flush method depending on the existing cache.
static void spi_flash_flush_keep_cache(bool keep_cache) {
    if (current_sector == NO_SECTOR_LOADED) {
        return;
    }
    #ifdef MICROPY_HW_LED_MSC
        port_pin_set_output_level(MICROPY_HW_LED_MSC, true);
    #endif
    temp_status_color(ACTIVE_WRITE);
    // If we've cached to the flash itself flush from there.
    if (MP_STATE_VM(flash_ram_cache) == NULL) {
        flush_scratch_flash();
    } else {
        flush_ram_cache(keep_cache);
    }
    current_sector = NO_SECTOR_LOADED;
    clear_temp_status();
    #ifdef MICROPY_HW_LED_MSC
        port_pin_set_output_level(MICROPY_HW_LED_MSC, false);
    #endif
}

// External flash function used. If called externally we assume we won't need
// the cache after.
void spi_flash_flush(void) {
    spi_flash_flush_keep_cache(false);
}

void flash_flush(void) {
    spi_flash_flush();
}

// Builds a partition entry for the MBR.
static void build_partition(uint8_t *buf, int boot, int type,
                            uint32_t start_block, uint32_t num_blocks) {
    buf[0] = boot;

    if (num_blocks == 0) {
        buf[1] = 0;
        buf[2] = 0;
        buf[3] = 0;
    } else {
        buf[1] = 0xff;
        buf[2] = 0xff;
        buf[3] = 0xff;
    }

    buf[4] = type;

    if (num_blocks == 0) {
        buf[5] = 0;
        buf[6] = 0;
        buf[7] = 0;
    } else {
        buf[5] = 0xff;
        buf[6] = 0xff;
        buf[7] = 0xff;
    }

    buf[8] = start_block;
    buf[9] = start_block >> 8;
    buf[10] = start_block >> 16;
    buf[11] = start_block >> 24;

    buf[12] = num_blocks;
    buf[13] = num_blocks >> 8;
    buf[14] = num_blocks >> 16;
    buf[15] = num_blocks >> 24;
}

static int32_t convert_block_to_flash_addr(uint32_t block) {
    if (SPI_FLASH_PART1_START_BLOCK <= block && block < spi_flash_get_block_count()) {
        // a block in partition 1
        block -= SPI_FLASH_PART1_START_BLOCK;
        return block * FILESYSTEM_BLOCK_SIZE;
    }
    // bad block
    return -1;
}

bool spi_flash_read_block(uint8_t *dest, uint32_t block) {
    if (block == 0) {
        // Fake the MBR so we can decide on our own partition table
        for (int i = 0; i < 446; i++) {
            dest[i] = 0;
        }

        build_partition(dest + 446, 0, 0x01 /* FAT12 */,
                        SPI_FLASH_PART1_START_BLOCK,
                        spi_flash_get_block_count() - SPI_FLASH_PART1_START_BLOCK);
        build_partition(dest + 462, 0, 0, 0, 0);
        build_partition(dest + 478, 0, 0, 0, 0);
        build_partition(dest + 494, 0, 0, 0, 0);

        dest[510] = 0x55;
        dest[511] = 0xaa;

        return true;
    } else if (block < SPI_FLASH_PART1_START_BLOCK) {
        memset(dest, 0, FILESYSTEM_BLOCK_SIZE);
        return true;
    } else {
        // Non-MBR block, get data from flash memory.
        int32_t address = convert_block_to_flash_addr(block);
        if (address == -1) {
            // bad block number
            return false;
        }

        // Mask out the lower bits that designate the address within the sector.
        uint32_t this_sector = address & (~(SPI_FLASH_ERASE_SIZE - 1));
        uint8_t block_index = (address / FILESYSTEM_BLOCK_SIZE) % (SPI_FLASH_ERASE_SIZE / FILESYSTEM_BLOCK_SIZE);
        uint8_t mask = 1 << (block_index);
        // We're reading from the currently cached sector.
        if (current_sector == this_sector && (mask & dirty_mask) > 0) {
            if (MP_STATE_VM(flash_ram_cache) != NULL) {
                uint8_t pages_per_block = FILESYSTEM_BLOCK_SIZE / SPI_FLASH_PAGE_SIZE;
                for (int i = 0; i < pages_per_block; i++) {
                    memcpy(dest + i * SPI_FLASH_PAGE_SIZE,
                           MP_STATE_VM(flash_ram_cache)[block_index * pages_per_block + i],
                           SPI_FLASH_PAGE_SIZE);
                }
                return true;
            } else {
                uint32_t scratch_address = SCRATCH_SECTOR + block_index * FILESYSTEM_BLOCK_SIZE;
                return read_flash(scratch_address, dest, FILESYSTEM_BLOCK_SIZE);
            }
        }
        return read_flash(address, dest, FILESYSTEM_BLOCK_SIZE);
    }
}

bool spi_flash_write_block(const uint8_t *data, uint32_t block) {
    if (block < SPI_FLASH_PART1_START_BLOCK) {
        // Fake writing below the flash partition.
        return true;
    } else {
        // Non-MBR block, copy to cache
        int32_t address = convert_block_to_flash_addr(block);
        if (address == -1) {
            // bad block number
            return false;
        }
        // Wait for any previous writes to finish.
        wait_for_flash_ready();
        // Mask out the lower bits that designate the address within the sector.
        uint32_t this_sector = address & (~(SPI_FLASH_ERASE_SIZE - 1));
        uint8_t block_index = (address / FILESYSTEM_BLOCK_SIZE) % (SPI_FLASH_ERASE_SIZE / FILESYSTEM_BLOCK_SIZE);
        uint8_t mask = 1 << (block_index);
        // Flush the cache if we're moving onto a sector or we're writing the
        // same block again.
        if (current_sector != this_sector || (mask & dirty_mask) > 0) {
            // Check to see if we'd write to an erased page. In that case we
            // can write directly.
            if (page_erased(address)) {
                return write_flash(address, data, FILESYSTEM_BLOCK_SIZE);
            }
            if (current_sector != NO_SECTOR_LOADED) {
                spi_flash_flush_keep_cache(true);
            }
            if (MP_STATE_VM(flash_ram_cache) == NULL && !allocate_ram_cache()) {
                erase_sector(SCRATCH_SECTOR);
                wait_for_flash_ready();
            }
            current_sector = this_sector;
            dirty_mask = 0;
        }
        dirty_mask |= mask;
        // Copy the block to the appropriate cache.
        if (MP_STATE_VM(flash_ram_cache) != NULL) {
            uint8_t pages_per_block = FILESYSTEM_BLOCK_SIZE / SPI_FLASH_PAGE_SIZE;
            for (int i = 0; i < pages_per_block; i++) {
                memcpy(MP_STATE_VM(flash_ram_cache)[block_index * pages_per_block + i],
                       data + i * SPI_FLASH_PAGE_SIZE,
                       SPI_FLASH_PAGE_SIZE);
            }
            return true;
        } else {
            uint32_t scratch_address = SCRATCH_SECTOR + block_index * FILESYSTEM_BLOCK_SIZE;
            return write_flash(scratch_address, data, FILESYSTEM_BLOCK_SIZE);
        }
    }
}

mp_uint_t spi_flash_read_blocks(uint8_t *dest, uint32_t block_num, uint32_t num_blocks) {
    for (size_t i = 0; i < num_blocks; i++) {
        if (!spi_flash_read_block(dest + i * FILESYSTEM_BLOCK_SIZE, block_num + i)) {
            return 1; // error
        }
    }
    return 0; // success
}

mp_uint_t spi_flash_write_blocks(const uint8_t *src, uint32_t block_num, uint32_t num_blocks) {
    for (size_t i = 0; i < num_blocks; i++) {
        if (!spi_flash_write_block(src + i * FILESYSTEM_BLOCK_SIZE, block_num + i)) {
            return 1; // error
        }
    }
    return 0; // success
}

/******************************************************************************/
// MicroPython bindings
//
// Expose the flash as an object with the block protocol.

// there is a singleton Flash object
STATIC const mp_obj_base_t spi_flash_obj = {&spi_flash_type};

STATIC mp_obj_t spi_flash_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    // return singleton object
    return (mp_obj_t)&spi_flash_obj;
}

STATIC mp_obj_t spi_flash_obj_readblocks(mp_obj_t self, mp_obj_t block_num, mp_obj_t buf) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_WRITE);
    mp_uint_t ret = spi_flash_read_blocks(bufinfo.buf, mp_obj_get_int(block_num), bufinfo.len / FILESYSTEM_BLOCK_SIZE);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(spi_flash_obj_readblocks_obj, spi_flash_obj_readblocks);

STATIC mp_obj_t spi_flash_obj_writeblocks(mp_obj_t self, mp_obj_t block_num, mp_obj_t buf) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_READ);
    mp_uint_t ret = spi_flash_write_blocks(bufinfo.buf, mp_obj_get_int(block_num), bufinfo.len / FILESYSTEM_BLOCK_SIZE);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(spi_flash_obj_writeblocks_obj, spi_flash_obj_writeblocks);

STATIC mp_obj_t spi_flash_obj_ioctl(mp_obj_t self, mp_obj_t cmd_in, mp_obj_t arg_in) {
    mp_int_t cmd = mp_obj_get_int(cmd_in);
    switch (cmd) {
        case BP_IOCTL_INIT: spi_flash_init(); return MP_OBJ_NEW_SMALL_INT(0);
        case BP_IOCTL_DEINIT: spi_flash_flush(); return MP_OBJ_NEW_SMALL_INT(0); // TODO properly
        case BP_IOCTL_SYNC: spi_flash_flush(); return MP_OBJ_NEW_SMALL_INT(0);
        case BP_IOCTL_SEC_COUNT: return MP_OBJ_NEW_SMALL_INT(spi_flash_get_block_count());
        case BP_IOCTL_SEC_SIZE: return MP_OBJ_NEW_SMALL_INT(spi_flash_get_block_size());
        default: return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(spi_flash_obj_ioctl_obj, spi_flash_obj_ioctl);

STATIC const mp_rom_map_elem_t spi_flash_obj_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_readblocks), MP_ROM_PTR(&spi_flash_obj_readblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&spi_flash_obj_writeblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl), MP_ROM_PTR(&spi_flash_obj_ioctl_obj) },
};

STATIC MP_DEFINE_CONST_DICT(spi_flash_obj_locals_dict, spi_flash_obj_locals_dict_table);

const mp_obj_type_t spi_flash_type = {
    { &mp_type_type },
    .name = MP_QSTR_SPIFlash,
    .make_new = spi_flash_obj_make_new,
    .locals_dict = (mp_obj_t)&spi_flash_obj_locals_dict,
};

void flash_init_vfs(fs_user_mount_t *vfs) {
    vfs->base.type = &mp_fat_vfs_type;
    vfs->flags |= FSUSER_NATIVE | FSUSER_HAVE_IOCTL;
    vfs->fatfs.drv = vfs;
    vfs->fatfs.part = 1; // flash filesystem lives on first partition
    vfs->readblocks[0] = (mp_obj_t)&spi_flash_obj_readblocks_obj;
    vfs->readblocks[1] = (mp_obj_t)&spi_flash_obj;
    vfs->readblocks[2] = (mp_obj_t)spi_flash_read_blocks; // native version
    vfs->writeblocks[0] = (mp_obj_t)&spi_flash_obj_writeblocks_obj;
    vfs->writeblocks[1] = (mp_obj_t)&spi_flash_obj;
    vfs->writeblocks[2] = (mp_obj_t)spi_flash_write_blocks; // native version
    vfs->u.ioctl[0] = (mp_obj_t)&spi_flash_obj_ioctl_obj;
    vfs->u.ioctl[1] = (mp_obj_t)&spi_flash_obj;
}
