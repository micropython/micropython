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
#include "py/objlist.h"
#include "py/runtime.h"
#include "py/ringbuf.h"

#if MICROPY_PY_NETWORK_WLAN_CSI

#include "esp_timer.h"
#include "esp_wifi.h"
#include "network_wlan_csi.h"
#include "modnetwork.h"
#include <stdint.h>
#include <string.h>

#if CONFIG_IDF_TARGET_ESP32C6 || CONFIG_IDF_TARGET_ESP32C5
#define WIFI_CSI_RXCTRL_V2 (1)
#else
#define WIFI_CSI_RXCTRL_V2 (0)
#endif

#define CSI_MAX_DATA_LEN (512)

typedef struct {
    uint32_t timestamp_us;
    uint32_t local_timestamp;
    uint16_t len;
    uint16_t sig_len;
    int8_t rssi;
    uint8_t rate;
    int8_t noise_floor;
    uint8_t channel;
    uint8_t ampdu_cnt;
    uint8_t rx_state;
    uint8_t sig_mode : 2;
    uint8_t mcs : 5;
    uint8_t cwb : 1;
    uint8_t smoothing : 1;
    uint8_t not_sounding : 1;
    uint8_t aggregation : 1;
    uint8_t stbc : 2;
    uint8_t fec_coding : 1;
    uint8_t sgi : 1;
    uint8_t secondary_channel : 2;
    uint8_t ant : 2;
    uint8_t _reserved : 1;
    uint8_t mac[6];
    int8_t data[CSI_MAX_DATA_LEN];
} csi_frame_t;

// ringbuf_t uses uint16_t for the byte size, so keep the Python-visible limit
// within the maximum addressable ringbuffer capacity.
#define CSI_MAX_BUFFER_SIZE ((UINT16_MAX - 1) / sizeof(csi_frame_t))

typedef struct {
    ringbuf_t ringbuffer;
    uint16_t buffer_size;
    volatile uint32_t dropped;
} csi_state_t;

static csi_state_t *wifi_csi_get_state(void) {
    csi_state_t *state = (csi_state_t *)MP_STATE_PORT(csi_state);
    if (state == NULL) {
        state = m_new_obj(csi_state_t);
        memset(state, 0, sizeof(*state));
        state->buffer_size = MICROPY_PY_NETWORK_WLAN_CSI_DEFAULT_BUFFER_SIZE;
        MP_STATE_PORT(csi_state) = state;
    }
    return state;
}

static void IRAM_ATTR wifi_csi_rx_cb(void *ctx, wifi_csi_info_t *info) {
    (void)ctx;

    csi_state_t *state = (csi_state_t *)MP_STATE_PORT(csi_state);
    if (state == NULL || state->ringbuffer.buf == NULL) {
        return;
    }

    // Keep this static to avoid putting a large frame on the ISR stack.
    static csi_frame_t frame;

    #if WIFI_CSI_RXCTRL_V2
    frame.rssi = info->rx_ctrl.rssi;
    frame.rate = info->rx_ctrl.rate;
    #else
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

    frame.noise_floor = info->rx_ctrl.noise_floor;
    frame.channel = info->rx_ctrl.channel;
    frame.local_timestamp = info->rx_ctrl.timestamp;
    frame.sig_len = info->rx_ctrl.sig_len;
    frame.rx_state = info->rx_ctrl.rx_state;
    memcpy(frame.mac, info->mac, sizeof(frame.mac));
    frame.timestamp_us = (uint32_t)esp_timer_get_time();

    if (info->buf != NULL && info->len > 0) {
        frame.len = info->len > CSI_MAX_DATA_LEN ? CSI_MAX_DATA_LEN : info->len;
        memcpy(frame.data, info->buf, frame.len);
    } else {
        frame.len = 0;
    }

    if (ringbuf_put_bytes(&state->ringbuffer, (uint8_t *)&frame, sizeof(frame)) != 0) {
        state->dropped++;
    }
}

#if WIFI_CSI_RXCTRL_V2
static void wifi_csi_build_config(wifi_csi_config_t *config) {
    *config = (wifi_csi_config_t) {
        .enable = 1,
        .acquire_csi_legacy = 1,
        .acquire_csi_ht20 = 1,
        .acquire_csi_ht40 = 0,
        .acquire_csi_su = 0,
        .acquire_csi_mu = 0,
        .acquire_csi_dcm = 0,
        .acquire_csi_beamformed = 0,
        .val_scale_cfg = 0,
        .dump_ack_en = 0,
    };
    #if CONFIG_IDF_TARGET_ESP32C6
    config->acquire_csi_he_stbc = 0;
    #endif
}
#else
static void wifi_csi_build_config(wifi_csi_config_t *config) {
    *config = (wifi_csi_config_t) {
        .lltf_en = 0,
        .htltf_en = 1,
        .stbc_htltf2_en = 0,
        .ltf_merge_en = 0,
        .channel_filter_en = 0,
        .manu_scale = 0,
        .shift = 0,
        .dump_ack_en = 0,
    };
}
#endif

