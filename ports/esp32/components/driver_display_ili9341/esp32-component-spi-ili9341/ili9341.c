/**
 * Copyright (c) 2022 Nicolai Electronics
 *
 * SPDX-License-Identifier: MIT
 */

#include <sdkconfig.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <esp_system.h>

#include <soc/gpio_reg.h>
#include <soc/gpio_sig_map.h>
#include <soc/gpio_struct.h>
#include <soc/spi_reg.h>
#include <esp_err.h>
#include <esp_log.h>
#include <driver/gpio.h>
#include <driver/spi_master.h>

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include <driver/gpio.h>

#include "include/ili9341.h"

static const char *TAG = "ili9341";

static void IRAM_ATTR ili9341_spi_pre_transfer_callback(spi_transaction_t *t) {
    ILI9341* device = ((ILI9341*) t->user);
    gpio_set_level(device->pin_dcx, device->dc_level);
}

const uint8_t ili9341_init_data[] = {
    ILI9341_POWERB,    3, 0x00, 0xC1, 0x30,
    ILI9341_POWER_SEQ, 4, 0x64, 0x03, 0x12, 0x81,
    ILI9341_DTCA,      3, 0x85, 0x00, 0x78,
    ILI9341_POWERA,    5, 0x39, 0x2C, 0x00, 0x34, 0x02,
    ILI9341_PRC,       1, 0x20,
    ILI9341_DTCB,      2, 0x00, 0x00,
    ILI9341_LCMCTRL,   1, 0x23,
    ILI9341_POWER2,    1, 0x10,
    ILI9341_VCOM1,     2, 0x3e, 0x28,
    ILI9341_VCOM2,     1, 0x86,
    ILI9341_MADCTL,    1, 0x48,
    ILI9341_COLMOD,    1, 0x55,
    ILI9341_FRMCTR1,   2, 0x00, 0x18,
    ILI9341_DFC,       3, 0x08, 0x82, 0x27,
    ILI9341_3GAMMA_EN, 1, 0x00,
    ILI9341_GAMSET,    1, 0x01,
    ILI9341_PVGAMCTRL, 15, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00,
    ILI9341_NVGAMCTRL, 15, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,
    0x00
};

esp_err_t ili9341_send(ILI9341* device, const uint8_t *data, const int len, const bool dc_level) {
    if (len == 0) return ESP_OK;
    if (device->spi_device == NULL) return ESP_FAIL;
    device->dc_level = dc_level;
    spi_transaction_t transaction = {
        .length = len * 8,  // transaction length is in bits
        .tx_buffer = data,
        .user = (void*) device,
    };
    return spi_device_transmit(device->spi_device, &transaction);
}

esp_err_t ili9341_write_init_data(ILI9341* device, const uint8_t * data) {
    if (device->spi_device == NULL) return ESP_FAIL;
    esp_err_t res = ESP_OK;
    uint8_t cmd, len;
    while(true) {
        cmd = *data++;
        if(!cmd) return ESP_OK; //END
        len = *data++;
        res = ili9341_send(device, &cmd, 1, false);
        if (res != ESP_OK) break;
        res = ili9341_send(device, data, len, true);
        if (res != ESP_OK) break;
        data+=len;
    }
    return res;
}

esp_err_t ili9341_send_command(ILI9341* device, const uint8_t cmd) {
    return ili9341_send(device, &cmd, 1, false);
}

esp_err_t ili9341_send_data(ILI9341* device, const uint8_t* data, const uint16_t length) {
    return ili9341_send(device, data, length, true);
}

esp_err_t ili9341_send_u32(ILI9341* device, const uint32_t data) {
    uint8_t buffer[4];
    buffer[0] = (data>>24)&0xFF;
    buffer[1] = (data>>16)&0xFF;
    buffer[2] = (data>> 8)&0xFF;
    buffer[3] = data      &0xFF;
    return ili9341_send(device, buffer, 4, true);
}

esp_err_t ili9341_set_addr_window(ILI9341* device, uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
    uint32_t xa = ((uint32_t)x << 16) | (x+w-1);
    uint32_t ya = ((uint32_t)y << 16) | (y+h-1);
    esp_err_t res;
    res = ili9341_send_command(device, ILI9341_CASET);
    if (res != ESP_OK) return res;
    res = ili9341_send_u32(device, xa);
    if (res != ESP_OK) return res;
    res = ili9341_send_command(device, ILI9341_RASET);
    if (res != ESP_OK) return res;
    res = ili9341_send_u32(device, ya);
    if (res != ESP_OK) return res;
    res = ili9341_send_command(device, ILI9341_RAMWR);
    return res;
}

