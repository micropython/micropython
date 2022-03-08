/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Scott Shawcroft for Adafruit Industries LLC
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

#include "shared-bindings/busio/I2C.h"
#include "py/mperrno.h"
#include "py/runtime.h"

#include "components/driver/include/driver/i2c.h"

#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "supervisor/shared/translate.h"

void common_hal_busio_i2c_construct(busio_i2c_obj_t *self,
    const mcu_pin_obj_t *scl, const mcu_pin_obj_t *sda, uint32_t frequency, uint32_t timeout) {
    // Pins 45 and 46 are "strapping" pins that impact start up behavior. They usually need to
    // be pulled-down so pulling them up for I2C is a bad idea. To make this hard, we don't
    // support I2C on these pins.
    //
    // 46 is also input-only so it'll never work.
    if (scl->number == 45 || scl->number == 46 || sda->number == 45 || sda->number == 46) {
        mp_raise_ValueError(translate("Invalid pins"));
    }

    #if CIRCUITPY_REQUIRE_I2C_PULLUPS
    // Test that the pins are in a high state. (Hopefully indicating they are pulled up.)
    gpio_set_direction(sda->number, GPIO_MODE_DEF_INPUT);
    gpio_set_direction(scl->number, GPIO_MODE_DEF_INPUT);

    gpio_pullup_dis(sda->number);
    gpio_pullup_dis(scl->number);
    gpio_pulldown_en(sda->number);
    gpio_pulldown_en(scl->number);

    common_hal_mcu_delay_us(10);

    gpio_pulldown_dis(sda->number);
    gpio_pulldown_dis(scl->number);

    #if CIRCUITPY_I2C_ALLOW_INTERNAL_PULL_UP
    gpio_pullup_en(sda->number);
    gpio_pullup_en(scl->number);
    #endif

    // We must pull up within 3us to achieve 400khz.
    common_hal_mcu_delay_us((1200000 + frequency - 1) / frequency);

    if (gpio_get_level(sda->number) == 0 || gpio_get_level(scl->number) == 0) {
        reset_pin_number(sda->number);
        reset_pin_number(scl->number);
        mp_raise_RuntimeError(translate("No pull up found on SDA or SCL; check your wiring"));
    }
    #endif

    self->xSemaphore = xSemaphoreCreateMutex();
    if (self->xSemaphore == NULL) {
        mp_raise_RuntimeError(translate("Unable to create lock"));
    }
    self->sda_pin = sda;
    self->scl_pin = scl;
    self->i2c_num = peripherals_i2c_get_free_num();

    if (self->i2c_num == I2C_NUM_MAX) {
        mp_raise_ValueError(translate("All I2C peripherals are in use"));
    }

    // Delete any previous driver.
    i2c_driver_delete(self->i2c_num);

    const i2c_config_t i2c_conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = self->sda_pin->number,
        .scl_io_num = self->scl_pin->number,
        #if CIRCUITPY_I2C_ALLOW_INTERNAL_PULL_UP
        .sda_pullup_en = GPIO_PULLUP_ENABLE,  /*!< Internal GPIO pull mode for I2C sda signal*/
        .scl_pullup_en = GPIO_PULLUP_ENABLE,  /*!< Internal GPIO pull mode for I2C scl signal*/
        #else
        .sda_pullup_en = GPIO_PULLUP_DISABLE,  /*!< Internal GPIO pull mode for I2C sda signal*/
        .scl_pullup_en = GPIO_PULLUP_DISABLE,  /*!< Internal GPIO pull mode for I2C scl signal*/
        #endif

        .master = {
            .clk_speed = frequency,
        }
    };

    // Initialize I2C.
    esp_err_t err = peripherals_i2c_init(self->i2c_num, &i2c_conf);
    if (err != ESP_OK) {
        if (err == ESP_FAIL) {
            mp_raise_OSError(MP_EIO);
        } else {
            mp_raise_ValueError(translate("Invalid argument"));
        }
    }

    claim_pin(sda);
    claim_pin(scl);
}

bool common_hal_busio_i2c_deinited(busio_i2c_obj_t *self) {
    return self->sda_pin == NULL;
}

void common_hal_busio_i2c_deinit(busio_i2c_obj_t *self) {
    if (common_hal_busio_i2c_deinited(self)) {
        return;
    }

    peripherals_i2c_deinit(self->i2c_num);

    common_hal_reset_pin(self->sda_pin);
    common_hal_reset_pin(self->scl_pin);
    self->sda_pin = NULL;
    self->scl_pin = NULL;
}

static esp_err_t i2c_zero_length_write(busio_i2c_obj_t *self, uint8_t addr, TickType_t timeout) {
    // i2c_master_write_to_device() won't do zero-length writes, so we do it by hand.
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, addr << 1, true);
    i2c_master_stop(cmd);
    esp_err_t result = i2c_master_cmd_begin(self->i2c_num, cmd, timeout);
    i2c_cmd_link_delete(cmd);
    return result;
}

bool common_hal_busio_i2c_probe(busio_i2c_obj_t *self, uint8_t addr) {
    esp_err_t result = i2c_zero_length_write(self, addr, 1);
    return result == ESP_OK;
}

bool common_hal_busio_i2c_try_lock(busio_i2c_obj_t *self) {
    if (self->has_lock) {
        return false;
    }
    self->has_lock = xSemaphoreTake(self->xSemaphore, 0) == pdTRUE;
    return self->has_lock;
}

bool common_hal_busio_i2c_has_lock(busio_i2c_obj_t *self) {
    return self->has_lock;
}

void common_hal_busio_i2c_unlock(busio_i2c_obj_t *self) {
    xSemaphoreGive(self->xSemaphore);
    self->has_lock = false;
}

static uint8_t convert_esp_err(esp_err_t result) {
    switch (result) {
        case ESP_OK:
            return 0;
        case ESP_FAIL:
            return MP_ENODEV;
        case ESP_ERR_TIMEOUT:
            return MP_ETIMEDOUT;
        default:
            return MP_EIO;
    }
}

uint8_t common_hal_busio_i2c_write(busio_i2c_obj_t *self, uint16_t addr, const uint8_t *data, size_t len) {
    return convert_esp_err(len == 0
        ? i2c_zero_length_write(self, addr, 100)
        : i2c_master_write_to_device(self->i2c_num, (uint8_t)addr, data, len, 100 /* wait in ticks */)
        );
}

uint8_t common_hal_busio_i2c_read(busio_i2c_obj_t *self, uint16_t addr, uint8_t *data, size_t len) {
    return convert_esp_err(
        i2c_master_read_from_device(self->i2c_num, (uint8_t)addr, data, len, 100 /* wait in ticks */));
}

uint8_t common_hal_busio_i2c_write_read(busio_i2c_obj_t *self, uint16_t addr,
    uint8_t *out_data, size_t out_len, uint8_t *in_data, size_t in_len) {
    return convert_esp_err(
        i2c_master_write_read_device(self->i2c_num, (uint8_t)addr,
            out_data, out_len, in_data, in_len, 100 /* wait in ticks */));
}

void common_hal_busio_i2c_never_reset(busio_i2c_obj_t *self) {
    never_reset_i2c(self->i2c_num);

    common_hal_never_reset_pin(self->scl_pin);
    common_hal_never_reset_pin(self->sda_pin);
}
