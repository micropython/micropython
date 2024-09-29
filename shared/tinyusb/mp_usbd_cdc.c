/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Ibrahim Abdelkader <iabdalkader@openmv.io>
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

#include "py/runtime.h"
#include "py/mphal.h"
#include "py/stream.h"
#include "extmod/modmachine.h"
#include "shared/runtime/mpirq.h"

#include "mp_usbd.h"
#include "mp_usbd_cdc.h"

#if MICROPY_HW_USB_CDC && MICROPY_HW_ENABLE_USBDEV

static uint8_t cdc_itf_pending; // keep track of cdc interfaces which need attention to poll
static int8_t cdc_connected_flush_delay = 0;
static void usbd_cdc_rx_event_callback(void);
static mp_uint_t machine_usbd_cdc_read(mp_obj_t self_in, void *buf, mp_uint_t size, int *errcode);

// Constants for USBD_CDC.irq trigger.
#define USBD_CDC_IRQ_RX (1)

#ifndef UNUSED
#define UNUSED(X) (void)X      /* To avoid gcc/g++ warnings */
#endif

uintptr_t mp_usbd_cdc_poll_interfaces(uintptr_t poll_flags) {
    uintptr_t ret = 0;
    if (!cdc_itf_pending) {
        // Explicitly run the USB stack as the scheduler may be locked (eg we are in
        // an interrupt handler) while there is data pending.
        mp_usbd_task();
    }

    // any CDC interfaces left to poll?
    // if (cdc_itf_pending && ringbuf_free(&stdin_ringbuf)) {
    //     for (uint8_t itf = 0; itf < 8; ++itf) {
    //         if (cdc_itf_pending & (1 << itf)) {
    //             tud_cdc_rx_cb(itf);
    //             if (!cdc_itf_pending) {
    //                 break;
    //             }
    //         }
    //     }
    // }
    if ((poll_flags & MP_STREAM_POLL_RD) && tud_cdc_available()) {
        ret |= MP_STREAM_POLL_RD;
    }
    if ((poll_flags & MP_STREAM_POLL_WR) &&
        (!tud_cdc_connected() || (tud_cdc_connected() && tud_cdc_write_available() > 0))) {
        // Always allow write when not connected, fifo will retain latest.
        // When connected operate as blocking, only allow if space is available.
        ret |= MP_STREAM_POLL_WR;
    }
    return ret;
}

mp_uint_t mp_usbd_cdc_rx_strn(char *buf, mp_uint_t len) {
    return tud_cdc_read(buf, len);
}

void tud_cdc_rx_cb(uint8_t itf) {
    #if !MICROPY_PY_OS_DUPTERM
    // consume pending USB data immediately to free usb buffer and keep the endpoint from stalling.
    // in case the ringbuffer is full, mark the CDC interface that need attention later on for polling
    cdc_itf_pending &= ~(1 << itf);
    for (uint32_t bytes_avail = tud_cdc_n_available(itf); bytes_avail > 0; --bytes_avail) {
        if (ringbuf_free(&stdin_ringbuf)) {
            int data_char = tud_cdc_read_char();
            #if MICROPY_KBD_EXCEPTION
            if (data_char == mp_interrupt_char) {
                // Clear the ring buffer
                stdin_ringbuf.iget = stdin_ringbuf.iput = 0;
                // and stop
                mp_sched_keyboard_interrupt();
            } else {
                ringbuf_put(&stdin_ringbuf, data_char);
            }
            #else
            ringbuf_put(&stdin_ringbuf, data_char);
            #endif
        } else {
            cdc_itf_pending |= (1 << itf);
            return;
        }
    }
    #endif
    usbd_cdc_rx_event_callback();
}

