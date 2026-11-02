// This file is part of the CircuitPython project: https://circuitpython.org
// SPDX-FileCopyrightText: Copyright (c) 2026 Przemyslaw Patrick Socha
// SPDX-License-Identifier: MIT

#include "shared-bindings/qspibus/QSPIBus.h"

#include "common-hal/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/util.h"

#include "py/gc.h"
#include "py/runtime.h"

#include "driver/gpio.h"
#include "esp_heap_caps.h"
#include "soc/soc_caps.h"
#include <string.h>

#define QSPI_OPCODE_WRITE_CMD (0x02U)
#define QSPI_OPCODE_WRITE_COLOR (0x32U)
#define LCD_CMD_RAMWR (0x2CU)
#define LCD_CMD_RAMWRC (0x3CU)
#define LCD_CMD_DISPOFF (0x28U)
#define LCD_CMD_SLPIN (0x10U)
#define QSPI_DMA_BUFFER_COUNT (2U)
#define QSPI_DMA_BUFFER_SIZE (16U * 1024U)
#define QSPI_COLOR_TIMEOUT_MS (1000U)
static void qspibus_release_dma_buffers(qspibus_qspibus_obj_t *self) {
    for (size_t i = 0; i < QSPI_DMA_BUFFER_COUNT; i++) {
        if (self->dma_buffer[i] != NULL) {
            heap_caps_free(self->dma_buffer[i]);
            self->dma_buffer[i] = NULL;
        }
    }
    self->dma_buffer_size = 0;
    self->active_buffer = 0;
    self->inflight_transfers = 0;
    self->transfer_in_progress = false;
}

static bool qspibus_allocate_dma_buffers(qspibus_qspibus_obj_t *self) {
    const size_t candidates[] = {
        QSPI_DMA_BUFFER_SIZE,
        QSPI_DMA_BUFFER_SIZE / 2,
        QSPI_DMA_BUFFER_SIZE / 4,
    };

    for (size_t c = 0; c < MP_ARRAY_SIZE(candidates); c++) {
        size_t size = candidates[c];
        bool ok = true;
        for (size_t i = 0; i < QSPI_DMA_BUFFER_COUNT; i++) {
            self->dma_buffer[i] = heap_caps_malloc(size, MALLOC_CAP_DMA | MALLOC_CAP_8BIT);
            if (self->dma_buffer[i] == NULL) {
                ok = false;
                break;
            }
        }
        if (ok) {
            self->dma_buffer_size = size;
            self->active_buffer = 0;
            self->inflight_transfers = 0;
            self->transfer_in_progress = false;
            return true;
        }
        qspibus_release_dma_buffers(self);
    }
    return false;
}

// Reset transfer bookkeeping after timeout/error. Drains any stale semaphore
// tokens that late ISR completions may have posted after the timeout expired.
static void qspibus_reset_transfer_state(qspibus_qspibus_obj_t *self) {
    self->inflight_transfers = 0;
    self->transfer_in_progress = false;
    if (self->transfer_done_sem != NULL) {
        while (xSemaphoreTake(self->transfer_done_sem, 0) == pdTRUE) {
        }
    }
}

static bool qspibus_wait_one_transfer_done(qspibus_qspibus_obj_t *self, TickType_t timeout) {
    if (self->inflight_transfers == 0) {
        self->transfer_in_progress = false;
        return true;
    }

    if (xSemaphoreTake(self->transfer_done_sem, timeout) != pdTRUE) {
        return false;
    }
    self->inflight_transfers--;
    self->transfer_in_progress = (self->inflight_transfers > 0);
    return true;
}

static bool qspibus_wait_all_transfers_done(qspibus_qspibus_obj_t *self, TickType_t timeout) {
    while (self->inflight_transfers > 0) {
        if (!qspibus_wait_one_transfer_done(self, timeout)) {
            return false;
        }
    }
    return true;
}

static void qspibus_send_command_bytes(
    qspibus_qspibus_obj_t *self,
    uint8_t command,
    const uint8_t *data,
    size_t len) {

    if (!self->bus_initialized) {
        raise_deinited_error();
    }
    if (self->inflight_transfers >= QSPI_DMA_BUFFER_COUNT) {
        if (!qspibus_wait_one_transfer_done(self, pdMS_TO_TICKS(QSPI_COLOR_TIMEOUT_MS))) {
            qspibus_reset_transfer_state(self);
            mp_raise_OSError_msg(MP_ERROR_TEXT("Operation timed out"));
        }
    }

    uint32_t packed_cmd = ((uint32_t)QSPI_OPCODE_WRITE_CMD << 24) | ((uint32_t)command << 8);
    esp_err_t err = esp_lcd_panel_io_tx_param(self->io_handle, packed_cmd, data, len);
    if (err != ESP_OK) {
        qspibus_reset_transfer_state(self);
        mp_raise_OSError_msg_varg(MP_ERROR_TEXT("%q failure: %d"), MP_QSTR_QSPI, (int)err);
    }
}

