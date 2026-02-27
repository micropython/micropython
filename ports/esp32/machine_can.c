/* The MIT License (MIT)
 *
 * Copyright (c) 2019 Musumeci Salvatore
 * Copyright (c) 2021 Ihor Nehrutsa
 * Copyright (c) 2022 Yuriy Makarov
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

#include "mpconfigport.h"
#include "py/obj.h"
#include "py/objarray.h"
#include "py/binary.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "mpconfigport.h"
#include "freertos/task.h"
#include "esp_idf_version.h"

#include "soc/soc_caps.h"
#include "esp_task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/twai.h"
#include "machine_can.h"

#if SOC_TWAI_SUPPORTED

#if MICROPY_PY_MACHINE_CAN

#define debug_printf(...) mp_printf(&mp_plat_print, __VA_ARGS__); mp_printf(&mp_plat_print, " | %d at %s\n", __LINE__, __FILE__);

// Default bitrate: 500kb
#define CAN_TASK_PRIORITY           (ESP_TASK_PRIO_MIN + 1)
#define CAN_TASK_STACK_SIZE         (1024 * 2)
#define CAN_DEFAULT_PRESCALER (8)
#define CAN_DEFAULT_SJW (3)
#define CAN_DEFAULT_BS1 (15)
#define CAN_DEFAULT_BS2 (4)
#define CAN_MAX_DATA_FRAME          (8)

static esp32_can_obj_t *esp32_can_objs[SOC_TWAI_CONTROLLER_NUM] = {};

// INTERNAL Deinitialize can
void can_deinit(esp32_can_obj_t *self) {
    if (self->handle) {
        twai_stop_v2(self->handle);
        check_esp_err(twai_driver_uninstall_v2(self->handle));
        self->handle = NULL;
    }
    if (self->irq_handler) {
        vTaskDelete(self->irq_handler);
        self->irq_handler = NULL;
    }
}

// This called from Ctrl-D soft reboot
void machine_can_deinit_all() {
    for (int i = 0; i < SOC_TWAI_CONTROLLER_NUM; i++) {
        can_deinit(esp32_can_objs[i]);
    }
}

static void esp32_can_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    esp32_can_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_printf(print, "CAN(");
    if (self->handle) {
        qstr mode;
        switch (self->g_config.mode) {
            case TWAI_MODE_LISTEN_ONLY:
                mode = MP_QSTR_LISTEN;
                break;
            case TWAI_MODE_NO_ACK:
                mode = MP_QSTR_NO_ACK;
                break;
            case TWAI_MODE_NORMAL:
                mode = MP_QSTR_NORMAL;
                break;
            default:
                mode = MP_QSTR_UNKNOWN;
                break;
        }
        mp_printf(print, "%d, tx=%u, rx=%u, bitrate=%u, mode=%q, loopback=%u, extframe=%u",
            self->g_config.controller_id,
            self->g_config.tx_io,
            self->g_config.rx_io,
            self->bitrate,
            mode,
            self->loopback,
            self->extframe);
    }
    mp_printf(print, ")");
}

// INTERNAL FUNCTION FreeRTOS IRQ task
static void esp32_can_irq_task(void *self_in) {
    esp32_can_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint32_t alerts;

    check_esp_err(twai_reconfigure_alerts_v2(self->handle, TWAI_ALERT_ALL, NULL));
    while (1) {
        check_esp_err(twai_read_alerts_v2(self->handle, &alerts, portMAX_DELAY));

        if (alerts & TWAI_ALERT_BUS_OFF) {
            debug_printf("Bus Off state");
            ++self->num_bus_off;
            for (int i = 3; i > 0; i--) {
                vTaskDelay(pdMS_TO_TICKS(1000));
            }
            twai_initiate_recovery_v2(self->handle); // Needs 128 occurrences of bus free signal
            debug_printf("Initiate bus recovery");
        }
        if (alerts & TWAI_ALERT_ERR_PASS) {
            // debug_printf("Entered Error Passive state");
            ++self->num_error_passive;
        }
        if (alerts & TWAI_ALERT_ABOVE_ERR_WARN) {
            // debug_printf("Surpassed Error Warning Limit");
            ++self->num_error_warning;
        }
        if (alerts & (TWAI_ALERT_BUS_RECOVERED)) {
            debug_printf("Bus Recovered");
            self->bus_recovery_success = 1;
            check_esp_err(twai_start_v2(self->handle));
        }

        if (alerts & (TWAI_ALERT_TX_FAILED | TWAI_ALERT_TX_SUCCESS)) {
            self->last_tx_success = alerts & TWAI_ALERT_TX_SUCCESS;
        }

        if (self->tx_callback != mp_const_none) {
            check_esp_err(twai_get_status_info_v2(self->handle, &self->status));
            if (alerts & TWAI_ALERT_TX_IDLE) {
                mp_sched_schedule(self->tx_callback, MP_OBJ_NEW_SMALL_INT(0));
            }
            if (alerts & TWAI_ALERT_TX_SUCCESS) {
                mp_sched_schedule(self->tx_callback, MP_OBJ_NEW_SMALL_INT(1));
            }
            if (alerts & TWAI_ALERT_TX_FAILED) {
                mp_sched_schedule(self->tx_callback, MP_OBJ_NEW_SMALL_INT(2));
            }
            if (alerts & TWAI_ALERT_TX_RETRIED) {
                mp_sched_schedule(self->tx_callback, MP_OBJ_NEW_SMALL_INT(3));
            }
        }

        if (self->rx_callback != mp_const_none) {
            if (alerts & TWAI_ALERT_RX_DATA) {
                check_esp_err(twai_get_status_info_v2(self->handle, &self->status));
                uint32_t msgs_to_rx = self->status.msgs_to_rx;

                if (msgs_to_rx == 1) {
                    // first message in queue
                    mp_sched_schedule(self->rx_callback, MP_OBJ_NEW_SMALL_INT(0));
                } else if (msgs_to_rx >= self->g_config.rx_queue_len) {
                    // queue is full
                    mp_sched_schedule(self->rx_callback, MP_OBJ_NEW_SMALL_INT(1));
                }
            }
            if (alerts & TWAI_ALERT_RX_QUEUE_FULL) {
                // queue overflow
                mp_sched_schedule(self->rx_callback, MP_OBJ_NEW_SMALL_INT(2));
            }
            if (alerts & TWAI_ALERT_RX_FIFO_OVERRUN) {
                mp_sched_schedule(self->rx_callback, MP_OBJ_NEW_SMALL_INT(3));
            }
        }
    }
}

static mp_obj_t esp32_can_init_helper(esp32_can_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_bitrate, ARG_mode, ARG_prescaler, ARG_sjw, ARG_bs1, ARG_bs2, ARG_auto_restart, ARG_extframe,
           ARG_tx_io, ARG_rx_io, ARG_clkout_io, ARG_bus_off_io, ARG_tx_queue, ARG_rx_queue};
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_bitrate, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 500000} },
        { MP_QSTR_mode, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = TWAI_MODE_NORMAL} },
        { MP_QSTR_prescaler, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = CAN_DEFAULT_PRESCALER} },
        { MP_QSTR_sjw, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = CAN_DEFAULT_SJW} },
        { MP_QSTR_bs1, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = CAN_DEFAULT_BS1} },
        { MP_QSTR_bs2, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = CAN_DEFAULT_BS2} },
        { MP_QSTR_auto_restart, MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_extframe, MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_tx, MP_ARG_INT, {.u_int = 4} },
        { MP_QSTR_rx, MP_ARG_INT, {.u_int = 5} },
        { MP_QSTR_clkout, MP_ARG_INT, {.u_int = TWAI_IO_UNUSED} },
        { MP_QSTR_bus_off, MP_ARG_INT, {.u_int = TWAI_IO_UNUSED} },
        { MP_QSTR_tx_queue, MP_ARG_INT, {.u_int = 5} },
        { MP_QSTR_rx_queue, MP_ARG_INT, {.u_int = 5} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // clear errors
    self->num_error_warning = 0;
    self->num_error_passive = 0;
    self->num_bus_off = 0;

    // Configure device
    self->g_config.mode = args[ARG_mode].u_int & 0x0F;
    self->g_config.tx_io = args[ARG_tx_io].u_int;
    self->g_config.rx_io = args[ARG_rx_io].u_int;
    self->g_config.clkout_io = args[ARG_clkout_io].u_int;
    self->g_config.bus_off_io = args[ARG_bus_off_io].u_int;
    self->g_config.tx_queue_len = args[ARG_tx_queue].u_int;
    self->g_config.rx_queue_len = args[ARG_rx_queue].u_int;
    self->g_config.alerts_enabled = TWAI_ALERT_ALL;
    // TWAI_ALERT_AND_LOG || TWAI_ALERT_BELOW_ERR_WARN || TWAI_ALERT_ERR_ACTIVE || TWAI_ALERT_BUS_RECOVERED ||
    // TWAI_ALERT_ABOVE_ERR_WARN || TWAI_ALERT_BUS_ERROR || TWAI_ALERT_ERR_PASS || TWAI_ALERT_BUS_OFF;
    self->g_config.clkout_divider = 0;
    self->loopback = ((args[ARG_mode].u_int & TWAI_MODE_NO_ACK) > 0);
    self->extframe = args[ARG_extframe].u_bool;
    if (args[ARG_auto_restart].u_bool) {
        mp_raise_NotImplementedError(MP_ERROR_TEXT("Auto-restart not supported"));
    }

    self->f_config = (twai_filter_config_t)TWAI_FILTER_CONFIG_ACCEPT_ALL();

    // Calculate CAN nominal bit timing from bitrate if provided
    self->bitrate = args[ARG_bitrate].u_int;
    switch (self->bitrate) {
        case 0:
            self->t_config = (twai_timing_config_t) {
                .brp = args[ARG_prescaler].u_int,
                .sjw = args[ARG_sjw].u_int,
                .tseg_1 = args[ARG_bs1].u_int,
                .tseg_2 = args[ARG_bs2].u_int,
                .triple_sampling = false
            };
            break;
        #ifdef TWAI_TIMING_CONFIG_1KBITS
        case 1000:
            self->t_config = (twai_timing_config_t)TWAI_TIMING_CONFIG_1KBITS();
            break;
        #endif
        #ifdef TWAI_TIMING_CONFIG_5KBITS
        case 5000:
            self->t_config = (twai_timing_config_t)TWAI_TIMING_CONFIG_5KBITS();
            break;
        #endif
        #ifdef TWAI_TIMING_CONFIG_10KBITS
        case 10000:
            self->t_config = (twai_timing_config_t)TWAI_TIMING_CONFIG_10KBITS();
            break;
        #endif
        #ifdef TWAI_TIMING_CONFIG_12_5KBITS
        case 12500:
            self->t_config = (twai_timing_config_t)TWAI_TIMING_CONFIG_12_5KBITS();
            break;
        #endif
        #ifdef TWAI_TIMING_CONFIG_16KBITS
        case 16000:
            self->t_config = (twai_timing_config_t)TWAI_TIMING_CONFIG_16KBITS();
            break;
        #endif
        #ifdef TWAI_TIMING_CONFIG_20KBITS
        case 20000:
            self->t_config = (twai_timing_config_t)TWAI_TIMING_CONFIG_20KBITS();
            break;
        #endif
        #ifdef TWAI_TIMING_CONFIG_25KBITS
        case 25000:
            self->t_config = (twai_timing_config_t)TWAI_TIMING_CONFIG_25KBITS();
            break;
        #endif
        #ifdef TWAI_TIMING_CONFIG_50KBITS
        case 50000:
            self->t_config = (twai_timing_config_t)TWAI_TIMING_CONFIG_50KBITS();
            break;
        #endif
        #ifdef TWAI_TIMING_CONFIG_100KBITS
        case 100000:
            self->t_config = (twai_timing_config_t)TWAI_TIMING_CONFIG_100KBITS();
            break;
        #endif
        #ifdef TWAI_TIMING_CONFIG_125KBITS
        case 125000:
            self->t_config = (twai_timing_config_t)TWAI_TIMING_CONFIG_125KBITS();
            break;
        #endif
        #ifdef TWAI_TIMING_CONFIG_250KBITS
        case 250000:
            self->t_config = (twai_timing_config_t)TWAI_TIMING_CONFIG_250KBITS();
            break;
        #endif
        #ifdef TWAI_TIMING_CONFIG_500KBITS
        case 500000:
            self->t_config = (twai_timing_config_t)TWAI_TIMING_CONFIG_500KBITS();
            break;
        #endif
        #ifdef TWAI_TIMING_CONFIG_800KBITS
        case 800000:
            self->t_config = (twai_timing_config_t)TWAI_TIMING_CONFIG_800KBITS();
            break;
        #endif
        #ifdef TWAI_TIMING_CONFIG_1MBITS
        case 1000000:
            self->t_config = (twai_timing_config_t)TWAI_TIMING_CONFIG_1MBITS();
            break;
        #endif
        default:
            self->bitrate = 0;
            mp_raise_ValueError(MP_ERROR_TEXT("Unable to set bitrate"));
            return mp_const_none;
    }

    check_esp_err(twai_driver_install_v2(&self->g_config, &self->t_config, &self->f_config, &self->handle));
    check_esp_err(twai_start_v2(self->handle));
    if (xTaskCreatePinnedToCore(esp32_can_irq_task, "can_irq_task", CAN_TASK_STACK_SIZE, self, CAN_TASK_PRIORITY, (TaskHandle_t *)&self->irq_handler, MP_TASK_COREID) != pdPASS) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("failed to create can irq task handler"));
    }
    return mp_const_none;
}

// CAN(bus, ...) No argument to get the object
// If no arguments are provided, the initialized object will be returned
static mp_obj_t esp32_can_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);
    if (mp_obj_is_int(args[0]) != true) {
        mp_raise_TypeError(MP_ERROR_TEXT("bus must be a number"));
    }

    // work out port
    mp_uint_t can_idx = mp_obj_get_int(args[0]);
    if (can_idx > SOC_TWAI_CONTROLLER_NUM - 1) {
//        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("out of CAN controllers:%d"), SOC_TWAI_CONTROLLER_NUM);
    }

    esp32_can_obj_t *self = mp_obj_malloc(esp32_can_obj_t, &machine_can_type);
    esp32_can_objs[can_idx] = self;

    self->g_config = (twai_general_config_t)TWAI_GENERAL_CONFIG_DEFAULT_V2(can_idx, GPIO_NUM_4, GPIO_NUM_5, TWAI_MODE_NORMAL);
    self->f_config = (twai_filter_config_t)TWAI_FILTER_CONFIG_ACCEPT_ALL();
    self->t_config = (twai_timing_config_t)TWAI_TIMING_CONFIG_500KBITS();
    self->handle = NULL;

    self->status.state = STOPPED;
    if (n_args > 1 || n_kw > 0) {
        self->tx_callback = mp_const_none;
        self->rx_callback = mp_const_none;
        self->irq_handler = NULL;
        self->rx_state = RX_STATE_FIFO_EMPTY;

        if (n_args > 1 || n_kw > 0) {
            // start the peripheral
            mp_map_t kw_args;
            mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
            esp32_can_init_helper(self, n_args - 1, args + 1, &kw_args);
        }
    }
    return MP_OBJ_FROM_PTR(self);
}

// init(tx, rx, bitrate, mode=NORMAL, tx_queue=2, rx_queue=5)
static mp_obj_t esp32_can_init(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    esp32_can_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    if (self->handle) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Driver is already initialized"));
        return mp_const_none;
    }
    return esp32_can_init_helper(self, n_args - 1, pos_args + 1, kw_args);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(esp32_can_init_obj, 4, esp32_can_init);

// deinit()
static mp_obj_t esp32_can_deinit(const mp_obj_t self_in) {
    esp32_can_obj_t *self = MP_OBJ_TO_PTR(self_in);
    can_deinit(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(esp32_can_deinit_obj, esp32_can_deinit);

// CAN.restart()
// Force a software restart of the controller, to allow transmission after a bus error
static mp_obj_t esp32_can_restart(mp_obj_t self_in) {
    esp32_can_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_esp_err(twai_get_status_info_v2(self->handle, &self->status));

    self->bus_recovery_success = -1;
    check_esp_err(twai_initiate_recovery_v2(self->handle));
    mp_hal_delay_ms(200); // FIXME: replace it with a smarter solution

    while (self->bus_recovery_success < 0) {
        MICROPY_EVENT_POLL_HOOK;
    }

    if (self->bus_recovery_success) {
        check_esp_err(twai_start_v2(self->handle));
    } else {
        mp_raise_OSError(MP_EIO);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(esp32_can_restart_obj, esp32_can_restart);

// Get the state of the controller
static mp_obj_t esp32_can_state(mp_obj_t self_in) {
    esp32_can_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_esp_err(twai_get_status_info_v2(self->handle, &self->status));
    return mp_obj_new_int(self->status.state);
}
static MP_DEFINE_CONST_FUN_OBJ_1(esp32_can_state_obj, esp32_can_state);

// info() -- Get info about error states and TX/RX buffers
static mp_obj_t esp32_can_info(size_t n_args, const mp_obj_t *args) {
    esp32_can_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    check_esp_err(twai_get_status_info_v2(self->handle, &self->status));
    mp_obj_t dict = mp_obj_new_dict(0);
    #define dict_key(key) mp_obj_new_str(#key, strlen(#key))
    #define dict_value(key) MP_OBJ_NEW_SMALL_INT(self->status.key)
    #define dict_store(key) mp_obj_dict_store(dict, dict_key(key), dict_value(key));
    dict_store(state);
    dict_store(msgs_to_tx);
    dict_store(msgs_to_rx);
    dict_store(tx_error_counter);
    dict_store(rx_error_counter);
    dict_store(tx_failed_count);
    dict_store(rx_missed_count);
    dict_store(rx_overrun_count);
    dict_store(arb_lost_count);
    dict_store(bus_error_count);
    return dict;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp32_can_info_obj, 1, 2, esp32_can_info);

// Get Alert info
static mp_obj_t esp32_can_alert(mp_obj_t self_in) {
    const esp32_can_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint32_t alerts;
    check_esp_err(twai_read_alerts_v2(self->handle, &alerts, 0));
    return mp_obj_new_int(alerts);
}
static MP_DEFINE_CONST_FUN_OBJ_1(esp32_can_alert_obj, esp32_can_alert);

// any() - return `True` if any message waiting, else `False`
static mp_obj_t esp32_can_any(mp_obj_t self_in) {
    esp32_can_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_esp_err(twai_get_status_info_v2(self->handle, &self->status));
    return mp_obj_new_bool((self->status.msgs_to_rx) > 0);
}
static MP_DEFINE_CONST_FUN_OBJ_1(esp32_can_any_obj, esp32_can_any);

// send([data], id, *, timeout=0, rtr=false, extframe=false)
// CAN.send(identifier, data, flags=0, fifo_equal=True)
static mp_obj_t esp32_can_send(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_data, ARG_id, ARG_timeout, ARG_rtr, ARG_extframe };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_data,     MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_id,       MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_timeout,  MP_ARG_KW_ONLY | MP_ARG_INT,  {.u_int = 0} },
        { MP_QSTR_rtr,      MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_extframe, MP_ARG_BOOL,                  {.u_bool = false} },
    };

    // parse args
    esp32_can_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // populate message
    twai_message_t tx_msg = {0};

    size_t length;
    mp_obj_t *items;
    mp_obj_get_array(args[ARG_data].u_obj, &length, &items);
    if (length > CAN_MAX_DATA_FRAME) {
        mp_raise_ValueError(MP_ERROR_TEXT("CAN data field too long"));
    }
    tx_msg.data_length_code = length;
    tx_msg.rtr = (args[ARG_rtr].u_bool ? 1 : 0);

    if (args[ARG_extframe].u_bool) {
        tx_msg.identifier = args[ARG_id].u_int & 0x1FFFFFFF;
        tx_msg.extd = 1;
    } else {
        tx_msg.identifier = args[ARG_id].u_int & 0x7FF;
    }
    if (self->loopback) {
        tx_msg.self = 1;
    }

    for (uint8_t i = 0; i < length; i++) {
        tx_msg.data[i] = mp_obj_get_int(items[i]);
    }

    check_esp_err(twai_get_status_info_v2(self->handle, &self->status));
    // debug_printf("state:%d", self->status.state);
    if (self->status.state == TWAI_STATE_RUNNING) {
        uint32_t timeout_ms = args[ARG_timeout].u_int;

        if (timeout_ms != 0) {
            self->last_tx_success = 0;
            uint32_t start = mp_hal_ticks_ms();
            check_esp_err(twai_transmit_v2(self->handle, &tx_msg, pdMS_TO_TICKS(timeout_ms)));
            while (!self->last_tx_success) {
                if (timeout_ms != portMAX_DELAY) {
                    if (mp_hal_ticks_ms() - start >= timeout_ms) {
                        mp_raise_OSError(MP_ETIMEDOUT);
                    }
                }
                MICROPY_EVENT_POLL_HOOK;
            }

            if (!self->last_tx_success) {
                mp_raise_OSError(MP_EIO);
            }
        } else {
            check_esp_err(twai_transmit_v2(self->handle, &tx_msg, portMAX_DELAY));
        }

        return mp_const_none;
    } else {
        mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("Driver is not in running state:%d"), self->status.state);
    }
}
static MP_DEFINE_CONST_FUN_OBJ_KW(esp32_can_send_obj, 3, esp32_can_send);

// CAN.recv(list=None, *, timeout=5000)
static mp_obj_t esp32_can_recv(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    esp32_can_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    enum { ARG_list, ARG_timeout };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_list, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 5000} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    uint32_t timeout_ms = args[ARG_timeout].u_int;

    check_esp_err(twai_get_status_info_v2(self->handle, &self->status));
    // debug_printf("1 state:%d", self->status.state);
    uint32_t start = mp_hal_ticks_ms();
    while (self->status.state != TWAI_STATE_RUNNING) {
        if (timeout_ms != portMAX_DELAY) {
            if (mp_hal_ticks_ms() - start >= timeout_ms) {
                mp_raise_OSError(MP_ETIMEDOUT);
            }
        }
        MICROPY_EVENT_POLL_HOOK;
        check_esp_err(twai_get_status_info_v2(self->handle, &self->status));
    }
    // debug_printf("2 state:%d", self->status.state);

    // receive the data
    twai_message_t rx_msg;
    check_esp_err(twai_receive_v2(self->handle, &rx_msg, pdMS_TO_TICKS(timeout_ms)));
    uint32_t rx_dlc = rx_msg.data_length_code;

    // Create the tuple, or get the list, that will hold the return values
    // Also populate the fourth element, either a new bytes or reuse existing memoryview
    mp_obj_t ret_obj = args[ARG_list].u_obj;
    mp_obj_t *items;
    if (ret_obj == mp_const_none) {
        ret_obj = mp_obj_new_tuple(4, NULL);
        items = ((mp_obj_tuple_t *)MP_OBJ_TO_PTR(ret_obj))->items;
        items[3] = mp_obj_new_bytes(rx_msg.data, rx_dlc);
    } else {
        // User should provide a list of length at least 4 to hold the values
        if (!mp_obj_is_type(ret_obj, &mp_type_list)) {
            mp_raise_TypeError(NULL);
        }
        mp_obj_list_t *list = MP_OBJ_TO_PTR(ret_obj);
        if (list->len < 4) {
            mp_raise_ValueError(NULL);
        }
        items = list->items;
        // Fourth element must be a memoryview which we assume points to a
        // byte-like array which is large enough, and then we resize it inplace
        if (!mp_obj_is_type(items[3], &mp_type_memoryview)) {
            mp_raise_TypeError(NULL);
        }
        mp_obj_array_t *mv = MP_OBJ_TO_PTR(items[3]);
        if (!(mv->typecode == (MP_OBJ_ARRAY_TYPECODE_FLAG_RW | BYTEARRAY_TYPECODE) || (mv->typecode | 0x20) == (MP_OBJ_ARRAY_TYPECODE_FLAG_RW | 'b'))) {
            mp_raise_ValueError(NULL);
        }
        mv->len = rx_dlc;
        memcpy(mv->items, rx_msg.data, rx_dlc);
    }
    items[0] = MP_OBJ_NEW_SMALL_INT(rx_msg.identifier);
    items[1] = rx_msg.extd ? mp_const_true : mp_const_false;
    items[2] = rx_msg.rtr ? mp_const_true : mp_const_false;

    // Return the result
    return ret_obj;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(esp32_can_recv_obj, 1, esp32_can_recv);

// Clear filters setting
static mp_obj_t esp32_can_clearfilter(mp_obj_t self_in) {
    esp32_can_obj_t *self = MP_OBJ_TO_PTR(self_in);

    // Defaults from TWAI_FILTER_CONFIG_ACCEPT_ALL
    self->f_config = (twai_filter_config_t)TWAI_FILTER_CONFIG_ACCEPT_ALL();

    // Apply filter
    check_esp_err(twai_stop_v2(self->handle));
    check_esp_err(twai_driver_uninstall_v2(self->handle));
    check_esp_err(twai_driver_install_v2(&self->g_config, &self->t_config, &self->f_config, &self->handle));
    check_esp_err(twai_start_v2(self->handle));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(esp32_can_clearfilter_obj, esp32_can_clearfilter);

// bank: 0 only
// mode: FILTER_RAW_SINGLE, FILTER_RAW_DUAL or FILTER_ADDR_SINGLE or FILTER_ADDR_DUAL
// params: [id, mask]
// rtr: ignored if FILTER_RAW
// Set CAN HW filter
// CAN.set_filters(filters)
static mp_obj_t esp32_can_set_filters(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_bank, ARG_mode, ARG_params, ARG_rtr, ARG_extframe };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_bank,     MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_mode,     MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_params,   MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_rtr,      MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_bool = false} },
        { MP_QSTR_extframe, MP_ARG_BOOL,                  {.u_bool = false} },
    };

    // parse args
    esp32_can_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    const int can_idx = args[ARG_bank].u_int;

    if (can_idx != 0) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Bank (%d) doesn't exist"), can_idx);
    }

    size_t len;
    mp_obj_t *params;
    mp_obj_get_array(args[ARG_params].u_obj, &len, &params);
    const int mode = args[ARG_mode].u_int;

    uint32_t id = mp_obj_get_int(params[0]);
    uint32_t mask = mp_obj_get_int(params[1]); // FIXME: Overflow in case 0xFFFFFFFF for mask
    if (mode == FILTER_RAW_SINGLE || mode == FILTER_RAW_DUAL) {
        if (len != 2) {
            mp_raise_ValueError(MP_ERROR_TEXT("params must be a 2-values list"));
        }
        self->f_config.single_filter = (mode == FILTER_RAW_SINGLE);
        self->f_config.acceptance_code = id;
        self->f_config.acceptance_mask = mask;
    } else {
        self->f_config.single_filter = self->extframe;
        // esp32_can_set_filters(self, id, mask, args[ARG_bank].u_int, args[ARG_rtr].u_int);
        // Check if bank is allowed
        int bank = 0;
        if (bank > ((self->extframe && self->f_config.single_filter) ? 0 : 1)) {
            mp_raise_ValueError(MP_ERROR_TEXT("CAN filter parameter error"));
        }
        uint32_t preserve_mask;
        int addr = 0;
        int rtr = 0;
        if (self->extframe) {
            addr = (addr & 0x1FFFFFFF) << 3 | (rtr ? 0x04 : 0);
            mask = (mask & 0x1FFFFFFF) << 3 | 0x03;
            preserve_mask = 0;
        } else if (self->f_config.single_filter) {
            addr = (((addr & 0x7FF) << 5) | (rtr ? 0x10 : 0));
            mask = ((mask & 0x7FF) << 5);
            mask |= 0xFFFFF000;
            preserve_mask = 0;
        } else {
            addr = (((addr & 0x7FF) << 5) | (rtr ? 0x10 : 0));
            mask = ((mask & 0x7FF) << 5);
            preserve_mask = 0xFFFF << (bank == 0 ? 16 : 0);
            if ((self->f_config.acceptance_mask & preserve_mask) == (0xFFFF << (bank == 0 ? 16 : 0))) {
                // Other filter accepts all; it will replaced duplicating current filter
                addr = addr | (addr << 16);
                mask = mask | (mask << 16);
                preserve_mask = 0;
            } else {
                addr = addr << (bank == 1 ? 16 : 0);
                mask = mask << (bank == 1 ? 16 : 0);
            }
        }
        self->f_config.acceptance_code &= preserve_mask;
        self->f_config.acceptance_code |= addr;
        self->f_config.acceptance_mask &= preserve_mask;
        self->f_config.acceptance_mask |= mask;
    }
    // Apply filter
    if (self->handle) {
        check_esp_err(twai_stop_v2(self->handle));
        check_esp_err(twai_driver_uninstall_v2(self->handle));
    }
    check_esp_err(twai_driver_install_v2(&self->g_config, &self->t_config, &self->f_config, &self->handle));
    check_esp_err(twai_start_v2(self->handle));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(esp32_can_set_filters_obj, 1, esp32_can_set_filters);

// CAN.irq_recv(callback, hard=False)
static mp_obj_t esp32_can_irq_recv(mp_obj_t self_in, mp_obj_t callback_in) {
    esp32_can_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (callback_in == mp_const_none) {
        // disable callback
        self->rx_callback = mp_const_none;
    } else if (mp_obj_is_callable(callback_in)) {
        // set up interrupt
        self->rx_callback = callback_in;
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(esp32_can_irq_recv_obj, esp32_can_irq_recv);

// CAN.irq_send(callback, hard=False)
static mp_obj_t esp32_can_irq_send(mp_obj_t self_in, mp_obj_t callback_in) {
    esp32_can_obj_t *self = MP_OBJ_TO_PTR(self_in);
    // mp_obj_t callback_in = NULL;
    if (callback_in == mp_const_none) {
        // disable callback
        self->tx_callback = mp_const_none;
    } else if (mp_obj_is_callable(callback_in)) {
        // set up interrupt
        self->tx_callback = callback_in;
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(esp32_can_irq_send_obj, esp32_can_irq_send);

// CAN.get_state()
static mp_obj_t esp32_can_get_state(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(esp32_can_get_state_obj, 3, esp32_can_get_state);

// CAN.get_counters([list] /)
static mp_obj_t esp32_can_get_counters(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(esp32_can_get_counters_obj, 3, esp32_can_get_counters);

// CAN.get_timings([list])
static mp_obj_t esp32_can_get_timings(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(esp32_can_get_timings_obj, 3, esp32_can_get_timings);

// CAN.reset(mode=CAN.Mode.NORMAL)
static mp_obj_t esp32_can_reset(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(esp32_can_reset_obj, 3, esp32_can_reset);

// CAN.mode([mode])
static mp_obj_t esp32_can_mode(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(esp32_can_mode_obj, 3, esp32_can_mode);

// Clear TX Queue
static mp_obj_t esp32_can_clear_tx_queue(mp_obj_t self_in) {
    const esp32_can_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(twai_clear_transmit_queue_v2(self->handle) == ESP_OK);
}
static MP_DEFINE_CONST_FUN_OBJ_1(esp32_can_clear_tx_queue_obj, esp32_can_clear_tx_queue);

// Clear RX Queue
static mp_obj_t esp32_can_clear_rx_queue(mp_obj_t self_in) {
    const esp32_can_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(twai_clear_receive_queue_v2(self->handle) == ESP_OK);
}
static MP_DEFINE_CONST_FUN_OBJ_1(esp32_can_clear_rx_queue_obj, esp32_can_clear_rx_queue);

static const mp_rom_map_elem_t esp32_can_locals_dict_table[] = {
    // CAN_ATTRIBUTES
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_CAN) },
    // Micropython Generic API
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&esp32_can_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&esp32_can_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_restart), MP_ROM_PTR(&esp32_can_restart_obj) },
    { MP_ROM_QSTR(MP_QSTR_state), MP_ROM_PTR(&esp32_can_state_obj) },
    { MP_ROM_QSTR(MP_QSTR_info), MP_ROM_PTR(&esp32_can_info_obj) },
    { MP_ROM_QSTR(MP_QSTR_any), MP_ROM_PTR(&esp32_can_any_obj) },
    { MP_ROM_QSTR(MP_QSTR_send), MP_ROM_PTR(&esp32_can_send_obj) },
    { MP_ROM_QSTR(MP_QSTR_recv), MP_ROM_PTR(&esp32_can_recv_obj) },
    { MP_ROM_QSTR(MP_QSTR_irq_send), MP_ROM_PTR(&esp32_can_irq_send_obj) },
    { MP_ROM_QSTR(MP_QSTR_irq_recv), MP_ROM_PTR(&esp32_can_irq_recv_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_filters), MP_ROM_PTR(&esp32_can_set_filters_obj) },
    { MP_ROM_QSTR(MP_QSTR_clearfilter), MP_ROM_PTR(&esp32_can_clearfilter_obj) },

    { MP_ROM_QSTR(MP_QSTR_get_state), MP_ROM_PTR(&esp32_can_get_state_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_counters), MP_ROM_PTR(&esp32_can_get_counters_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_timings), MP_ROM_PTR(&esp32_can_get_timings_obj) },
    { MP_ROM_QSTR(MP_QSTR_reset), MP_ROM_PTR(&esp32_can_reset_obj) },
    { MP_ROM_QSTR(MP_QSTR_mode), MP_ROM_PTR(&esp32_can_mode_obj) },
    // ESP32 Specific API
    { MP_OBJ_NEW_QSTR(MP_QSTR_clear_tx_queue), MP_ROM_PTR(&esp32_can_clear_tx_queue_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_clear_rx_queue), MP_ROM_PTR(&esp32_can_clear_rx_queue_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_get_alerts), MP_ROM_PTR(&esp32_can_alert_obj) },
    // CAN_MODE
    // class CAN.Mode
    { MP_ROM_QSTR(MP_QSTR_MODE_NORMAL), MP_ROM_INT(MODE_NORMAL) },
    { MP_ROM_QSTR(MP_QSTR_MODE_SLEEP), MP_ROM_INT(MODE_SLEEP) },
    { MP_ROM_QSTR(MP_QSTR_MODE_LOOPBACK), MP_ROM_INT(MODE_LOOPBACK) },
    { MP_ROM_QSTR(MP_QSTR_MODE_SILENT), MP_ROM_INT(MODE_SILENT) },
    { MP_ROM_QSTR(MP_QSTR_MODE_SILENT_LOOPBACK), MP_ROM_INT(MODE_SILENT_LOOPBACK) },
    { MP_ROM_QSTR(MP_QSTR_MODE_LISTEN_ONLY), MP_ROM_INT(MODE_LISTEN_ONLY) },
/* esp32 can modes
TWAI_MODE_NORMAL      - Normal operating mode where TWAI controller can send/receive/acknowledge messages
TWAI_MODE_NO_ACK      - Transmission does not require acknowledgment. Use this mode for self testing. // This mode is useful when self testing the TWAI controller (loopback of transmissions).
TWAI_MODE_LISTEN_ONLY - The TWAI controller will not influence the bus (No transmissions or acknowledgments) but can receive messages. // This mode is suited for bus monitor applications.
*/
/* stm32 can modes
#define CAN_MODE_NORMAL             FDCAN_MODE_NORMAL
#define CAN_MODE_LOOPBACK           FDCAN_MODE_EXTERNAL_LOOPBACK
#define CAN_MODE_SILENT             FDCAN_MODE_BUS_MONITORING
#define CAN_MODE_SILENT_LOOPBACK    FDCAN_MODE_INTERNAL_LOOPBACK
*/
    // CAN_STATE
    // class CAN.State
    { MP_ROM_QSTR(MP_QSTR_STATE_STOPPED), MP_ROM_INT(TWAI_STATE_STOPPED) },
    { MP_ROM_QSTR(MP_QSTR_STATE_ACTIVE), MP_ROM_INT(TWAI_STATE_RUNNING) },
    { MP_ROM_QSTR(MP_QSTR_STATE_WARNING), MP_ROM_INT(-1) },
    { MP_ROM_QSTR(MP_QSTR_STATE_PASSIVE), MP_ROM_INT(-1) },
    { MP_ROM_QSTR(MP_QSTR_STATE_BUS_OFF), MP_ROM_INT(TWAI_STATE_BUS_OFF) },
    { MP_ROM_QSTR(MP_QSTR_STATE_RECOVERING), MP_ROM_INT(TWAI_STATE_RECOVERING) }, // esp32 specific

    // class CAN.MessageFlags
    { MP_ROM_QSTR(MP_QSTR_FLAG_RTR), MP_ROM_INT(FLAG_RTR) },
    { MP_ROM_QSTR(MP_QSTR_FLAG_EXT_ID), MP_ROM_INT(FLAG_EXT_ID) },
    { MP_ROM_QSTR(MP_QSTR_FLAG_FD_F), MP_ROM_INT(FLAG_FD_F) },
    { MP_ROM_QSTR(MP_QSTR_FLAG_BRS), MP_ROM_INT(FLAG_BRS) },
    { MP_ROM_QSTR(FLAG_UNORDERED), MP_ROM_INT(FLAG_UNORDERED) },
    // class CAN.RecvErrors
    { MP_ROM_QSTR(MP_QSTR_RECV_ERR_FULL), MP_ROM_INT(RECV_ERR_FULL) },
    { MP_ROM_QSTR(MP_QSTR_RECV_ERR_OVERRUN), MP_ROM_INT(RECV_ERR_OVERRUN) },
    { MP_ROM_QSTR(MP_QSTR_RECV_ERR_ESI), MP_ROM_INT(RECV_ERR_ESI) },
    // class CAN.SendErrors
    { MP_ROM_QSTR(MP_QSTR_ARB), MP_ROM_INT(ARB) },
    { MP_ROM_QSTR(MP_QSTR_NACK), MP_ROM_INT(NACK) },
    { MP_ROM_QSTR(MP_QSTR_ERR), MP_ROM_INT(ERR) },
    // CAN_FILTER_MODE
    { MP_ROM_QSTR(MP_QSTR_FILTER_RAW_SINGLE), MP_ROM_INT(FILTER_RAW_SINGLE) },
    { MP_ROM_QSTR(MP_QSTR_FILTER_RAW_DUAL), MP_ROM_INT(FILTER_RAW_DUAL) },
    { MP_ROM_QSTR(MP_QSTR_FILTER_ADDRESS), MP_ROM_INT(FILTER_ADDRESS) },
    // CAN_ALERT
    { MP_ROM_QSTR(MP_QSTR_ALERT_TX_IDLE), MP_ROM_INT(TWAI_ALERT_TX_IDLE) },
    { MP_ROM_QSTR(MP_QSTR_ALERT_TX_SUCCESS), MP_ROM_INT(TWAI_ALERT_TX_SUCCESS) },
    { MP_ROM_QSTR(MP_QSTR_ALERT_RX_DATA), MP_ROM_INT(TWAI_ALERT_RX_DATA) },
    { MP_ROM_QSTR(MP_QSTR_ALERT_BELOW_ERR_WARN), MP_ROM_INT(TWAI_ALERT_BELOW_ERR_WARN) },
    { MP_ROM_QSTR(MP_QSTR_ALERT_ERR_ACTIVE), MP_ROM_INT(TWAI_ALERT_ERR_ACTIVE) },
    { MP_ROM_QSTR(MP_QSTR_ALERT_RECOVERY_IN_PROGRESS), MP_ROM_INT(TWAI_ALERT_RECOVERY_IN_PROGRESS) },
    { MP_ROM_QSTR(MP_QSTR_ALERT_BUS_RECOVERED), MP_ROM_INT(TWAI_ALERT_BUS_RECOVERED) },
    { MP_ROM_QSTR(MP_QSTR_ALERT_ARB_LOST), MP_ROM_INT(TWAI_ALERT_ARB_LOST) },
    { MP_ROM_QSTR(MP_QSTR_ALERT_ABOVE_ERR_WARN), MP_ROM_INT(TWAI_ALERT_ABOVE_ERR_WARN) },
    { MP_ROM_QSTR(MP_QSTR_ALERT_BUS_ERROR), MP_ROM_INT(TWAI_ALERT_BUS_ERROR) },
    { MP_ROM_QSTR(MP_QSTR_ALERT_TX_FAILED), MP_ROM_INT(TWAI_ALERT_TX_FAILED) },
    { MP_ROM_QSTR(MP_QSTR_ALERT_RX_QUEUE_FULL), MP_ROM_INT(TWAI_ALERT_RX_QUEUE_FULL) },
    { MP_ROM_QSTR(MP_QSTR_ALERT_ERR_PASS), MP_ROM_INT(TWAI_ALERT_ERR_PASS) },
    { MP_ROM_QSTR(MP_QSTR_ALERT_BUS_OFF), MP_ROM_INT(TWAI_ALERT_BUS_OFF) },
    { MP_ROM_QSTR(MP_QSTR_ALERT_RX_FIFO_OVERRUN), MP_ROM_INT(TWAI_ALERT_RX_FIFO_OVERRUN) },
    { MP_ROM_QSTR(MP_QSTR_ALERT_TX_RETRIED), MP_ROM_INT(TWAI_ALERT_TX_RETRIED) },
    { MP_ROM_QSTR(MP_QSTR_ALERT_PERIPH_RESET), MP_ROM_INT(TWAI_ALERT_PERIPH_RESET) },
    { MP_ROM_QSTR(MP_QSTR_ALERT_ALL), MP_ROM_INT(TWAI_ALERT_ALL) },
    { MP_ROM_QSTR(MP_QSTR_ALERT_NONE), MP_ROM_INT(TWAI_ALERT_NONE) }
};
static MP_DEFINE_CONST_DICT(esp32_can_locals_dict, esp32_can_locals_dict_table);

// Python object definition
MP_DEFINE_CONST_OBJ_TYPE(
    machine_can_type,
    MP_QSTR_CAN,
    MP_TYPE_FLAG_NONE,
    make_new, esp32_can_make_new,
    print, esp32_can_print,
    locals_dict, (mp_obj_dict_t *)&esp32_can_locals_dict
    );

#endif // MICROPY_PY_MACHINE_CAN

#endif // SOC_TWAI_SUPPORTED
