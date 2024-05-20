// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>
#include <string.h>

#include "shared-bindings/paralleldisplaybus/ParallelBus.h"

#include "bindings/espidf/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/microcontroller/__init__.h"

#include "common-hal/audiobusio/__init__.h"
#include "common-hal/microcontroller/Pin.h"
#include "py/runtime.h"

#include "driver/gpio.h"

/*
 *  Current pin limitations for ESP32-S2 ParallelBus:
 *   - data0 pin must be byte aligned
 */
static bool _transfer_done(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx) {
    paralleldisplaybus_parallelbus_obj_t *self = user_ctx;
    self->transfer_done = true;
    return false;
}

void common_hal_paralleldisplaybus_parallelbus_construct_nonsequential(paralleldisplaybus_parallelbus_obj_t *self,
    uint8_t n_pins, const mcu_pin_obj_t **data_pins,
    const mcu_pin_obj_t *command, const mcu_pin_obj_t *chip_select,
    const mcu_pin_obj_t *write, const mcu_pin_obj_t *read, const mcu_pin_obj_t *reset, uint32_t frequency) {

    if (n_pins != 8 && n_pins != 16) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("Number of data_pins must be 8 or 16, not %d"), n_pins);
    }

    for (uint8_t i = 0; i < n_pins; i++) {
        if (!common_hal_mcu_pin_is_free(data_pins[i])) {
            mp_raise_ValueError_varg(MP_ERROR_TEXT("Bus pin %d is already in use"), i);
        }
    }

    self->config.dc_gpio_num = common_hal_mcu_pin_number(command);
    self->config.wr_gpio_num = common_hal_mcu_pin_number(write);   // write strobe
    self->config.clk_src = LCD_CLK_SRC_DEFAULT;
    self->config.bus_width = n_pins;
    self->config.max_transfer_bytes = 512;
    for (uint8_t i = 0; i < n_pins; i++) {
        self->config.data_gpio_nums[i] = common_hal_mcu_pin_number(data_pins[i]);
    }
    CHECK_ESP_RESULT(esp_lcd_new_i80_bus(&self->config, &self->bus_handle));

    esp_lcd_panel_io_i80_config_t panel_io_config = {
        .cs_gpio_num = -1, // We manage CS
        .pclk_hz = frequency,
        .trans_queue_depth = 1, // We block anyway
        .on_color_trans_done = _transfer_done,
        .user_ctx = self,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8
    };
    panel_io_config.dc_levels.dc_cmd_level = 0;
    panel_io_config.dc_levels.dc_data_level = 1;
    panel_io_config.dc_levels.dc_idle_level = 1;
    CHECK_ESP_RESULT(esp_lcd_new_panel_io_i80(self->bus_handle, &panel_io_config, &self->panel_io_handle));

    if (read != NULL) {
        common_hal_never_reset_pin(read);
        gpio_config_t read_config = {
            .pin_bit_mask = 1ull << read->number,
                .mode = GPIO_MODE_OUTPUT,
                .pull_up_en = GPIO_PULLUP_DISABLE,
                .pull_down_en = GPIO_PULLDOWN_DISABLE,
                .intr_type = GPIO_INTR_DISABLE,
        };
        gpio_config(&read_config);
        self->read_pin_number = read->number;
        gpio_set_level(read->number, true);
    }

    self->reset_pin_number = NO_PIN;
    if (reset != NULL) {
        common_hal_never_reset_pin(reset);
        self->reset_pin_number = reset->number;
    }

    self->cs_pin_number = common_hal_mcu_pin_number(chip_select);
    gpio_config_t chip_select_config = {
        .pin_bit_mask = 1ull << self->cs_pin_number,
            .mode = GPIO_MODE_OUTPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&chip_select_config);
    gpio_set_level(self->cs_pin_number, true);

    common_hal_never_reset_pin(chip_select);
    common_hal_never_reset_pin(command);
    common_hal_never_reset_pin(write);

    for (uint8_t i = 0; i < n_pins; i++) {
        common_hal_never_reset_pin(data_pins[i]);
    }
}