static void qspibus_send_color_bytes(
    qspibus_qspibus_obj_t *self,
    uint8_t command,
    const uint8_t *data,
    size_t len) {

    if (!self->bus_initialized) {
        raise_deinited_error();
    }

    if (len == 0) {
        qspibus_send_command_bytes(self, command, NULL, 0);
        return;
    }

    // RAMWR must transition to RAMWRC for continued payload chunks.
    uint8_t chunk_command = command;
    const uint8_t *cursor = data;
    size_t remaining = len;

    // Drain stale semaphore tokens that late ISR completions may have
    // posted after a previous qspibus_reset_transfer_state(). Without
    // this, a stale token could satisfy a future wait, causing the next
    // transfer to skip its real DMA-done wait.
    if (self->inflight_transfers == 0 && self->transfer_done_sem != NULL) {
        while (xSemaphoreTake(self->transfer_done_sem, 0) == pdTRUE) {
        }
    }

    while (remaining > 0) {
        // inflight_transfers is only modified in task context (never from ISR),
        // so no atomic/critical section is needed. The ISR only signals the
        // counting semaphore; all counter bookkeeping happens task-side.
        if (self->inflight_transfers >= QSPI_DMA_BUFFER_COUNT) {
            if (!qspibus_wait_one_transfer_done(self, pdMS_TO_TICKS(QSPI_COLOR_TIMEOUT_MS))) {
                qspibus_reset_transfer_state(self);
                mp_raise_OSError_msg(MP_ERROR_TEXT("Operation timed out"));
            }
        }

        size_t chunk = remaining;
        if (chunk > self->dma_buffer_size) {
            chunk = self->dma_buffer_size;
        }

        uint8_t *buffer = self->dma_buffer[self->active_buffer];
        memcpy(buffer, cursor, chunk);

        uint32_t packed_cmd = ((uint32_t)QSPI_OPCODE_WRITE_COLOR << 24) | ((uint32_t)chunk_command << 8);
        esp_err_t err = esp_lcd_panel_io_tx_color(self->io_handle, packed_cmd, buffer, chunk);
        if (err != ESP_OK) {
            qspibus_reset_transfer_state(self);
            mp_raise_OSError_msg_varg(MP_ERROR_TEXT("%q failure: %d"), MP_QSTR_QSPI, (int)err);
        }

        self->inflight_transfers++;
        self->transfer_in_progress = true;
        self->active_buffer = (self->active_buffer + 1) % QSPI_DMA_BUFFER_COUNT;

        if (chunk_command == LCD_CMD_RAMWR) {
            chunk_command = LCD_CMD_RAMWRC;
        }

        cursor += chunk;
        remaining -= chunk;
    }

    // Let DMA complete asynchronously. The next begin_transaction() will
    // wait for all in-flight transfers, allowing fill_area() computation
    // to overlap with DMA.  The explicit wait is only needed for the
    // Python write_data() API path where callers expect the transfer to
    // be finished on return.
}

static bool qspibus_is_color_payload_command(uint8_t command) {
    return command == LCD_CMD_RAMWR || command == LCD_CMD_RAMWRC;
}

static void qspibus_panel_sleep_best_effort(qspibus_qspibus_obj_t *self) {
    if (!self->bus_initialized || self->io_handle == NULL) {
        return;
    }

    if (!qspibus_wait_all_transfers_done(self, pdMS_TO_TICKS(QSPI_COLOR_TIMEOUT_MS))) {
        qspibus_reset_transfer_state(self);
    }

    // If a command is buffered, flush it first so the panel state machine
    // doesn't get a truncated transaction before sleep.
    if (self->has_pending_command) {
        uint32_t pending = ((uint32_t)QSPI_OPCODE_WRITE_CMD << 24) | ((uint32_t)self->pending_command << 8);
        (void)esp_lcd_panel_io_tx_param(self->io_handle, pending, NULL, 0);
        self->has_pending_command = false;
    }

    uint32_t disp_off = ((uint32_t)QSPI_OPCODE_WRITE_CMD << 24) | ((uint32_t)LCD_CMD_DISPOFF << 8);
    (void)esp_lcd_panel_io_tx_param(self->io_handle, disp_off, NULL, 0);
    vTaskDelay(pdMS_TO_TICKS(20));

    uint32_t sleep_in = ((uint32_t)QSPI_OPCODE_WRITE_CMD << 24) | ((uint32_t)LCD_CMD_SLPIN << 8);
    (void)esp_lcd_panel_io_tx_param(self->io_handle, sleep_in, NULL, 0);
    vTaskDelay(pdMS_TO_TICKS(120));
}

