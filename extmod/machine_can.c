/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Angus Gratton
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
#include "py/objstr.h"
#include "py/runtime.h"

#if MICROPY_PY_MACHINE_CAN

#include <stdlib.h>
#include <string.h>
#include "extmod/modmachine.h"
#include "extmod/machine_can.h"
#include "extmod/machine_can_port.h"

#ifndef MICROPY_HW_NUM_CAN
#define MICROPY_HW_NUM_CAN 1
#endif

#define CAN_MSG_RTR (1 << 0)
#define CAN_MSG_EXTENDED_ID (1 << 1)
#define CAN_MSG_FD_F (1 << 2)
#define CAN_MSG_BRS (1 << 3)

#define CAN_CLASSIC_MAX_LEN 8

// The port provides implementations of the static machine_can_port.h functions in this file.
#include MICROPY_PY_MACHINE_CAN_INCLUDEFILE

// Note: not all possible return values of flags() are allowed as triggers
#define CAN_IRQ_ALLOWED_TRIGGERS (MP_CAN_IRQ_TX | MP_CAN_IRQ_RX | MP_CAN_IRQ_STATE)

// Port can override any of these limits if necessary
#ifndef CAN_TSEG1_MIN
#define CAN_TSEG1_MIN 1
#endif
#ifndef CAN_TSEG1_MAX
#define CAN_TSEG1_MAX 16
#endif
#ifndef CAN_TSEG2_MIN
#define CAN_TSEG2_MIN 1
#endif
#ifndef CAN_TSEG2_MAX
#define CAN_TSEG2_MAX 8
#endif
#ifndef CAN_SJW_MIN
#define CAN_SJW_MIN 1
#endif
#ifndef CAN_SJW_MAX
#define CAN_SJW_MAX 4
#endif

#if MICROPY_HW_ENABLE_FDCAN
// CAN-FD BRS (Baud Rate Switch) default limits
#ifndef CAN_FD_BRS_TSEG1_MIN
#define CAN_FD_BRS_TSEG1_MIN 1
#endif
#ifndef CAN_FD_BRS_TSEG1_MAX
#define CAN_FD_BRS_TSEG1_MAX 32
#endif
#ifndef CAN_FD_BRS_TSEG2_MIN
#define CAN_FD_BRS_TSEG2_MIN 1
#endif
#ifndef CAN_FD_BRS_TSEG2_MAX
#define CAN_FD_BRS_TSEG2_MAX 16
#endif
#ifndef CAN_FD_BRS_SJW_MIN
#define CAN_FD_BRS_SJW_MIN 1
#endif
#ifndef CAN_FD_BRS_SJW_MAX
#define CAN_FD_BRS_SJW_MAX 16
#endif
#endif // MICROPY_HW_ENABLE_FDCAN

#ifndef CAN_FILTERS_STD_EXT_SEPARATE
// Set if the hardware maintains separate filter indexes for Standard vs Extended IDs
#define CAN_FILTERS_STD_EXT_SEPARATE 0
#endif
#ifndef CAN_PORT_PRINT_FUNCTION
// If this is set then the port should define its own function machine_can_print()
#define CAN_PORT_PRINT_FUNCTION 0
#endif

// Non port-specific functions follow

