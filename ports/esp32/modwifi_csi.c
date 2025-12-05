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
#include "py/ringbuf.h"

#if MICROPY_PY_NETWORK_WLAN_CSI

#include "esp_log.h"
#include "esp_timer.h"
#include "esp_wifi.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "modwifi_csi.h"
#include "modnetwork.h"
#include <string.h>

static const char *TAG = "wifi_csi";

// Global CSI state (static - only used within this file)
static csi_state_t g_csi_state = {
    .ringbuffer =
    {
        .buf = NULL,
        .size = 0,
        .iget = 0,
        .iput = 0,
    },
    .config =
    {
        .lltf_en = true,
        .htltf_en = true,
        .stbc_htltf2_en = true,
        .ltf_merge_en = true,
        .channel_filter_en = false,
        .manu_scale = false,
        .shift = 0,
        .buffer_size = MICROPY_PY_NETWORK_WLAN_CSI_DEFAULT_BUFFER_SIZE,
    },
    .dropped = 0,
    .enabled = false,
};

// ============================================================================
// CSI Callback (ISR Context)
// ============================================================================

void IRAM_ATTR wifi_csi_rx_cb(void *ctx, wifi_csi_info_t *info) {
    if (!g_csi_state.enabled || g_csi_state.ringbuffer.buf == NULL) {
        return;
    }

    csi_frame_t frame;

    // Extract metadata
    // Note: ESP32-C5 and ESP32-C6 have a different rx_ctrl structure with fewer
    // fields
    #if CONFIG_IDF_TARGET_ESP32C6 || CONFIG_IDF_TARGET_ESP32C5
    // ESP32-C5/C6: Many fields are not available in esp_wifi_rxctrl_t
    // Initialize all fields to zero, then assign available ones
    memset(&frame, 0, sizeof(csi_frame_t));
    // Fields not available in C5/C6 rx_ctrl are already zero from memset above
    // Now assign available fields
    frame.rssi = info->rx_ctrl.rssi;
    frame.rate = info->rx_ctrl.rate;
    #else
    // ESP32, ESP32-S2, ESP32-S3, ESP32-C3: Full rx_ctrl structure available
    frame.rssi = info->rx_ctrl.rssi;
    frame.rate = info->rx_ctrl.rate;
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
    int result = ringbuf_put_bytes(&g_csi_state.ringbuffer, (uint8_t *)&frame, sizeof(csi_frame_t));
    if (result != 0) {
        g_csi_state.dropped++;
    }
}

// ============================================================================
// CSI Control Functions
// ============================================================================

// Forward declarations
static esp_err_t wifi_csi_enable(void);
static esp_err_t wifi_csi_disable(void);

