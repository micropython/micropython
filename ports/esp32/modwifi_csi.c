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
#include "modwifi_csi.h"
#include "modnetwork.h"
#include <string.h>

static const char *TAG = "wifi_csi";

// Global CSI state (static - only used within this file)
// C99 mandates all static structure fields to be initialized with 0/NULL unless specified otherwise
static csi_state_t g_csi_state = {};

// ============================================================================
// CSI Callback (ISR Context)
// ============================================================================

void IRAM_ATTR wifi_csi_rx_cb(void *ctx, wifi_csi_info_t *info) {
    if (g_csi_state.ringbuffer.buf == NULL) {
        return;
    }

    // Use static frame to avoid stack overflow in ISR context
    // (csi_frame_t is ~543 bytes, too large for ISR stack which is typically 2-4KB)
    static csi_frame_t frame;

    // Extract metadata
    // Note: ESP32-C5/C6 have a different rx_ctrl structure with fewer fields
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
    // Note: No memset needed here - all fields are explicitly assigned below,
    // unlike C5/C6 where some fields are unavailable and must be zero-initialized
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
    if (info->buf != NULL && info->len > 0) {
        frame.len = info->len > CSI_MAX_DATA_LEN ? CSI_MAX_DATA_LEN : info->len;
        memcpy(frame.data, info->buf, frame.len);
    } else {
        frame.len = 0;
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

// Helper function to build modern API (C5/C6) CSI config from unified config
#if CONFIG_IDF_TARGET_ESP32C6 || CONFIG_IDF_TARGET_ESP32C5
static void build_modern_csi_config(wifi_csi_config_t *out, const csi_config_t *in) {
    // Build modern API (C5/C6) CSI config from unified config
    // All values are read from the unified config structure, showing default values explicitly
    *out = (wifi_csi_config_t) {
        .enable = 1,  // Master enable (REQUIRED)
        .acquire_csi_legacy = 1,  // Always enabled for modern API (fallback for legacy routers, optimized for motion detection)
        .acquire_csi_ht20 = in->htltf_en,  // From ht_ltf parameter (default: true)
        .acquire_csi_ht40 = in->acquire_csi_ht40,  // Default: false (less stable)
        .acquire_csi_su = in->acquire_csi_su,  // Default: true (WiFi 6 support)
        .acquire_csi_mu = in->acquire_csi_mu,  // Default: false (rarely used)
        .acquire_csi_dcm = in->acquire_csi_dcm,  // Default: false (not commonly used)
        .acquire_csi_beamformed = in->acquire_csi_beamformed,  // Default: false (alters channel estimation)
        .val_scale_cfg = in->val_scale_cfg,  // From scale parameter (default: 0 = auto-scaling)
        .dump_ack_en = in->dump_ack_en,  // From capture_ack parameter (default: false)
    };
    #if CONFIG_IDF_TARGET_ESP32C6
    out->acquire_csi_he_stbc = in->stbc_htltf2_en;  // From stbc parameter (default: false)
    #endif
}

// Helper function to build legacy CSI config from unified config
#else
static void build_legacy_csi_config(wifi_csi_config_t *out, const csi_config_t *in) {
    // Build legacy API (ESP32, ESP32-S2, ESP32-S3, ESP32-C3) CSI config from unified config
    // All values are read from the unified config structure, showing default values explicitly
    *out = (wifi_csi_config_t) {
        .lltf_en = in->lltf_en,  // From legacy_ltf parameter (default: false, HT-LTF only)
        .htltf_en = in->htltf_en,  // From ht_ltf parameter (default: true, PRIMARY - best SNR)
        .stbc_htltf2_en = in->stbc_htltf2_en,  // From stbc parameter (default: false)
        .ltf_merge_en = in->ltf_merge_en,  // Default: false (no merge, HT-LTF only)
        .channel_filter_en = in->channel_filter_en,  // Default: false (raw subcarriers)
        .manu_scale = in->manu_scale,  // From scale parameter (None/0=auto=false, 1-15=manual=true)
        .shift = in->shift,  // From scale parameter (1-15) when manu_scale=true, default: 0
        .dump_ack_en = in->dump_ack_en,  // From capture_ack parameter (default: false)
    };
}
#endif

static esp_err_t wifi_csi_enable(void) {
    if (g_csi_state.ringbuffer.buf != NULL) {
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
    // Modern API (C5/C6): WiFi 6 config
    // CRITICAL: Must specify which CSI types to acquire, otherwise callback is never invoked!
    wifi_csi_config_t csi_config;
    build_modern_csi_config(&csi_config, &g_csi_state.config);
    #else
    // Legacy API (ESP32, ESP32-S2, ESP32-S3, ESP32-C3): use legacy CSI API with LTF fields
    wifi_csi_config_t csi_config;
    build_legacy_csi_config(&csi_config, &g_csi_state.config);
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
    // because buffer is NULL (checked in wifi_csi_rx_cb)

    ESP_LOGI(TAG, "Allocating buffer (size=%lu frames)...",
        (unsigned long)g_csi_state.config.buffer_size);
    csi_frame_t *frames_buffer = m_new(csi_frame_t, g_csi_state.config.buffer_size);
    if (!frames_buffer) {
        ESP_LOGE(TAG, "Failed to allocate CSI buffer");
        esp_wifi_set_csi_rx_cb(NULL, NULL);
        return ESP_ERR_NO_MEM;
    }
    ESP_LOGI(TAG, "Buffer allocated OK");

    // Store buffer in root pointer to prevent GC from collecting it
    // This is critical: without this, the GC could free the buffer while
    // the ISR callback is still writing to it, causing heap corruption.
    MP_STATE_PORT(csi_ringbuf_root) = frames_buffer;

    // Initialize ringbuffer fields BEFORE setting buf pointer.
    // The ISR checks buf != NULL to determine if CSI is active, so buf must
    // be set LAST to ensure size/iget/iput are valid when ISR starts writing.
    g_csi_state.ringbuffer.size = sizeof(csi_frame_t) * g_csi_state.config.buffer_size;
    g_csi_state.ringbuffer.iget = 0;
    g_csi_state.ringbuffer.iput = 0;
    g_csi_state.ringbuffer.buf = (uint8_t *)frames_buffer;  // Enable ISR writes LAST

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
        m_del(csi_frame_t, (csi_frame_t *)g_csi_state.ringbuffer.buf, g_csi_state.config.buffer_size);
        g_csi_state.ringbuffer.buf = NULL;
        MP_STATE_PORT(csi_ringbuf_root) = NULL;
        return ret;
    }

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
    if (g_csi_state.ringbuffer.buf == NULL) {
        return ESP_OK;
    }

    // Disable CSI first to stop new callbacks from being invoked
    esp_err_t ret = esp_wifi_set_csi(false);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to disable CSI: %d", ret);
        return ret;
    }

    // Then unregister callback (safe now that CSI is disabled)
    ret = esp_wifi_set_csi_rx_cb(NULL, NULL);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to unregister CSI callback: %d", ret);
        return ret;
    }

    // Clean up: deallocate buffer and clear root pointer
    m_del(csi_frame_t, (csi_frame_t *)g_csi_state.ringbuffer.buf, g_csi_state.config.buffer_size);
    g_csi_state.ringbuffer.buf = NULL;
    MP_STATE_PORT(csi_ringbuf_root) = NULL;
    g_csi_state.dropped = 0;

    ESP_LOGI(TAG, "CSI disabled and state cleaned");
    return ESP_OK;
}

