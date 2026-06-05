/**
 * Copyright (c) 2022 Nicolai Electronics
 *
 * SPDX-License-Identifier: MIT
 */

#include "rp2040.h"

#include <driver/gpio.h>
#include <driver/i2c.h>
#include <sdkconfig.h>

static const char* TAG = "RP2040";

esp_err_t rp2040_read_reg(RP2040* device, uint8_t reg, uint8_t* value, size_t value_len) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    esp_err_t        res = i2c_master_start(cmd);
    if (res != ESP_OK) {
        i2c_cmd_link_delete(cmd);
        return res;
    }
    res = i2c_master_write_byte(cmd, (device->i2c_address << 1) | I2C_MASTER_WRITE, true);
    if (res != ESP_OK) {
        i2c_cmd_link_delete(cmd);
        return res;
    }
    res = i2c_master_write_byte(cmd, reg, true);
    if (res != ESP_OK) {
        i2c_cmd_link_delete(cmd);
        return res;
    }
    res = i2c_master_start(cmd);
    if (res != ESP_OK) {
        i2c_cmd_link_delete(cmd);
        return res;
    }
    res = i2c_master_write_byte(cmd, (device->i2c_address << 1) | I2C_MASTER_READ, true);
    if (res != ESP_OK) {
        i2c_cmd_link_delete(cmd);
        return res;
    }
    if (value_len > 1) {
        res = i2c_master_read(cmd, value, value_len - 1, false);
        if (res != ESP_OK) {
            i2c_cmd_link_delete(cmd);
            return res;
        }
    }
    res = i2c_master_read_byte(cmd, &value[value_len - 1], true);
    if (res != ESP_OK) {
        i2c_cmd_link_delete(cmd);
        return res;
    }
    res = i2c_master_stop(cmd);
    if (res != ESP_OK) {
        i2c_cmd_link_delete(cmd);
        return res;
    }
    if (device->i2c_semaphore != NULL) xSemaphoreTake(device->i2c_semaphore, portMAX_DELAY);
    res = i2c_master_cmd_begin(device->i2c_bus, cmd, 500 / portTICK_PERIOD_MS);
    if (device->i2c_semaphore != NULL) xSemaphoreGive(device->i2c_semaphore);
    i2c_cmd_link_delete(cmd);
    return res;
}

esp_err_t rp2040_write_reg(RP2040* device, uint8_t reg, uint8_t* value, size_t value_len) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    esp_err_t        res = i2c_master_start(cmd);
    if (res != ESP_OK) {
        i2c_cmd_link_delete(cmd);
        return res;
    }
    res = i2c_master_write_byte(cmd, (device->i2c_address << 1) | I2C_MASTER_WRITE, true);
    if (res != ESP_OK) {
        i2c_cmd_link_delete(cmd);
        return res;
    }
    res = i2c_master_write_byte(cmd, reg, true);
    if (res != ESP_OK) {
        i2c_cmd_link_delete(cmd);
        return res;
    }
    for (size_t i = 0; i < value_len; i++) {
        res = i2c_master_write_byte(cmd, value[i], true);
        if (res != ESP_OK) {
            i2c_cmd_link_delete(cmd);
            return res;
        }
    }
    res = i2c_master_stop(cmd);
    if (res != ESP_OK) {
        i2c_cmd_link_delete(cmd);
        return res;
    }
    if (device->i2c_semaphore != NULL) xSemaphoreTake(device->i2c_semaphore, portMAX_DELAY);
    res = i2c_master_cmd_begin(device->i2c_bus, cmd, 500 / portTICK_PERIOD_MS);
    if (device->i2c_semaphore != NULL) xSemaphoreGive(device->i2c_semaphore);
    i2c_cmd_link_delete(cmd);
    return res;
}

void _send_input_change(RP2040* device, uint8_t input, bool value) {
    rp2040_input_message_t message;
    message.input = input;
    message.state = value;
    xQueueSend(device->queue, &message, portMAX_DELAY);
}