static esp_err_t wifi_csi_enable(void) {
    if (g_csi_state.enabled) {
        ESP_LOGI(TAG, "Already enabled");
        return ESP_OK;
    }

    esp_err_t ret;

    ESP_LOGI(TAG, "Starting CSI enable sequence...");

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
        ESP_LOGE(TAG, "Failed to set WiFi protocol: 0x%x (CSI requires proper protocol mode)", ret);
        return ret;
    }
    ESP_LOGI(TAG, "WiFi protocol set to 802.11b/g/n/ax (WiFi 6 enabled)");
    #else
    // ESP32, ESP32-S2, ESP32-S3, ESP32-C3: WiFi 4 only (802.11b/g/n)
    ret = esp_wifi_set_protocol(
        WIFI_IF_STA, WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set WiFi protocol: 0x%x (CSI requires proper protocol mode)", ret);
        return ret;
    }
    ESP_LOGI(TAG, "WiFi protocol set to 802.11b/g/n");
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

    #if CONFIG_IDF_TARGET_ESP32C6 || CONFIG_IDF_TARGET_ESP32C5
    // ESP32-C6 / ESP32-C5: WiFi 6 config. Reference: https://github.com/espressif/esp-idf/issues/14271
    // CRITICAL: Must specify which CSI types to acquire, otherwise callback is never invoked!
    wifi_csi_config_t csi_config = {
        .enable = 1, // Master enable for CSI acquisition (REQUIRED)
        .acquire_csi_legacy = 1, // Acquire L-LTF CSI from legacy 802.11a/g packets (CRITICAL: required for callback)
        .acquire_csi_ht20 = 1, // Acquire HT-LTF CSI from 802.11n HT20 packets (CRITICAL: required for HT packets, improves MIMO)
        .acquire_csi_ht40 = 0, // Acquire HT-LTF CSI from 802.11n HT40 packets (40MHz, disabled for stability, enable if router uses HT40)
        .acquire_csi_su = 1, // Acquire HE-LTF CSI from 802.11ax HE20 SU packets (WiFi 6 support, if router supports 802.11ax)
        .acquire_csi_mu = 0, // Acquire HE-LTF CSI from 802.11ax HE20 MU packets (disabled, not using WiFi 6 MU-MIMO)
        .acquire_csi_dcm = 0, // Acquire CSI from DCM packets (WiFi 6 long-range feature, disabled, not commonly used)
        .acquire_csi_beamformed = 0, // Acquire CSI from beamformed packets (beamforming directs signal, disabled, not needed)
        .val_scale_cfg = 0, // CSI value scaling (0-8): 0=auto (recommended), 1-8=manual with shift bits, controls amplitude normalization
        .dump_ack_en = 0, // Enable capture of 802.11 ACK frames (disabled to reduce overhead)
    };
    #if CONFIG_IDF_TARGET_ESP32C6
    csi_config.acquire_csi_he_stbc = 0;     // Acquire CSI from 802.11ax HE STBC packets (improves reliability with multiple antennas, disabled, not commonly used)
    #endif
    #else
    // ESP32, ESP32-S2, ESP32-S3, ESP32-C3 use legacy CSI API with LTF fields
    wifi_csi_config_t csi_config = {
        .lltf_en = g_csi_state.config.lltf_en, // Enable Legacy Long Training Field (L-LTF) CSI capture (present in all 802.11a/g packets, provides base channel estimation with 64 subcarriers)
        .htltf_en = g_csi_state.config.htltf_en, // Enable HT Long Training Field (HT-LTF) CSI capture (present in 802.11n HT packets, provides improved channel estimation for MIMO)
        .stbc_htltf2_en = g_csi_state.config.stbc_htltf2_en, // Enable Space-Time Block Code HT-LTF2 capture (STBC uses 2 antennas to improve reliability, captures second HT-LTF when STBC is active)
        .ltf_merge_en = g_csi_state.config.ltf_merge_en, // Merge L-LTF and HT-LTF data by averaging (true: average for stability, false: use only HT-LTF for precision)
        .channel_filter_en = g_csi_state.config.channel_filter_en, // Channel filter to smooth adjacent subcarriers (true: filter/smooth, 52 useful subcarriers, false: keep independence, 64 total)
        .manu_scale = g_csi_state.config.manu_scale, // Manual vs automatic CSI data scaling (false: auto-scaling recommended, adapts dynamically, true: manual scaling requires .shift parameter
        .shift = g_csi_state.config.shift, // Shift value for manual scaling (0-15, only used when manu_scale = true)
        .dump_ack_en = false, // Enable capture of 802.11 ACK frames (disabled to reduce overhead)
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
    // Note: If we fail after this point, callback remains registered but won't be called
    // because buffer is NULL and enabled is false

    // Allocate buffer from internal RAM that's accessible from ISR context
    // Buffer must be accessible from IRAM_ATTR callback (wifi_csi_rx_cb)
    // Allocate at the end to make cleanup easier if earlier steps fail
    ESP_LOGI(TAG, "Allocating buffer (size=%lu frames)...",
        (unsigned long)g_csi_state.config.buffer_size);
    void *frames_buffer = heap_caps_calloc(g_csi_state.config.buffer_size, sizeof(csi_frame_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    if (!frames_buffer) {
        ESP_LOGE(TAG, "Failed to allocate CSI buffer");
        return ESP_ERR_NO_MEM;
    }
    ESP_LOGI(TAG, "Buffer allocated OK");
    g_csi_state.ringbuffer.buf = (uint8_t *)frames_buffer;
    g_csi_state.ringbuffer.size = sizeof(csi_frame_t) * g_csi_state.config.buffer_size;
    g_csi_state.ringbuffer.iget = 0;
    g_csi_state.ringbuffer.iput = 0;

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
        // Clean up buffer if CSI enable failed
        heap_caps_free(g_csi_state.ringbuffer.buf);
        memset((void *)&g_csi_state.ringbuffer, 0x00, sizeof(ringbuf_t));
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

static esp_err_t wifi_csi_disable(void) {
    if (!g_csi_state.enabled) {
        return ESP_OK;
    }

    esp_err_t ret = esp_wifi_set_csi(false);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to disable CSI: %d", ret);
        return ret;
    }

    // Unregister callback to prevent it from being called after disable
    ret = esp_wifi_set_csi_rx_cb(NULL, NULL);
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "Failed to unregister CSI callback: %d", ret);
        // Continue with cleanup even if callback unregister fails
    }

    // Clean up: deallocate buffer and reset config to defaults
    if (g_csi_state.ringbuffer.buf != NULL) {
        heap_caps_free(g_csi_state.ringbuffer.buf);
        memset((void *)&g_csi_state.ringbuffer, 0x00, sizeof(ringbuf_t));
        g_csi_state.dropped = 0;
    }

    // Reset config to defaults
    g_csi_state.config.lltf_en = true;
    g_csi_state.config.htltf_en = true;
    g_csi_state.config.stbc_htltf2_en = true;
    g_csi_state.config.ltf_merge_en = true;
    g_csi_state.config.channel_filter_en = true;
    g_csi_state.config.manu_scale = false;
    g_csi_state.config.shift = 0;
    g_csi_state.config.buffer_size = MICROPY_PY_NETWORK_WLAN_CSI_DEFAULT_BUFFER_SIZE;

    g_csi_state.enabled = false;
    ESP_LOGI(TAG, "CSI disabled and state cleaned");
    return ESP_OK;
}

