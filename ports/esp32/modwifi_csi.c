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

// ============================================================================
// Gain Lock Support (ESP32-S3, C3, C5, C6 only)
// ============================================================================
// Based on Espressif esp-csi recommendations for stable CSI measurements.
// These PHY functions are in the ESP-IDF PHY blob but not in public headers.
#if CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32C5 || CONFIG_IDF_TARGET_ESP32C6
#define WIFI_CSI_GAIN_LOCK_SUPPORTED 1
extern void phy_fft_scale_force(bool force_en, int8_t force_value);  // fft_gain is signed
extern void phy_force_rx_gain(int force_en, int force_value);

// PHY RX Control structure to access hidden gain fields
typedef struct {
    unsigned : 32;  // reserved
    unsigned : 32;  // reserved
    unsigned : 32;  // reserved
    unsigned : 32;  // reserved
    unsigned : 32;  // reserved
    unsigned : 16;  // reserved
    signed fft_gain : 8;     // FFT gain is signed (-128 to 127)
    unsigned agc_gain : 8;   // AGC gain is unsigned (0-255)
    unsigned : 32;  // reserved
    unsigned : 32;  // reserved
    #if CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32C5 || CONFIG_IDF_TARGET_ESP32C6
    unsigned : 32;  // reserved
    unsigned : 32;  // reserved
    unsigned : 32;  // reserved
    #endif
    unsigned : 32;  // reserved
} wifi_pkt_rx_ctrl_phy_t;
#else
#define WIFI_CSI_GAIN_LOCK_SUPPORTED 0
#endif

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

    // Extract gain fields from hidden PHY structure (ESP32-S3, C3, C5, C6 only)
    #if WIFI_CSI_GAIN_LOCK_SUPPORTED
    const wifi_pkt_rx_ctrl_phy_t *phy_info = (const wifi_pkt_rx_ctrl_phy_t *)info;
    frame.fft_gain = phy_info->fft_gain;
    frame.agc_gain = phy_info->agc_gain;
    #else
    frame.agc_gain = 0;
    frame.fft_gain = 0;
    #endif

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
    // Build C5/C6 CSI config from unified config (newer ESP-IDF API structure)
    // All values are read from the unified config structure, showing default values explicitly
    *out = (wifi_csi_config_t) {
        .enable = 1,  // Master enable (REQUIRED)
        .acquire_csi_legacy = 1,  // Always enabled for C5/C6 (for compatibility, optimized for motion detection)
        .acquire_csi_ht20 = in->htltf_en,  // From ht_ltf parameter (default: true)
        .acquire_csi_ht40 = in->acquire_csi_ht40,  // Default: false (disabled)
        .acquire_csi_su = in->acquire_csi_su,  // Default: false (disabled)
        .acquire_csi_mu = in->acquire_csi_mu,  // Default: false (disabled)
        .acquire_csi_dcm = in->acquire_csi_dcm,  // Default: false (disabled)
        .acquire_csi_beamformed = in->acquire_csi_beamformed,  // Default: false (disabled)
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
    // Check if MicroPython state shows CSI was already enabled
    if (g_csi_state.ringbuffer.buf != NULL) {
        // CSI is already enabled in MicroPython - user must disable first
        ESP_LOGE(TAG, "CSI already enabled. Call csi_disable() first to reconfigure.");
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t ret;

    // Cleanup ESP-IDF CSI state to handle stale state after soft reset.
    // After soft reset (CTRL+D), MicroPython restarts but ESP-IDF keeps the old
    // callback registered and CSI enabled. Without this cleanup, the enable
    // sequence would fail or produce no packets.
    ESP_LOGI(TAG, "Cleaning up any stale ESP-IDF CSI state...");
    esp_wifi_set_csi(false);
    esp_wifi_set_csi_rx_cb(NULL, NULL);

    ESP_LOGI(TAG, "Starting CSI enable sequence...");

    #if CONFIG_IDF_TARGET_ESP32C6 || CONFIG_IDF_TARGET_ESP32C5
    // C5/C6: Newer ESP-IDF CSI API structure
    wifi_csi_config_t csi_config;
    build_modern_csi_config(&csi_config, &g_csi_state.config);
    #else
    // ESP32/S2/S3/C3: Legacy ESP-IDF CSI API structure
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
    ESP_LOGI(TAG, "CSI enabled successfully (ESP32-C6 mode)");
    #elif CONFIG_IDF_TARGET_ESP32C5
    ESP_LOGI(TAG, "CSI enabled successfully (ESP32-C5 mode)");
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
        {MP_QSTR_legacy_ltf, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false}},  // Legacy API: Enable L-LTF / C5/C6: acquire_csi_legacy always 1
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
    g_csi_state.config.acquire_csi_ht40 = false;  // HT40 disabled (would capture 128 subcarriers)
    g_csi_state.config.acquire_csi_su = false;    // HE20 SU disabled (would capture 256 subcarriers)
    g_csi_state.config.acquire_csi_mu = false;    // MU-MIMO disabled
    g_csi_state.config.acquire_csi_dcm = false;   // DCM disabled
    g_csi_state.config.acquire_csi_beamformed = false;  // Beamformed disabled

    // Map scale to appropriate API parameters
    // If scale is None or 0: auto-scaling
    // If scale is 1-15: manual scaling with shift value
    mp_obj_t scale_obj = parsed_args[ARG_scale].u_obj;
    if (scale_obj == MP_OBJ_NULL || scale_obj == mp_const_none) {
        // Auto-scaling (default)
        g_csi_state.config.manu_scale = false;  // Legacy devices: auto-scaling
        g_csi_state.config.val_scale_cfg = 0;   // C5/C6: auto-scaling
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
            g_csi_state.config.manu_scale = true;  // Legacy devices: manual scaling
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
//                 noise_floor, ampdu_cnt, secondary_channel, ant, sig_len, rx_state,
//                 agc_gain, fft_gain)
// Note: agc_gain (unsigned 0-255) and fft_gain (signed -128 to 127) are only valid on
//       ESP32-S3, C3, C5, C6 (0 on other platforms)
static mp_obj_t network_wlan_csi_read(mp_obj_t self_in) {
    (void)self_in; // WLAN object not used, CSI is global state

    csi_frame_t frame;

    if (!wifi_csi_read_frame(&frame)) {
        return mp_const_none;
    }

    // Create tuple with frame data (order: most important fields first)
    // Tuple order matches logical importance: rssi, channel, mac, timestamps, data, then metadata
    // Added agc_gain and fft_gain at the end for backward compatibility
    mp_obj_t items[24];

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
    // Gain fields (ESP32-S3, C3, C5, C6 only - 0 on other platforms)
    items[22] = MP_OBJ_NEW_SMALL_INT(frame.agc_gain);  // uint8_t (0-255)
    items[23] = MP_OBJ_NEW_SMALL_INT(frame.fft_gain);  // int8_t (-128 to 127)

    return mp_obj_new_tuple(24, items);
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

// ============================================================================
// Gain Lock Functions
// ============================================================================

// wlan.csi_force_gain(agc, fft)
// Forces AGC and FFT gain to fixed values for stable CSI measurements.
// This eliminates amplitude variations caused by automatic gain control.
//
// Args:
//   agc: AGC gain value (0-255, unsigned), or None to re-enable automatic control
//   fft: FFT gain value (-128 to 127, signed), or None to re-enable automatic control
//
// Usage:
//   # Lock gain after calibration (typical values from 300-packet median)
//   wlan.csi_force_gain(39, -41)  # Note: fft_gain is signed
//
//   # Re-enable automatic gain control
//   wlan.csi_force_gain(None, None)
//
// Supported: ESP32-S3, ESP32-C3, ESP32-C5, ESP32-C6
// Not supported: ESP32, ESP32-S2 (raises NotImplementedError)
static mp_obj_t network_wlan_csi_force_gain(mp_obj_t self_in, mp_obj_t agc_obj, mp_obj_t fft_obj) {
    (void)self_in;

    #if WIFI_CSI_GAIN_LOCK_SUPPORTED
    if (agc_obj == mp_const_none && fft_obj == mp_const_none) {
        // Disable forced gain, re-enable AGC
        phy_force_rx_gain(0, 0);
        phy_fft_scale_force(false, 0);
        ESP_LOGI(TAG, "Gain lock disabled (AGC re-enabled)");
    } else if (agc_obj == mp_const_none || fft_obj == mp_const_none) {
        mp_raise_ValueError(MP_ERROR_TEXT("Both agc and fft must be set, or both None"));
    } else {
        // Force both gains
        int agc = mp_obj_get_int(agc_obj);
        int fft = mp_obj_get_int(fft_obj);

        // AGC is unsigned (0-255), FFT is signed (-128 to 127)
        if (agc < 0 || agc > 255) {
            mp_raise_ValueError(MP_ERROR_TEXT("agc must be 0-255"));
        }
        if (fft < -128 || fft > 127) {
            mp_raise_ValueError(MP_ERROR_TEXT("fft must be -128 to 127"));
        }

        phy_fft_scale_force(true, (int8_t)fft);
        phy_force_rx_gain(1, agc);
        ESP_LOGI(TAG, "Gain locked: AGC=%d, FFT=%d", agc, fft);
    }
    return mp_const_none;
    #else
    (void)agc_obj;
    (void)fft_obj;
    mp_raise_NotImplementedError(MP_ERROR_TEXT("Gain lock not supported on ESP32/ESP32-S2"));
    return mp_const_none;
    #endif
}
MP_DEFINE_CONST_FUN_OBJ_3(network_wlan_csi_force_gain_obj, network_wlan_csi_force_gain);

// wlan.csi_gain_lock_supported()
// Returns True if gain lock is supported on this platform.
static mp_obj_t network_wlan_csi_gain_lock_supported(mp_obj_t self_in) {
    (void)self_in;
    #if WIFI_CSI_GAIN_LOCK_SUPPORTED
    return mp_const_true;
    #else
    return mp_const_false;
    #endif
}
MP_DEFINE_CONST_FUN_OBJ_1(network_wlan_csi_gain_lock_supported_obj, network_wlan_csi_gain_lock_supported);

// Register root pointer to prevent GC from collecting the CSI buffer.
// The buffer is allocated with m_new() and stored in g_csi_state.ringbuffer.buf.
// Without this, the GC would not see any Python reference to the buffer
// and could free it while the ISR callback is still writing to it, causing
// heap corruption (TLSF assertion: block_size(block) >= size).
MP_REGISTER_ROOT_POINTER(void *csi_ringbuf_root);

#endif // MICROPY_PY_NETWORK_WLAN_CSI