static esp_err_t wifi_csi_enable(csi_state_t *state) {
    if (state->ringbuffer.buf != NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    wifi_csi_config_t config;
    wifi_csi_build_config(&config);

    esp_err_t err = esp_wifi_set_csi_config(&config);
    if (err != ESP_OK) {
        return err;
    }

    ringbuf_alloc(&state->ringbuffer, sizeof(csi_frame_t) * state->buffer_size);
    state->dropped = 0;

    err = esp_wifi_set_csi_rx_cb(wifi_csi_rx_cb, NULL);
    if (err != ESP_OK) {
        m_del(uint8_t, state->ringbuffer.buf, state->ringbuffer.size);
        state->ringbuffer.buf = NULL;
        state->ringbuffer.size = 0;
        return err;
    }

    err = esp_wifi_set_csi(true);
    if (err != ESP_OK) {
        esp_wifi_set_csi_rx_cb(NULL, NULL);
        m_del(uint8_t, state->ringbuffer.buf, state->ringbuffer.size);
        state->ringbuffer.buf = NULL;
        state->ringbuffer.size = 0;
        return err;
    }

    return ESP_OK;
}

static esp_err_t wifi_csi_disable(csi_state_t *state) {
    if (state == NULL || state->ringbuffer.buf == NULL) {
        return ESP_OK;
    }

    esp_err_t err = esp_wifi_set_csi(false);
    if (err != ESP_OK) {
        return err;
    }

    err = esp_wifi_set_csi_rx_cb(NULL, NULL);
    if (err != ESP_OK) {
        return err;
    }

    m_del(uint8_t, state->ringbuffer.buf, state->ringbuffer.size);
    state->ringbuffer.buf = NULL;
    state->ringbuffer.size = 0;
    state->ringbuffer.iget = 0;
    state->ringbuffer.iput = 0;
    state->dropped = 0;
    return ESP_OK;
}

void wifi_csi_deinit(void) {
    csi_state_t *state = (csi_state_t *)MP_STATE_PORT(csi_state);
    if (state == NULL) {
        return;
    }

    if (state->ringbuffer.buf != NULL) {
        esp_wifi_set_csi(false);
        esp_wifi_set_csi_rx_cb(NULL, NULL);
        m_del(uint8_t, state->ringbuffer.buf, state->ringbuffer.size);
    }

    m_del_obj(csi_state_t, state);
    MP_STATE_PORT(csi_state) = NULL;
}

static bool wifi_csi_read_frame(csi_frame_t *frame) {
    csi_state_t *state = (csi_state_t *)MP_STATE_PORT(csi_state);
    if (state == NULL || state->ringbuffer.buf == NULL) {
        return false;
    }

    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
    int result = ringbuf_get_bytes(&state->ringbuffer, (uint8_t *)frame, sizeof(*frame));
    MICROPY_END_ATOMIC_SECTION(atomic_state);
    return result == 0;
}

static mp_obj_list_t *network_wlan_csi_get_result_list(mp_obj_t result_in) {
    mp_obj_list_t *result = mp_obj_list_optional_arg(result_in, 22);
    if (result->items[5] == MP_OBJ_NULL) {
        result->items[5] = mp_const_none;
    } else if (result->items[5] != mp_const_none && !mp_obj_is_type(result->items[5], &mp_type_bytearray)) {
        mp_raise_TypeError(MP_ERROR_TEXT("result data must be bytearray"));
    }

    return result;
}

static mp_obj_array_t *network_wlan_csi_update_data(mp_obj_t *data_obj, const csi_frame_t *frame) {
    mp_obj_array_t *data = NULL;
    if (*data_obj != mp_const_none) {
        data = MP_OBJ_TO_PTR(*data_obj);
        size_t capacity = data->len + data->free;
        if (capacity < frame->len) {
            data = NULL;
        }
    }

    if (data == NULL) {
        *data_obj = mp_obj_new_bytearray(frame->len, frame->data);
        data = MP_OBJ_TO_PTR(*data_obj);
        return data;
    }

    memcpy(data->items, frame->data, frame->len);
    size_t capacity = data->len + data->free;
    data->len = frame->len;
    data->free = capacity - frame->len;
    return data;
}

static mp_obj_t network_wlan_csi_enable(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    (void)args[0];

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buffer_size, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = MICROPY_PY_NETWORK_WLAN_CSI_DEFAULT_BUFFER_SIZE} },
    };

    mp_arg_val_t parsed_args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, parsed_args);

    mp_int_t buffer_size = parsed_args[0].u_int;
    if (buffer_size < 1 || buffer_size > CSI_MAX_BUFFER_SIZE) {
        mp_raise_ValueError(MP_ERROR_TEXT("buffer_size out of range"));
    }

    csi_state_t *state = wifi_csi_get_state();
    if (state->ringbuffer.buf != NULL) {
        esp_exceptions(ESP_ERR_INVALID_STATE);
    }
    state->buffer_size = buffer_size;
    esp_exceptions(wifi_csi_enable(state));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(network_wlan_csi_enable_obj, 1, network_wlan_csi_enable);