static bool wifi_csi_read_frame(csi_frame_t *frame) {
    // Disable interrupts to prevent race condition with ISR callback (wifi_csi_rx_cb)
    // which writes to the same ringbuffer. This ensures atomic read of the frame.
    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
    int result = ringbuf_get_bytes(&g_csi_state.ringbuffer, (uint8_t *)frame, sizeof(csi_frame_t));
    MICROPY_END_ATOMIC_SECTION(atomic_state);
    return result == 0;
}

// ============================================================================
// MicroPython Bindings
// ============================================================================

// wlan.csi_enable(**kwargs)
static mp_obj_t network_wlan_csi_enable(size_t n_args, const mp_obj_t *args,
    mp_map_t *kw_args) {
    (void)args[0]; // WLAN object not used, CSI is global state

    enum {
        ARG_buffer_size,
        ARG_legacy_ltf,
        ARG_ht_ltf,
        ARG_stbc,
        ARG_capture_ack,
        ARG_scale,
    };
    static const mp_arg_t allowed_args[] = {
        {MP_QSTR_buffer_size,
         MP_ARG_KW_ONLY | MP_ARG_INT,
         {.u_int = MICROPY_PY_NETWORK_WLAN_CSI_DEFAULT_BUFFER_SIZE}},
        {MP_QSTR_legacy_ltf, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false}},  // Legacy API: Enable L-LTF / Modern API: acquire_csi_legacy always 1
        {MP_QSTR_ht_ltf, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true}},  // HT-LTF enabled (PRIMARY - best SNR)
        {MP_QSTR_stbc, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false}},  // STBC HT-LTF2 / HE-STBC (disabled for consistency)
        {MP_QSTR_capture_ack, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false}},  // Capture ACK frames (adds noise, not useful)
        {MP_QSTR_scale, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL}},  // Scale value: None/0=auto, 1-15=manual shift
    };

    mp_arg_val_t parsed_args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, args + 1, kw_args, MP_ARRAY_SIZE(allowed_args),
        allowed_args, parsed_args);

    // Validate buffer_size before modifying state
    uint32_t buffer_size = parsed_args[ARG_buffer_size].u_int;
    if (buffer_size < 1 || buffer_size > 1024) {
        mp_raise_ValueError(
            MP_ERROR_TEXT("buffer_size must be between 1 and 1024"));
    }

    // Update configuration with provided parameters
    g_csi_state.config.buffer_size = buffer_size;
    g_csi_state.config.lltf_en = parsed_args[ARG_legacy_ltf].u_bool;
    g_csi_state.config.htltf_en = parsed_args[ARG_ht_ltf].u_bool;
    g_csi_state.config.stbc_htltf2_en = parsed_args[ARG_stbc].u_bool;
    g_csi_state.config.dump_ack_en = parsed_args[ARG_capture_ack].u_bool;

    // Set WiFi 6 (C5/C6) defaults - these are not exposed as Python parameters
    // acquire_csi_su=true enables HE-LTF CSI from 802.11ax HE20 SU packets (essential for WiFi 6 routers)
    g_csi_state.config.acquire_csi_su = true;
    g_csi_state.config.acquire_csi_ht40 = false;  // HT40 less stable
    g_csi_state.config.acquire_csi_mu = false;    // MU-MIMO rarely used
    g_csi_state.config.acquire_csi_dcm = false;   // DCM not commonly used
    g_csi_state.config.acquire_csi_beamformed = false;  // Alters channel estimation

    // Map scale to appropriate API parameters
    // If scale is None or 0: auto-scaling
    // If scale is 1-15: manual scaling with shift value
    mp_obj_t scale_obj = parsed_args[ARG_scale].u_obj;
    if (scale_obj == MP_OBJ_NULL || scale_obj == mp_const_none) {
        // Auto-scaling (default)
        g_csi_state.config.manu_scale = false;  // Legacy API: auto-scaling
        g_csi_state.config.val_scale_cfg = 0;   // Modern API: auto-scaling
        g_csi_state.config.shift = 0;
    } else {
        // Manual scaling: get shift value from scale parameter
        int scale_val = mp_obj_get_int(scale_obj);
        if (scale_val < 0 || scale_val > 15) {
            mp_raise_ValueError(MP_ERROR_TEXT("scale must be 0-15 or None"));
        }
        if (scale_val == 0) {
            // scale=0 means auto-scaling
            g_csi_state.config.manu_scale = false;
            g_csi_state.config.val_scale_cfg = 0;
            g_csi_state.config.shift = 0;
        } else {
            // scale=1-15: manual scaling with shift
            g_csi_state.config.manu_scale = true;  // Legacy API: manual scaling
            g_csi_state.config.shift = scale_val;
            // Map shift (0-15) to val_scale_cfg (1-7) for modern API
            uint8_t val_scale = (scale_val / 2) + 1;
            if (val_scale > 7) {
                val_scale = 7;
            }
            g_csi_state.config.val_scale_cfg = val_scale;  // Set directly, so helper can just read it
        }
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

    // Use MP_OBJ_NEW_SMALL_INT for values known to fit in a small int (faster than mp_obj_new_int)
    // Small ints avoid heap allocation and the runtime check in mp_obj_new_int.
    // Use mp_obj_new_int for 32-bit timestamps that may exceed small int range.
    items[0] = MP_OBJ_NEW_SMALL_INT(frame.rssi);  // int8_t
    items[1] = MP_OBJ_NEW_SMALL_INT(frame.channel);  // uint8_t
    items[2] = mp_obj_new_bytes(frame.mac, 6);
    items[3] = mp_obj_new_int(frame.timestamp_us);  // uint32_t - may exceed small int
    items[4] = mp_obj_new_int(frame.local_timestamp);  // uint32_t - may exceed small int

    // CSI data as array('b') - int8_t values matching ESP-IDF API
    mp_obj_array_t *csi_array =
        MP_OBJ_TO_PTR(mp_obj_new_bytearray(frame.len, frame.data));
    csi_array->typecode = 'b';
    items[5] = MP_OBJ_FROM_PTR(csi_array);

    // Metadata fields - all are uint8_t or uint16_t, safe for small int
    items[6] = MP_OBJ_NEW_SMALL_INT(frame.rate);  // uint8_t
    items[7] = MP_OBJ_NEW_SMALL_INT(frame.sig_mode);  // uint8_t
    items[8] = MP_OBJ_NEW_SMALL_INT(frame.mcs);  // uint8_t
    items[9] = MP_OBJ_NEW_SMALL_INT(frame.cwb);  // uint8_t
    items[10] = MP_OBJ_NEW_SMALL_INT(frame.smoothing);  // uint8_t
    items[11] = MP_OBJ_NEW_SMALL_INT(frame.not_sounding);  // uint8_t
    items[12] = MP_OBJ_NEW_SMALL_INT(frame.aggregation);  // uint8_t
    items[13] = MP_OBJ_NEW_SMALL_INT(frame.stbc);  // uint8_t
    items[14] = MP_OBJ_NEW_SMALL_INT(frame.fec_coding);  // uint8_t
    items[15] = MP_OBJ_NEW_SMALL_INT(frame.sgi);  // uint8_t
    items[16] = MP_OBJ_NEW_SMALL_INT(frame.noise_floor);  // int8_t
    items[17] = MP_OBJ_NEW_SMALL_INT(frame.ampdu_cnt);  // uint8_t
    items[18] = MP_OBJ_NEW_SMALL_INT(frame.secondary_channel);  // uint8_t
    items[19] = MP_OBJ_NEW_SMALL_INT(frame.ant);  // uint8_t
    items[20] = MP_OBJ_NEW_SMALL_INT(frame.sig_len);  // uint16_t
    items[21] = MP_OBJ_NEW_SMALL_INT(frame.rx_state);  // uint8_t

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
        return MP_OBJ_NEW_SMALL_INT(0);
    }

    // Disable interrupts to get consistent read of ringbuffer state
    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
    size_t available_bytes = ringbuf_avail(&g_csi_state.ringbuffer);
    MICROPY_END_ATOMIC_SECTION(atomic_state);

    // Convert bytes to frame count (max 1024 frames, fits in small int)
    return MP_OBJ_NEW_SMALL_INT(available_bytes / sizeof(csi_frame_t));
}
MP_DEFINE_CONST_FUN_OBJ_1(network_wlan_csi_available_obj,
    network_wlan_csi_available);

// Register root pointer to prevent GC from collecting the CSI buffer.
// The buffer is allocated with m_new() and stored in g_csi_state.ringbuffer.buf.
// Without this, the GC would not see any Python reference to the buffer
// and could free it while the ISR callback is still writing to it, causing
// heap corruption (TLSF assertion: block_size(block) >= size).
MP_REGISTER_ROOT_POINTER(void *csi_ringbuf_root);

#endif // MICROPY_PY_NETWORK_WLAN_CSI