mp_uint_t mp_usbd_cdc_tx_strn(const char *str, mp_uint_t len) {
    if (!tusb_inited()) {
        return 0;
    }
    size_t i = 0;
    while (i < len) {
        uint32_t n = len - i;
        if (n > CFG_TUD_CDC_EP_BUFSIZE) {
            n = CFG_TUD_CDC_EP_BUFSIZE;
        }
        if (tud_cdc_connected()) {
            // If CDC port is connected but the buffer is full, wait for up to USC_CDC_TIMEOUT ms.
            mp_uint_t t0 = mp_hal_ticks_ms();
            while (n > tud_cdc_write_available() && (mp_uint_t)(mp_hal_ticks_ms() - t0) < MICROPY_HW_USB_CDC_TX_TIMEOUT) {
                mp_event_wait_ms(1);

                // Explicitly run the USB stack as the scheduler may be locked (eg we
                // are in an interrupt handler), while there is data pending.
                mp_usbd_task();
            }
            // Limit write to available space in tx buffer when connected.
            n = MIN(n, tud_cdc_write_available());
            if (n == 0) {
                break;
            }
        }
        // When not connected we always write to usb fifo, ensuring it has latest data.
        uint32_t n2 = tud_cdc_write(str + i, n);
        tud_cdc_write_flush();
        i += n2;
    }
    return i;
}

void tud_sof_cb(uint32_t frame_count) {
    if (--cdc_connected_flush_delay < 0) {
        // Finished on-connection delay, disable SOF interrupt again.
        tud_sof_cb_enable(false);
        tud_cdc_write_flush();
    }
}

#endif

#if MICROPY_HW_ENABLE_USBDEV && ( \
    MICROPY_HW_USB_CDC_1200BPS_TOUCH || \
    MICROPY_HW_USB_CDC || \
    MICROPY_HW_USB_CDC_DTR_RTS_BOOTLOADER)

#if MICROPY_HW_USB_CDC_1200BPS_TOUCH || MICROPY_HW_USB_CDC_DTR_RTS_BOOTLOADER
static mp_sched_node_t mp_bootloader_sched_node;

static void usbd_cdc_run_bootloader_task(mp_sched_node_t *node) {
    mp_hal_delay_ms(250);
    machine_bootloader(0, NULL);
}
#endif

#if MICROPY_HW_USB_CDC_DTR_RTS_BOOTLOADER
static struct {
    bool dtr : 1;
    bool rts : 1;
} prev_line_state = {0};
#endif

void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts) {
    #if MICROPY_HW_USB_CDC
    if (dtr) {
        // A host application has started to open the cdc serial port.
        // Wait a few ms for host to be ready then send tx buffer.
        // High speed connection SOF fires at 125us, full speed at 1ms.
        cdc_connected_flush_delay = (tud_speed_get() == TUSB_SPEED_HIGH) ? 128 : 16;
        tud_sof_cb_enable(true);
    }
    #endif
    #if MICROPY_HW_USB_CDC_DTR_RTS_BOOTLOADER
    if (dtr && !rts) {
        if (prev_line_state.rts && !prev_line_state.dtr) {
            mp_sched_schedule_node(&mp_bootloader_sched_node, usbd_cdc_run_bootloader_task);
        }
    }
    prev_line_state.rts = rts;
    prev_line_state.dtr = dtr;
    #endif
    #if MICROPY_HW_USB_CDC_1200BPS_TOUCH
    if (dtr == false && rts == false) {
        // Device is disconnected.
        cdc_line_coding_t line_coding;
        tud_cdc_n_get_line_coding(itf, &line_coding);
        if (line_coding.bit_rate == 1200) {
            // Delay bootloader jump to allow the USB stack to service endpoints.
            mp_sched_schedule_node(&mp_bootloader_sched_node, usbd_cdc_run_bootloader_task);
        }
    }
    #endif
}

/******************************************************************************/
// MicroPython bindings for USB CDC

/// \moduleref machine
/// \class USBD_CDC - USB virtual comm port
///
/// The USB_CDC class allows creation of an object representing the USB
/// virtual comm port.  It can be used to read and write data over USB to
/// the connected host.

typedef struct _machine_usbd_cdc_obj_t {
    mp_obj_base_t base;
    // usbd_cdc_itf_t *cdc_itf;

} machine_usbd_cdc_obj_t;

