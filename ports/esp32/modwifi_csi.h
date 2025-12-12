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
    // 16-bit fields (aligned to 2 bytes)
    uint16_t len;                 // Actual length of CSI data
    uint16_t sig_len;             // Signal length
    // 8-bit fields grouped together
    int8_t rssi;                  // RSSI value
    uint8_t rate;                 // Data rate
    int8_t noise_floor;           // Noise floor
    uint8_t channel;              // Primary channel
    uint8_t ampdu_cnt;            // AMPDU count (8 bits in ESP-IDF rx_ctrl)
    uint8_t rx_state;             // RX state (8 bits in ESP-IDF rx_ctrl: 0=success, others=error)
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
// Unified configuration for both legacy (ESP32/S2/S3/C3) and modern (ESP32-C5/C6) APIs
// Uses bitfields to reduce memory footprint (similar to ESP-IDF structure)
typedef struct {
    // Common parameters (used by both APIs)
    // Note: lltf_en=false for legacy API (HT-LTF only), but modern API (C5/C6) always uses acquire_csi_legacy=1 (hardcoded)
    uint16_t lltf_en : 1;         // Legacy API: Enable L-LTF (default: false, HT-LTF only) / Modern API: acquire_csi_legacy (always 1, fallback for legacy routers)
    uint16_t htltf_en : 1;        // Legacy API: Enable HT-LTF / Modern API: acquire_csi_ht20 (PRIMARY - best SNR)
    uint16_t stbc_htltf2_en : 1;  // Legacy API: Enable STBC HT-LTF2 / Modern API: acquire_csi_he_stbc (default: false, disabled)
    uint16_t dump_ack_en : 1;     // Enable capture of 802.11 ACK frames (default: false, adds noise, not useful)

    // Legacy API-only parameters (ESP32, ESP32-S2, ESP32-S3, ESP32-C3)
    uint16_t ltf_merge_en : 1;    // Merge L-LTF and HT-LTF data by averaging (default: false, no merge, HT-LTF only)
    uint16_t channel_filter_en : 1; // Channel filter to smooth adjacent subcarriers (default: false, raw subcarriers)
    uint16_t manu_scale : 1;      // Manual vs automatic CSI data scaling (default: false = auto, true = manual)
    uint16_t shift : 4;           // Shift value for manual scaling (0-15, only used when manu_scale = true)

    // Modern API-only parameters (ESP32-C5, ESP32-C6)
    uint16_t acquire_csi_ht40 : 1;     // Acquire HT-LTF CSI from 802.11n HT40 packets (default: false, less stable)
    uint16_t acquire_csi_su : 1;       // Acquire HE-LTF CSI from 802.11ax HE20 SU packets (default: true, WiFi 6 support)
    uint16_t acquire_csi_mu : 1;       // Acquire HE-LTF CSI from 802.11ax HE20 MU packets (default: false, rarely used)
    uint16_t acquire_csi_dcm : 1;      // Acquire CSI from DCM packets (default: false, long-range feature, not needed)
    uint16_t acquire_csi_beamformed : 1; // Acquire CSI from beamformed packets (default: false, alters channel estimation)
    uint16_t val_scale_cfg : 3;        // CSI value scaling (0-7): 0=auto (default, recommended), 1-7=manual

    uint16_t _reserved : 1;       // Reserved for alignment
    uint32_t buffer_size;         // Buffer size (number of frames) - kept as uint32_t for large values
} csi_config_t;

// CSI module state
typedef struct {
    ringbuf_t ringbuffer;      // Circular buffer
    csi_config_t config;        // Current configuration
    volatile uint32_t dropped;   // Counter for dropped frames (modified from ISR)
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