// Calculate baud rate prescaler, and if necessary also find tseg1, tseg2
// to satisfy sample_point
//
static int calculate_brp(int bitrate_nom, int f_clock, int *tseg1, int *tseg2, int sample_point, bool is_fd_brs) {
    bool find_tseg = (*tseg1 == -1); // We need to calculate tseg1, tseg2

    // Set min/max limits for Classic CAN
    int brp_min = CAN_BRP_MIN;
    int brp_max = CAN_BRP_MAX;
    int tseg1_min = CAN_TSEG1_MIN;
    int tseg1_max = CAN_TSEG1_MAX;
    int tseg2_min = CAN_TSEG2_MIN;
    int tseg2_max = CAN_TSEG2_MAX;
    #if MICROPY_HW_ENABLE_FDCAN
    // If CAN-FD controller, set min/max limits for CAN-FD BRS
    if (is_fd_brs) {
        brp_min = CAN_FD_BRS_BRP_MIN;
        brp_max = CAN_FD_BRS_BRP_MAX;
        tseg1_min = CAN_FD_BRS_TSEG1_MIN;
        tseg1_max = CAN_FD_BRS_TSEG1_MAX;
        tseg2_min = CAN_FD_BRS_TSEG2_MIN;
        tseg2_max = CAN_FD_BRS_TSEG2_MAX;
    }
    #else
    (void)is_fd_brs;
    #endif

    int brp_best = brp_min;
    int best_bitrate_err = bitrate_nom; // best case deviation from bitrate_nom, start at max
    int best_sample_err = 10000; // Units of .01%, start at max

    for (int brp = brp_max; brp >= brp_min; brp--) {
        unsigned scaled_clock = f_clock / brp;

        if (find_tseg) {
            // Find the total number of time quanta that gets closest to the nominal bitrate
            int ts_total = (scaled_clock + bitrate_nom / 2) / bitrate_nom;

            if (ts_total < tseg1_min + tseg2_min + 1
                || ts_total > tseg1_max + tseg2_max + 1) {
                // The total time quanta doesn't fit in the allowed range
                continue;
            }

            int bitrate_err = abs((int)bitrate_nom - (int)(scaled_clock / ts_total));

            if (bitrate_err > best_bitrate_err) {
                // This result is worse than our current best bitrate
                continue;
            }

            // Look for tseg1 & tseg2 that come closest to the sample point
            for (int ts1 = tseg1_min; ts1 <= tseg1_max; ts1++) {
                int ts2 = ts_total - 1 - ts1;
                if (ts2 >= tseg2_min && ts2 <= tseg2_max) {
                    int try_sample = 10000 * (1 + ts1) / (1 + ts1 + ts2); // sample point, units of .01%
                    int try_err = abs(try_sample - sample_point * 100);
                    // Priorities for selecting the best:
                    // 1. Smallest bitrate error.
                    // 2. Smallest sample point error.
                    // 3. Shorter (i.e. more total) time quanta (meaning if all else is equal, choose lower brp).
                    if (bitrate_err < best_bitrate_err || try_err <= best_sample_err) {
                        *tseg1 = ts1;
                        *tseg2 = ts2;
                        best_sample_err = try_err;
                        brp_best = brp;
                        best_bitrate_err = bitrate_err;
                    }
                }
            }
        } else {
            // tseg1 and tseg2 already set, find brp with the lowest bitrate error
            int ts_total = *tseg1 + *tseg2 + 1;
            int bitrate_err = abs((int)bitrate_nom - (int)(scaled_clock / ts_total));
            if (bitrate_err <= best_bitrate_err) {
                brp_best = brp;
                best_bitrate_err = bitrate_err;
            }
        }
    }

    if (best_bitrate_err == bitrate_nom) {
        // Didn't find any eligible bitrates
        mp_raise_ValueError(MP_ERROR_TEXT("Unable to calculate BRP for baudrate"));
    }

    return brp_best;
}

static int machine_can_get_actual_bitrate(machine_can_obj_t *self) {
    int f_clock = machine_can_port_f_clock(self);
    return f_clock / self->brp / (1 + self->tseg1 + self->tseg2);
}