// const machine_usbd_cdc_obj_t machine_usbd_cdc_obj[MICROPY_HW_USB_CDC_NUM] = {
//     {{&machine_usbd_cdc_type}, &usb_device.usbd_cdc_itf[0]},
//     #if MICROPY_HW_USB_CDC_NUM >= 2
//     {{&machine_usbd_cdc_type}, &usb_device.usbd_cdc_itf[1]},
//     #endif
//     #if MICROPY_HW_USB_CDC_NUM >= 3
//     {{&machine_usbd_cdc_type}, &usb_device.usbd_cdc_itf[2]},
//     #endif
// };
const machine_usbd_cdc_obj_t machine_usbd_cdc_obj = {{&machine_usbd_cdc_type}};// , &usb_device.usbd_cdc_itf[0]};


static bool machine_usbd_cdc_irq_scheduled;// [MICROPY_HW_USB_CDC_NUM];
static void machine_usbd_cdc_attach_to_repl(const machine_usbd_cdc_obj_t *self, bool attached);

void machine_usbd_cdc_init0(void) {
    // for (size_t i = 0; i < MICROPY_HW_USB_CDC_NUM; ++i) {
    MP_STATE_PORT(machine_usbd_cdc_irq) = mp_const_none;
    machine_usbd_cdc_irq_scheduled = false;
    // }

    #if MICROPY_PY_OS_DUPTERM
    // #if MICROPY_HW_USB_CDC_REPL
    // Activate USB_CDC(0) on dupterm slot 1 for the REPL
    // todo auto detect appropriate slot
    MP_STATE_VM(dupterm_objs[1]) = MP_OBJ_FROM_PTR(&machine_usbd_cdc_obj);
    machine_usbd_cdc_attach_to_repl(&machine_usbd_cdc_obj, true);
    #endif

}

static mp_obj_t machine_usbd_cdc_irq_run(mp_obj_t self_in) {
    // machine_usbd_cdc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    // uint8_t idx = self->cdc_idx;
    mp_obj_t callback = MP_STATE_PORT(machine_usbd_cdc_irq);
    machine_usbd_cdc_irq_scheduled = false;
    if (callback != mp_const_none && tud_cdc_available()) {
        mp_call_function_1(callback, self_in);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_usbd_cdc_irq_run_obj, machine_usbd_cdc_irq_run);

void usbd_cdc_rx_event_callback(void) {
    // uint8_t idx = cdc->cdc_idx;
    mp_obj_t self = MP_OBJ_FROM_PTR(&machine_usbd_cdc_obj);
    mp_obj_t callback = MP_STATE_PORT(machine_usbd_cdc_irq);
    if (callback != mp_const_none && !machine_usbd_cdc_irq_scheduled) {
        machine_usbd_cdc_irq_scheduled = mp_sched_schedule(MP_OBJ_FROM_PTR(&machine_usbd_cdc_irq_run_obj), self);
    }
}

static void machine_usbd_cdc_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    // int id = ((machine_usbd_cdc_obj_t *)MP_OBJ_TO_PTR(self_in))->cdc_itf->cdc_idx;
    mp_printf(print, "USBD_CDC()");
}

void machine_usbd_cdc_set_interrupt_char(int c) {
    tud_cdc_set_wanted_char(c);
}

static void machine_usbd_cdc_attach_to_repl(const machine_usbd_cdc_obj_t *self, bool attached) {
    UNUSED(self);
    // self->attached_to_repl = attached;
    if (attached) {
        // Default behavior is non-blocking when attached to repl
        // self->flow &= ~USBD_CDC_FLOWCONTROL_CTS;
        #if MICROPY_KBD_EXCEPTION
        machine_usbd_cdc_set_interrupt_char(mp_interrupt_char);
        #endif
    } else {
        // self->flow |= USBD_CDC_FLOWCONTROL_CTS;
        #if MICROPY_KBD_EXCEPTION
        machine_usbd_cdc_set_interrupt_char(-1);
        #endif
    }
}

#if MICROPY_KBD_EXCEPTION
void tud_cdc_rx_wanted_cb(uint8_t itf, char wanted_char) {
    // Clear the date buffer
    // stdin_ringbuf.iget = stdin_ringbuf.iput = 0;
    tud_cdc_read_flush();
    // and stop
    mp_sched_keyboard_interrupt();
}
#endif
/// \classmethod \constructor()
/// Create a new USB_CDC object.
static mp_obj_t machine_usbd_cdc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 0, 1, false);

    // TODO raise exception if USB is not configured for CDC

    // int id = (n_args == 0) ? 0 : mp_obj_get_int(args[0]);
    // if (0 <= id && id < MICROPY_HW_USB_CDC_NUM) {
    return MP_OBJ_FROM_PTR(&machine_usbd_cdc_obj);
    // } else {
    //    mp_raise_ValueError(NULL);
    // }
}