static void rp2040_intr_task(void* arg) {
    RP2040*  device = (RP2040*) arg;
    uint32_t state;

    while (1) {
        if (xSemaphoreTake(device->_intr_trigger, portMAX_DELAY)) {
            esp_err_t res = rp2040_read_reg(device, RP2040_REG_INPUT1, (uint8_t*) &state, 4);
            if (res != ESP_OK) {
                ESP_LOGE(TAG, "RP2040 interrupt task failed to read from RP2040");
                continue;
            }
            // ESP_LOGW(TAG, "RP2040 input state %08x", state);
            uint16_t interrupt = state >> 16;
            uint16_t values    = state & 0xFFFF;
            for (uint8_t index = 0; index < 16; index++) {
                if ((interrupt >> index) & 0x01) {
                    _send_input_change(device, index, (values >> index) & 0x01);
                }
            }
        }
    }
}

static void IRAM_ATTR rp2040_intr_handler(void* arg) {
    /* in interrupt handler context */
    RP2040* device = (RP2040*) arg;
    xSemaphoreGiveFromISR(device->_intr_trigger, NULL);
    portYIELD_FROM_ISR();
}

esp_err_t rp2040_init(RP2040* device) {
    esp_err_t res;
    static bool isr_installed = false;

    res = rp2040_get_firmware_version(device, &device->_fw_version);
    if (res != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read firmware version");
        return res;
    }

    if (device->_fw_version < 0x01) {
        ESP_LOGE(TAG, "Unsupported RP2040 firmware version (%u) found", device->_fw_version);
        return ESP_ERR_INVALID_VERSION;
    }

    res = rp2040_read_reg(device, RP2040_REG_GPIO_DIR, &device->_gpio_direction, 1);
    if (res != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read GPIO direction");
        return res;
    }

    res = rp2040_read_reg(device, RP2040_REG_GPIO_OUT, &device->_gpio_value, 1);
    if (res != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read GPIO state");
        return res;
    }

    // Create interrupt trigger
    device->_intr_trigger = xSemaphoreCreateBinary();
    if (device->_intr_trigger == NULL) return ESP_ERR_NO_MEM;

    // Attach interrupt to interrupt pin
    if (device->pin_interrupt >= 0) {
        // Install ISR service once
        if (!isr_installed) {
            gpio_install_isr_service(ESP_INTR_FLAG_IRAM);
            isr_installed = true;
        }
        res = gpio_isr_handler_add(device->pin_interrupt, rp2040_intr_handler, (void*) device);
        if (res != ESP_OK) return res;

        gpio_config_t io_conf = {
            .intr_type    = GPIO_INTR_NEGEDGE,
            .mode         = GPIO_MODE_INPUT,
            .pin_bit_mask = 1LL << device->pin_interrupt,
            .pull_down_en = 0,
            .pull_up_en   = 1,
        };

        res = gpio_config(&io_conf);
        if (res != ESP_OK) return res;

        xTaskCreate(&rp2040_intr_task, "RP2040 interrupt", 4096, (void*) device, 10, &device->_intr_task_handle);
        xSemaphoreGive(device->_intr_trigger);
    }

    return ESP_OK;
}

esp_err_t rp2040_get_firmware_version(RP2040* device, uint8_t* version) {
    esp_err_t res = rp2040_read_reg(device, RP2040_REG_FW_VER, version, 1);
    if (res == ESP_OK) {
        device->_fw_version = *version;
    }
    return res;
}

esp_err_t rp2040_get_bootloader_version(RP2040* device, uint8_t* version) {
    if (device->_fw_version != 0xFF) return ESP_FAIL;
    return rp2040_read_reg(device, RP2040_BL_REG_BL_VER, version, 1);
}

esp_err_t rp2040_get_bootloader_state(RP2040* device, uint8_t* state) {
    if (device->_fw_version != 0xFF) return ESP_FAIL;
    return rp2040_read_reg(device, RP2040_BL_REG_BL_STATE, state, 1);
}

esp_err_t rp2040_set_bootloader_ctrl(RP2040* device, uint8_t action) {
    if (device->_fw_version != 0xFF) return ESP_FAIL;
    return rp2040_write_reg(device, RP2040_BL_REG_BL_CTRL, &action, 1);
}