static bool IRAM_ATTR qspibus_on_color_trans_done(
    esp_lcd_panel_io_handle_t io_handle,
    esp_lcd_panel_io_event_data_t *event_data,
    void *user_ctx) {
    (void)io_handle;
    (void)event_data;

    qspibus_qspibus_obj_t *self = (qspibus_qspibus_obj_t *)user_ctx;
    if (self->transfer_done_sem == NULL) {
        return false;
    }
    BaseType_t x_higher_priority_task_woken = pdFALSE;

    xSemaphoreGiveFromISR(self->transfer_done_sem, &x_higher_priority_task_woken);
    return x_higher_priority_task_woken == pdTRUE;
}

void common_hal_qspibus_qspibus_construct(
    qspibus_qspibus_obj_t *self,
    const mcu_pin_obj_t *clock,
    const mcu_pin_obj_t *data0,
    const mcu_pin_obj_t *data1,
    const mcu_pin_obj_t *data2,
    const mcu_pin_obj_t *data3,
    const mcu_pin_obj_t *cs,
    const mcu_pin_obj_t *dcx,
    const mcu_pin_obj_t *reset,
    uint32_t frequency) {

    self->io_handle = NULL;
    self->host_id = SPI2_HOST;
    self->clock_pin = clock->number;
    self->data0_pin = data0->number;
    self->data1_pin = data1->number;
    self->data2_pin = data2->number;
    self->data3_pin = data3->number;
    self->cs_pin = cs->number;
    self->dcx_pin = (dcx != NULL) ? dcx->number : -1;
    self->reset_pin = (reset != NULL) ? reset->number : -1;
    self->frequency = frequency;
    self->bus_initialized = false;
    self->in_transaction = false;
    self->has_pending_command = false;
    self->pending_command = 0;
    self->transfer_in_progress = false;
    self->active_buffer = 0;
    self->inflight_transfers = 0;
    self->dma_buffer_size = 0;
    self->dma_buffer[0] = NULL;
    self->dma_buffer[1] = NULL;
    self->transfer_done_sem = NULL;

    self->transfer_done_sem = xSemaphoreCreateCounting(QSPI_DMA_BUFFER_COUNT, 0);
    if (self->transfer_done_sem == NULL) {
        mp_raise_msg(&mp_type_MemoryError, MP_ERROR_TEXT("ESP-IDF memory allocation failed"));
    }

    if (!qspibus_allocate_dma_buffers(self)) {
        common_hal_qspibus_qspibus_deinit(self);
        mp_raise_msg(&mp_type_MemoryError, MP_ERROR_TEXT("Could not allocate DMA capable buffer"));
    }

    const spi_bus_config_t bus_config = {
        .sclk_io_num = self->clock_pin,
        .mosi_io_num = self->data0_pin,
        .miso_io_num = self->data1_pin,
        .quadwp_io_num = self->data2_pin,
        .quadhd_io_num = self->data3_pin,
        .max_transfer_sz = self->dma_buffer_size,
        .flags = SPICOMMON_BUSFLAG_MASTER | SPICOMMON_BUSFLAG_GPIO_PINS,
    };

    esp_err_t err = spi_bus_initialize(self->host_id, &bus_config, SPI_DMA_CH_AUTO);
    if (err != ESP_OK) {
        common_hal_qspibus_qspibus_deinit(self);
        mp_raise_ValueError_varg(MP_ERROR_TEXT("%q in use"), MP_QSTR_SPI);
    }

    // Mark bus as initialized so deinit knows to call spi_bus_free().
    self->bus_initialized = true;

    const esp_lcd_panel_io_spi_config_t io_config = {
        .cs_gpio_num = self->cs_pin,
        .dc_gpio_num = -1,
        .spi_mode = 0,
        .pclk_hz = self->frequency,
        .trans_queue_depth = QSPI_DMA_BUFFER_COUNT,
        .on_color_trans_done = qspibus_on_color_trans_done,
        .user_ctx = self,
        .lcd_cmd_bits = 32,
        .lcd_param_bits = 8,
        .flags = {
            .quad_mode = 1,
        },
    };

    err = esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)self->host_id, &io_config, &self->io_handle);
    if (err != ESP_OK) {
        common_hal_qspibus_qspibus_deinit(self);
        mp_raise_OSError_msg_varg(MP_ERROR_TEXT("%q failure: %d"), MP_QSTR_QSPI, (int)err);
    }

    // Mark the bus pins never-reset so they stay attached to the SPI peripheral
    // across reset_all_pins() at VM end; the display (e.g. board.DISPLAY) outlives
    // the VM. deinit() clears this again via reset_pin_number().
    claim_pin(clock);
    common_hal_never_reset_pin(clock);
    claim_pin(data0);
    common_hal_never_reset_pin(data0);
    claim_pin(data1);
    common_hal_never_reset_pin(data1);
    claim_pin(data2);
    common_hal_never_reset_pin(data2);
    claim_pin(data3);
    common_hal_never_reset_pin(data3);
    claim_pin(cs);
    common_hal_never_reset_pin(cs);
    if (dcx != NULL) {
        claim_pin(dcx);
        common_hal_never_reset_pin(dcx);
        gpio_set_direction((gpio_num_t)self->dcx_pin, GPIO_MODE_OUTPUT);
        gpio_set_level((gpio_num_t)self->dcx_pin, 1);
    }

    if (reset != NULL) {
        claim_pin(reset);
        common_hal_never_reset_pin(reset);

        gpio_set_direction((gpio_num_t)self->reset_pin, GPIO_MODE_OUTPUT);
        gpio_set_level((gpio_num_t)self->reset_pin, 0);
        vTaskDelay(pdMS_TO_TICKS(10));
        gpio_set_level((gpio_num_t)self->reset_pin, 1);
        vTaskDelay(pdMS_TO_TICKS(120));
    }
}