static mp_obj_t machine_can_deinit(mp_obj_t self_in) {
    machine_can_obj_t *self = MP_OBJ_TO_PTR(self_in);
    assert(self == MP_STATE_PORT(machine_can_objs)[self->can_idx]); // Should only be one
    self->mp_irq_trigger = 0;
    self->mp_irq_obj = NULL;
    if (self->port) {
        machine_can_update_irqs(self);
        machine_can_port_deinit(self);
        self->port = NULL;
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(machine_can_deinit_obj, machine_can_deinit);

static void machine_can_init_helper(machine_can_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_bitrate, ARG_mode, ARG_sample_point, ARG_sjw, ARG_tseg1, ARG_tseg2, ARG_auto_retransmit};
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_bitrate, MP_ARG_INT | MP_ARG_REQUIRED },
        { MP_QSTR_mode, MP_ARG_INT, {.u_int = MP_CAN_MODE_NORMAL} },
        { MP_QSTR_sample_point, MP_ARG_INT, {.u_int = 75} },
        { MP_QSTR_sjw, MP_ARG_INT, {.u_int = CAN_SJW_MIN } },
        { MP_QSTR_tseg1, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_tseg2, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_auto_retransmit, MP_ARG_BOOL, {.u_bool = true} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Verify arguments

    int bitrate_nom = args[ARG_bitrate].u_int;

    machine_can_mode_t mode = args[ARG_mode].u_int;
    if (mode >= MP_CAN_MODE_MAX || !machine_can_port_supports_mode(self, mode)) {
        mp_raise_ValueError(MP_ERROR_TEXT("mode"));
    }

    int sjw = args[ARG_sjw].u_int;
    if (sjw < CAN_SJW_MIN || sjw > CAN_SJW_MAX) {
        mp_raise_ValueError(MP_ERROR_TEXT("sjw"));
    }

    int tseg1 = args[ARG_tseg1].u_int;
    int tseg2 = args[ARG_tseg2].u_int;
    if ((tseg1 != -1 && (tseg1 < CAN_TSEG1_MIN || tseg1 > CAN_TSEG1_MAX))
        || (tseg1 == -1 && tseg2 != -1)) {
        mp_raise_ValueError(MP_ERROR_TEXT("tseg1"));
    }
    if ((tseg2 != -1 && (tseg2 < CAN_TSEG2_MIN || tseg2 > CAN_TSEG2_MAX))
        || (tseg2 == -1 && tseg1 != -1)) {
        mp_raise_ValueError(MP_ERROR_TEXT("tseg2"));
    }

    int sample_point = args[ARG_sample_point].u_int;
    if (sample_point <= 0 || sample_point >= 100) {
        // Probably can make these values more restrictive
        mp_raise_ValueError(MP_ERROR_TEXT("sample_point"));
    }

    int f_clock = machine_can_port_f_clock(self);

    // This function will also set tseg1 and tseg2 if they are -1
    int brp = calculate_brp(bitrate_nom, f_clock, &tseg1, &tseg2, sample_point, false);

    // Set up the hardware
    self->tseg1 = tseg1;
    self->tseg2 = tseg2;
    self->brp = brp;
    self->sjw = sjw;
    self->auto_retransmit = args[ARG_auto_retransmit].u_bool;
    self->mode = mode;
    memset(&self->counters, 0, sizeof(self->counters));

    if (self->port != NULL) {
        machine_can_port_deinit(self);
    }
    machine_can_port_init(self);
    // This second call may be redundant, depending on machine_can_port_init() on this port
    machine_can_port_set_mode(self, mode);
    self->mode = mode;
}

mp_uint_t machine_can_get_index(mp_obj_t identifier) {
    mp_uint_t can_num;
    if (mp_obj_is_str(identifier)) {
        const char *port = mp_obj_str_get_str(identifier);
        if (0) {
        #ifdef MICROPY_HW_CAN1_NAME
        } else if (strcmp(port, MICROPY_HW_CAN1_NAME) == 0) {
            can_num = 1;
        #endif
        #ifdef MICROPY_HW_CAN2_NAME
        } else if (strcmp(port, MICROPY_HW_CAN2_NAME) == 0) {
            can_num = 2;
        #endif
        #ifdef MICROPY_HW_CAN3_NAME
        } else if (strcmp(port, MICROPY_HW_CAN3_NAME) == 0) {
            can_num = 3;
        #endif
        } else {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("CAN(%s) doesn't exist"), port);
        }
    } else {
        can_num = mp_obj_get_int(identifier);
    }
    if (can_num < 1 || can_num > MICROPY_HW_NUM_CAN) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("CAN(%d) doesn't exist"), can_num);
    }

    // check if the CAN is reserved for system use or not
    if (MICROPY_HW_CAN_IS_RESERVED(can_num)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("CAN(%d) is reserved"), can_num);
    }

    return can_num - 1;
}

