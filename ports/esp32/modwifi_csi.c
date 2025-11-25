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

#include "py/mphal.h"
#include "py/objarray.h"
#include "py/runtime.h"

#if MICROPY_PY_NETWORK_WLAN_CSI

#include "esp_log.h"
#include "esp_timer.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "modwifi_csi.h"
#include <string.h>

// Define STATIC if not already defined
#ifndef STATIC
#define STATIC static
#endif

static const char *TAG = "wifi_csi";

// Global CSI state
csi_state_t g_csi_state = {
    .buffer =
    {
        .frames = NULL,
        .head = 0,
        .tail = 0,
        .size = 0,
        .dropped = 0,
        .initialized = false,
    },
    .config =
    {
        .lltf_en = true,
        .htltf_en = true,
        .stbc_htltf2_en = true,
        .ltf_merge_en = true,
        .channel_filter_en = true,
        .manu_scale = false,
        .shift = 0,
        .buffer_size = CSI_DEFAULT_BUFFER_SIZE,
    },
    .enabled = false,
};

// ============================================================================
// Circular Buffer Implementation (Lock-Free)
// ============================================================================

static bool csi_buffer_init(csi_buffer_t *buf, uint32_t size) {
    if (buf->initialized) {
        // Already initialized, free old buffer
        if (buf->frames) {
            free(buf->frames);
        }
    }

    buf->frames = (csi_frame_t *)malloc(sizeof(csi_frame_t) * size);
    if (buf->frames == NULL) {
        ESP_LOGE(TAG, "Failed to allocate CSI buffer");
        return false;
    }

    buf->size = size;
    buf->head = 0;
    buf->tail = 0;
    buf->dropped = 0;
    buf->initialized = true;

    ESP_LOGI(TAG, "CSI buffer initialized: %lu frames", (unsigned long)size);
    return true;
}

static void csi_buffer_deinit(csi_buffer_t *buf) {
    if (buf->initialized && buf->frames) {
        free(buf->frames);
        buf->frames = NULL;
        buf->initialized = false;
        buf->dropped = 0; // Reset dropped counter for clean state
    }
}

static bool csi_buffer_is_empty(const csi_buffer_t *buf) {
    return buf->head == buf->tail;
}

// Called from ISR context - must be fast and non-blocking
static bool csi_buffer_write(csi_buffer_t *buf, const csi_frame_t *frame) {
    if (!buf->initialized) {
        return false;
    }

    uint32_t next_head = (buf->head + 1) % buf->size;

    if (next_head == buf->tail) {
        // Buffer full, drop frame
        buf->dropped++;
        return false;
    }

    // Copy frame data
    memcpy(&buf->frames[buf->head], frame, sizeof(csi_frame_t));

    // Update head (atomic on ESP32)
    buf->head = next_head;

    return true;
}

// Called from Python context
static bool csi_buffer_read(csi_buffer_t *buf, csi_frame_t *frame) {
    if (!buf->initialized || csi_buffer_is_empty(buf)) {
        return false;
    }

    // Copy frame data
    memcpy(frame, &buf->frames[buf->tail], sizeof(csi_frame_t));

    // Update tail
    buf->tail = (buf->tail + 1) % buf->size;

    return true;
}

// ============================================================================
// CSI Callback (ISR Context)
// ============================================================================

