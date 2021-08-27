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
#include "supervisor/spi_flash_api.h"

#include <stdint.h>
#include <string.h>

#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "supervisor/shared/external_flash/common_commands.h"
#include "supervisor/shared/external_flash/external_flash.h"
#include "py/mpconfig.h"

digitalio_digitalinout_obj_t cs_pin;
busio_spi_obj_t supervisor_flash_spi_bus;

const external_flash_device *flash_device;
uint32_t spi_flash_baudrate;

// Enable the flash over SPI.
static bool flash_enable(void) {
    if (common_hal_busio_spi_try_lock(&supervisor_flash_spi_bus)) {
        common_hal_digitalio_digitalinout_set_value(&cs_pin, false);
        return true;
    }
    return false;
}

// Disable the flash over SPI.
static void flash_disable(void) {
    common_hal_digitalio_digitalinout_set_value(&cs_pin, true);
    common_hal_busio_spi_unlock(&supervisor_flash_spi_bus);
}

static bool transfer(uint8_t *command, uint32_t command_length, uint8_t *data_in, uint8_t *data_out, uint32_t data_length) {
    if (!flash_enable()) {
        return false;
    }
    bool status = common_hal_busio_spi_write(&supervisor_flash_spi_bus, command, command_length);
    if (status) {
        if (data_in != NULL && data_out != NULL) {
            status = common_hal_busio_spi_transfer(&supervisor_flash_spi_bus, data_out, data_in, data_length);
        } else if (data_out != NULL) {
            status = common_hal_busio_spi_read(&supervisor_flash_spi_bus, data_out, data_length, 0xff);
        } else if (data_in != NULL) {
            status = common_hal_busio_spi_write(&supervisor_flash_spi_bus, data_in, data_length);
        }
    }
    flash_disable();
    return status;
}

static bool transfer_command(uint8_t command, uint8_t *data_in, uint8_t *data_out, uint32_t data_length) {
    return transfer(&command, 1, data_in, data_out, data_length);
}

bool spi_flash_command(uint8_t command) {
    return transfer_command(command, NULL, NULL, 0);
}

bool spi_flash_read_command(uint8_t command, uint8_t *data, uint32_t data_length) {
    return transfer_command(command, NULL, data, data_length);
}

bool spi_flash_write_command(uint8_t command, uint8_t *data, uint32_t data_length) {
    return transfer_command(command, data, NULL, data_length);
}

// Pack the low 24 bits of the address into a uint8_t array.
static void address_to_bytes(uint32_t address, uint8_t *bytes) {
    bytes[0] = (address >> 16) & 0xff;
    bytes[1] = (address >> 8) & 0xff;
    bytes[2] = address & 0xff;
}

bool spi_flash_sector_command(uint8_t command, uint32_t address) {
    uint8_t request[4] = {command, 0x00, 0x00, 0x00};
    address_to_bytes(address, request + 1);
    return transfer(request, 4, NULL, NULL, 0);
}

bool spi_flash_write_data(uint32_t address, uint8_t *data, uint32_t data_length) {
    uint8_t request[4] = {CMD_PAGE_PROGRAM, 0x00, 0x00, 0x00};
    // Write the SPI flash write address into the bytes following the command byte.
    address_to_bytes(address, request + 1);
    if (!flash_enable()) {
        return false;
    }
    common_hal_busio_spi_configure(&supervisor_flash_spi_bus, spi_flash_baudrate, 0, 0, 8);
    bool status = common_hal_busio_spi_write(&supervisor_flash_spi_bus, request, 4);
    if (status) {
        status = common_hal_busio_spi_write(&supervisor_flash_spi_bus, data, data_length);
    }
    flash_disable();
    return status;
}

bool spi_flash_read_data(uint32_t address, uint8_t *data, uint32_t data_length) {
    uint8_t request[5] = {CMD_READ_DATA, 0x00, 0x00, 0x00};
    uint8_t command_length = 4;
    if (flash_device->supports_fast_read) {
        request[0] = CMD_FAST_READ_DATA;
        command_length = 5;
    }
    // Write the SPI flash read address into the bytes following the command byte.
    address_to_bytes(address, request + 1);
    if (!flash_enable()) {
        return false;
    }
    common_hal_busio_spi_configure(&supervisor_flash_spi_bus, spi_flash_baudrate, 0, 0, 8);
    bool status = common_hal_busio_spi_write(&supervisor_flash_spi_bus, request, command_length);
    if (status) {
        status = common_hal_busio_spi_read(&supervisor_flash_spi_bus, data, data_length, 0xff);
    }
    flash_disable();
    return status;
}

void spi_flash_init(void) {
    cs_pin.base.type = &digitalio_digitalinout_type;
    common_hal_digitalio_digitalinout_construct(&cs_pin, SPI_FLASH_CS_PIN);


    // Set CS high (disabled).
    common_hal_digitalio_digitalinout_switch_to_output(&cs_pin, true, DRIVE_MODE_PUSH_PULL);
    common_hal_digitalio_digitalinout_never_reset(&cs_pin);

    supervisor_flash_spi_bus.base.type = &busio_spi_type;
    common_hal_busio_spi_construct(&supervisor_flash_spi_bus, SPI_FLASH_SCK_PIN, SPI_FLASH_MOSI_PIN, SPI_FLASH_MISO_PIN);
    common_hal_busio_spi_never_reset(&supervisor_flash_spi_bus);
}

void spi_flash_init_device(const external_flash_device *device) {
    flash_device = device;
    spi_flash_baudrate = device->max_clock_speed_mhz * 1000000;
    if (spi_flash_baudrate > SPI_FLASH_MAX_BAUDRATE) {
        spi_flash_baudrate = SPI_FLASH_MAX_BAUDRATE;
    }
}
