#include <sdkconfig.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <esp_log.h>
#include <esp_err.h>

#include <driver/spi_master.h>
#include <driver/i2c.h>

#include "include/buses.h"

#define I2C_MASTER_TX_BUF_DISABLE  0
#define I2C_MASTER_RX_BUF_DISABLE  0
#define WRITE_BIT      I2C_MASTER_WRITE // I2C master write
#define READ_BIT       I2C_MASTER_READ  // I2C master read
#define ACK_CHECK_EN   0x1     // I2C master will check ack from slave
#define ACK_CHECK_DIS  0x0     // I2C master will not check ack from slave
#define ACK_VAL        0x0     // I2C ack value
#define NACK_VAL       0x1     // I2C nack value


static SemaphoreHandle_t i2c0_mux = NULL;
static SemaphoreHandle_t i2c1_mux = NULL;

static const char* TAG = "buses";

esp_err_t start_buses() {
    // This function initializes the VSPI, HSPI and I2C buses of the ESP32
    esp_err_t res = ESP_OK;

    #ifdef CONFIG_BUS_VSPI_ENABLE
        spi_bus_config_t vspiBusConfiguration = {0};
        vspiBusConfiguration.mosi_io_num     = CONFIG_PIN_NUM_VSPI_MOSI;
        vspiBusConfiguration.miso_io_num     = CONFIG_PIN_NUM_VSPI_MISO;
        vspiBusConfiguration.sclk_io_num     = CONFIG_PIN_NUM_VSPI_CLK;
        vspiBusConfiguration.quadwp_io_num   = CONFIG_PIN_NUM_VSPI_WP;
        vspiBusConfiguration.quadhd_io_num   = CONFIG_PIN_NUM_VSPI_HD;
        vspiBusConfiguration.max_transfer_sz = CONFIG_BUS_VSPI_MAX_TRANSFERSIZE;
        res = spi_bus_initialize(VSPI_HOST, &vspiBusConfiguration, CONFIG_BUS_VSPI_DMA_CHANNEL);
        if (res != ESP_OK) return res;
    #endif

    #ifdef CONFIG_BUS_HSPI_ENABLE
        spi_bus_config_t vspiBusConfiguration = {0};
        vspiBusConfiguration.mosi_io_num     = CONFIG_PIN_NUM_HSPI_MOSI;
        vspiBusConfiguration.miso_io_num     = CONFIG_PIN_NUM_HSPI_MISO;
        vspiBusConfiguration.sclk_io_num     = CONFIG_PIN_NUM_HSPI_CLK;
        vspiBusConfiguration.quadwp_io_num   = CONFIG_PIN_NUM_HSPI_WP;
        vspiBusConfiguration.quadhd_io_num   = CONFIG_PIN_NUM_HSPI_HD;
        vspiBusConfiguration.max_transfer_sz = CONFIG_BUS_HSPI_MAX_TRANSFERSIZE;
        res = spi_bus_initialize(HSPI_HOST, &vspiBusConfiguration, CONFIG_BUS_HSPI_DMA_CHANNEL);
        if (res != ESP_OK) return res;
    #endif

    #ifdef CONFIG_BUS_I2C0_ENABLE
        i2c_config_t i2c0BusConfiguration = {0};
        i2c0BusConfiguration.mode             = I2C_MODE_MASTER;
        i2c0BusConfiguration.sda_io_num       = CONFIG_PIN_NUM_I2C0_DATA;
        i2c0BusConfiguration.scl_io_num       = CONFIG_PIN_NUM_I2C0_CLK;
        i2c0BusConfiguration.master.clk_speed = CONFIG_I2C0_MASTER_FREQ_HZ,
        #ifdef CONFIG_PIN_PULL_I2C0_DATA
            i2c0BusConfiguration.sda_pullup_en = GPIO_PULLUP_ENABLE,
        #else
            i2c0BusConfiguration.sda_pullup_en = GPIO_PULLUP_DISABLE,
        #endif
        #ifdef CONFIG_PIN_PULL_I2C0_CLK
            i2c0BusConfiguration.scl_pullup_en = GPIO_PULLUP_ENABLE,
        #else
            i2c0BusConfiguration.scl_pullup_en = GPIO_PULLUP_DISABLE,
        #endif

        res = i2c_param_config(I2C_NUM_0, &i2c0BusConfiguration);
        if (res != ESP_OK) return res;
        res = i2c_set_timeout(I2C_NUM_0, 20000); // 250 us ( 20000 clock cycles @ APB freq = 80 MHz )
        if (res != ESP_OK) return res;
        res = i2c_driver_install(I2C_NUM_0, i2c0BusConfiguration.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
        if (res != ESP_OK) return res;
        
        i2c0_mux = xSemaphoreCreateMutex();
        if (i2c0_mux == NULL) return ESP_ERR_NO_MEM;
    #endif

    #ifdef CONFIG_BUS_I2C1_ENABLE
        i2c_config_t i2c1BusConfiguration = {0};
        i2c1BusConfiguration.mode             = I2C_MODE_MASTER;
        i2c1BusConfiguration.sda_io_num       = CONFIG_PIN_NUM_I2C1_DATA;
        i2c1BusConfiguration.scl_io_num       = CONFIG_PIN_NUM_I2C1_CLK;
        i2c1BusConfiguration.master.clk_speed = CONFIG_I2C1_MASTER_FREQ_HZ,
        #ifdef CONFIG_PIN_PULL_I2C1_DATA
            i2c1BusConfiguration.sda_pullup_en = GPIO_PULLUP_ENABLE,
        #else
            i2c1BusConfiguration.sda_pullup_en = GPIO_PULLUP_DISABLE,
        #endif
        #ifdef CONFIG_PIN_PULL_I2C1_CLK
            i2c1BusConfiguration.scl_pullup_en = GPIO_PULLUP_ENABLE,
        #else
            i2c1BusConfiguration.scl_pullup_en = GPIO_PULLUP_DISABLE,
        #endif

        res = i2c_param_config(I2C_NUM_1, &i2c1BusConfiguration);
        if (res != ESP_OK) return res;
        res = i2c_set_timeout(I2C_NUM_1, 20000); // 250 us ( 20000 clock cycles @ APB freq = 80 MHz )
        if (res != ESP_OK) return res;
        res = i2c_driver_install(I2C_NUM_1, i2c1BusConfiguration.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
        if (res != ESP_OK) return res;
        
        i2c1_mux = xSemaphoreCreateMutex();
        if (i2c1_mux == NULL) return ESP_ERR_NO_MEM;
    #endif

    return res;
}

/* I2C helper functions */

esp_err_t driver_i2c_read_bytes(int bus, uint8_t addr, uint8_t *value, size_t value_len) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    esp_err_t res = i2c_master_start(cmd);
    if (res != ESP_OK) { i2c_cmd_link_delete(cmd); return res; }
    res = i2c_master_write_byte(cmd, ( addr << 1 ) | READ_BIT, ACK_CHECK_EN);
    if (res != ESP_OK) { i2c_cmd_link_delete(cmd); return res; }
    if (value_len > 1) {
        res = i2c_master_read(cmd, value, value_len-1, ACK_VAL);
        if (res != ESP_OK) { i2c_cmd_link_delete(cmd); return res; }
    }
    res = i2c_master_read_byte(cmd, &value[value_len-1], NACK_VAL);
    if (res != ESP_OK) { i2c_cmd_link_delete(cmd); return res; }
    res = i2c_master_stop(cmd);
    if (res != ESP_OK) { i2c_cmd_link_delete(cmd); return res; }   

    SemaphoreHandle_t mux = (bus == I2C_NUM_1) ? i2c1_mux : i2c0_mux;
    if (xSemaphoreTake(mux, portMAX_DELAY) != pdTRUE) return ESP_ERR_TIMEOUT; // Wait for I2C bus to become available
    res = i2c_master_cmd_begin(bus, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    xSemaphoreGive(mux);
    return res;
}

esp_err_t driver_i2c_read_reg(int bus, uint8_t addr, uint8_t reg, uint8_t *value, size_t value_len) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    esp_err_t res = i2c_master_start(cmd);
    if (res != ESP_OK) { i2c_cmd_link_delete(cmd); return res; }
    res = i2c_master_write_byte(cmd, ( addr << 1 ) | WRITE_BIT, ACK_CHECK_EN);
    if (res != ESP_OK) { i2c_cmd_link_delete(cmd); return res; }
    res = i2c_master_write_byte(cmd, reg, ACK_CHECK_EN);
    if (res != ESP_OK) { i2c_cmd_link_delete(cmd); return res; }
    res = i2c_master_start(cmd);
    if (res != ESP_OK) { i2c_cmd_link_delete(cmd); return res; }
    res = i2c_master_write_byte(cmd, ( addr << 1 ) | READ_BIT, ACK_CHECK_EN);
    if (res != ESP_OK) { i2c_cmd_link_delete(cmd); return res; }
    if (value_len > 1) {
        res = i2c_master_read(cmd, value, value_len-1, ACK_VAL);
        if (res != ESP_OK) { i2c_cmd_link_delete(cmd); return res; }
    }
    res = i2c_master_read_byte(cmd, &value[value_len-1], NACK_VAL);
    if (res != ESP_OK) { i2c_cmd_link_delete(cmd); return res; }
    res = i2c_master_stop(cmd);
    if (res != ESP_OK) { i2c_cmd_link_delete(cmd); return res; }
    
    SemaphoreHandle_t mux = (bus == I2C_NUM_1) ? i2c1_mux : i2c0_mux;
    if (xSemaphoreTake(mux, portMAX_DELAY) != pdTRUE) return ESP_ERR_TIMEOUT; // Wait for I2C bus to become available
    res = i2c_master_cmd_begin(bus, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    xSemaphoreGive(mux);
    return res;
}

esp_err_t driver_i2c_read_event(int bus, uint8_t addr, uint8_t *buf) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    esp_err_t res = i2c_master_start(cmd);
    if (res != ESP_OK) { i2c_cmd_link_delete(cmd); return res; }
    res = i2c_master_write_byte(cmd, ( addr << 1 ) | READ_BIT, ACK_CHECK_EN);
    if (res != ESP_OK) { i2c_cmd_link_delete(cmd); return res; }
    res = i2c_master_read(cmd, buf, 2, ACK_VAL);
    if (res != ESP_OK) { i2c_cmd_link_delete(cmd); return res; }
    res = i2c_master_read_byte(cmd, &buf[2], NACK_VAL);
    if (res != ESP_OK) { i2c_cmd_link_delete(cmd); return res; }
    res = i2c_master_stop(cmd);
    if (res != ESP_OK) { i2c_cmd_link_delete(cmd); return res; }

    SemaphoreHandle_t mux = (bus == I2C_NUM_1) ? i2c1_mux : i2c0_mux;
    if (xSemaphoreTake(mux, portMAX_DELAY) != pdTRUE) return ESP_ERR_TIMEOUT; // Wait for I2C bus to become available
    res = i2c_master_cmd_begin(bus, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    xSemaphoreGive(mux);
    return res;
}

esp_err_t driver_i2c_write_byte(int bus, uint8_t addr, uint8_t value) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    esp_err_t res = i2c_master_start(cmd);
    if (res != ESP_OK) { i2c_cmd_link_delete(cmd); return res; }
    res = i2c_master_write_byte(cmd, ( addr << 1 ) | WRITE_BIT, ACK_CHECK_EN);
    if (res != ESP_OK) { i2c_cmd_link_delete(cmd); return res; }
    res = i2c_master_write_byte(cmd, value, ACK_CHECK_EN);
    if (res != ESP_OK) { i2c_cmd_link_delete(cmd); return res; }
    res = i2c_master_stop(cmd);
    if (res != ESP_OK) { i2c_cmd_link_delete(cmd); return res; }

    SemaphoreHandle_t mux = (bus == I2C_NUM_1) ? i2c1_mux : i2c0_mux;
    if (xSemaphoreTake(mux, portMAX_DELAY) != pdTRUE) return ESP_ERR_TIMEOUT; // Wait for I2C bus to become available
    res = i2c_master_cmd_begin(bus, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    xSemaphoreGive(mux);
    return res;
}

esp_err_t driver_i2c_write_reg(int bus, uint8_t addr, uint8_t reg, uint8_t value) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    esp_err_t res = i2c_master_start(cmd);
    if (res != ESP_OK) { i2c_cmd_link_delete(cmd); return res; }
    res = i2c_master_write_byte(cmd, ( addr << 1 ) | WRITE_BIT, ACK_CHECK_EN);
    if (res != ESP_OK) { i2c_cmd_link_delete(cmd); return res; }
    res = i2c_master_write_byte(cmd, reg, ACK_CHECK_EN);
    if (res != ESP_OK) { i2c_cmd_link_delete(cmd); return res; }
    res = i2c_master_write_byte(cmd, value, ACK_CHECK_EN);
    if (res != ESP_OK) { i2c_cmd_link_delete(cmd); return res; }
    res = i2c_master_stop(cmd);
    if (res != ESP_OK) { i2c_cmd_link_delete(cmd); return res; }

    SemaphoreHandle_t mux = (bus == I2C_NUM_1) ? i2c1_mux : i2c0_mux;
    if (xSemaphoreTake(mux, portMAX_DELAY) != pdTRUE) return ESP_ERR_TIMEOUT; // Wait for I2C bus to become available
    res = i2c_master_cmd_begin(bus, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    xSemaphoreGive(mux);
    return res;
}

esp_err_t driver_i2c_write_reg_n(int bus, uint8_t addr, uint8_t reg, uint8_t *value, size_t value_len) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    esp_err_t res = i2c_master_start(cmd);
    if (res != ESP_OK) { i2c_cmd_link_delete(cmd); return res; }
    res = i2c_master_write_byte(cmd, ( addr << 1 ) | WRITE_BIT, ACK_CHECK_EN);
    if (res != ESP_OK) { i2c_cmd_link_delete(cmd); return res; }
    res = i2c_master_write_byte(cmd, reg, ACK_CHECK_EN);
    if (res != ESP_OK) { i2c_cmd_link_delete(cmd); return res; }
    for (size_t i = 0; i < value_len; i++) {
        res = i2c_master_write_byte(cmd, value[i], ACK_CHECK_EN);
        if (res != ESP_OK) { i2c_cmd_link_delete(cmd); return res; }
    }
    res = i2c_master_stop(cmd);
    if (res != ESP_OK) { i2c_cmd_link_delete(cmd); return res; }

    SemaphoreHandle_t mux = (bus == I2C_NUM_1) ? i2c1_mux : i2c0_mux;
    if (xSemaphoreTake(mux, portMAX_DELAY) != pdTRUE) return ESP_ERR_TIMEOUT; // Wait for I2C bus to become available
    res = i2c_master_cmd_begin(bus, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    xSemaphoreGive(mux);
    return res;
}

esp_err_t driver_i2c_write_buffer(int bus, uint8_t addr, const uint8_t* buffer, uint16_t len) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    esp_err_t res = i2c_master_start(cmd);
    if (res != ESP_OK) { i2c_cmd_link_delete(cmd); return res; }
    res = i2c_master_write_byte(cmd, ( addr << 1 ) | WRITE_BIT, ACK_CHECK_EN);
    if (res != ESP_OK) { i2c_cmd_link_delete(cmd); return res; }
    for (uint16_t i = 0; i < len; i++) {
        res = i2c_master_write_byte(cmd, buffer[i], ACK_CHECK_EN);
        if (res != ESP_OK) { i2c_cmd_link_delete(cmd); return res; }
    }
    res = i2c_master_stop(cmd);
    if (res != ESP_OK) { i2c_cmd_link_delete(cmd); return res; }

    SemaphoreHandle_t mux = (bus == I2C_NUM_1) ? i2c1_mux : i2c0_mux;
    if (xSemaphoreTake(mux, portMAX_DELAY) != pdTRUE) return ESP_ERR_TIMEOUT; // Wait for I2C bus to become available
    res = i2c_master_cmd_begin(bus, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    xSemaphoreGive(mux);
    return res;
}

esp_err_t driver_i2c_write_buffer_reg(int bus, uint8_t addr, uint8_t reg, const uint8_t* buffer, uint16_t len) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    esp_err_t res = i2c_master_start(cmd);
    if (res != ESP_OK) { i2c_cmd_link_delete(cmd); return res; }
    res = i2c_master_write_byte(cmd, ( addr << 1 ) | WRITE_BIT, ACK_CHECK_EN);
    if (res != ESP_OK) { i2c_cmd_link_delete(cmd); return res; }
    res = i2c_master_write_byte(cmd, reg, ACK_CHECK_EN);
    if (res != ESP_OK) { i2c_cmd_link_delete(cmd); return res; }
    for (uint16_t i = 0; i < len; i++) {
        res = i2c_master_write_byte(cmd, buffer[i], ACK_CHECK_EN);
        if (res != ESP_OK) { i2c_cmd_link_delete(cmd); return res; }
    }
    res = i2c_master_stop(cmd);
    if (res != ESP_OK) { i2c_cmd_link_delete(cmd); return res; }

    SemaphoreHandle_t mux = (bus == I2C_NUM_1) ? i2c1_mux : i2c0_mux;
    if (xSemaphoreTake(mux, portMAX_DELAY) != pdTRUE) return ESP_ERR_TIMEOUT; // Wait for I2C bus to become available
    res = i2c_master_cmd_begin(bus, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    xSemaphoreGive(mux);
    return res;
}

esp_err_t driver_i2c_write_reg32(int bus, uint8_t addr, uint8_t reg, uint32_t value) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    esp_err_t res = i2c_master_start(cmd);
    if (res != ESP_OK) { i2c_cmd_link_delete(cmd); return res; }
    res = i2c_master_write_byte(cmd, ( addr << 1 ) | WRITE_BIT, ACK_CHECK_EN);
    if (res != ESP_OK) { i2c_cmd_link_delete(cmd); return res; }
    res = i2c_master_write_byte(cmd, reg, ACK_CHECK_EN);
    if (res != ESP_OK) { i2c_cmd_link_delete(cmd); return res; }
    res = i2c_master_write_byte(cmd, (value)&0xFF, ACK_CHECK_EN);
    if (res != ESP_OK) { i2c_cmd_link_delete(cmd); return res; }
    res = i2c_master_write_byte(cmd, (value>>8)&0xFF, ACK_CHECK_EN);
    if (res != ESP_OK) { i2c_cmd_link_delete(cmd); return res; }
    res = i2c_master_write_byte(cmd, (value>>16)&0xFF, ACK_CHECK_EN);
    if (res != ESP_OK) { i2c_cmd_link_delete(cmd); return res; }
    res = i2c_master_write_byte(cmd, (value>>24)&0xFF, ACK_CHECK_EN);
    if (res != ESP_OK) { i2c_cmd_link_delete(cmd); return res; }
    res = i2c_master_stop(cmd);
    if (res != ESP_OK) { i2c_cmd_link_delete(cmd); return res; }

    SemaphoreHandle_t mux = (bus == I2C_NUM_1) ? i2c1_mux : i2c0_mux;
    if (xSemaphoreTake(mux, portMAX_DELAY) != pdTRUE) return ESP_ERR_TIMEOUT; // Wait for I2C bus to become available
    res = i2c_master_cmd_begin(bus, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    xSemaphoreGive(mux);
    return res;
}
