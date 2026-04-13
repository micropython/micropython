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

#include "include/ice40.h"

static const char *TAG = "ice40";

// SPI communication

static void IRAM_ATTR ice40_pre_transaction_cb(spi_transaction_t* transaction) {
    ICE40* device = (ICE40*) transaction->user;
    if (device->cs_enabled) {
        gpio_set_level(device->pin_cs, false);
    }
}

static void IRAM_ATTR ice40_post_transaction_cb(spi_transaction_t* transaction) {
    ICE40* device = (ICE40*) transaction->user;
    if (device->cs_enabled) {
        gpio_set_level(device->pin_cs, true);
    }
}

esp_err_t ice40_send_turbo(ICE40* device, const uint8_t* data, uint32_t length) {
    if (device->_spi_device_turbo == NULL) return ESP_FAIL;
    spi_transaction_t transaction = {
        .user = (void*) device,
        .length = length * 8,
        .tx_buffer = data,
        .rx_buffer = NULL
    };
    return spi_device_transmit(device->_spi_device_turbo, &transaction);
}

esp_err_t ice40_send(ICE40* device, const uint8_t* data, uint32_t length) {
    if (device->_spi_device_hd == NULL) return ESP_FAIL;
    spi_transaction_t transaction = {
        .user = (void*) device,
        .length = length * 8,
        .tx_buffer = data,
        .rx_buffer = NULL
    };
    return spi_device_transmit(device->_spi_device_hd, &transaction);
}

esp_err_t ice40_receive(ICE40* device, uint8_t* data, uint32_t length) {
    if (device->_spi_device_hd == NULL) return ESP_FAIL;
    spi_transaction_t transaction = {
        .user = (void*) device,
        .length = 0,
        .rxlength = length * 8,
        .tx_buffer = NULL,
        .rx_buffer = data
    };
    return spi_device_transmit(device->_spi_device_hd, &transaction);
}

esp_err_t ice40_transaction(ICE40* device, uint8_t* data_out, uint32_t out_length, uint8_t* data_in, uint32_t in_length) {
    if (device->_spi_device_fd == NULL) return ESP_FAIL;
    spi_transaction_t transaction = {
        .user = (void*) device,
        .length = out_length * 8,
        .rxlength = in_length * 8,
        .tx_buffer = data_out,
        .rx_buffer = data_in
    };
    return spi_device_transmit(device->_spi_device_fd, &transaction);
}

// Device state management

esp_err_t ice40_enable(ICE40* device) {
    esp_err_t res = ESP_FAIL;
    if (device->pin_reset >= 0) {
        res = gpio_set_level(device->pin_reset, true);
        if (res != ESP_OK) return res;
    }
    if (device->set_reset != NULL) {
        res = device->set_reset(true);
        if (res != ESP_OK) {
            ESP_LOGE(TAG, "Failed to set reset pin (true)");
            return res;
        }
    }
    return res;
}

esp_err_t ice40_disable(ICE40* device) {
    esp_err_t res;
    device->cs_enabled = false;
    if (device->pin_reset >= 0) {
        res = gpio_set_level(device->pin_reset, false);
        if (res != ESP_OK) return res;
    }
    if (device->set_reset != NULL) {
        res = device->set_reset(false);
        if (res != ESP_OK) {
            ESP_LOGE(TAG, "Failed to set reset pin (false)");
            return res;
        }
    }
    return gpio_set_level(device->pin_cs, true);
}

esp_err_t ice40_get_done(ICE40* device, bool* done) {
    if (device->pin_done >= 0) {
        *done = gpio_get_level(device->pin_done);
        return ESP_OK;
    }
    if (device->get_done != NULL) {
        return device->get_done(done);
    }
    return ESP_FAIL;
}