void IRAM_ATTR wifi_csi_rx_cb(void *ctx, wifi_csi_info_t *info) {
    if (!g_csi_state.enabled || !g_csi_state.buffer.initialized) {
        return;
    }

    csi_frame_t frame;
    memset(&frame, 0, sizeof(csi_frame_t));

    // Extract metadata
    // Note: ESP32-C5 and ESP32-C6 have a different rx_ctrl structure with fewer
    // fields
    frame.rssi = info->rx_ctrl.rssi;
    frame.rate = info->rx_ctrl.rate;

    #if CONFIG_IDF_TARGET_ESP32C6 || CONFIG_IDF_TARGET_ESP32C5
    // ESP32-C5/C6: Many fields are not available in esp_wifi_rxctrl_t
    // Set them to default values
    frame.sig_mode = 0;
    frame.mcs = 0;
    frame.cwb = 0;
    frame.smoothing = 0;
    frame.not_sounding = 0;
    frame.aggregation = 0;
    frame.stbc = 0;
    frame.fec_coding = 0;
    frame.sgi = 0;
    frame.ampdu_cnt = 0;
    frame.secondary_channel = 0;
    frame.ant = 0;
    #else
    // ESP32, ESP32-S2, ESP32-S3, ESP32-C3: Full rx_ctrl structure available
    frame.sig_mode = info->rx_ctrl.sig_mode;
    frame.mcs = info->rx_ctrl.mcs;
    frame.cwb = info->rx_ctrl.cwb;
    frame.smoothing = info->rx_ctrl.smoothing;
    frame.not_sounding = info->rx_ctrl.not_sounding;
    frame.aggregation = info->rx_ctrl.aggregation;
    frame.stbc = info->rx_ctrl.stbc;
    frame.fec_coding = info->rx_ctrl.fec_coding;
    frame.sgi = info->rx_ctrl.sgi;
    frame.ampdu_cnt = info->rx_ctrl.ampdu_cnt;
    frame.secondary_channel = info->rx_ctrl.secondary_channel;
    frame.ant = info->rx_ctrl.ant;
    #endif

    // Common fields available on all targets
    frame.noise_floor = info->rx_ctrl.noise_floor;
    frame.channel = info->rx_ctrl.channel;
    frame.local_timestamp = info->rx_ctrl.timestamp;
    frame.sig_len = info->rx_ctrl.sig_len;
    frame.rx_state = info->rx_ctrl.rx_state;

    // Copy MAC address
    memcpy(frame.mac, info->mac, 6);

    // Get timestamp
    frame.timestamp_us = (uint32_t)esp_timer_get_time();

    // Copy CSI data (info->buf is int8_t*, info->len is in bytes)
    frame.len = info->len > CSI_MAX_DATA_LEN ? CSI_MAX_DATA_LEN : info->len;
    if (info->buf && frame.len > 0) {
        memcpy(frame.data, info->buf, frame.len);
    }

    // Write to circular buffer
    csi_buffer_write(&g_csi_state.buffer, &frame);
}

// ============================================================================
// CSI Control Functions
// ============================================================================

void wifi_csi_init(void) {
    // Initialize with default configuration
    if (!g_csi_state.buffer.initialized) {
        csi_buffer_init(&g_csi_state.buffer, g_csi_state.config.buffer_size);
    }
}

void wifi_csi_deinit(void) {
    if (g_csi_state.enabled) {
        wifi_csi_disable();
    }
    csi_buffer_deinit(&g_csi_state.buffer);
}