static bool wifi_csi_read_frame(csi_frame_t *frame) {
    return ringbuf_get_bytes(&g_csi_state.ringbuffer, (uint8_t *)frame, sizeof(csi_frame_t)) == 0;
}

// ============================================================================
// MicroPython Bindings
// ============================================================================

// wlan.csi_enable(**kwargs)
static mp_obj_t network_wlan_csi_enable(size_t n_args, const mp_obj_t *args,
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
         {.u_int = MICROPY_PY_NETWORK_WLAN_CSI_DEFAULT_BUFFER_SIZE}},
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
    esp_exceptions(wifi_csi_enable());

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(network_wlan_csi_enable_obj, 1,
    network_wlan_csi_enable);

// wlan.csi_disable()
static mp_obj_t network_wlan_csi_disable(mp_obj_t self_in) {
    (void)self_in; // WLAN object not used, CSI is global state
    esp_exceptions(wifi_csi_disable());
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(network_wlan_csi_disable_obj,
    network_wlan_csi_disable);

// wlan.csi_read() -> tuple or None
// Returns tuple: (rssi, channel, mac, timestamp, local_timestamp, data, rate, sig_mode,
//                 mcs, cwb, smoothing, not_sounding, aggregation, stbc, fec_coding, sgi,
//                 noise_floor, ampdu_cnt, secondary_channel, ant, sig_len, rx_state)
static mp_obj_t network_wlan_csi_read(mp_obj_t self_in) {
    (void)self_in; // WLAN object not used, CSI is global state

    csi_frame_t frame;

    if (!wifi_csi_read_frame(&frame)) {
        return mp_const_none;
    }

    // Create tuple with frame data (order: most important fields first)
    // Tuple order matches logical importance: rssi, channel, mac, timestamps, data, then metadata
    mp_obj_t items[22];

    items[0] = mp_obj_new_int(frame.rssi);
    items[1] = mp_obj_new_int(frame.channel);
    items[2] = mp_obj_new_bytes(frame.mac, 6);
    items[3] = mp_obj_new_int(frame.timestamp_us);
    items[4] = mp_obj_new_int(frame.local_timestamp);

    // CSI data as array('b') - int8_t values matching ESP-IDF API
    mp_obj_array_t *csi_array =
        MP_OBJ_TO_PTR(mp_obj_new_bytearray_by_ref(frame.len, frame.data));
    csi_array->typecode = 'b';
    items[5] = MP_OBJ_FROM_PTR(csi_array);

    // Metadata fields
    items[6] = mp_obj_new_int(frame.rate);
    items[7] = mp_obj_new_int(frame.sig_mode);
    items[8] = mp_obj_new_int(frame.mcs);
    items[9] = mp_obj_new_int(frame.cwb);
    items[10] = mp_obj_new_int(frame.smoothing);
    items[11] = mp_obj_new_int(frame.not_sounding);
    items[12] = mp_obj_new_int(frame.aggregation);
    items[13] = mp_obj_new_int(frame.stbc);
    items[14] = mp_obj_new_int(frame.fec_coding);
    items[15] = mp_obj_new_int(frame.sgi);
    items[16] = mp_obj_new_int(frame.noise_floor);
    items[17] = mp_obj_new_int(frame.ampdu_cnt);
    items[18] = mp_obj_new_int(frame.secondary_channel);
    items[19] = mp_obj_new_int(frame.ant);
    items[20] = mp_obj_new_int(frame.sig_len);
    items[21] = mp_obj_new_int(frame.rx_state);

    return mp_obj_new_tuple(22, items);
}
MP_DEFINE_CONST_FUN_OBJ_1(network_wlan_csi_read_obj, network_wlan_csi_read);

// wlan.csi_dropped() -> int
static mp_obj_t network_wlan_csi_dropped(mp_obj_t self_in) {
    (void)self_in; // WLAN object not used, CSI is global state
    return mp_obj_new_int(g_csi_state.dropped);
}
MP_DEFINE_CONST_FUN_OBJ_1(network_wlan_csi_dropped_obj,
    network_wlan_csi_dropped);

// wlan.csi_available() -> int
static mp_obj_t network_wlan_csi_available(mp_obj_t self_in) {
    (void)self_in; // WLAN object not used, CSI is global state

    if (g_csi_state.ringbuffer.buf == NULL) {
        return mp_obj_new_int(0);
    }

    // ringbuf_avail returns bytes, but we need frame count
    // Note: available_bytes should always be a multiple of sizeof(csi_frame_t)
    // since we only write complete frames, but we use integer division for safety
    size_t available_bytes = ringbuf_avail(&g_csi_state.ringbuffer);
    return mp_obj_new_int(available_bytes / sizeof(csi_frame_t));
}
MP_DEFINE_CONST_FUN_OBJ_1(network_wlan_csi_available_obj,
    network_wlan_csi_available);

#endif // MICROPY_PY_NETWORK_WLAN_CSI