#define MADCTL_MY  0x80  ///< Bottom to topp
#define MADCTL_MX  0x40  ///< Right to left
#define MADCTL_MV  0x20  ///< Reverse Mode
#define MADCTL_ML  0x10  ///< LCD refresh Bottom to top
#define MADCTL_RGB 0x00  ///< Red-Green-Blue pixel order
#define MADCTL_BGR 0x08  ///< Blue-Green-Red pixel order
#define MADCTL_MH  0x04 ///< LCD refresh right to left

esp_err_t ili9341_set_cfg(ILI9341* device, uint8_t rotation, bool color_mode) {
    rotation = rotation & 0x03;
    uint8_t m = 0;

    switch (rotation) {
            case 0:
                m |= MADCTL_MX;
                break;
            case 1:
                m |= MADCTL_MV;
                break;
            case 2:
                m |= MADCTL_MY;
                break;
            case 3:
                m |= (MADCTL_MX | MADCTL_MY | MADCTL_MV);
                break;
    }

    if (color_mode) {
        m |= MADCTL_BGR;
    } else {
        m |= MADCTL_RGB;
    }

    uint8_t commands[2] = {ILI9341_MADCTL, m};
    esp_err_t res = ili9341_send(device, commands, 1, false);
    if (res != ESP_OK) return res;
    res = ili9341_send(device, commands+1, 1, true);
    return res;
}