esp_err_t wifi_csi_enable(void) {
    if (g_csi_state.enabled) {
        ESP_LOGI(TAG, "Already enabled");
        return ESP_OK;
    }

    esp_err_t ret;

    ESP_LOGI(TAG, "Starting CSI enable sequence...");

    // Ensure buffer is initialized
    if (!g_csi_state.buffer.initialized) {
        ESP_LOGI(TAG, "Initializing buffer (size=%lu)...",
            (unsigned long)g_csi_state.config.buffer_size);
        if (!csi_buffer_init(&g_csi_state.buffer, g_csi_state.config.buffer_size)) {
            ESP_LOGE(TAG, "Failed to initialize buffer");
            return ESP_ERR_NO_MEM;
        }
        ESP_LOGI(TAG, "Buffer initialized OK");
    }

    // Configure WiFi protocol mode
    #if CONFIG_IDF_TARGET_ESP32C6 || CONFIG_IDF_TARGET_ESP32C5
    // ESP32-C5/C6: Enable WiFi 6 (802.11ax) for improved performance and CSI
    // capture
    ESP_LOGI(TAG, "Setting WiFi protocol to 802.11b/g/n/ax...");
    ret = esp_wifi_set_protocol(WIFI_IF_STA,
        WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G |
        WIFI_PROTOCOL_11N | WIFI_PROTOCOL_11AX);
    ESP_LOGD(TAG, "esp_wifi_set_protocol returned: 0x%x", ret);
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "Failed to set WiFi protocol: 0x%x", ret);
    } else {
        ESP_LOGI(TAG, "WiFi protocol set to 802.11b/g/n/ax (WiFi 6 enabled)");
    }
    #else
    // ESP32, ESP32-S2, ESP32-S3, ESP32-C3: WiFi 4 only (802.11b/g/n)
    ret = esp_wifi_set_protocol(
        WIFI_IF_STA, WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N);
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "Failed to set WiFi protocol: 0x%x", ret);
    } else {
        ESP_LOGI(TAG, "WiFi protocol set to 802.11b/g/n");
    }
    #endif

    // Configure WiFi bandwidth (HT20 for stability)
    ESP_LOGI(TAG, "Setting WiFi bandwidth to HT20...");
    ret = esp_wifi_set_bandwidth(WIFI_IF_STA, WIFI_BW_HT20);
    ESP_LOGD(TAG, "esp_wifi_set_bandwidth returned: 0x%x", ret);
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "Failed to set bandwidth: 0x%x", ret);
    } else {
        ESP_LOGI(TAG, "WiFi bandwidth set to HT20 (20MHz)");
    }

    // IMPORTANT: For ESP32-C6, promiscuous mode MUST be called BEFORE configuring
    // CSI This initializes internal WiFi structures required for CSI, even when
    // set to false
    ESP_LOGI(TAG, "Initializing promiscuous mode (false)...");
    ret = esp_wifi_set_promiscuous(false);
    ESP_LOGD(TAG, "esp_wifi_set_promiscuous returned: 0x%x", ret);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set promiscuous mode: 0x%x", ret);
        return ret;
    }
    ESP_LOGI(TAG, "Promiscuous mode: disabled (CSI from connected AP only)");

    #if CONFIG_IDF_TARGET_ESP32C6
    // ESP32-C6: Full WiFi 6 config with HE STBC support
    // Reference: https://github.com/espressif/esp-idf/issues/14271
    // CRITICAL: Must specify which CSI types to acquire, otherwise callback is
    // never invoked!
    wifi_csi_config_t csi_config = {
        .enable = 1, // Master enable for CSI acquisition (REQUIRED)

        .acquire_csi_legacy =
            1, // Acquire L-LTF CSI from legacy 802.11a/g packets
               // CRITICAL: Required for CSI callback to be invoked!
               // Captures channel state from non-HT packets

        .acquire_csi_ht20 = 1, // Acquire HT-LTF CSI from 802.11n HT20 packets
                               // CRITICAL: Required for CSI from HT packets!
                               // Provides improved channel estimation for MIMO

        .acquire_csi_ht40 =
            0, // Acquire HT-LTF CSI from 802.11n HT40 packets (40MHz bandwidth)
               // Disabled (using HT20 for stability)
               // Enable if router uses HT40 and you need 128 subcarriers

        .acquire_csi_su =
            1, // Acquire HE-LTF CSI from 802.11ax HE20 SU (Single-User) packets
               // Enabled for WiFi 6 support (if router supports 802.11ax)

        .acquire_csi_mu =
            0, // Acquire HE-LTF CSI from 802.11ax HE20 MU (Multi-User) packets
               // Disabled (not using WiFi 6 MU-MIMO)

        .acquire_csi_dcm = 0, // Acquire CSI from DCM (Dual Carrier Modulation)
                              // packets DCM is a WiFi 6 feature for long-range
                              // transmission Disabled (not commonly used)

        .acquire_csi_beamformed =
            0, // Acquire CSI from beamformed packets
               // Beamforming directs signal toward receiver
               // Disabled (not needed for general CSI capture)

        .acquire_csi_he_stbc = 0, // Acquire CSI from 802.11ax HE STBC packets
                                  // STBC improves reliability using multiple
                                  // antennas Disabled (not commonly used)

        .val_scale_cfg = 0, // CSI value scaling configuration (0-8)
                            // 0 = automatic scaling (recommended)
                            // 1-8 = manual scaling with shift bits
                            // Controls normalization of CSI amplitude values

        .dump_ack_en = 0, // Enable capture of 802.11 ACK frames
                          // Disabled to reduce overhead (ACK frames not needed)
    };
    #elif CONFIG_IDF_TARGET_ESP32C5
    // ESP32-C5: WiFi 6 config without HE STBC field
    // Note: ESP32-C5 has a slightly different wifi_csi_acquire_config_t structure
    wifi_csi_config_t csi_config = {
        .enable = 1, // Master enable for CSI acquisition (REQUIRED)

        .acquire_csi_legacy =
            1, // Acquire L-LTF CSI from legacy 802.11a/g packets
               // CRITICAL: Required for CSI callback to be invoked!

        .acquire_csi_ht20 = 1, // Acquire HT-LTF CSI from 802.11n HT20 packets
                               // CRITICAL: Required for CSI from HT packets!

        .acquire_csi_ht40 =
            0, // Acquire HT-LTF CSI from 802.11n HT40 packets (40MHz bandwidth)
               // Disabled (using HT20 for stability)

        .acquire_csi_su = 1, // Acquire HE-LTF CSI from 802.11ax HE20 SU
                             // (Single-User) packets Enabled for WiFi 6 support

        .acquire_csi_mu =
            0, // Acquire HE-LTF CSI from 802.11ax HE20 MU (Multi-User) packets
               // Disabled (not using WiFi 6 MU-MIMO)

        .acquire_csi_dcm = 0, // Acquire CSI from DCM (Dual Carrier Modulation)
                              // packets Disabled (not commonly used)

        .acquire_csi_beamformed =
            0, // Acquire CSI from beamformed packets
               // Disabled (not needed for general CSI capture)

        // Note: acquire_csi_he_stbc field not available on ESP32-C5

        .val_scale_cfg = 0, // CSI value scaling configuration (0-8)
                            // 0 = automatic scaling (recommended)

        .dump_ack_en = 0, // Enable capture of 802.11 ACK frames
                          // Disabled to reduce overhead
    };
    #else
    // ESP32, ESP32-S2, ESP32-S3, ESP32-C3 use legacy CSI API with LTF fields
    wifi_csi_config_t csi_config = {
        .lltf_en =
            g_csi_state.config
            .lltf_en,   // Enable Legacy Long Training Field (L-LTF) CSI capture
                        // L-LTF is present in all 802.11a/g packets
                        // Provides base channel estimation (64 subcarriers)

        .htltf_en =
            g_csi_state.config
            .htltf_en,   // Enable HT Long Training Field (HT-LTF) CSI capture
                         // HT-LTF is present in 802.11n (HT) packets
                         // Provides improved channel estimation for MIMO

        .stbc_htltf2_en =
            g_csi_state.config
            .stbc_htltf2_en,   // Enable Space-Time Block Code HT-LTF2 capture
                               // STBC uses 2 antennas to improve reliability
                               // Captures second HT-LTF when STBC is active

        .ltf_merge_en =
            g_csi_state.config
            .ltf_merge_en,   // Merge L-LTF and HT-LTF data by averaging
                             // true: Average L-LTF and HT-LTF for HT packets
                             // (more stable) false: Use only HT-LTF for HT
                             // packets (more precise)

        .channel_filter_en =
            g_csi_state.config
            .channel_filter_en,   // Channel filter to smooth adjacent
                                  // subcarriers true: Filter/smooth adjacent
                                  // subcarriers (52 useful) false: Keep
                                  // subcarrier independence (64 total)

        .manu_scale =
            g_csi_state.config.manu_scale, // Manual vs automatic CSI data scaling
                                           // false: Auto-scaling (recommended,
                                           // adapts dynamically) true: Manual
                                           // scaling (requires .shift parameter)

        .shift =
            g_csi_state.config.shift, // Shift value for manual scaling (0-15)
                                      // Only used when manu_scale = true

        .dump_ack_en = false, // Enable capture of 802.11 ACK frames
                              // Disabled to reduce overhead
    };
    #endif

    ESP_LOGI(TAG, "Setting CSI config...");
    ret = esp_wifi_set_csi_config(&csi_config);
    ESP_LOGD(TAG, "esp_wifi_set_csi_config returned: 0x%x", ret);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set CSI config: 0x%x", ret);
        return ret;
    }

    ESP_LOGI(TAG, "Registering CSI callback...");
    ret = esp_wifi_set_csi_rx_cb(wifi_csi_rx_cb, NULL);
    ESP_LOGD(TAG, "esp_wifi_set_csi_rx_cb returned: 0x%x", ret);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register callback: 0x%x", ret);
        return ret;
    }

    // Enable CSI LAST
    ESP_LOGI(TAG, "Calling esp_wifi_set_csi(true)...");
    ret = esp_wifi_set_csi(true);
    ESP_LOGD(TAG, "esp_wifi_set_csi returned: 0x%x (ESP_OK=0x%x)", ret, ESP_OK);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to enable CSI (error code: 0x%x)", ret);
        ESP_LOGE(TAG, "Common causes:");
        ESP_LOGE(TAG, "  - WiFi not connected");
        ESP_LOGE(TAG, "  - CSI config invalid");
        ESP_LOGE(TAG, "  - Hardware limitation");
        return ret;
    }

    g_csi_state.enabled = true;

    #if CONFIG_IDF_TARGET_ESP32C6
    ESP_LOGI(TAG, "CSI enabled successfully (ESP32-C6 mode - WiFi 6 support)");
    #elif CONFIG_IDF_TARGET_ESP32C5
    ESP_LOGI(TAG, "CSI enabled successfully (ESP32-C5 mode - WiFi 6 support)");
    #elif CONFIG_IDF_TARGET_ESP32S3
    ESP_LOGI(TAG, "CSI enabled successfully (ESP32-S3 mode)");
    #elif CONFIG_IDF_TARGET_ESP32S2
    ESP_LOGI(TAG, "CSI enabled successfully (ESP32-S2 mode)");
    #elif CONFIG_IDF_TARGET_ESP32C3
    ESP_LOGI(TAG, "CSI enabled successfully (ESP32-C3 mode)");
    #else
    ESP_LOGI(TAG, "CSI enabled successfully (ESP32 mode)");
    #endif

    return ESP_OK;
}

