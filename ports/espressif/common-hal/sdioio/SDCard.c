// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Jacob Rigby
//
// SPDX-License-Identifier: MIT

#include <stdbool.h>
#include "esp_err.h"

#include "driver/sdmmc_host.h"
#ifndef CONFIG_SOC_SDMMC_USE_GPIO_MATRIX
#include "soc/sdmmc_pins.h"
#endif
#include "ports/espressif/esp-idf/components/sdmmc/include/sdmmc_cmd.h"

#include "extmod/vfs.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/util.h"
#include "shared-bindings/sdioio/SDCard.h"
#include "py/mperrno.h"
#include "py/runtime.h"

#include "esp_log.h"
static const char *TAG = "SDCard.c";

static bool slot_in_use[2];

static void common_hal_sdioio_sdcard_check_for_deinit(sdioio_sdcard_obj_t *self) {
    if (common_hal_sdioio_sdcard_deinited(self)) {
        raise_deinited_error();
    }
}

static int check_pins(const mcu_pin_obj_t *clock, const mcu_pin_obj_t *command, const uint8_t num_data, const mcu_pin_obj_t **data) {
    #ifdef CONFIG_SOC_SDMMC_USE_GPIO_MATRIX
    // ESP32-S3 and P4 can use any pin for any SDMMC func in either slot
    // Default to SLOT_1 for SD cards
    ESP_LOGI(TAG, "Using chip with CONFIG_SOC_SDMMC_USE_GPIO_MATRIX");
    if (!slot_in_use[1]
        #ifdef CONFIG_ESP_HOSTED_SDIO_HOST_INTERFACE
        && CONFIG_ESP_HOSTED_SDIO_SLOT != 1
        #endif
        ) {
        return SDMMC_HOST_SLOT_1;
    } else if (!slot_in_use[0]
               #ifdef CONFIG_ESP_HOSTED_SDIO_HOST_INTERFACE
               && CONFIG_ESP_HOSTED_SDIO_SLOT != 0
               #endif
               ) {
        return SDMMC_HOST_SLOT_0;
    }
    #else
    if (command->number == SDMMC_SLOT0_IOMUX_PIN_NUM_CMD && clock->number == SDMMC_SLOT0_IOMUX_PIN_NUM_CLK && data[0]->number == SDMMC_SLOT0_IOMUX_PIN_NUM_D0) {
        // Might be slot 0
        if (num_data == 1 || (num_data == 4 && data[1]->number == SDMMC_SLOT0_IOMUX_PIN_NUM_D1 && data[2]->number == SDMMC_SLOT0_IOMUX_PIN_NUM_D2 && data[3]->number == SDMMC_SLOT0_IOMUX_PIN_NUM_D3)) {
            return SDMMC_HOST_SLOT_0;
        }
    } else if (command->number == SDMMC_SLOT1_IOMUX_PIN_NUM_CMD && clock->number == SDMMC_SLOT1_IOMUX_PIN_NUM_CLK && data[0]->number == SDMMC_SLOT1_IOMUX_PIN_NUM_D0) {
        // Might be slot 1
        if (num_data == 1 || (num_data == 4 && data[1]->number == SDMMC_SLOT1_IOMUX_PIN_NUM_D1 && data[2]->number == SDMMC_SLOT1_IOMUX_PIN_NUM_D2 && data[3]->number == SDMMC_SLOT1_IOMUX_PIN_NUM_D3)) {
            return SDMMC_HOST_SLOT_1;
        }
    }
    #endif
    return -1;
}

uint8_t get_slot_index(sdioio_sdcard_obj_t *self) {
    if (self->slot == SDMMC_HOST_SLOT_0) {
        return 0;
    } else {
        return 1;
    }
}