esp_err_t ili9341_reset(ILI9341* device) {
    if (device->mutex != NULL) xSemaphoreTake(device->mutex, portMAX_DELAY);
    if (device->pin_reset >= 0) {
        ESP_LOGI(TAG, "reset");
        esp_err_t res = gpio_set_level(device->pin_reset, false);
        if (res != ESP_OK) return res;
        res = gpio_set_direction(device->pin_reset, GPIO_MODE_OUTPUT);
        if (res != ESP_OK) return res;
        vTaskDelay(50 / portTICK_PERIOD_MS);
        res = gpio_set_direction(device->pin_reset, GPIO_MODE_INPUT);
        if (res != ESP_OK) return res;
        vTaskDelay(50 / portTICK_PERIOD_MS);
    } else {
        ESP_LOGI(TAG, "(no reset pin available)");
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    if (device->mutex != NULL) xSemaphoreGive(device->mutex);
    return ESP_OK;
}

esp_err_t ili9341_set_sleep(ILI9341* device, const bool state) {
    esp_err_t res;
    ESP_LOGI(TAG, "sleep mode %s", state ? "on" : "off");
    if (device->mutex != NULL) xSemaphoreTake(device->mutex, portMAX_DELAY);
    if (state) {
        res = ili9341_send_command(device, ILI9341_SLPIN);
        if (res != ESP_OK) return res;
    } else {
        res = ili9341_send_command(device, ILI9341_SLPOUT);
        if (res != ESP_OK) return res;
    }
    if (device->mutex != NULL) xSemaphoreGive(device->mutex);
    return res;
}

esp_err_t ili9341_set_display(ILI9341* device, const bool state) {
    esp_err_t res;
    ESP_LOGI(TAG, "sleep display %s", state ? "on" : "off");
    if (device->mutex != NULL) xSemaphoreTake(device->mutex, portMAX_DELAY);
    
    if (state) {
        res = ili9341_send_command(device, ILI9341_DISPON);
        if (res != ESP_OK) return res;
    } else {
        res = ili9341_send_command(device, ILI9341_DISPOFF);
        if (res != ESP_OK) return res;
    }
    if (device->mutex != NULL) xSemaphoreGive(device->mutex);
    return res;
}

esp_err_t ili9341_set_invert(ILI9341* device, const bool state) {
    ESP_LOGI(TAG, "invert %s", state ? "on" : "off");
    if (device->mutex != NULL) xSemaphoreTake(device->mutex, portMAX_DELAY);
    
    if (state) {
        return ili9341_send_command(device, ILI9341_INVON);
    } else {
        return ili9341_send_command(device, ILI9341_INVOFF);
    }
    if (device->mutex != NULL) xSemaphoreGive(device->mutex);
}

esp_err_t ili9341_set_partial_scanning(ILI9341* device, const uint16_t start, const uint16_t end) {
    if (device->mutex != NULL) xSemaphoreTake(device->mutex, portMAX_DELAY);
    if ((start == 0) && (end >= ILI9341_WIDTH - 1)) {
        ESP_LOGI(TAG, "partial scanning off");
        return ili9341_send_command(device, ILI9341_NORON);
    } else {
        ESP_LOGI(TAG, "partial scanning on (%u to %u)", start, end);
        esp_err_t res = ili9341_send_command(device, ILI9341_PTLAR);
        if (res != ESP_OK) return res;
        res = ili9341_send_u32(device, (start << 16) | end);
        if (res != ESP_OK) return res;
        return ili9341_send_command(device, ILI9341_PTLON);
    }
    if (device->mutex != NULL) xSemaphoreGive(device->mutex);
}

esp_err_t ili9341_set_tearing_effect_line(ILI9341* device, const bool state) {
    ESP_LOGI(TAG, "tearing effect line %s", state ? "on" : "off");
    if (device->mutex != NULL) xSemaphoreTake(device->mutex, portMAX_DELAY);
    if (state) {
        return ili9341_send_command(device, ILI9341_TEON);
    } else {
        return ili9341_send_command(device, ILI9341_TEOFF);
    }
    if (device->mutex != NULL) xSemaphoreGive(device->mutex);
}

esp_err_t ili9341_set_idle_mode(ILI9341* device, const bool state) {
    ESP_LOGI(TAG, "idle mode %s", state ? "on" : "off");
    if (device->mutex != NULL) xSemaphoreTake(device->mutex, portMAX_DELAY);
    if (state) {
        return ili9341_send_command(device, ILI9341_IDMON);
    } else {
        return ili9341_send_command(device, ILI9341_IDMOFF);
    }
    if (device->mutex != NULL) xSemaphoreGive(device->mutex);
}

esp_err_t ili9341_init(ILI9341* device) {
    esp_err_t res;
    
    if (device->pin_dcx < 0) return ESP_FAIL;
    if (device->pin_cs < 0) return ESP_FAIL;
    
    /*if (device->mutex == NULL) {
        device->mutex = xSemaphoreCreateMutex();
    }*/
    
    if (device->mutex != NULL) xSemaphoreGive(device->mutex);

    //Initialize data/clock select GPIO pin
    res = gpio_set_direction(device->pin_dcx, GPIO_MODE_OUTPUT);
    if (res != ESP_OK) return res;

    if (device->spi_device == NULL) {
        spi_device_interface_config_t devcfg = {
            .command_bits     = 0,
            .address_bits     = 0,
            .dummy_bits       = 0,
            .mode             = 0, // SPI mode 0
            .duty_cycle_pos   = 128,
            .cs_ena_pretrans  = 0,
            .cs_ena_posttrans = 0,
            .clock_speed_hz   = device->spi_speed,
            .input_delay_ns   = 0,
            .spics_io_num     = device->pin_cs,
            .flags            = SPI_DEVICE_HALFDUPLEX,
            .queue_size       = 1,
            .pre_cb           = ili9341_spi_pre_transfer_callback, // Handles D/C line
            .post_cb          = NULL
        };
        res = spi_bus_add_device(VSPI_HOST, &devcfg, &device->spi_device);
        if (res != ESP_OK) return res;
    }

    if (device->callback != NULL) {
        device->callback(false);
    }

    //Reset the LCD display
    res = ili9341_reset(device);
    if (res != ESP_OK) return res;

    //Send the initialization data to the LCD display
    res = ili9341_write_init_data(device, ili9341_init_data);
    if (res != ESP_OK) return res;

    //Disable sleep mode
    res = ili9341_set_sleep(device, false);
    if (res != ESP_OK) return res;

    //Turn on the LCD
    res = ili9341_send_command(device, ILI9341_DISPON);
    if (res != ESP_OK) return res;
    
    //Configure orientation
    res = ili9341_set_cfg(device, device->rotation, device->color_mode);
    if (res != ESP_OK) return res;

    return ESP_OK;
}

esp_err_t ili9341_deinit(ILI9341* device) {
    if (device->mutex != NULL) xSemaphoreTake(device->mutex, portMAX_DELAY); // Block access to the peripheral while deinitialized
    esp_err_t res;
    if (device->spi_device != NULL) {
        res = spi_bus_remove_device(device->spi_device);
        device->spi_device = NULL;
        if (res != ESP_OK) return res;
    }
    res = gpio_set_direction(device->pin_dcx, GPIO_MODE_INPUT);
    if (res != ESP_OK) return res;
    res = gpio_set_direction(device->pin_cs, GPIO_MODE_INPUT);
    if (res != ESP_OK) return res;
    if (device->callback != NULL) {
        device->callback(true);
    }
    res = ili9341_reset(device);
    if (res != ESP_OK) return res;
    return res;
}

esp_err_t ili9341_select(ILI9341* device, const bool state) {
    if (device->spi_device != NULL) return ESP_FAIL;
    return gpio_set_level(device->pin_cs, !state);
}

esp_err_t ili9341_write(ILI9341* device, const uint8_t *buffer) {
    return ili9341_write_partial_direct(device, buffer, 0, 0, ILI9341_WIDTH, ILI9341_HEIGHT);
}

esp_err_t ili9341_write_partial_direct(ILI9341* device, const uint8_t *buffer, uint16_t x, uint16_t y, uint16_t width, uint16_t height) { // Without conversion
    if (device->spi_device == NULL) return ESP_FAIL;
    if (device->mutex != NULL) xSemaphoreTake(device->mutex, portMAX_DELAY);
    esp_err_t res = ili9341_set_addr_window(device, x, y, width, height);
    if (res != ESP_OK) {
        if (device->mutex != NULL) xSemaphoreGive(device->mutex);
        return res;
    }
    
    uint32_t position = 0;
    while (width * height * 2 - position > 0) {
        uint32_t length = device->spi_max_transfer_size;
        if (width * height * 2 - position < device->spi_max_transfer_size) length = width * height * 2 - position;
        
        res = ili9341_send(device, &buffer[position], length, true);
        if (res != ESP_OK) {
            if (device->mutex != NULL) xSemaphoreGive(device->mutex);
            return res;
        }
        position += length;
    }
    if (device->mutex != NULL) xSemaphoreGive(device->mutex);
    return res;
}

esp_err_t ili9341_write_partial(ILI9341* device, const uint8_t *frameBuffer, uint16_t x, uint16_t y, uint16_t width, uint16_t height) { // With conversion
    if (device->mutex != NULL) xSemaphoreTake(device->mutex, portMAX_DELAY);
    
    if (device->spi_device == NULL) {
        if (device->mutex != NULL) xSemaphoreGive(device->mutex);
        return ESP_FAIL;
    }
    esp_err_t res = ESP_OK;

    uint8_t *buffer = heap_caps_malloc(device->spi_max_transfer_size, MALLOC_CAP_8BIT);
    if (buffer == NULL) {
        if (device->mutex != NULL) xSemaphoreGive(device->mutex);
        return ESP_FAIL;
    }

    while (width > 0) {
        uint16_t transactionWidth = width;
        if (transactionWidth*2 > device->spi_max_transfer_size) {
            transactionWidth = device->spi_max_transfer_size / 2;
        }
        res = ili9341_set_addr_window(device, x, y, transactionWidth, height);
        if (res != ESP_OK) {
            free(buffer);
            if (device->mutex != NULL) xSemaphoreGive(device->mutex);
            return res;
        }
        for (uint16_t currentLine = 0; currentLine < height; currentLine++) {
            for (uint16_t i = 0; i<transactionWidth; i++) {
                buffer[i*2+0] = frameBuffer[((x+i)+(y+currentLine)*ILI9341_WIDTH)*2+0];
                buffer[i*2+1] = frameBuffer[((x+i)+(y+currentLine)*ILI9341_WIDTH)*2+1];
            }
            res = ili9341_send(device, buffer, transactionWidth*2, true);
            if (res != ESP_OK) {
                free(buffer);
                if (device->mutex != NULL) xSemaphoreGive(device->mutex);
                return res;
            }
        }
        width -= transactionWidth;
        x += transactionWidth;
    }
    free(buffer);
    if (device->mutex != NULL) xSemaphoreGive(device->mutex);
    return res;
}
