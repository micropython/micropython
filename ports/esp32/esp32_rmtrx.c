#include "py/mphal.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "modmachine.h"
#include "modesp32.h"

#if SOC_RMT_SUPPORTED
#include "esp_task.h"
#include "driver/rmt_rx.h"

// This exposes the ESP32's RMT module to MicroPython. RMT is provided by the Espressif ESP-IDF:
//
//    https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/peripherals/rmt.html
//
// With some examples provided:
//
//    https://github.com/espressif/arduino-esp32/tree/master/libraries/ESP32/examples/RMT
//
// RMT allows accurate (down to 12.5ns resolution) transmit - and receive - of pulse signals.
// Originally designed to generate infrared remote control signals, the module is very
// flexible and quite easy-to-use.
//
// This code exposes the RMT RX feature.

// Forward declaration
extern const mp_obj_type_t esp32_rmtrx_type;

// Python object impl structure
typedef struct _esp32_rmtrx_obj_t {
    mp_obj_base_t base;
    rmt_channel_handle_t channel;
    gpio_num_t pin;
    unsigned int resolution_hz;

    size_t num_symbols;
    rmt_symbol_word_t *symbols;

    bool rx_active;
    rmt_receive_config_t rx_config;
    // double buffering of received data
    size_t recv_count;
    int *recv_data;

    // soft filtering values
    unsigned int soft_min_len;
    unsigned int soft_max_len;
    unsigned int soft_min_ns;
    unsigned int soft_max_ns;
    unsigned int soft_min_value;
    unsigned int soft_max_value;
} esp32_rmtrx_obj_t;

#define SOFT_MAX_LIMIT 0x7fffffff

// Interrupt handler
static bool IRAM_ATTR rmt_recv_done(rmt_channel_handle_t channel, const rmt_rx_done_event_data_t *edata, void *udata) {
    esp32_rmtrx_obj_t *self = udata;

    if (self->recv_count) {
        // user hasn't read the last reception yet
        goto out;
    }

    const rmt_symbol_word_t *data = edata->received_symbols;
    size_t len = edata->num_symbols;
    int odd = (data[len - 1].duration1 == 0) ? 1 : 0;
    int list_len = len * 2 - odd;

    if (list_len < self->soft_min_len || list_len > self->soft_max_len) {
        goto out;
    }

    for (size_t i = 0; i < len; i++) {
        int n0 = (uint16_t)data[i].duration0;
        if (n0 < self->soft_min_value || n0 > self->soft_max_value) {
            goto out;
        }
        self->recv_data[i * 2 + 0] = n0 * (data[i].level0 ? +1 : -1);

        if (odd && i == (len - 1)) {
            continue;
        }

        int n1 = (uint16_t)data[i].duration1;
        if (n1 < self->soft_min_value || n1 > self->soft_max_value) {
            goto out;
        }
        self->recv_data[i * 2 + 1] = n1 * (data[i].level1 ? +1 : -1);
    }

    // commit the reception
    self->recv_count = list_len;

out:
    if (self->rx_active) {
        rmt_receive(self->channel, self->symbols, self->num_symbols * sizeof(rmt_symbol_word_t), &self->rx_config);
    }

    return false;
}


static mp_obj_t esp32_rmtrx_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pin,           MP_ARG_REQUIRED | MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_num_symbols,                     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = SOC_RMT_MEM_WORDS_PER_CHANNEL} },
        { MP_QSTR_min_ns,        MP_ARG_REQUIRED | MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_max_ns,        MP_ARG_REQUIRED | MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_resolution_hz, MP_ARG_REQUIRED | MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_soft_min_len,                    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_soft_max_len,                    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = SOFT_MAX_LIMIT} },
        { MP_QSTR_soft_min_ns,                     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_soft_max_ns,                     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = SOFT_MAX_LIMIT} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    gpio_num_t pin_id = machine_pin_get_id(args[0].u_obj);
    int num_symbols = args[1].u_int;
    if ((num_symbols < SOC_RMT_MEM_WORDS_PER_CHANNEL) || ((num_symbols % 2) == 1)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("num_symbols must be even and at least %d"), SOC_RMT_MEM_WORDS_PER_CHANNEL);
    }

    // RMT new driver does not have a way to specify a group clock divisor, see
    // https://github.com/espressif/esp-idf/issues/14760#issuecomment-2430770601 and
    // https://github.com/espressif/esp-idf/issues/11262
    // since signal_range_min_ns must fit in an 8-bit register, the value is limited
    // to 3190ns (80MHz clock * 3190 / 1000000000 [conversion to ns] == 255).

    int min_ns = args[2].u_int;
    if (min_ns <= 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("min_ns must be positive"));
    }
    int max_ns = args[3].u_int;
    if (max_ns <= min_ns) {
        mp_raise_ValueError(MP_ERROR_TEXT("max_ns must be bigger than min_ns"));
    }
    int resolution_hz = args[4].u_int;
    if (resolution_hz <= 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("resolution_hz must be positive"));
    }
    unsigned int unit_width_in_ns = 1000000000 / resolution_hz;

    int soft_min_len = args[5].u_int;
    int soft_max_len = args[6].u_int;
    int soft_min_ns = args[7].u_int;
    int soft_max_ns = args[8].u_int;

    if (soft_min_len < 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("soft_min_len must be positive"));
    }
    if (soft_min_len > soft_max_len) {
        mp_raise_ValueError(MP_ERROR_TEXT("soft_min_len must be less or equal than soft_max_len"));
    }
    if (soft_min_ns < 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("soft_min_ns must be positive"));
    }
    if (soft_min_ns > soft_max_ns) {
        mp_raise_ValueError(MP_ERROR_TEXT("soft_min_ns must be less or equal than soft_max_ns"));
    }

    esp32_rmtrx_obj_t *self = mp_obj_malloc_with_finaliser(esp32_rmtrx_obj_t, &esp32_rmtrx_type);

    self->channel = NULL;
    self->num_symbols = num_symbols;
    self->symbols = m_realloc(NULL, num_symbols * sizeof(rmt_symbol_word_t));
    self->recv_data = m_realloc(NULL, num_symbols * 2 * sizeof(int));
    self->recv_count = 0;
    self->resolution_hz = resolution_hz;
    self->soft_min_len = soft_min_len;
    self->soft_max_len = soft_max_len;
    self->soft_min_ns = soft_min_ns;
    self->soft_min_value = soft_min_ns / unit_width_in_ns;
    self->soft_max_ns = soft_max_ns;
    self->soft_max_value = soft_max_ns / unit_width_in_ns;

    self->rx_config.signal_range_min_ns = min_ns;
    self->rx_config.signal_range_max_ns = max_ns;

    self->pin = pin_id;

    rmt_rx_channel_config_t rx_ch_conf = {
        .gpio_num = self->pin = pin_id,
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = resolution_hz,
        .mem_block_symbols = num_symbols,
    };

    check_esp_err(rmt_new_rx_channel(&rx_ch_conf, &self->channel));
    rmt_rx_event_callbacks_t cbs = {
        .on_recv_done = rmt_recv_done,
    };
    check_esp_err(rmt_rx_register_event_callbacks(self->channel, &cbs, self));
    check_esp_err(rmt_enable(self->channel));

    return MP_OBJ_FROM_PTR(self);
}