// init(*, flow=-1)
static mp_obj_t machine_usbd_cdc_init(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_flow };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_flow, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    // machine_usbd_cdc_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // flow control
    // if (args[ARG_flow].u_int != -1) {
    //    self->flow = args[ARG_flow].u_int;
    // }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_usbd_cdc_init_obj, 1, machine_usbd_cdc_init);

// static mp_obj_t machine_usbd_cdc_setinterrupt(mp_obj_t self_in, mp_obj_t int_chr_in) {
//     mp_hal_set_interrupt_char(mp_obj_get_int(int_chr_in));
//     return mp_const_none;
// }
// static MP_DEFINE_CONST_FUN_OBJ_2(machine_usbd_cdc_setinterrupt_obj, machine_usbd_cdc_setinterrupt);

static mp_obj_t machine_usbd_cdc_isconnected(mp_obj_t self_in) {
    UNUSED(self_in);
    return mp_obj_new_bool(tud_cdc_connected());
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_usbd_cdc_isconnected_obj, machine_usbd_cdc_isconnected);

/// \method any()
/// Return `True` if any characters waiting, else `False`.
static mp_obj_t machine_usbd_cdc_any(mp_obj_t self_in) {
    UNUSED(self_in);
    if (tud_cdc_available()) {
        return mp_const_true;
    } else {
        return mp_const_false;
    }
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_usbd_cdc_any_obj, machine_usbd_cdc_any);

/// \method send(data, *, timeout=5000)
/// Send data over the USB CDC:
///
///   - `data` is the data to send (an integer to send, or a buffer object).
///   - `timeout` is the timeout in milliseconds to wait for the send.
///
/// Return value: number of bytes sent.
// static const mp_arg_t machine_usbd_cdc_send_args[] = {
//     { MP_QSTR_data,    MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
//     { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 5000} },
// };
// #define MACHINE_USBD_CDC_SEND_NUM_ARGS MP_ARRAY_SIZE(machine_usbd_cdc_send_args)

// static mp_obj_t machine_usbd_cdc_send(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
//     // parse args
//     machine_usbd_cdc_obj_t *self = MP_OBJ_TO_PTR(args[0]);
//     mp_arg_val_t vals[MACHINE_USBD_CDC_SEND_NUM_ARGS];
//     mp_arg_parse_all(n_args - 1, args + 1, kw_args, MACHINE_USBD_CDC_SEND_NUM_ARGS, machine_usbd_cdc_send_args, vals);

//     // get the buffer to send from
//     mp_buffer_info_t bufinfo;
//     uint8_t data[1];
//     pyb_buf_get_for_send(vals[0].u_obj, &bufinfo, data);

//     // send the data
//     int ret = usbd_cdc_tx(self, bufinfo.buf, bufinfo.len, vals[1].u_int);

//     return mp_obj_new_int(ret);
// }
// static MP_DEFINE_CONST_FUN_OBJ_KW(machine_usbd_cdc_send_obj, 1, machine_usbd_cdc_send);

/// \method recv(data, *, timeout=5000)
///
/// Receive data on the bus:
///
///   - `data` can be an integer, which is the number of bytes to receive,
///     or a mutable buffer, which will be filled with received bytes.
///   - `timeout` is the timeout in milliseconds to wait for the receive.
///
/// Return value: if `data` is an integer then a new buffer of the bytes received,
/// otherwise the number of bytes read into `data` is returned.
// static mp_obj_t machine_usbd_cdc_recv(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
//     // parse args
//     machine_usbd_cdc_obj_t *self = MP_OBJ_TO_PTR(args[0]);
//     mp_arg_val_t vals[MACHINE_USBD_CDC_SEND_NUM_ARGS];
//     mp_arg_parse_all(n_args - 1, args + 1, kw_args, MACHINE_USBD_CDC_SEND_NUM_ARGS, machine_usbd_cdc_send_args, vals);