esp_err_t rp2040_reboot_to_bootloader(RP2040* device) {
    if ((device->_fw_version < 0x01) || (device->_fw_version == 0xFF)) return ESP_FAIL;
    uint8_t value = 0xBE;
    return rp2040_write_reg(device, RP2040_REG_BL_TRIGGER, &value, 1);
}

esp_err_t rp2040_get_gpio_dir(RP2040* device, uint8_t gpio, bool* direction) {
    if ((device->_fw_version < 0x01) || (device->_fw_version == 0xFF)) return ESP_FAIL;
    esp_err_t res = rp2040_read_reg(device, RP2040_REG_GPIO_DIR, &device->_gpio_direction, 1);
    if (res != ESP_OK) return res;
    *direction = (device->_gpio_direction >> gpio) & 0x01;
    return ESP_OK;
}
esp_err_t rp2040_set_gpio_dir(RP2040* device, uint8_t gpio, bool direction) {
    if ((device->_fw_version < 0x01) || (device->_fw_version == 0xFF)) return ESP_FAIL;
    if (direction) {
        device->_gpio_direction |= 1UL << gpio;
    } else {
        device->_gpio_direction &= ~(1UL << gpio);
    }
    return rp2040_write_reg(device, RP2040_REG_GPIO_DIR, &device->_gpio_direction, 1);
}

esp_err_t rp2040_get_gpio_value(RP2040* device, uint8_t gpio, bool* value) {
    if ((device->_fw_version < 0x01) || (device->_fw_version == 0xFF)) return ESP_FAIL;
    uint8_t   reg_value;
    esp_err_t res = rp2040_read_reg(device, RP2040_REG_GPIO_IN, &reg_value, 1);
    if (res != ESP_OK) return res;
    *value = (reg_value >> gpio) & 0x01;
    return ESP_OK;
}

esp_err_t rp2040_set_gpio_value(RP2040* device, uint8_t gpio, bool value) {
    if ((device->_fw_version < 0x01) || (device->_fw_version == 0xFF)) return ESP_FAIL;
    if (value) {
        device->_gpio_value |= 1UL << gpio;
    } else {
        device->_gpio_value &= ~(1UL << gpio);
    }
    return rp2040_write_reg(device, RP2040_REG_GPIO_OUT, &device->_gpio_value, 1);
}

esp_err_t rp2040_get_lcd_backlight(RP2040* device, uint8_t* brightness) {
    if ((device->_fw_version < 0x01) || (device->_fw_version == 0xFF)) return ESP_FAIL;
    return rp2040_read_reg(device, RP2040_REG_LCD_BACKLIGHT, brightness, 1);
}

esp_err_t rp2040_set_lcd_backlight(RP2040* device, uint8_t brightness) {
    if ((device->_fw_version < 0x01) || (device->_fw_version == 0xFF)) return ESP_OK;  // Ignore if unsupported
    return rp2040_write_reg(device, RP2040_REG_LCD_BACKLIGHT, &brightness, 1);
}

esp_err_t rp2040_set_fpga(RP2040* device, bool enabled) {
    if ((device->_fw_version < 0x01) || (device->_fw_version == 0xFF)) return ESP_FAIL;
    uint8_t value = enabled ? 0x01 : 0x00;
    return rp2040_write_reg(device, RP2040_REG_FPGA, &value, 1);
}

esp_err_t rp2040_set_fpga_loopback(RP2040* device, bool enabled, bool loopback) {
    if ((device->_fw_version < 0x01) || (device->_fw_version == 0xFF)) return ESP_FAIL;
    uint8_t value = (enabled ? 0x01 : 0x00) | (loopback ? 0x02 : 0x00);
    return rp2040_write_reg(device, RP2040_REG_FPGA, &value, 1);
}

esp_err_t rp2040_read_buttons(RP2040* device, uint16_t* value) {
    if ((device->_fw_version < 0x01) || (device->_fw_version == 0xFF)) return ESP_FAIL;
    return rp2040_read_reg(device, RP2040_REG_INPUT1, (uint8_t*) value, 2);
}