void common_hal_sdioio_sdcard_construct(sdioio_sdcard_obj_t *self,
    const mcu_pin_obj_t *clock, const mcu_pin_obj_t *command,
    uint8_t num_data, const mcu_pin_obj_t **data, uint32_t frequency) {
    if (num_data != 4 && num_data != 1) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("Number of data_pins must be %d or %d, not %d"), 1, 4, num_data);
    }

    self->num_data = num_data;
    int sd_slot = check_pins(clock, command, num_data, data);
    if (sd_slot != SDMMC_HOST_SLOT_0 && sd_slot != SDMMC_HOST_SLOT_1) {
        // Bad pin combo
        raise_ValueError_invalid_pins();
    }

    // max 40Mhz frequency
    mp_arg_validate_int_max(frequency, 40000000, MP_QSTR_frequency);
    ESP_LOGI(TAG, "Using slot %d", sd_slot);
    self->slot = (uint8_t)sd_slot;
    esp_err_t err = ESP_OK;

    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    host.slot = sd_slot;
    host.max_freq_khz = frequency / 1000;

    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    slot_config.width = num_data;
    slot_config.clk = clock->number;
    self->clock = clock->number;
    slot_config.cmd = command->number;
    self->command = command->number;
    slot_config.d0 = data[0]->number;
    self->data[0] = data[0]->number;
    if (num_data == 4) {
        slot_config.d1 = data[1]->number;
        self->data[1] = data[1]->number;
        slot_config.d2 = data[2]->number;
        self->data[2] = data[2]->number;
        slot_config.d3 = data[3]->number;
        self->data[3] = data[3]->number;
    }

    ESP_LOGI(TAG, "slot_config:\nwidth: %d, clk: %d, cmd: %d\nd0: %d, d1: %d, d2: %d, d3: %d",
        slot_config.width, slot_config.clk, slot_config.cmd,
        slot_config.d0, slot_config.d1, slot_config.d2, slot_config.d3);

    if (!slot_in_use[0] && !slot_in_use[1]) {
        err = sdmmc_host_init();
        if (err != ESP_OK) {
            mp_raise_OSError_msg_varg(MP_ERROR_TEXT("SDIO Init Error 0x%02x"), err);
        }
    }

    err = sdmmc_host_init_slot(sd_slot, &slot_config);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "Failed to initialize SDMMC slot: %x", err);
        mp_raise_OSError_msg_varg(MP_ERROR_TEXT("SDIO Init Error 0x%02x"), err);
    }
    // sdmmc_card_t card;
    // self->card = malloc(sizeof(sdmmc_card_t));
    err = sdmmc_card_init(&host, &self->card);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "Failed to initialize SDMMC card: %x", err);
        mp_raise_OSError_msg_varg(MP_ERROR_TEXT("SDIO Init Error 0x%02x"), err);
    }

    common_hal_sdioio_sdcard_check_for_deinit(self);

    slot_in_use[get_slot_index(self)] = true;

    claim_pin(clock);
    claim_pin(command);
    for (size_t i = 0; i < num_data; i++) {
        claim_pin(data[i]);
    }

    ESP_LOGI(TAG, "Initialized SD card with ID %d:%d-%s",
        self->card.cid.mfg_id, self->card.cid.oem_id, self->card.cid.name);

    ESP_LOGI(TAG, "Number of sectors: %d with sector_size: %d",
        self->card.csd.capacity, self->card.csd.sector_size);

    self->frequency = self->card.real_freq_khz * 1000;
    ESP_LOGI(TAG, "Real frequency is %lu", self->frequency);
    self->capacity = self->card.csd.capacity;  // Reported number of sectors
    ESP_LOGI(TAG, "Reported capacity is %lu", self->capacity);

    return;
}

uint32_t common_hal_sdioio_sdcard_get_count(sdioio_sdcard_obj_t *self) {
    return self->capacity;
}

uint32_t common_hal_sdioio_sdcard_get_frequency(sdioio_sdcard_obj_t *self) {
    return self->frequency;
}

uint8_t common_hal_sdioio_sdcard_get_width(sdioio_sdcard_obj_t *self) {
    return self->num_data;
}

static void check_whole_block(mp_buffer_info_t *bufinfo, int sector_size) {
    if (bufinfo->len % sector_size) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("Buffer must be a multiple of %d bytes"), sector_size);
    }
}