esp_err_t ice40_load_bitstream(ICE40* device, const uint8_t* bitstream, uint32_t length) {
    const uint8_t dummy[20] = {0};
    bool done;

    // Put ICE40 in reset state
    esp_err_t res = ice40_disable(device);
    if (res != ESP_OK) return res;

    // Set CS pin low
    res = gpio_set_level(device->pin_cs, false);
    if (res != ESP_OK) return res;

    // Put ICE40 in SPI slave download state
    res = ice40_enable(device);
    if (res != ESP_OK) return res;

    // Wait 2 ms
    vTaskDelay(2 / portTICK_PERIOD_MS);

    // Check iCE40 is reset
    res = ice40_get_done(device, &done);
    if (res != ESP_OK) return res;
    if (done) {
        ESP_LOGE(TAG, "FPGA SIGNALS DONE AFTER RESET, HARDWARE ERROR");
        return ESP_FAIL;
    }

    // Set CS pin high
    res = gpio_set_level(device->pin_cs, true);
    if (res != ESP_OK) return res;

    // Send 8 dummy clocks
    ice40_send(device, dummy, 1);

    // Set CS pin low
    res = gpio_set_level(device->pin_cs, false);
    if (res != ESP_OK) return res;

    // Send the whole bitstream
    uint32_t position = 0;
    while (position < length) {
        uint32_t remaining = length - position;
        uint32_t transferSize = (remaining < device->spi_max_transfer_size) ? remaining : device->spi_max_transfer_size;
        ice40_send(device, bitstream + position, transferSize);
        position += transferSize;
    }

    // Set CS pin high
    res = gpio_set_level(device->pin_cs, true);
    if (res != ESP_OK) return res;

    // Send > 149 dummy cycles
    ice40_send(device, dummy, sizeof(dummy));

    // Check configuration worked
    res = ice40_get_done(device, &done);
    if (res != ESP_OK) return res;

    // Enable the CS pin (to allow normal SPI transfers)
    device->cs_enabled = true;

    // Done !
    return done ? ESP_OK : ESP_FAIL;
}

esp_err_t ice40_init(ICE40* device) {
    esp_err_t res;

    if (device->pin_reset >= 0) {
        res = gpio_set_direction(device->pin_reset, GPIO_MODE_OUTPUT);
        if (res != ESP_OK) return res;
    }
        
    if (device->pin_done >= 0) {
        res = gpio_set_direction(device->pin_done, GPIO_MODE_INPUT);
        if (res != ESP_OK) return res;
    }
        
    if (device->pin_int >= 0) {
        res = gpio_set_direction(device->pin_int, GPIO_MODE_INPUT);
        if (res != ESP_OK) return res;
    }
    
    if (device->pin_cs < 0) return ESP_FAIL;
    res = gpio_set_direction(device->pin_cs, GPIO_MODE_OUTPUT);
    if (res != ESP_OK) return res;
    
    res = gpio_set_level(device->pin_cs, true);
    if (res != ESP_OK) return res;
    
    device->cs_enabled = false;
        
    spi_device_interface_config_t device_config_fd = {
        .clock_speed_hz = device->spi_speed_full_duplex,
        .input_delay_ns = device->spi_input_delay_ns,
        .mode           = 0,
        .spics_io_num   = -1,
        .queue_size     = 1,
        .flags          = 0,
        .pre_cb         = ice40_pre_transaction_cb,
        .post_cb        = ice40_post_transaction_cb,
        .command_bits   = 0,
        .address_bits   = 0,
        .dummy_bits     = 0
    };

    res = spi_bus_add_device(device->spi_bus, &device_config_fd, &device->_spi_device_fd);
    if (res != ESP_OK) return res;

    spi_device_interface_config_t device_config_hd = {
        .clock_speed_hz = device->spi_speed_half_duplex,
        .input_delay_ns = device->spi_input_delay_ns,
        .mode           = 0,
        .spics_io_num   = -1,
        .queue_size     = 1,
        .flags          = SPI_DEVICE_HALFDUPLEX,
        .pre_cb         = ice40_pre_transaction_cb,
        .post_cb        = ice40_post_transaction_cb,
        .command_bits   = 0,
        .address_bits   = 0,
        .dummy_bits     = 0
    };

    res = spi_bus_add_device(device->spi_bus, &device_config_hd, &device->_spi_device_hd);
    if (res != ESP_OK) return res;

    spi_device_interface_config_t device_config_turbo = {
        .clock_speed_hz = device->spi_speed_turbo,
        .input_delay_ns = device->spi_input_delay_ns,
        .mode           = 0,
        .spics_io_num   = -1,
        .queue_size     = 1,
        .flags          = SPI_DEVICE_HALFDUPLEX,
        .pre_cb         = ice40_pre_transaction_cb,
        .post_cb        = ice40_post_transaction_cb,
        .command_bits   = 0,
        .address_bits   = 0,
        .dummy_bits     = 0
    };

    res = spi_bus_add_device(device->spi_bus, &device_config_turbo, &device->_spi_device_turbo);
    if (res != ESP_OK) return res;

    return ice40_disable(device);
}