esp_err_t rp2040_get_uid(RP2040* device, uint8_t* uid) {
    if ((device->_fw_version < 0x01) || (device->_fw_version == 0xFF)) return ESP_FAIL;
    return rp2040_read_reg(device, RP2040_REG_UID0, uid, 8);
}

const float conversion_factor = 3.3f / (1 << 12);  // 12-bit ADC with 3.3v vref

esp_err_t rp2040_read_vbat_raw(RP2040* device, uint16_t* value) {
    if ((device->_fw_version < 0x02) || (device->_fw_version == 0xFF)) return ESP_FAIL;
    return rp2040_read_reg(device, RP2040_REG_ADC_VALUE_VBAT_LO, (uint8_t*) value, 2);
}

esp_err_t rp2040_read_vbat(RP2040* device, float* value) {
    uint16_t  raw;
    esp_err_t res = rp2040_read_vbat_raw(device, &raw);
    if (res != ESP_OK) return res;
    *value = raw * conversion_factor * 2;  // Connected through 100k/100k divider
    return res;
}

esp_err_t rp2040_read_vusb_raw(RP2040* device, uint16_t* value) {
    if ((device->_fw_version < 0x02) || (device->_fw_version == 0xFF)) return ESP_FAIL;
    return rp2040_read_reg(device, RP2040_REG_ADC_VALUE_VUSB_LO, (uint8_t*) value, 2);
}

esp_err_t rp2040_read_vusb(RP2040* device, float* value) {
    uint16_t  raw;
    esp_err_t res = rp2040_read_vusb_raw(device, &raw);
    if (res != ESP_OK) return res;
    *value = raw * conversion_factor * 2;  // Connected through 100k/100k divider
    return res;
}

esp_err_t rp2040_read_temp(RP2040* device, uint16_t* value) {
    if ((device->_fw_version < 0x02) || (device->_fw_version == 0xFF)) return ESP_FAIL;
    return rp2040_read_reg(device, RP2040_REG_ADC_VALUE_TEMP_LO, (uint8_t*) value, 2);
}

esp_err_t rp2040_get_charging(RP2040* device, uint8_t* charging) {
    if ((device->_fw_version < 0x02) || (device->_fw_version == 0xFF)) return ESP_FAIL;
    return rp2040_read_reg(device, RP2040_REG_CHARGING_STATE, charging, 1);
}

esp_err_t rp2040_get_usb(RP2040* device, uint8_t* usb) {
    if ((device->_fw_version < 0x01) || (device->_fw_version == 0xFF)) return ESP_FAIL;
    return rp2040_read_reg(device, RP2040_REG_USB, usb, 1);
}

esp_err_t rp2040_get_webusb_mode(RP2040* device, uint8_t* mode) {
    if ((device->_fw_version < 0x02) || (device->_fw_version == 0xFF)) return ESP_FAIL;
    return rp2040_read_reg(device, RP2040_REG_WEBUSB_MODE, mode, 1);
}

esp_err_t rp2040_exit_webusb_mode(RP2040* device) {
    if ((device->_fw_version < 0x0E) || (device->_fw_version == 0xFF)) return ESP_FAIL;
    uint8_t mode = 0;
    return rp2040_write_reg(device, RP2040_REG_WEBUSB_MODE, &mode, 1);
}

esp_err_t rp2040_get_crash_state(RP2040* device, uint8_t* crash_debug) {
    if ((device->_fw_version < 0x06) || (device->_fw_version == 0xFF)) return ESP_FAIL;
    return rp2040_read_reg(device, RP2040_REG_CRASH_DEBUG, crash_debug, 1);
}

esp_err_t rp2040_ir_send(RP2040* device, uint16_t address, uint8_t command) {
    if ((device->_fw_version < 0x06) || (device->_fw_version == 0xFF)) return ESP_FAIL;
    uint8_t buffer[4];
    buffer[0] = address & 0xFF;  // Address low byte
    buffer[1] = address >> 8;    // Address high byte
    buffer[2] = command;         // Command
    buffer[3] = 0x01;            // Trigger
    return rp2040_write_reg(device, RP2040_REG_IR_ADDRESS_LO, buffer, sizeof(buffer));
}