void common_hal_qspibus_qspibus_deinit(qspibus_qspibus_obj_t *self) {
    if (self->bus_initialized) {
        qspibus_panel_sleep_best_effort(self);
        self->in_transaction = false;

        if (self->io_handle != NULL) {
            esp_lcd_panel_io_del(self->io_handle);
            self->io_handle = NULL;
        }

        spi_bus_free(self->host_id);
        self->bus_initialized = false;
    }

    if (self->transfer_done_sem != NULL) {
        // Set NULL before delete so late ISR callbacks (if any) see NULL and skip.
        SemaphoreHandle_t sem = self->transfer_done_sem;
        self->transfer_done_sem = NULL;
        vSemaphoreDelete(sem);
    }

    qspibus_release_dma_buffers(self);

    reset_pin_number(self->clock_pin);
    reset_pin_number(self->data0_pin);
    reset_pin_number(self->data1_pin);
    reset_pin_number(self->data2_pin);
    reset_pin_number(self->data3_pin);
    reset_pin_number(self->cs_pin);
    if (self->dcx_pin >= 0) {
        reset_pin_number(self->dcx_pin);
    }
    if (self->reset_pin >= 0) {
        reset_pin_number(self->reset_pin);
    }

    self->bus_initialized = false;
    self->in_transaction = false;
    self->has_pending_command = false;
    self->pending_command = 0;
    self->transfer_in_progress = false;
    self->inflight_transfers = 0;
}

bool common_hal_qspibus_qspibus_deinited(qspibus_qspibus_obj_t *self) {
    return !self->bus_initialized;
}

void common_hal_qspibus_qspibus_write_command(
    qspibus_qspibus_obj_t *self,
    uint8_t command) {
    if (!self->bus_initialized) {
        raise_deinited_error();
    }
    if (self->in_transaction) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Internal error"));
    }

    // If caller stages command-only operations repeatedly, flush the previous
    // pending command as no-data before replacing it.
    if (self->has_pending_command) {
        qspibus_send_command_bytes(self, self->pending_command, NULL, 0);
    }

    self->pending_command = command;
    self->has_pending_command = true;
}

void common_hal_qspibus_qspibus_write_data(
    qspibus_qspibus_obj_t *self,
    const uint8_t *data,
    size_t len) {
    if (!self->bus_initialized) {
        raise_deinited_error();
    }
    if (self->in_transaction) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Internal error"));
    }
    if (len == 0) {
        if (self->has_pending_command) {
            qspibus_send_command_bytes(self, self->pending_command, NULL, 0);
            self->has_pending_command = false;
        }
        return;
    }
    if (!self->has_pending_command) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Internal error"));
    }

    if (qspibus_is_color_payload_command(self->pending_command)) {
        qspibus_send_color_bytes(self, self->pending_command, data, len);
        // Python API: wait for DMA to finish so callers see the transfer as
        // complete on return. The internal displayio path skips this wait
        // to allow fill_area/DMA overlap.
        if (!qspibus_wait_all_transfers_done(self, pdMS_TO_TICKS(QSPI_COLOR_TIMEOUT_MS))) {
            qspibus_reset_transfer_state(self);
            mp_raise_OSError_msg(MP_ERROR_TEXT("Operation timed out"));
        }
    } else {
        qspibus_send_command_bytes(self, self->pending_command, data, len);
    }
    self->has_pending_command = false;
}