//     // get the buffer to receive into
//     vstr_t vstr;
//     mp_obj_t o_ret = pyb_buf_get_for_recv(vals[0].u_obj, &vstr);

//     // receive the data
//     int ret = usbd_cdc_rx(self, (uint8_t *)vstr.buf, vstr.len, vals[1].u_int);

//     // return the received data
//     if (o_ret != MP_OBJ_NULL) {
//         return mp_obj_new_int(ret); // number of bytes read into given buffer
//     } else {
//         vstr.len = ret; // set actual number of bytes read
//         return mp_obj_new_bytes_from_vstr(&vstr); // create a new buffer
//     }
// }
// static MP_DEFINE_CONST_FUN_OBJ_KW(machine_usbd_cdc_recv_obj, 1, machine_usbd_cdc_recv);

static mp_obj_t machine_usbd_cdc_recv(uint n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_data, ARG_timeout };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_dest, MP_ARG_INT | MP_ARG_OBJ | MP_ARG_REQUIRED,  {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_timeout, MP_ARG_INT | MP_ARG_KW_ONLY,  {.u_int = 5000 } },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    vstr_t vstr;
    mp_obj_t o_ret = mp_const_none;
    if (mp_obj_is_int(args[ARG_data].u_obj)) {
        // allocate a new bytearray of given length
        vstr_init_len(&vstr, mp_obj_get_int(args[ARG_data].u_obj));
    } else {
        // get the existing buffer
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(args[ARG_data].u_obj, &bufinfo, MP_BUFFER_WRITE);
        vstr.buf = bufinfo.buf;
        vstr.len = bufinfo.len;
        o_ret = args[ARG_data].u_obj;
    }
    size_t read = 0;
    uint32_t start = HAL_GetTick();
    size_t timeout = args[ARG_timeout].u_int;
    while (read < vstr.len && (HAL_GetTick() - start < timeout)) {

        // int ret = machine_usbd_cdc_read(pos_args[0], vstr.buf, vstr.len - read, &errcode);
        int ret = mp_usbd_cdc_rx_strn((char *)vstr.buf + read, vstr.len - read);

        if (ret == 0) {
            // mp_usbd_task();
        } else {
            read += ret;
        }
        mp_event_wait_ms(timeout);
    }

    // return the received data
    if (o_ret != MP_OBJ_NULL) {
        return mp_obj_new_int(read); // number of bytes read into given buffer
    } else {
        vstr.len = read; // set actual number of bytes read
        return mp_obj_new_bytes_from_vstr(&vstr); // create a new buffer
    }
}
MP_DEFINE_CONST_FUN_OBJ_KW(machine_usbd_cdc_recv_obj, 2, machine_usbd_cdc_recv);

// irq(handler=None, trigger=IRQ_RX, hard=False)
static mp_obj_t machine_usbd_cdc_irq(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_handler, ARG_trigger, ARG_hard };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_handler, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_trigger, MP_ARG_INT, {.u_int = USBD_CDC_IRQ_RX} },
        { MP_QSTR_hard, MP_ARG_BOOL, {.u_bool = false} },
    };
    // machine_usbd_cdc_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (n_args > 1 || kw_args->used != 0) {
        // Check the handler.
        mp_obj_t handler = args[MP_IRQ_ARG_INIT_handler].u_obj;
        if (handler != mp_const_none && !mp_obj_is_callable(handler)) {
            mp_raise_ValueError(MP_ERROR_TEXT("handler must be None or callable"));
        }

        // Check the trigger.
        mp_uint_t trigger = args[MP_IRQ_ARG_INIT_trigger].u_int;
        if (trigger == 0) {
            handler = mp_const_none;
        } else if (trigger != USBD_CDC_IRQ_RX) {
            mp_raise_ValueError(MP_ERROR_TEXT("unsupported trigger"));
        }

        // Check hard/soft.
        if (args[MP_IRQ_ARG_INIT_hard].u_bool) {
            mp_raise_ValueError(MP_ERROR_TEXT("hard unsupported"));
        }

        // Reconfigure the IRQ.
        MP_STATE_PORT(machine_usbd_cdc_irq) = handler;
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_usbd_cdc_irq_obj, 1, machine_usbd_cdc_irq);