esp_err_t rp2040_get_reset_attempted(RP2040* device, uint8_t* reset_attempted) {
    if ((device->_fw_version < 0x08) || (device->_fw_version == 0xFF)) return ESP_FAIL;
    return rp2040_read_reg(device, RP2040_REG_RESET_ATTEMPTED, reset_attempted, 1);
}

esp_err_t rp2040_set_reset_attempted(RP2040* device, uint8_t reset_attempted) {
    if ((device->_fw_version < 0x08) || (device->_fw_version == 0xFF)) return ESP_FAIL;
    return rp2040_write_reg(device, RP2040_REG_RESET_ATTEMPTED, &reset_attempted, 1);
}

esp_err_t rp2040_set_reset_lock(RP2040* device, uint8_t lock) {
    if ((device->_fw_version < 0x08) || (device->_fw_version == 0xFF)) return ESP_FAIL;
    return rp2040_write_reg(device, RP2040_REG_RESET_LOCK, &lock, 1);
}

esp_err_t rp2040_set_ws2812_mode(RP2040* device, uint8_t mode) {
    if ((device->_fw_version < 0x09) || (device->_fw_version == 0xFF)) return ESP_FAIL;
    return rp2040_write_reg(device, RP2040_REG_WS2812_MODE, &mode, 1);
}

esp_err_t rp2040_set_ws2812_data(RP2040* device, uint8_t position, uint32_t value) {
    if ((device->_fw_version < 0x09) || (device->_fw_version == 0xFF)) return ESP_FAIL;
    if (position >= 10) return ESP_FAIL;
    return rp2040_write_reg(device, RP2040_REG_WS2812_LED0_DATA0 + (position * 4), (uint8_t*) &value, 4);
}

esp_err_t rp2040_set_ws2812_length(RP2040* device, uint8_t length) {
    if ((device->_fw_version < 0x09) || (device->_fw_version == 0xFF)) return ESP_FAIL;
    return rp2040_write_reg(device, RP2040_REG_WS2812_LENGTH, (uint8_t*) &length, 1);
}

esp_err_t rp2040_ws2812_trigger(RP2040* device) {
    if ((device->_fw_version < 0x09) || (device->_fw_version == 0xFF)) return ESP_FAIL;
    uint8_t value = 0;
    return rp2040_write_reg(device, RP2040_REG_WS2812_TRIGGER, (uint8_t*) &value, 1);
}

esp_err_t rp2040_set_msc_control(RP2040* device, uint8_t value) {
    if ((device->_fw_version < 0x0D) || (device->_fw_version == 0xFF)) return ESP_FAIL;
    return rp2040_write_reg(device, RP2040_REG_MSC_CONTROL, &value, 1);
}

esp_err_t rp2040_get_msc_state(RP2040* device, uint8_t* value) {
    if ((device->_fw_version < 0x0D) || (device->_fw_version == 0xFF)) return ESP_FAIL;
    return rp2040_read_reg(device, RP2040_REG_MSC_STATE, value, 1);
}

esp_err_t rp2040_set_msc_block_count(RP2040* device, uint8_t lun, uint32_t value) {
    if ((device->_fw_version < 0x0D) || (device->_fw_version == 0xFF)) return ESP_FAIL;
    if (lun > 1) return ESP_FAIL;
    return rp2040_write_reg(device, (lun == 1) ? RP2040_REG_MSC1_BLOCK_COUNT_LO_A : RP2040_REG_MSC0_BLOCK_COUNT_LO_A, (uint8_t*) &value, 4);
}

esp_err_t rp2040_set_msc_block_size(RP2040* device, uint8_t lun, uint16_t value) {
    if ((device->_fw_version < 0x0D) || (device->_fw_version == 0xFF)) return ESP_FAIL;
    if (lun > 1) return ESP_FAIL;
    return rp2040_write_reg(device, (lun == 1) ? RP2040_REG_MSC1_BLOCK_SIZE_LO : RP2040_REG_MSC0_BLOCK_SIZE_LO, (uint8_t*) &value, 2);
}