static mp_obj_t machine_can_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // work out port
    mp_uint_t can_idx = machine_can_get_index(args[0]);

    machine_can_obj_t *self = MP_STATE_PORT(machine_can_objs)[can_idx];
    if (self == NULL) {
        self = m_new_obj(machine_can_obj_t);

        *self = (machine_can_obj_t) {
            .base = { .type = &machine_can_type },
            .can_idx = can_idx,
        };
        MP_STATE_PORT(machine_can_objs)[can_idx] = self;
    }

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    machine_can_init_helper(self, n_args, args, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t machine_can_init(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    machine_can_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    machine_can_init_helper(self, n_args, pos_args, kw_args);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(machine_can_init_obj, 1, machine_can_init);

void machine_can_deinit_all(void) {
    for (int i = 0; i < MICROPY_HW_NUM_CAN; i++) {
        mp_obj_t can = MP_OBJ_FROM_PTR(MP_STATE_PORT(machine_can_objs)[i]);
        if (can) {
            machine_can_deinit(can);
            MP_STATE_PORT(machine_can_objs)[i] = NULL;
        }
    }
}

// Raise an exception if the CAN controller is not initialised
static void machine_can_check_initialised(machine_can_obj_t *self) {
    // Use self->port being allocated as indicator that controller is initialised
    if (self->port == NULL) {
        mp_raise_OSError(MP_EINVAL);
    }
}

static mp_uint_t can_irq_trigger(mp_obj_t self_in, mp_uint_t new_trigger) {
    machine_can_obj_t *self = MP_OBJ_TO_PTR(self_in);
    machine_can_check_initialised(self);
    self->mp_irq_trigger = new_trigger;
    // Call into port layer to update hardware IRQ config
    machine_can_update_irqs(self);
    return 0;
}

static mp_uint_t can_irq_info(mp_obj_t self_in, mp_uint_t info_type) {
    machine_can_obj_t *self = MP_OBJ_TO_PTR(self_in);
    machine_can_check_initialised(self);
    if (info_type == MP_IRQ_INFO_FLAGS) {
        // Call into port layer to get current irq flags
        return machine_can_port_irq_flags(self);
    } else if (info_type == MP_IRQ_INFO_TRIGGERS) {
        return self->mp_irq_trigger;
    }
    return 0;
}

static const mp_irq_methods_t can_irq_methods = {
    .trigger = can_irq_trigger,
    .info = can_irq_info,
};

static mp_obj_t mp_machine_can_irq(machine_can_obj_t *self, bool any_args, mp_arg_val_t *args) {
    machine_can_check_initialised(self);
    if (self->mp_irq_obj == NULL) {
        self->mp_irq_trigger = 0;
        self->mp_irq_obj = mp_irq_new(&can_irq_methods, MP_OBJ_FROM_PTR(self));
    }

    if (any_args) {
        // TODO: refactor this into a helper to save some code size
        mp_uint_t trigger = args[MP_IRQ_ARG_INIT_trigger].u_int;
        mp_uint_t not_supported = trigger & ~CAN_IRQ_ALLOWED_TRIGGERS;
        if (not_supported) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("trigger 0x%08x unsupported"), not_supported);
        }

        mp_obj_t handler = args[MP_IRQ_ARG_INIT_handler].u_obj;
        if (handler != mp_const_none && !mp_obj_is_callable(handler)) {
            mp_raise_ValueError(MP_ERROR_TEXT("handler must be None or callable"));
        }

        self->mp_irq_obj->handler = handler;
        self->mp_irq_obj->ishard = args[MP_IRQ_ARG_INIT_hard].u_bool;

        can_irq_trigger(MP_OBJ_FROM_PTR(self), trigger);
    }

    return MP_OBJ_FROM_PTR(self->mp_irq_obj);
}