void common_hal_paralleldisplaybus_parallelbus_construct(paralleldisplaybus_parallelbus_obj_t *self,
    const mcu_pin_obj_t *data0, const mcu_pin_obj_t *command, const mcu_pin_obj_t *chip_select,
    const mcu_pin_obj_t *write, const mcu_pin_obj_t *read, const mcu_pin_obj_t *reset, uint32_t frequency) {
    char buf[7];
    const mcu_pin_obj_t *data_pins[8];
    for (int i = 0; i < 8; i++) {
        snprintf(buf, sizeof(buf), "GPIO%d", data0->number + i);
        data_pins[i] = validate_obj_is_free_pin(
            mp_obj_dict_get(MP_OBJ_FROM_PTR(&mcu_pin_globals), mp_obj_new_str(buf, strlen(buf))), MP_QSTR_pin);
    }
    common_hal_paralleldisplaybus_parallelbus_construct_nonsequential(self, 8, data_pins, command, chip_select, write, read, reset, frequency);
}


void common_hal_paralleldisplaybus_parallelbus_deinit(paralleldisplaybus_parallelbus_obj_t *self) {
    if (!self->panel_io_handle) {
        return;
    }
    esp_lcd_panel_io_del(self->panel_io_handle);
    self->panel_io_handle = NULL;
    esp_lcd_del_i80_bus(self->bus_handle);
    self->bus_handle = NULL;
    for (uint8_t i = 0; i < self->config.bus_width; i++) {
        reset_pin_number(self->config.data_gpio_nums[i]);
    }

    reset_pin_number(self->config.dc_gpio_num);
    reset_pin_number(self->config.wr_gpio_num);
    reset_pin_number(self->cs_pin_number);
    reset_pin_number(self->read_pin_number);
    reset_pin_number(self->reset_pin_number);
}

bool common_hal_paralleldisplaybus_parallelbus_reset(mp_obj_t obj) {
    /* SNIP - same as from SAMD and NRF ports */
    paralleldisplaybus_parallelbus_obj_t *self = MP_OBJ_TO_PTR(obj);
    if (self->reset_pin_number == NO_PIN) {
        return false;
    }

    gpio_set_level(self->reset_pin_number, false);
    common_hal_mcu_delay_us(4);
    gpio_set_level(self->reset_pin_number, true);
    return true;

}

bool common_hal_paralleldisplaybus_parallelbus_bus_free(mp_obj_t obj) {
    /* SNIP - same as from SAMD and NRF ports */
    return true;
}

bool common_hal_paralleldisplaybus_parallelbus_begin_transaction(mp_obj_t obj) {
    paralleldisplaybus_parallelbus_obj_t *self = MP_OBJ_TO_PTR(obj);
    gpio_set_level(self->cs_pin_number, false);
    return true;
}


void common_hal_paralleldisplaybus_parallelbus_send(mp_obj_t obj, display_byte_type_t byte_type,
    display_chip_select_behavior_t chip_select, const uint8_t *data, uint32_t data_length) {
    paralleldisplaybus_parallelbus_obj_t *self = MP_OBJ_TO_PTR(obj);
    if (data_length == 0) {
        return;
    }
    if (byte_type == DISPLAY_DATA) {
        // We don't use the color transmit function because this buffer will be small-ish. displayio
        // will already partition it into small pieces.
        self->transfer_done = false;
        CHECK_ESP_RESULT(esp_lcd_panel_io_tx_color(self->panel_io_handle, -1, data, data_length));
        while (!self->transfer_done) {
            RUN_BACKGROUND_TASKS;
        }
    } else if (data_length == 1) {
        CHECK_ESP_RESULT(esp_lcd_panel_io_tx_param(self->panel_io_handle, data[0], NULL, 0));
    } else {
        // Command with length > 1 isn't supported.
    }
}

void common_hal_paralleldisplaybus_parallelbus_end_transaction(mp_obj_t obj) {
    paralleldisplaybus_parallelbus_obj_t *self = MP_OBJ_TO_PTR(obj);
    gpio_set_level(self->cs_pin_number, true);
}