bool common_hal_qspibus_qspibus_reset(mp_obj_t obj) {
    qspibus_qspibus_obj_t *self = MP_OBJ_TO_PTR(obj);
    if (!self->bus_initialized || self->reset_pin < 0) {
        return false;
    }

    gpio_set_level((gpio_num_t)self->reset_pin, 0);
    vTaskDelay(pdMS_TO_TICKS(10));
    gpio_set_level((gpio_num_t)self->reset_pin, 1);
    vTaskDelay(pdMS_TO_TICKS(120));
    return true;
}

bool common_hal_qspibus_qspibus_bus_free(mp_obj_t obj) {
    qspibus_qspibus_obj_t *self = MP_OBJ_TO_PTR(obj);
    return self->bus_initialized && !self->in_transaction && !self->transfer_in_progress && !self->has_pending_command;
}

bool common_hal_qspibus_qspibus_begin_transaction(mp_obj_t obj) {
    qspibus_qspibus_obj_t *self = MP_OBJ_TO_PTR(obj);
    if (!self->bus_initialized || self->in_transaction || self->has_pending_command) {
        return false;
    }
    // Wait for any in-flight DMA to complete before starting a new
    // transaction. This replaces the old non-blocking bus_free() check
    // and enables fill_area()/DMA overlap: the CPU fills the next
    // subrectangle while the previous DMA runs, and this wait only
    // blocks when we actually need the bus for the next send.
    if (self->transfer_in_progress) {
        if (!qspibus_wait_all_transfers_done(self, pdMS_TO_TICKS(QSPI_COLOR_TIMEOUT_MS))) {
            qspibus_reset_transfer_state(self);
            return false;
        }
    }
    self->in_transaction = true;
    return true;
}

void common_hal_qspibus_qspibus_send(
    mp_obj_t obj,
    display_byte_type_t data_type,
    display_chip_select_behavior_t chip_select,
    const uint8_t *data,
    uint32_t data_length) {
    qspibus_qspibus_obj_t *self = MP_OBJ_TO_PTR(obj);
    (void)chip_select;
    if (!self->bus_initialized) {
        raise_deinited_error();
    }
    if (!self->in_transaction) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Internal error"));
    }

    if (data_type == DISPLAY_COMMAND) {
        for (uint32_t i = 0; i < data_length; i++) {
            if (self->has_pending_command) {
                qspibus_send_command_bytes(self, self->pending_command, NULL, 0);
            }
            self->pending_command = data[i];
            self->has_pending_command = true;
        }
        return;
    }

    if (!self->has_pending_command) {
        if (data_length == 0) {
            // Zero-length data write after a no-data command is benign.
            return;
        }
        mp_raise_RuntimeError(MP_ERROR_TEXT("Internal error"));
    }

    if (data_length == 0) {
        qspibus_send_command_bytes(self, self->pending_command, NULL, 0);
        self->has_pending_command = false;
        return;
    }

    if (qspibus_is_color_payload_command(self->pending_command)) {
        qspibus_send_color_bytes(self, self->pending_command, data, data_length);
    } else {
        qspibus_send_command_bytes(self, self->pending_command, data, data_length);
    }
    self->has_pending_command = false;
}

void common_hal_qspibus_qspibus_end_transaction(mp_obj_t obj) {
    qspibus_qspibus_obj_t *self = MP_OBJ_TO_PTR(obj);
    if (!self->bus_initialized) {
        return;
    }
    if (self->has_pending_command) {
        qspibus_send_command_bytes(self, self->pending_command, NULL, 0);
        self->has_pending_command = false;
    }
    self->in_transaction = false;
}

void common_hal_qspibus_qspibus_flush(mp_obj_t obj) {
    qspibus_qspibus_obj_t *self = MP_OBJ_TO_PTR(obj);
    if (!self->bus_initialized) {
        return;
    }
    if (!qspibus_wait_all_transfers_done(self, pdMS_TO_TICKS(QSPI_COLOR_TIMEOUT_MS))) {
        qspibus_reset_transfer_state(self);
    }
}

void common_hal_qspibus_qspibus_collect_ptrs(mp_obj_t obj) {
    (void)obj;
}