esp_err_t wifi_csi_disable(void) {
    if (!g_csi_state.enabled) {
        return ESP_OK;
    }

    esp_err_t ret = esp_wifi_set_csi(false);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to disable CSI: %d", ret);
        return ret;
    }

    // Clean up: deallocate buffer and reset config to defaults
    csi_buffer_deinit(&g_csi_state.buffer);

    // Reset config to defaults
    g_csi_state.config.lltf_en = true;
    g_csi_state.config.htltf_en = true;
    g_csi_state.config.stbc_htltf2_en = true;
    g_csi_state.config.ltf_merge_en = true;
    g_csi_state.config.channel_filter_en = true;
    g_csi_state.config.manu_scale = false;
    g_csi_state.config.shift = 0;
    g_csi_state.config.buffer_size = CSI_DEFAULT_BUFFER_SIZE;

    g_csi_state.enabled = false;
    ESP_LOGI(TAG, "CSI disabled and state cleaned");
    return ESP_OK;
}

bool wifi_csi_read_frame(csi_frame_t *frame) {
    return csi_buffer_read(&g_csi_state.buffer, frame);
}

// ============================================================================
// MicroPython Bindings
// ============================================================================

// wlan.csi_enable(**kwargs)
STATIC mp_obj_t network_wlan_csi_enable(size_t n_args, const mp_obj_t *args,
    mp_map_t *kw_args) {
    (void)args[0]; // WLAN object not used, CSI is global state

    enum {
        ARG_lltf_en,
        ARG_htltf_en,
        ARG_stbc_htltf2_en,
        ARG_ltf_merge_en,
        ARG_channel_filter_en,
        ARG_manu_scale,
        ARG_shift,
        ARG_buffer_size
    };
    static const mp_arg_t allowed_args[] = {
        {MP_QSTR_lltf_en, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true}},
        {MP_QSTR_htltf_en, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true}},
        {MP_QSTR_stbc_htltf2_en, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true}},
        {MP_QSTR_ltf_merge_en, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true}},
        {MP_QSTR_channel_filter_en,
         MP_ARG_KW_ONLY | MP_ARG_BOOL,
         {.u_bool = true}},
        {MP_QSTR_manu_scale, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false}},
        {MP_QSTR_shift, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0}},
        {MP_QSTR_buffer_size,
         MP_ARG_KW_ONLY | MP_ARG_INT,
         {.u_int = CSI_DEFAULT_BUFFER_SIZE}},
    };

    mp_arg_val_t parsed_args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, args + 1, kw_args, MP_ARRAY_SIZE(allowed_args),
        allowed_args, parsed_args);

    // Update configuration with provided parameters
    g_csi_state.config.lltf_en = parsed_args[ARG_lltf_en].u_bool;
    g_csi_state.config.htltf_en = parsed_args[ARG_htltf_en].u_bool;
    g_csi_state.config.stbc_htltf2_en = parsed_args[ARG_stbc_htltf2_en].u_bool;
    g_csi_state.config.ltf_merge_en = parsed_args[ARG_ltf_merge_en].u_bool;
    g_csi_state.config.channel_filter_en =
        parsed_args[ARG_channel_filter_en].u_bool;
    g_csi_state.config.manu_scale = parsed_args[ARG_manu_scale].u_bool;
    g_csi_state.config.shift =
        parsed_args[ARG_shift].u_int & 0x0F; // Limit to 0-15
    g_csi_state.config.buffer_size = parsed_args[ARG_buffer_size].u_int;

    if (g_csi_state.config.buffer_size < 1 ||
        g_csi_state.config.buffer_size > 1024) {
        mp_raise_ValueError(
            MP_ERROR_TEXT("buffer_size must be between 1 and 1024"));
    }

    // Enable CSI with the configuration
    esp_err_t ret = wifi_csi_enable();
    if (ret != ESP_OK) {
        mp_raise_OSError(ret);
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(network_wlan_csi_enable_obj, 1,
    network_wlan_csi_enable);

// wlan.csi_disable()
STATIC mp_obj_t network_wlan_csi_disable(mp_obj_t self_in) {
    (void)self_in; // WLAN object not used, CSI is global state
    esp_err_t ret = wifi_csi_disable();
    if (ret != ESP_OK) {
        mp_raise_OSError(ret);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(network_wlan_csi_disable_obj,
    network_wlan_csi_disable);

// wlan.csi_read() -> dict or None
STATIC mp_obj_t network_wlan_csi_read(mp_obj_t self_in) {
    (void)self_in; // WLAN object not used, CSI is global state

    csi_frame_t frame;

    if (!wifi_csi_read_frame(&frame)) {
        return mp_const_none;
    }

    // Create dictionary for frame data
    mp_obj_t dict = mp_obj_new_dict(20);

    // Add metadata
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_rssi),
        mp_obj_new_int(frame.rssi));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_rate),
        mp_obj_new_int(frame.rate));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_sig_mode),
        mp_obj_new_int(frame.sig_mode));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_mcs),
        mp_obj_new_int(frame.mcs));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_cwb),
        mp_obj_new_int(frame.cwb));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_smoothing),
        mp_obj_new_int(frame.smoothing));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_not_sounding),
        mp_obj_new_int(frame.not_sounding));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_aggregation),
        mp_obj_new_int(frame.aggregation));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_stbc),
        mp_obj_new_int(frame.stbc));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_fec_coding),
        mp_obj_new_int(frame.fec_coding));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_sgi),
        mp_obj_new_int(frame.sgi));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_noise_floor),
        mp_obj_new_int(frame.noise_floor));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_ampdu_cnt),
        mp_obj_new_int(frame.ampdu_cnt));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_channel),
        mp_obj_new_int(frame.channel));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_secondary_channel),
        mp_obj_new_int(frame.secondary_channel));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_timestamp),
        mp_obj_new_int(frame.timestamp_us));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_local_timestamp),
        mp_obj_new_int(frame.local_timestamp));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_ant),
        mp_obj_new_int(frame.ant));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_sig_len),
        mp_obj_new_int(frame.sig_len));

    // MAC address as bytes
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_mac),
        mp_obj_new_bytes(frame.mac, 6));

    // CSI data as array('b') - int8_t values matching ESP-IDF API
    mp_obj_array_t *csi_array =
        MP_OBJ_TO_PTR(mp_obj_new_bytearray_by_ref(frame.len, frame.data));
    csi_array->typecode = 'b';
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_data),
        MP_OBJ_FROM_PTR(csi_array));

    return dict;
}
MP_DEFINE_CONST_FUN_OBJ_1(network_wlan_csi_read_obj, network_wlan_csi_read);

// wlan.csi_dropped() -> int
STATIC mp_obj_t network_wlan_csi_dropped(mp_obj_t self_in) {
    (void)self_in; // WLAN object not used, CSI is global state
    return mp_obj_new_int(g_csi_state.buffer.dropped);
}
MP_DEFINE_CONST_FUN_OBJ_1(network_wlan_csi_dropped_obj,
    network_wlan_csi_dropped);

// wlan.csi_available() -> int
STATIC mp_obj_t network_wlan_csi_available(mp_obj_t self_in) {
    (void)self_in; // WLAN object not used, CSI is global state

    if (!g_csi_state.buffer.initialized) {
        return mp_obj_new_int(0);
    }

    uint32_t head = g_csi_state.buffer.head;
    uint32_t tail = g_csi_state.buffer.tail;
    uint32_t size = g_csi_state.buffer.size;

    uint32_t available = (head >= tail) ? (head - tail) : (size - tail + head);
    return mp_obj_new_int(available);
}
MP_DEFINE_CONST_FUN_OBJ_1(network_wlan_csi_available_obj,
    network_wlan_csi_available);

#endif // MICROPY_PY_NETWORK_WLAN_CSI