static const mp_rom_map_elem_t machine_usbd_cdc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_usbd_cdc_init_obj) },
    // { MP_ROM_QSTR(MP_QSTR_setinterrupt), MP_ROM_PTR(&machine_usbd_cdc_setinterrupt_obj) },
    { MP_ROM_QSTR(MP_QSTR_isconnected), MP_ROM_PTR(&machine_usbd_cdc_isconnected_obj) },
    { MP_ROM_QSTR(MP_QSTR_any), MP_ROM_PTR(&machine_usbd_cdc_any_obj) },
    // { MP_ROM_QSTR(MP_QSTR_send), MP_ROM_PTR(&machine_usbd_cdc_send_obj) },
    { MP_ROM_QSTR(MP_QSTR_recv), MP_ROM_PTR(&machine_usbd_cdc_recv_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_stream_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_stream_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_readline), MP_ROM_PTR(&mp_stream_unbuffered_readline_obj)},
    { MP_ROM_QSTR(MP_QSTR_readlines), MP_ROM_PTR(&mp_stream_unbuffered_readlines_obj)},
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_stream_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&mp_identity_obj) },
    { MP_ROM_QSTR(MP_QSTR_irq), MP_ROM_PTR(&machine_usbd_cdc_irq_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&mp_identity_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&mp_identity_obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&mp_stream___exit___obj) },

    // class constants
    // { MP_ROM_QSTR(MP_QSTR_RTS), MP_ROM_INT(USBD_CDC_FLOWCONTROL_RTS) },
    // { MP_ROM_QSTR(MP_QSTR_CTS), MP_ROM_INT(USBD_CDC_FLOWCONTROL_CTS) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_RX), MP_ROM_INT(USBD_CDC_IRQ_RX) },
};

static MP_DEFINE_CONST_DICT(machine_usbd_cdc_locals_dict, machine_usbd_cdc_locals_dict_table);

static mp_uint_t machine_usbd_cdc_read(mp_obj_t self_in, void *buf, mp_uint_t size, int *errcode) {
    UNUSED(self_in);
    // machine_usbd_cdc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    int ret = mp_usbd_cdc_rx_strn((char *)buf, size);
    if (ret == 0) {
        // return EAGAIN error to indicate non-blocking
        *errcode = MP_EAGAIN;
        return MP_STREAM_ERROR;
    }
    return ret;
}

static mp_uint_t machine_usbd_cdc_write(mp_obj_t self_in, const void *buf, mp_uint_t size, int *errcode) {
    UNUSED(self_in);
    int ret = mp_usbd_cdc_tx_strn((const char *)buf, size);
    if (ret == 0) {
        // return EAGAIN error to indicate non-blocking
        *errcode = MP_EAGAIN;
        return MP_STREAM_ERROR;
    }
    return ret;
}

static mp_uint_t machine_usbd_cdc_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    mp_uint_t ret;
    machine_usbd_cdc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (request == MP_STREAM_POLL) {
        uintptr_t flags = arg;
        ret = mp_usbd_cdc_poll_interfaces(flags);
    } else if (request == MP_STREAM_CLOSE) {
        ret = 0;
    } else if (request == MP_STREAM_REPL_ATTACHED) {
        machine_usbd_cdc_attach_to_repl(self, arg);
        ret = 0;
    } else {
        *errcode = MP_EINVAL;
        ret = MP_STREAM_ERROR;
    }
    return ret;
}

static const mp_stream_p_t machine_usbd_cdc_stream_p = {
    .read = machine_usbd_cdc_read,
    .write = machine_usbd_cdc_write,
    .ioctl = machine_usbd_cdc_ioctl,
};

MP_DEFINE_CONST_OBJ_TYPE(
    machine_usbd_cdc_type,
    MP_QSTR_USBD_CDC,
    MP_TYPE_FLAG_ITER_IS_STREAM,
    make_new, machine_usbd_cdc_make_new,
    print, machine_usbd_cdc_print,
    protocol, &machine_usbd_cdc_stream_p,
    locals_dict, &machine_usbd_cdc_locals_dict
    );

MP_REGISTER_ROOT_POINTER(mp_obj_t machine_usbd_cdc_irq);

#endif