static void esp32_rmtrx_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    esp32_rmtrx_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "RMT RX pin=%u num_symbols=%u resolution_hz=%u min_ns=%u max_ns=%u "
        "soft_min_len=%u soft_max_len=%u soft_min_ns=%u soft_max_ns=%u",
        self->pin, self->num_symbols, self->resolution_hz,
        self->rx_config.signal_range_min_ns, self->rx_config.signal_range_max_ns,
        self->soft_min_len, self->soft_max_len, self->soft_min_ns, self->soft_max_ns);
}


static mp_obj_t esp32_rmtrx_deinit(mp_obj_t self_in) {
    esp32_rmtrx_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->pin != -1) {
        self->pin = -1;

        rmt_disable(self->channel);
        rmt_del_channel(self->channel);

        m_free(self->symbols);
        self->num_symbols = 0;
        m_free(self->recv_data);
        self->recv_data = 0;
        self->recv_count = 0;
        self->rx_active = false;
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(esp32_rmtrx_deinit_obj, esp32_rmtrx_deinit);


static mp_obj_t esp32_rmtrx_active(size_t n_args, const mp_obj_t *args) {
    esp32_rmtrx_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (n_args == 1) {
        return self->rx_active ? mp_const_true : mp_const_false;
    }

    if (self->pin == -1) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("already deinitialized"));
    }

    if (mp_obj_is_true(args[1])) {
        if (!self->rx_active) {
            self->rx_active = true;
            self->recv_count = 0;
            check_esp_err(rmt_receive(self->channel, self->symbols, self->num_symbols * sizeof(rmt_symbol_word_t), &self->rx_config));
        }
        return mp_const_true;
    }

    if (self->rx_active) {
        self->rx_active = false;
        self->recv_count = 0;
    }
    return mp_const_false;
}

static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp32_rmtrx_active_obj, 1, 2, esp32_rmtrx_active);


static mp_obj_t esp32_rmtrx_get_data(mp_obj_t self_in) {
    esp32_rmtrx_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->pin == -1) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("already deinitialized"));
    }

    if (!self->recv_count) {
        return mp_const_none;
    }

    mp_obj_t list = mp_obj_new_list(self->recv_count, NULL);
    mp_obj_list_t *list_in = MP_OBJ_TO_PTR(list);

    for (size_t i = 0; i < self->recv_count; i++) {
        list_in->items[i] = MP_OBJ_NEW_SMALL_INT(self->recv_data[i]);
    }

    self->recv_count = 0;

    return list;
}

static MP_DEFINE_CONST_FUN_OBJ_1(esp32_rmtrx_get_data_obj, esp32_rmtrx_get_data);


static const mp_rom_map_elem_t esp32_rmtrx_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&esp32_rmtrx_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&esp32_rmtrx_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_active), MP_ROM_PTR(&esp32_rmtrx_active_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_data), MP_ROM_PTR(&esp32_rmtrx_get_data_obj) },

    // Constants
    { MP_ROM_QSTR(MP_QSTR_PULSE_MAX), MP_ROM_INT(32767) },
};
static MP_DEFINE_CONST_DICT(esp32_rmtrx_locals_dict, esp32_rmtrx_locals_dict_table);


static mp_uint_t esp32_rmtrx_stream_ioctl(
    mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    if (request != MP_STREAM_POLL) {
        *errcode = MP_EINVAL;
        return MP_STREAM_ERROR;
    }
    esp32_rmtrx_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_uint_t ret = 0;
    if ((arg & MP_STREAM_POLL_RD) && self->recv_count) {
        ret |= MP_STREAM_POLL_RD;
    }
    return ret;
}

static const mp_stream_p_t esp32_rmtrx_stream_p = {
    .ioctl = esp32_rmtrx_stream_ioctl,
};

MP_DEFINE_CONST_OBJ_TYPE(
    esp32_rmtrx_type,
    MP_QSTR_RMTRX,
    MP_TYPE_FLAG_NONE,
    make_new, esp32_rmtrx_make_new,
    print, esp32_rmtrx_print,
    locals_dict, &esp32_rmtrx_locals_dict,
    protocol, &esp32_rmtrx_stream_p
    );

#endif // SOC_RMT_SUPPORTED
