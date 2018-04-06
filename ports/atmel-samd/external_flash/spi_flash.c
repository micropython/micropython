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
#include "spi_flash_api.h"

#include <stdint.h>
#include <string.h>

#include "external_flash/common_commands.h"
#include "peripherals.h"
#include "shared_dma.h"

#include "hal_gpio.h"
#include "hal_spi_m_sync.h"

struct spi_m_sync_descriptor spi_flash_desc;

// Enable the flash over SPI.
static void flash_enable(void) {
    gpio_set_pin_level(SPI_FLASH_CS_PIN, false);
}

// Disable the flash over SPI.
static void flash_disable(void) {
    gpio_set_pin_level(SPI_FLASH_CS_PIN, true);
}

static bool transfer(uint8_t* command, uint32_t command_length, uint8_t* data_in, uint8_t* data_out, uint32_t data_length) {
    struct spi_xfer xfer = { command, NULL, command_length };
    flash_enable();
    int32_t status = spi_m_sync_transfer(&spi_flash_desc, &xfer);
    if (status >= 0 && !(data_in == NULL && data_out == NULL)) {
        struct spi_xfer data_xfer = {data_in, data_out, data_length};
        status = spi_m_sync_transfer(&spi_flash_desc, &data_xfer);
    }
    flash_disable();
    return status >= 0;
}

static bool transfer_command(uint8_t command, uint8_t* data_in, uint8_t* data_out, uint32_t data_length) {
    return transfer(&command, 1, data_in, data_out, data_length);
}

bool spi_flash_command(uint8_t command) {
    return transfer_command(command, NULL, NULL, 0);
}

bool spi_flash_read_command(uint8_t command, uint8_t* data, uint32_t data_length) {
    return transfer_command(command, NULL, data, data_length);
}

bool spi_flash_write_command(uint8_t command, uint8_t* data, uint32_t data_length) {
    return transfer_command(command, data, NULL, data_length);
}

// Pack the low 24 bits of the address into a uint8_t array.
static void address_to_bytes(uint32_t address, uint8_t* bytes) {
    bytes[0] = (address >> 16) & 0xff;
    bytes[1] = (address >> 8) & 0xff;
    bytes[2] = address & 0xff;
}

bool spi_flash_sector_command(uint8_t command, uint32_t address) {
    uint8_t request[4] = {command, 0x00, 0x00, 0x00};
    address_to_bytes(address, request + 1);
    return transfer(request, 4, NULL, NULL, 0);
}

bool spi_flash_write_data(uint32_t address, uint8_t* data, uint32_t data_length) {
    uint8_t request[4] = {CMD_PAGE_PROGRAM, 0x00, 0x00, 0x00};
    // Write the SPI flash write address into the bytes following the command byte.
    address_to_bytes(address, request + 1);
    struct spi_xfer xfer = { request, NULL, 4 };
    flash_enable();
    int32_t status = spi_m_sync_transfer(&spi_flash_desc, &xfer);
    if (status >= 0) {
        status = sercom_dma_write(spi_flash_desc.dev.prvt, data, data_length);
    }
    flash_disable();
    return status >= 0;
}

bool spi_flash_read_data(uint32_t address, uint8_t* data, uint32_t data_length) {
    uint8_t request[4] = {CMD_READ_DATA, 0x00, 0x00, 0x00};
    // Write the SPI flash write address into the bytes following the command byte.
    address_to_bytes(address, request + 1);
    struct spi_xfer xfer = { request, NULL, 4 };
    flash_enable();
    int32_t status = spi_m_sync_transfer(&spi_flash_desc, &xfer);
    if (status >= 0) {
        status = sercom_dma_read(spi_flash_desc.dev.prvt, data, data_length, 0xff);
    }
    flash_disable();
    return status >= 0;
}

void spi_flash_init(void) {
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

void spi_flash_init_device(const external_flash_device* device) {

}
