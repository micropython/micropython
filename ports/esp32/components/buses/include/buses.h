#pragma once

#include <stdint.h>
#include <esp_err.h>

extern esp_err_t start_buses();

extern esp_err_t driver_i2c_read_bytes(int bus, uint8_t addr, uint8_t *value, size_t value_len);
extern esp_err_t driver_i2c_read_reg(int bus, uint8_t addr, uint8_t reg, uint8_t *value, size_t value_len);
extern esp_err_t driver_i2c_read_event(int bus, uint8_t addr, uint8_t *buf);
extern esp_err_t driver_i2c_write_byte(int bus, uint8_t addr, uint8_t value);
extern esp_err_t driver_i2c_write_reg(int bus, uint8_t addr, uint8_t reg, uint8_t value);
extern esp_err_t driver_i2c_write_reg_n(int bus, uint8_t addr, uint8_t reg, uint8_t *value, size_t value_len);
extern esp_err_t driver_i2c_write_buffer(int bus, uint8_t addr, const uint8_t* buffer, uint16_t len);
extern esp_err_t driver_i2c_write_buffer_reg(int bus, uint8_t addr, uint8_t reg, const uint8_t* buffer, uint16_t len);
extern esp_err_t driver_i2c_write_reg32(int bus, uint8_t addr, uint8_t reg, uint32_t value);