mp_negative_errno_t sdioio_sdcard_writeblocks(mp_obj_t self_in, uint8_t *buf,
    uint32_t start_block, uint32_t num_blocks) {
    sdioio_sdcard_obj_t *self = MP_OBJ_TO_PTR(self_in);
    ESP_LOGI(TAG, "in sdioio_sdcard_writeblocks");
    esp_err_t err = sdmmc_write_sectors(&self->card, buf, start_block, num_blocks);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "Failed to write blocks with err 0x%X", err);
        return -MP_EIO;
    }
    return 0;
}

mp_negative_errno_t common_hal_sdioio_sdcard_writeblocks(sdioio_sdcard_obj_t *self, uint32_t start_block, mp_buffer_info_t *bufinfo) {
    common_hal_sdioio_sdcard_check_for_deinit(self);
    check_whole_block(bufinfo, self->card.csd.sector_size);

    uint32_t num_blocks = bufinfo->len / self->card.csd.sector_size;
    return sdioio_sdcard_writeblocks(MP_OBJ_FROM_PTR(self), bufinfo->buf,
        start_block, num_blocks);
}

mp_negative_errno_t sdioio_sdcard_readblocks(mp_obj_t self_in, uint8_t *buf,
    uint32_t start_block, uint32_t num_blocks) {
    sdioio_sdcard_obj_t *self = MP_OBJ_TO_PTR(self_in);
    ESP_LOGI(TAG, "in sdioio_sdcard_readblocks");
    esp_err_t err = sdmmc_read_sectors(&self->card, buf, start_block, num_blocks);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "Failed to read blocks with err 0x%X", err);
        return -MP_EIO;
    }
    return 0;
}

mp_negative_errno_t common_hal_sdioio_sdcard_readblocks(sdioio_sdcard_obj_t *self, uint32_t start_block, mp_buffer_info_t *bufinfo) {
    common_hal_sdioio_sdcard_check_for_deinit(self);
    check_whole_block(bufinfo, self->card.csd.sector_size);

    uint32_t num_blocks = bufinfo->len / self->card.csd.sector_size;
    return sdioio_sdcard_readblocks(MP_OBJ_FROM_PTR(self), bufinfo->buf,
        start_block, num_blocks);
}

// Native function for VFS blockdev layer
bool sdioio_sdcard_ioctl(mp_obj_t self_in, size_t cmd, size_t arg,
    mp_int_t *out_value) {
    sdioio_sdcard_obj_t *self = MP_OBJ_TO_PTR(self_in);
    *out_value = 0;

    switch (cmd) {
        case MP_BLOCKDEV_IOCTL_DEINIT:
        case MP_BLOCKDEV_IOCTL_SYNC:
            // SDIO operations are synchronous, no action needed
            return true;

        case MP_BLOCKDEV_IOCTL_BLOCK_COUNT:
            *out_value = common_hal_sdioio_sdcard_get_count(self);
            return true;

        case MP_BLOCKDEV_IOCTL_BLOCK_SIZE:
            *out_value = 512;  // SD cards use 512-byte sectors
            return true;

        default:
            return false;  // Unsupported command
    }
}

bool common_hal_sdioio_sdcard_configure(sdioio_sdcard_obj_t *self, uint32_t frequency, uint8_t bits) {
    return true;
}

bool common_hal_sdioio_sdcard_deinited(sdioio_sdcard_obj_t *self) {
    return self->command == COMMON_HAL_MCU_NO_PIN;
}

void common_hal_sdioio_sdcard_deinit(sdioio_sdcard_obj_t *self) {
    if (common_hal_sdioio_sdcard_deinited(self)) {
        return;
    }

    sdmmc_host_deinit_slot(self->slot);
    slot_in_use[get_slot_index(self)] = false;


    reset_pin_number(self->command);
    self->command = COMMON_HAL_MCU_NO_PIN;
    reset_pin_number(self->clock);
    self->clock = COMMON_HAL_MCU_NO_PIN;
    for (size_t i = 0; i < self->num_data; i++) {
        reset_pin_number(self->data[i]);
        self->data[i] = COMMON_HAL_MCU_NO_PIN;
    }

    return;
}
