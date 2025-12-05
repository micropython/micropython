/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 Francesco Pace <francesco.pace@gmail.com>
 * Copyright (c) 2025 MicroPython CSI Module Contributors
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

#ifndef MICROPY_INCLUDED_ESP32_MODWIFI_CSI_H
#define MICROPY_INCLUDED_ESP32_MODWIFI_CSI_H

#include "py/obj.h"

#if MICROPY_PY_NETWORK_WLAN_CSI

#include "py/ringbuf.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"

// Maximum CSI data size: ESP32 can provide up to 512 bytes
#define CSI_MAX_DATA_LEN 512

// CSI frame structure - stored in circular buffer
// Uses bitfields to reduce memory footprint (similar to ESP-IDF structure)
// Fields ordered by size to minimize padding (uint32_t, uint16_t, uint8_t/int8_t, bitfields, arrays)
typedef struct {
    // 32-bit fields first (aligned to 4 bytes)
    uint32_t timestamp_us;       // Timestamp in microseconds
    uint32_t local_timestamp;     // Local timestamp
    uint32_t rx_state;            // RX state
    // 16-bit fields (aligned to 2 bytes)
    uint16_t len;                 // Actual length of CSI data
    uint16_t ampdu_cnt;           // AMPDU count
    uint16_t sig_len;             // Signal length
    // 8-bit fields grouped together
    int8_t rssi;                  // RSSI value
    uint8_t rate;                 // Data rate
    int8_t noise_floor;           // Noise floor
    uint8_t channel;              // Primary channel
    // Packed bitfields to reduce size (similar to ESP-IDF rx_ctrl structure)
    // Total: 20 bits (2+5+1+1+1+1+2+1+1+2+2+1) - uses uint8_t to match ESP-IDF layout
    // Note: uint8_t causes compiler to pack into multiple bytes (3 bytes total with 4-bit padding)
    // Could use uint32_t for single-word access, but keeping uint8_t for ESP-IDF compatibility
    uint8_t sig_mode : 2;        // Signal mode (legacy, HT, VHT) - 2 bits
    uint8_t mcs : 5;             // MCS index (0-31) - 5 bits
    uint8_t cwb : 1;             // Channel bandwidth - 1 bit
    uint8_t smoothing : 1;       // Smoothing applied - 1 bit
    uint8_t not_sounding : 1;    // Not sounding frame - 1 bit
    uint8_t aggregation : 1;     // Aggregation - 1 bit
    uint8_t stbc : 2;            // STBC - 2 bits
    uint8_t fec_coding : 1;      // FEC coding - 1 bit
    uint8_t sgi : 1;             // Short GI - 1 bit
    uint8_t secondary_channel : 2; // Secondary channel - 2 bits
    uint8_t ant : 2;             // Antenna - 2 bits
    uint8_t _reserved : 1;       // Reserved for alignment - 1 bit
    // Arrays last (MAC address, then large CSI data array)
    uint8_t mac[6];               // Source MAC address
    int8_t data[CSI_MAX_DATA_LEN]; // CSI data (I/Q values)
} csi_frame_t;

// CSI configuration structure
// Uses bitfields to reduce memory footprint (similar to ESP-IDF structure)
typedef struct {
    // Packed bitfields: 6 booleans (6 bits) + shift (4 bits) + reserved (6 bits) = 16 bits total
    // Uses uint16_t because total is exactly 16 bits - fits perfectly in one machine word
    // This allows compiler to read/write entire config in a single atomic operation
    uint16_t lltf_en : 1;         // Enable Legacy Long Training Field
    uint16_t htltf_en : 1;        // Enable HT Long Training Field
    uint16_t stbc_htltf2_en : 1;  // Enable STBC HT-LTF2
    uint16_t ltf_merge_en : 1;    // Enable LTF merge
    uint16_t channel_filter_en : 1; // Enable channel filter
    uint16_t manu_scale : 1;      // Manual scale
    uint16_t shift : 4;           // Shift value (0-15) - needs 4 bits for range 0-15
    uint16_t _reserved : 6;       // Reserved for future use / alignment to 16-bit boundary
    uint32_t buffer_size;         // Buffer size (number of frames) - kept as uint32_t for large values
} csi_config_t;

// CSI module state
typedef struct {
    ringbuf_t ringbuffer;      // Circular buffer
    csi_config_t config;        // Current configuration
    volatile uint32_t dropped;   // Counter for dropped frames (modified from ISR)
    volatile bool enabled;       // CSI enabled flag (read from ISR)
} csi_state_t;

// Function prototype for callback (used by ESP-IDF)
void wifi_csi_rx_cb(void *ctx, wifi_csi_info_t *info);

// MicroPython WLAN method objects (exposed to network_wlan.c)
extern const mp_obj_fun_builtin_var_t network_wlan_csi_enable_obj;
extern const mp_obj_fun_builtin_fixed_t network_wlan_csi_disable_obj;
extern const mp_obj_fun_builtin_fixed_t network_wlan_csi_read_obj;
extern const mp_obj_fun_builtin_fixed_t network_wlan_csi_dropped_obj;
extern const mp_obj_fun_builtin_fixed_t network_wlan_csi_available_obj;

#endif // MICROPY_PY_NETWORK_WLAN_CSI

#endif // MICROPY_INCLUDED_ESP32_MODWIFI_CSI_H