static mp_obj_t machine_can_send(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    machine_can_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    machine_can_check_initialised(self);

    enum { ARG_id, ARG_data, ARG_flags, /*ARG_context*/ };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id,    MP_ARG_REQUIRED | MP_ARG_INT, {} },
        { MP_QSTR_data,  MP_ARG_REQUIRED | MP_ARG_OBJ, {} },
        { MP_QSTR_flags, MP_ARG_INT, {.u_int = 0} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args,
        MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_uint_t can_id = args[ARG_id].u_int;
    mp_uint_t flags = args[ARG_flags].u_int;

    mp_buffer_info_t data;
    mp_get_buffer_raise(args[ARG_data].u_obj, &data, MP_BUFFER_READ);

    if (data.len > machine_can_port_max_data_len(flags)) {
        mp_raise_msg(&mp_type_OverflowError, MP_ERROR_TEXT("data too long"));
    }
    if (can_id >= ((flags & CAN_MSG_FLAG_EXT_ID) ? (1 << 29) : (1 << 11))) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid id"));
    }

    mp_int_t res = machine_can_port_send(self, can_id, data.buf, data.len, flags);

    return MP_OBJ_NEW_SMALL_INT(res);
}
MP_DEFINE_CONST_FUN_OBJ_KW(machine_can_send_obj, 3, machine_can_send);

static mp_obj_t machine_can_cancel_send(mp_obj_t self_in, mp_obj_t arg) {
    machine_can_obj_t *self = MP_OBJ_TO_PTR(self_in);
    machine_can_check_initialised(self);
    mp_uint_t idx = mp_obj_get_uint(arg);
    if (idx >= CAN_TX_QUEUE_LEN) {
        mp_raise_type(&mp_type_IndexError);
    }
    bool res = machine_can_port_cancel_send(self, mp_obj_get_uint(arg));
    return mp_obj_new_bool(res);
}
MP_DEFINE_CONST_FUN_OBJ_2(machine_can_cancel_send_obj, machine_can_cancel_send);

static mp_obj_t machine_can_recv(size_t n_args, const mp_obj_t *args) {
    machine_can_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    machine_can_check_initialised(self);

    uint8_t data[MP_CAN_MAX_LEN];
    size_t dlen = sizeof(data);
    mp_uint_t id = 0;
    mp_uint_t flags = 0;
    mp_uint_t errors = 0;
    mp_obj_array_t *data_memoryview;
    bool use_arg_as_result = n_args > 1 && args[1] != mp_const_none;
    mp_obj_list_t *result = mp_obj_list_optional_arg(use_arg_as_result ? args[1] : mp_const_none, RECV_ARG_LEN);
    mp_obj_t *items = result->items;

    // Validate the memoryview item if list passed in arg
    if (use_arg_as_result) {
        if (!mp_obj_is_type(items[RECV_ARG_DATA], &mp_type_memoryview)) {
            mp_raise_TypeError(NULL);
        }
    }

    // Call the port-specific receive function
    if (!machine_can_port_recv(self, data, &dlen, &id, &flags, &errors)) {
        return mp_const_none; // Nothing to return
    }

    // Create memoryview for the result list, if not passed in
    if (!use_arg_as_result) {
        // Make the result memoryview large enough for the entire result buffer, not the recv result
        // ... it will be resized further down
        // (Potentially wasteful, but means the result can be safely reused as arg to a subsequent call)
        void *backing_buf = m_malloc(MP_CAN_MAX_LEN);
        items[RECV_ARG_DATA] = mp_obj_new_memoryview('B', MP_CAN_MAX_LEN, backing_buf);
    }
    // TODO: length check any memoryview 'result' that was passed in as arg?
    // (may not be possible as I don't think memoryview records the size of its backing buffer)

    data_memoryview = MP_OBJ_TO_PTR(items[RECV_ARG_DATA]); // type of obj was checked already

    if ((flags & CAN_MSG_FLAG_RTR) == 0) {
        memcpy(data_memoryview->items, data, dlen);
    } else {
        // Remote request, return a memoryview of 0s with correct length
        memset(data_memoryview->items, 0, dlen);
    }
    data_memoryview->len = dlen;

    items[RECV_ARG_ID] = MP_OBJ_NEW_SMALL_INT(id);
    items[RECV_ARG_FLAGS] = MP_OBJ_NEW_SMALL_INT(flags);
    items[RECV_ARG_ERRORS] = MP_OBJ_NEW_SMALL_INT(errors);

    return MP_OBJ_FROM_PTR(result);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_can_recv_obj, 1, 2, machine_can_recv);

