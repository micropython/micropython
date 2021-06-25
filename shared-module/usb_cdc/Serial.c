/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Dan Halbert for Adafruit Industries
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

#include "lib/utils/interrupt_char.h"
#include "shared-module/usb_cdc/Serial.h"
#include "supervisor/shared/tick.h"

#include "tusb.h"

size_t common_hal_usb_cdc_serial_read(usb_cdc_serial_obj_t *self, uint8_t *data, size_t len, int *errcode) {

    const bool wait_forever = self->timeout < 0.0f;
    const bool wait_for_timeout = self->timeout > 0.0f;

    // Read up to len bytes immediately.
    // The number of bytes read will not be larger than what is already in the TinyUSB FIFO.
    uint32_t total_num_read = tud_cdc_n_read(self->idx, data, len);

    if (wait_forever || wait_for_timeout) {
        // Continue filling the buffer past what we already read.
        len -= total_num_read;
        data += total_num_read;

        // Read more if we have time.
        // Use special routine to avoid pulling in uint64-float-compatible math routines.
        uint64_t timeout_ms = float_to_uint64(self->timeout * 1000);  // Junk value if timeout < 0.
        uint64_t start_ticks = supervisor_ticks_ms64();

        uint32_t num_read = 0;
        while (total_num_read < len &&
               (wait_forever || supervisor_ticks_ms64() - start_ticks <= timeout_ms)) {

            // Wait for a bit, and check for ctrl-C.
            RUN_BACKGROUND_TASKS;
            if (mp_hal_is_interrupted()) {
                return 0;
            }

            // Advance buffer pointer and reduce number of bytes that need to be read.
            len -= num_read;
            data += num_read;

            // Try to read another batch of bytes.
            num_read = tud_cdc_n_read(self->idx, data, len);
            total_num_read += num_read;
        }
    }

    return total_num_read;
}

size_t common_hal_usb_cdc_serial_write(usb_cdc_serial_obj_t *self, const uint8_t *data, size_t len, int *errcode) {
    const bool wait_forever = self->write_timeout < 0.0f;
    const bool wait_for_timeout = self->write_timeout > 0.0f;

    // Write as many bytes as possible immediately.
    // The number of bytes written at once will not be larger than what can fit in the TinyUSB FIFO.
    uint32_t total_num_written = tud_cdc_n_write(self->idx, data, len);
    tud_cdc_n_write_flush(self->idx);

    if (wait_forever || wait_for_timeout) {
        // Continue writing the rest of the buffer.
        len -= total_num_written;
        data += total_num_written;

        // Write more if we have time.
        // Use special routine to avoid pulling in uint64-float-compatible math routines.
        uint64_t timeout_ms = float_to_uint64(self->write_timeout * 1000);  // Junk value if write_timeout < 0.
        uint64_t start_ticks = supervisor_ticks_ms64();

        uint32_t num_written = 0;
        while (total_num_written < len &&
               (wait_forever || supervisor_ticks_ms64() - start_ticks <= timeout_ms)) {

            // Wait for a bit, and check for ctrl-C.
            RUN_BACKGROUND_TASKS;
            if (mp_hal_is_interrupted()) {
                return 0;
            }

            // Advance buffer pointer and reduce number of bytes that need to be written.
            len -= num_written;
            data += num_written;

            // Try to write another batch of bytes.
            num_written = tud_cdc_n_write(self->idx, data, len);
            tud_cdc_n_write_flush(self->idx);
            total_num_written += num_written;
        }
    }

    return total_num_written;
}

uint32_t common_hal_usb_cdc_serial_get_in_waiting(usb_cdc_serial_obj_t *self) {
    return tud_cdc_n_available(self->idx);
}

uint32_t common_hal_usb_cdc_serial_get_out_waiting(usb_cdc_serial_obj_t *self) {
    // Return number of FIFO bytes currently occupied.
    return CFG_TUD_CDC_TX_BUFSIZE - tud_cdc_n_write_available(self->idx);
}

void common_hal_usb_cdc_serial_reset_input_buffer(usb_cdc_serial_obj_t *self) {
    tud_cdc_n_read_flush(self->idx);
}

uint32_t common_hal_usb_cdc_serial_reset_output_buffer(usb_cdc_serial_obj_t *self) {
    return tud_cdc_n_write_clear(self->idx);
}

uint32_t common_hal_usb_cdc_serial_flush(usb_cdc_serial_obj_t *self) {
    return tud_cdc_n_write_flush(self->idx);
}

bool common_hal_usb_cdc_serial_get_connected(usb_cdc_serial_obj_t *self) {
    return tud_cdc_n_connected(self->idx);
}

mp_float_t common_hal_usb_cdc_serial_get_timeout(usb_cdc_serial_obj_t *self) {
    return self->timeout;
}

void common_hal_usb_cdc_serial_set_timeout(usb_cdc_serial_obj_t *self, mp_float_t timeout) {
    self->timeout = timeout;
}

mp_float_t common_hal_usb_cdc_serial_get_write_timeout(usb_cdc_serial_obj_t *self) {
    return self->write_timeout;
}

void common_hal_usb_cdc_serial_set_write_timeout(usb_cdc_serial_obj_t *self, mp_float_t write_timeout) {
    self->write_timeout = write_timeout;
}