static mp_obj_t network_wlan_csi_disable(mp_obj_t self_in) {
    (void)self_in;
    csi_state_t *state = (csi_state_t *)MP_STATE_PORT(csi_state);
    if (state != NULL) {
        esp_exceptions(wifi_csi_disable(state));
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(network_wlan_csi_disable_obj, network_wlan_csi_disable);

static mp_obj_t network_wlan_csi_read(size_t n_args, const mp_obj_t *args) {
    (void)args[0];

    csi_frame_t frame;
    if (!wifi_csi_read_frame(&frame)) {
        return mp_const_none;
    }

    mp_obj_list_t *result = network_wlan_csi_get_result_list(n_args > 1 ? args[1] : mp_const_none);

    result->items[0] = MP_OBJ_NEW_SMALL_INT(frame.rssi);
    result->items[1] = MP_OBJ_NEW_SMALL_INT(frame.channel);
    result->items[2] = mp_obj_new_bytes(frame.mac, sizeof(frame.mac));
    result->items[3] = mp_obj_new_int(frame.timestamp_us);
    result->items[4] = mp_obj_new_int(frame.local_timestamp);

    mp_obj_array_t *csi_data = network_wlan_csi_update_data(&result->items[5], &frame);
    csi_data->typecode = 'b';

    result->items[6] = MP_OBJ_NEW_SMALL_INT(frame.rate);
    result->items[7] = MP_OBJ_NEW_SMALL_INT(frame.sig_mode);
    result->items[8] = MP_OBJ_NEW_SMALL_INT(frame.mcs);
    result->items[9] = MP_OBJ_NEW_SMALL_INT(frame.cwb);
    result->items[10] = MP_OBJ_NEW_SMALL_INT(frame.smoothing);
    result->items[11] = MP_OBJ_NEW_SMALL_INT(frame.not_sounding);
    result->items[12] = MP_OBJ_NEW_SMALL_INT(frame.aggregation);
    result->items[13] = MP_OBJ_NEW_SMALL_INT(frame.stbc);
    result->items[14] = MP_OBJ_NEW_SMALL_INT(frame.fec_coding);
    result->items[15] = MP_OBJ_NEW_SMALL_INT(frame.sgi);
    result->items[16] = MP_OBJ_NEW_SMALL_INT(frame.noise_floor);
    result->items[17] = MP_OBJ_NEW_SMALL_INT(frame.ampdu_cnt);
    result->items[18] = MP_OBJ_NEW_SMALL_INT(frame.secondary_channel);
    result->items[19] = MP_OBJ_NEW_SMALL_INT(frame.ant);
    result->items[20] = MP_OBJ_NEW_SMALL_INT(frame.sig_len);
    result->items[21] = MP_OBJ_NEW_SMALL_INT(frame.rx_state);
    return MP_OBJ_FROM_PTR(result);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(network_wlan_csi_read_obj, 1, 2, network_wlan_csi_read);

static mp_obj_t network_wlan_csi_dropped(mp_obj_t self_in) {
    (void)self_in;
    csi_state_t *state = (csi_state_t *)MP_STATE_PORT(csi_state);
    return mp_obj_new_int(state == NULL ? 0 : state->dropped);
}
MP_DEFINE_CONST_FUN_OBJ_1(network_wlan_csi_dropped_obj, network_wlan_csi_dropped);

static mp_obj_t network_wlan_csi_available(mp_obj_t self_in) {
    (void)self_in;

    csi_state_t *state = (csi_state_t *)MP_STATE_PORT(csi_state);
    if (state == NULL || state->ringbuffer.buf == NULL) {
        return MP_OBJ_NEW_SMALL_INT(0);
    }

    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
    size_t available = ringbuf_avail(&state->ringbuffer);
    MICROPY_END_ATOMIC_SECTION(atomic_state);
    return MP_OBJ_NEW_SMALL_INT(available / sizeof(csi_frame_t));
}
MP_DEFINE_CONST_FUN_OBJ_1(network_wlan_csi_available_obj, network_wlan_csi_available);

MP_REGISTER_ROOT_POINTER(void *csi_state);

#endif // MICROPY_PY_NETWORK_WLAN_CSI
