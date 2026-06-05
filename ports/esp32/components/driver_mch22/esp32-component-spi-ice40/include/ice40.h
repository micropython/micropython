#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <esp_err.h>
#include <driver/spi_master.h>

typedef esp_err_t (*ice40_get_done_t)(bool*);
typedef esp_err_t (*ice40_set_reset_t)(bool);

typedef struct ICE40 {
    //  Pins
    int spi_bus;
    int pin_cs;
    int pin_done;
    int pin_reset;
    int pin_int;
    
    // Configuration
    uint32_t spi_speed_full_duplex;
    uint32_t spi_speed_half_duplex;
    uint32_t spi_speed_turbo;
    uint32_t spi_input_delay_ns;
    uint32_t spi_max_transfer_size;
    
    // External pin handlers
    ice40_get_done_t get_done;
    ice40_set_reset_t set_reset;
    
    // Internal state
    spi_device_handle_t _spi_device_fd;
    spi_device_handle_t _spi_device_hd;
    spi_device_handle_t _spi_device_turbo;
    bool cs_enabled;
} ICE40;

// Raw SPI transfer functions
esp_err_t ice40_send_turbo(ICE40* device, const uint8_t* data, uint32_t length);
esp_err_t ice40_send(ICE40* device, const uint8_t* data, uint32_t length);
esp_err_t ice40_receive(ICE40* device, uint8_t* data, uint32_t length);
esp_err_t ice40_transaction(ICE40* device, uint8_t* data_out, uint32_t out_length, uint8_t* data_in, uint32_t in_length);

// Devic state management
esp_err_t ice40_enable(ICE40* device);
esp_err_t ice40_disable(ICE40* device);
esp_err_t ice40_get_done(ICE40* device, bool* done);
esp_err_t ice40_load_bitstream(ICE40* device, const uint8_t* bitstream, uint32_t length);

// Driver initialization
esp_err_t ice40_init(ICE40* device);