static mp_obj_t machine_can_set_filters(mp_obj_t self_in, mp_obj_t filters) {
    machine_can_obj_t *self = MP_OBJ_TO_PTR(self_in);
    machine_can_check_initialised(self);
    mp_obj_iter_buf_t iter_buf;
    int idx = 0;
    #if CAN_FILTERS_STD_EXT_SEPARATE
    int idx_ext = 0;
    #endif
    mp_obj_t filter;

    machine_can_port_clear_filters(self);

    if (filters == mp_const_none) {
        return mp_const_none; // Pass None to disable all receive filters
    }

    // Walk the iterable argument and call machine_can_port_set_filter()
    // for each item
    filters = mp_getiter(filters, &iter_buf);
    while ((filter = mp_iternext(filters)) != MP_OBJ_STOP_ITERATION) {
        mp_obj_t *args;
        size_t arg_len;
        mp_obj_get_array(filter, &arg_len, &args);
        if (arg_len != 3) {
            mp_raise_ValueError(MP_ERROR_TEXT("Filter items must have length 3"));
        }
        mp_uint_t flags = mp_obj_get_int(args[2]);
        // This function is expected to raise an exception if filter cannot be set
        machine_can_port_set_filter(self,
            #if CAN_FILTERS_STD_EXT_SEPARATE
            (flags & CAN_MSG_FLAG_EXT_ID) ? idx_ext++ : idx++,
            #else
            idx++,
            #endif
            mp_obj_get_int(args[0]),
            mp_obj_get_int(args[1]),
            flags
            );
    }

    if (idx == 0) {
        // Passing an empty filters list means "allow all" - pass standard IDs
        // via filter 0 and extended IDs via filter 1.
        machine_can_port_set_filter(self, 0, 0, 0, 0);
        machine_can_port_set_filter(self, 1, 0, 0, CAN_MSG_FLAG_EXT_ID);
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(machine_can_set_filters_obj, machine_can_set_filters);

static mp_obj_t machine_can_get_state(mp_obj_t self_in) {
    machine_can_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->port == NULL) {
        return MP_OBJ_NEW_SMALL_INT(MP_CAN_STATE_STOPPED);
    }
    return MP_OBJ_NEW_SMALL_INT(machine_can_port_get_state(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(machine_can_get_state_obj, machine_can_get_state);

static mp_obj_t machine_can_get_counters(size_t n_args, const mp_obj_t *args) {
    machine_can_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    machine_can_counters_t *counters = &self->counters;
    mp_obj_list_t *list = mp_obj_list_optional_arg(n_args > 1 ? args[1] : mp_const_none, 8);
    machine_can_check_initialised(self);
    machine_can_port_update_counters(self);

    // Note: the members of 'counters' are laid out in the same order,
    // so compiler should be able to infer some kind of loop here...
    list->items[0] = MP_OBJ_NEW_SMALL_INT(counters->tec);
    list->items[1] = MP_OBJ_NEW_SMALL_INT(counters->rec);
    list->items[2] = MP_OBJ_NEW_SMALL_INT(counters->num_warning);
    list->items[3] = MP_OBJ_NEW_SMALL_INT(counters->num_passive);
    list->items[4] = MP_OBJ_NEW_SMALL_INT(counters->num_bus_off);
    list->items[5] = MP_OBJ_NEW_SMALL_INT(counters->tx_pending);
    list->items[6] = MP_OBJ_NEW_SMALL_INT(counters->rx_pending);
    list->items[7] = MP_OBJ_NEW_SMALL_INT(counters->rx_overruns);

    return MP_OBJ_FROM_PTR(list);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_can_get_counters_obj, 1, 2, machine_can_get_counters);

static mp_obj_t machine_can_get_timings(size_t n_args, const mp_obj_t *args) {
    machine_can_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    machine_can_timings_t timings = { 0 };
    mp_obj_list_t *list = mp_obj_list_optional_arg(n_args > 1 ? args[1] : mp_const_none, 6);

    list->items[0] = MP_OBJ_NEW_SMALL_INT(machine_can_get_actual_bitrate(self));
    list->items[1] = MP_OBJ_NEW_SMALL_INT(self->sjw);
    list->items[2] = MP_OBJ_NEW_SMALL_INT(self->tseg1);
    list->items[3] = MP_OBJ_NEW_SMALL_INT(self->tseg2);
    #if MICROPY_HW_ENABLE_FDCAN
    mp_obj_list_t *fd_list = mp_obj_list_optional_arg(list->items[4], 4);
    // TODO: CAN-FD timings support
    #else
    list->items[4] = mp_const_none;
    #endif
    list->items[5] = machine_can_port_get_additional_timings(self, n_args > 1 ? list->items[5] : mp_const_none);

    return MP_OBJ_FROM_PTR(list);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_can_get_timings_obj, 1, 2, machine_can_get_timings);


static mp_obj_t machine_can_restart(mp_obj_t self_in) {
    machine_can_obj_t *self = MP_OBJ_TO_PTR(self_in);
    machine_can_check_initialised(self);
    machine_can_port_restart(self);
    memset(&self->counters, 0, sizeof(self->counters));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(machine_can_restart_obj, machine_can_restart);

#if !CAN_PORT_PRINT_FUNCTION
static void machine_can_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_can_obj_t *self = MP_OBJ_TO_PTR(self_in);
    // We don't store the bitrate argument, instead print the real achieved bitrate
    int f_clock = machine_can_port_f_clock(self);
    int actual_bitrate = machine_can_get_actual_bitrate(self);
    mp_printf(print, "CAN(%d, bitrate=%u, mode=%u, sjw=%u, tseg1=%u, tseg2=%u, f_clock=%u, auto_retransmit=%d)",
        self->can_idx + 1,
        actual_bitrate,
        self->mode,
        self->sjw,
        self->tseg1,
        self->tseg2,
        f_clock,
        self->auto_retransmit);
}
#endif

// CAN.irq(handler, trigger, hard)
static mp_obj_t machine_can_irq(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    mp_arg_val_t args[MP_IRQ_ARG_INIT_NUM_ARGS];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_IRQ_ARG_INIT_NUM_ARGS, mp_irq_init_args, args);
    machine_can_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    bool any_args = n_args > 1 || kw_args->used != 0;
    return MP_OBJ_FROM_PTR(mp_machine_can_irq(self, any_args, args));
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_can_irq_obj, 1, machine_can_irq);

static const mp_rom_map_elem_t machine_can_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_can_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_can_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_irq), MP_ROM_PTR(&machine_can_irq_obj) },
    { MP_ROM_QSTR(MP_QSTR_send), MP_ROM_PTR(&machine_can_send_obj) },
    { MP_ROM_QSTR(MP_QSTR_cancel_send), MP_ROM_PTR(&machine_can_cancel_send_obj) },
    { MP_ROM_QSTR(MP_QSTR_recv), MP_ROM_PTR(&machine_can_recv_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_filters), MP_ROM_PTR(&machine_can_set_filters_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_state), MP_ROM_PTR(&machine_can_get_state_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_counters), MP_ROM_PTR(&machine_can_get_counters_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_timings), MP_ROM_PTR(&machine_can_get_timings_obj) },
    { MP_ROM_QSTR(MP_QSTR_restart), MP_ROM_PTR(&machine_can_restart_obj) },

    // Mode enum constants
    { MP_ROM_QSTR(MP_QSTR_MODE_NORMAL), MP_ROM_INT(MP_CAN_MODE_NORMAL) },
    { MP_ROM_QSTR(MP_QSTR_MODE_SLEEP), MP_ROM_INT(MP_CAN_MODE_SLEEP) },
    { MP_ROM_QSTR(MP_QSTR_MODE_LOOPBACK), MP_ROM_INT(MP_CAN_MODE_LOOPBACK) },
    { MP_ROM_QSTR(MP_QSTR_MODE_SILENT), MP_ROM_INT(MP_CAN_MODE_SILENT) },
    { MP_ROM_QSTR(MP_QSTR_MODE_SILENT_LOOPBACK), MP_ROM_INT(MP_CAN_MODE_SILENT_LOOPBACK) },

    // State enum constants
    { MP_ROM_QSTR(MP_QSTR_STATE_STOPPED), MP_ROM_INT(MP_CAN_STATE_STOPPED) },
    { MP_ROM_QSTR(MP_QSTR_STATE_ACTIVE), MP_ROM_INT(MP_CAN_STATE_ACTIVE) },
    { MP_ROM_QSTR(MP_QSTR_STATE_WARNING), MP_ROM_INT(MP_CAN_STATE_WARNING) },
    { MP_ROM_QSTR(MP_QSTR_STATE_PASSIVE), MP_ROM_INT(MP_CAN_STATE_PASSIVE) },
    { MP_ROM_QSTR(MP_QSTR_STATE_BUS_OFF), MP_ROM_INT(MP_CAN_STATE_BUS_OFF) },

    // Message Flag enum constants
    { MP_ROM_QSTR(MP_QSTR_FLAG_RTR), MP_ROM_INT(CAN_MSG_FLAG_RTR) },
    { MP_ROM_QSTR(MP_QSTR_FLAG_EXT_ID), MP_ROM_INT(CAN_MSG_FLAG_EXT_ID) },
    #if MICROPY_HW_ENABLE_FDCAN
    { MP_ROM_QSTR(MP_QSTR_FLAG_FD_F), MP_ROM_INT(CAN_MSG_FLAG_FD_F) },
    { MP_ROM_QSTR(MP_QSTR_FLAG_BRS), MP_ROM_INT(CAN_MSG_FLAG_BRS) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_FLAG_UNORDERED), MP_ROM_INT(CAN_MSG_FLAG_UNORDERED) },

    // Receive Error Flag enum constants
    { MP_ROM_QSTR(MP_QSTR_RECV_ERR_FULL), MP_ROM_INT(CAN_RECV_ERR_FULL) },
    { MP_ROM_QSTR(MP_QSTR_RECV_ERR_OVERRUN), MP_ROM_INT(CAN_RECV_ERR_OVERRUN) },
    #if MICROPY_HW_ENABLE_FDCAN
    { MP_ROM_QSTR(MP_QSTR_RECV_ERR_ESI), MP_ROM_INT(CAN_RECV_ERR_ESI) },
    #endif

    // IRQ enum constants
    { MP_ROM_QSTR(MP_QSTR_IRQ_RX), MP_ROM_INT(MP_CAN_IRQ_RX) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_TX), MP_ROM_INT(MP_CAN_IRQ_TX) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_STATE), MP_ROM_INT(MP_CAN_IRQ_STATE) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_TX_FAILED), MP_ROM_INT(MP_CAN_IRQ_TX_FAILED) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_TX_IDX_SHIFT), MP_ROM_INT(MP_CAN_IRQ_IDX_SHIFT) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_TX_IDX_MASK), MP_ROM_INT(MP_CAN_IRQ_IDX_MASK) },

    // Other constants
    { MP_ROM_QSTR(MP_QSTR_TX_QUEUE_LEN), MP_ROM_INT(CAN_TX_QUEUE_LEN) },
};
static MP_DEFINE_CONST_DICT(machine_can_locals_dict, machine_can_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_can_type,
    MP_QSTR_CAN,
    0,
    make_new, machine_can_make_new,
    print, machine_can_print,
    locals_dict, &machine_can_locals_dict
    );

MP_REGISTER_ROOT_POINTER(void *machine_can_objs[MICROPY_HW_NUM_CAN]);

#endif // MICROPY_PY_MACHINE_CAN
