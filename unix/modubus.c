/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Delio Brignoli
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

#include <assert.h>
#include <libubus.h>
#include <libubox/uloop.h>

#include "py/nlr.h"
#include "py/runtime.h"
#include "py/binary.h"
#include "py/builtin.h"

#define RAISE_ERRNO(err_flag, error_val) \
    { if (err_flag == -1) \
        { nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(error_val))); } }

STATIC void ubus_data_handler(struct ubus_request *req, int type, struct blob_attr *msg);
STATIC void ubus_fd_handler(struct ubus_request *req, int fd);
STATIC void ubus_complete_handler(struct ubus_request *req, int ret);

STATIC mp_obj_t mp_call_function_n_kw_protected(mp_obj_t fun, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    nlr_buf_t nlr;
    mp_obj_t res = mp_const_none;
    if (nlr_push(&nlr) == 0) {
        res = mp_call_function_n_kw(fun, n_args, n_kw, args);
        nlr_pop();
    } else {
        mp_obj_print_exception(&mp_plat_print, MP_OBJ_FROM_PTR(nlr.ret_val));
    }
    return res;
}

typedef struct _mp_obj_ubus_ev_t {
    mp_obj_base_t base;
    mp_obj_t ev_cb;
    struct ubus_event_handler ev;
} mp_obj_ubus_ev_t;

const mp_obj_type_t mp_type_ubus_ev;

typedef struct _mp_obj_ubus_obj_t {
    mp_obj_base_t base;
    struct ubus_object obj;
    struct ubus_object_type type;
    mp_obj_t method_cb;
    struct ubus_method methods[];
} mp_obj_ubus_obj_t;

const mp_obj_type_t mp_type_ubus_obj;

typedef struct _mp_obj_ubus_req_t {
    mp_obj_base_t base;
    mp_obj_t data_cb;
    mp_obj_t fd_cb;
    mp_obj_t complete_cb;
    struct ubus_request req;
} mp_obj_ubus_req_t;

const mp_obj_type_t mp_type_ubus_req;

typedef struct _mp_obj_ubus_subscriber_t {
    mp_obj_base_t base;
    mp_obj_t notification_cb;
    struct ubus_subscriber sub;
} mp_obj_ubus_subscriber_t;

const mp_obj_type_t mp_type_ubus_subscriber;

typedef struct _mp_obj_ubus_ctx_t {
    mp_obj_base_t base;
    bool was_connected_once;
    mp_obj_t diconnection_cb;
    struct ubus_context ctx;
} mp_obj_ubus_ctx_t;

const mp_obj_type_t mp_type_ubus_ctx;

STATIC mp_obj_t ctx_make_new(const mp_obj_type_t *type_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void)type_in;
    (void)n_kw;

    mp_obj_ubus_ctx_t *o = m_new_obj_with_finaliser(mp_obj_ubus_ctx_t);
    o->base.type = &mp_type_ubus_ctx;
    o->was_connected_once = false;
    return MP_OBJ_FROM_PTR(o);
}

STATIC void ctx_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_ubus_ctx_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<_ubus_ctx %p>", &self->ctx);
}

STATIC void connection_lost(struct ubus_context *ctx) {
    mp_obj_ubus_ctx_t *self = container_of(ctx, mp_obj_ubus_ctx_t, ctx);
    if (self->diconnection_cb == mp_const_none)
        return;
    mp_call_function_1_protected(self->diconnection_cb, MP_OBJ_FROM_PTR(self));
}

STATIC mp_obj_t ubus_ctx_connect(mp_obj_t self_in, mp_obj_t path_in, mp_obj_t func_in) {
    int r = -1;
    mp_obj_ubus_ctx_t *self = MP_OBJ_TO_PTR(self_in);
    const char *path = NULL;
    if (path_in != mp_const_none)
        path = mp_obj_str_get_str(path_in);
    /* figure if this is the first time the context is connected */
    if (!self->was_connected_once)
        r = ubus_connect_ctx(&self->ctx, path);
    else
        r = ubus_reconnect(&self->ctx, path);
    RAISE_ERRNO(r, errno);
    if (r == 0) {
        self->was_connected_once = true;
        self->diconnection_cb = func_in;
        self->ctx.connection_lost = connection_lost;
    }
    return MP_OBJ_NEW_SMALL_INT(r);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(ubus_ctx_connect_obj, ubus_ctx_connect);

STATIC mp_obj_t ubus_ctx_shutdown(mp_obj_t self_in) {
    mp_obj_ubus_ctx_t *self = MP_OBJ_TO_PTR(self_in);
    ubus_shutdown(&self->ctx);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ubus_ctx_shutdown_obj, ubus_ctx_shutdown);

STATIC mp_obj_t ubus_ctx_fileno(mp_obj_t self_in) {
    mp_obj_ubus_ctx_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(self->ctx.sock.fd);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ubus_ctx_fileno_obj, ubus_ctx_fileno);

STATIC mp_obj_t ubus_ctx_process_recv_data(mp_obj_t self_in) {
    mp_obj_ubus_ctx_t *self = MP_OBJ_TO_PTR(self_in);
    self->ctx.sock.cb(&self->ctx.sock, ULOOP_READ);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ubus_ctx_process_recv_data_obj, ubus_ctx_process_recv_data);

STATIC mp_obj_t ubus_ctx_process_pending(mp_obj_t self_in) {
    mp_obj_ubus_ctx_t *self = MP_OBJ_TO_PTR(self_in);
    self->ctx.pending_timer.cb(&self->ctx.pending_timer);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ubus_ctx_process_pending_obj, ubus_ctx_process_pending);

STATIC void ubus_lookup_handler(struct ubus_context *ctx,
                      struct ubus_object_data *obj,
                      void *py_func)
{
    mp_obj_t py_cb_func = (mp_obj_t)py_func;
    if (py_cb_func == mp_const_none)
        return;
    mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(3, NULL));
    t->items[0] = mp_obj_new_str(obj->path, strlen(obj->path), false);
    t->items[1] = mp_obj_new_int(obj->id);
    t->items[2] = mp_obj_new_int(obj->type_id);
    mp_call_function_1_protected(py_cb_func, MP_OBJ_FROM_PTR(t));
}

STATIC mp_obj_t ubus_ctx_lookup(mp_obj_t self_in, mp_obj_t path_in, mp_obj_t func_in) {
    mp_obj_ubus_ctx_t *self = MP_OBJ_TO_PTR(self_in);
    const char *path = mp_obj_str_get_str(path_in);
    int r = ubus_lookup(&self->ctx, path, ubus_lookup_handler, func_in);
    RAISE_ERRNO(r, errno);
    return MP_OBJ_NEW_SMALL_INT(r);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(ubus_ctx_lookup_obj, ubus_ctx_lookup);

/* ctx.invoke_async(req, obj_id, method_name, msg=None) */
STATIC mp_obj_t ubus_ctx_invoke_async(size_t n_args, const mp_obj_t *args) {
    struct blob_attr *attr = NULL;
    mp_obj_ubus_ctx_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_obj_ubus_req_t *req = MP_OBJ_TO_PTR(args[4]);
    uint32_t obj_id = mp_obj_get_int(args[1]);
    const char *method = mp_obj_str_get_str(args[2]);

    if (n_args > 3 && args[3] != mp_const_none) {
        mp_buffer_info_t buf_info = {0};
        mp_get_buffer_raise(args[3], &buf_info, MP_BUFFER_READ);
        attr = alloca(buf_info.len+sizeof(*attr));
        attr->id_len = cpu_to_be32(buf_info.len+sizeof(*attr));
        memcpy(attr->data, buf_info.buf, buf_info.len);
    }

    int r = ubus_invoke_async(&self->ctx, obj_id, method, attr, &req->req);
    RAISE_ERRNO(r, errno);
    if (r == 0) {
        req->req.data_cb = ubus_data_handler;
        req->req.complete_cb = ubus_complete_handler;
        req->req.fd_cb = ubus_fd_handler;
    }
    return MP_OBJ_NEW_SMALL_INT(r);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ubus_ctx_invoke_async_obj, 4, 5, ubus_ctx_invoke_async);

STATIC mp_obj_t ubus_ctx_complete_request_async(mp_obj_t self_in, mp_obj_t req_in) {
    mp_obj_ubus_ctx_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_ubus_req_t *req = MP_OBJ_TO_PTR(req_in);
    ubus_complete_request_async(&self->ctx, &req->req);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ubus_ctx_complete_request_async_obj, ubus_ctx_complete_request_async);

STATIC mp_obj_t ubus_ctx_abort_request(mp_obj_t self_in, mp_obj_t req_in) {
    mp_obj_ubus_ctx_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_ubus_req_t *req = MP_OBJ_TO_PTR(req_in);
    ubus_abort_request(&self->ctx, &req->req);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ubus_ctx_abort_request_obj, ubus_ctx_abort_request);

STATIC mp_obj_t ubus_ctx_send_reply(mp_obj_t self_in, mp_obj_t req_in, mp_obj_t msg_in) {
    mp_buffer_info_t buf_info = {0};
    mp_obj_ubus_ctx_t *self = MP_OBJ_TO_PTR(self_in);
    struct ubus_request_data *_req = (struct ubus_request_data *)mp_obj_get_int(req_in);

    mp_get_buffer_raise(msg_in, &buf_info, MP_BUFFER_READ);
    struct blob_attr *attr = alloca(buf_info.len+sizeof(*attr));
    attr->id_len = cpu_to_be32(buf_info.len+sizeof(*attr));
    memcpy(attr->data, buf_info.buf, buf_info.len);

    int r = ubus_send_reply(&self->ctx, _req, attr);
    RAISE_ERRNO(r, errno);
    return MP_OBJ_NEW_SMALL_INT(r);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(ubus_ctx_send_reply_obj, ubus_ctx_send_reply);

STATIC mp_obj_t ubus_ctx_add_object(mp_obj_t self_in, mp_obj_t obj_in) {
    mp_obj_ubus_ctx_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_ubus_obj_t *obj = MP_OBJ_TO_PTR(obj_in);
    if (obj->obj.id)
        return MP_OBJ_NEW_SMALL_INT(0);
    int r = ubus_add_object(&self->ctx, &obj->obj);
    RAISE_ERRNO(r, errno);
    return MP_OBJ_NEW_SMALL_INT(r);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ubus_ctx_add_object_obj, ubus_ctx_add_object);

STATIC mp_obj_t ubus_ctx_remove_object(mp_obj_t self_in, mp_obj_t obj_in) {
    mp_obj_ubus_ctx_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_ubus_obj_t *obj = MP_OBJ_TO_PTR(obj_in);
    int r = ubus_remove_object(&self->ctx, &obj->obj);
    RAISE_ERRNO(r, errno);
    return MP_OBJ_NEW_SMALL_INT(r);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ubus_ctx_remove_object_obj, ubus_ctx_remove_object);

STATIC mp_obj_t ubus_ctx_register_subscriber(mp_obj_t self_in, mp_obj_t sub_in) {
    mp_obj_ubus_ctx_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_ubus_subscriber_t *sub = MP_OBJ_TO_PTR(sub_in);
    if (sub->sub.obj.id)
        return MP_OBJ_NEW_SMALL_INT(0);
    int r = ubus_register_subscriber(&self->ctx, &sub->sub);
    RAISE_ERRNO(r, errno);
    return MP_OBJ_NEW_SMALL_INT(r);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ubus_ctx_register_subscriber_obj, ubus_ctx_register_subscriber);

STATIC mp_obj_t ubus_ctx_unregister_subscriber(mp_obj_t self_in, mp_obj_t sub_in) {
    mp_obj_ubus_ctx_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_ubus_subscriber_t *sub = MP_OBJ_TO_PTR(sub_in);
    int r = ubus_unregister_subscriber(&self->ctx, &sub->sub);
    RAISE_ERRNO(r, errno);
    return MP_OBJ_NEW_SMALL_INT(r);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ubus_ctx_unregister_subscriber_obj, ubus_ctx_unregister_subscriber);

STATIC mp_obj_t ubus_ctx_subscribe(mp_obj_t self_in, mp_obj_t sub_in, mp_obj_t obj_id_in) {
    mp_obj_ubus_ctx_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_ubus_subscriber_t *sub = MP_OBJ_TO_PTR(sub_in);
    uint32_t obj_id = mp_obj_int_get_checked(obj_id_in);
    int r = ubus_subscribe(&self->ctx, &sub->sub, obj_id);
    RAISE_ERRNO(r, errno);
    return MP_OBJ_NEW_SMALL_INT(r);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(ubus_ctx_subscribe_obj, ubus_ctx_subscribe);

STATIC mp_obj_t ubus_ctx_unsubscribe(mp_obj_t self_in, mp_obj_t sub_in, mp_obj_t obj_id_in) {
    mp_obj_ubus_ctx_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_ubus_subscriber_t *sub = MP_OBJ_TO_PTR(sub_in);
    uint32_t obj_id = mp_obj_int_get_checked(obj_id_in);
    int r = ubus_unsubscribe(&self->ctx, &sub->sub, obj_id);
    RAISE_ERRNO(r, errno);
    return MP_OBJ_NEW_SMALL_INT(r);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(ubus_ctx_unsubscribe_obj, ubus_ctx_unsubscribe);

/* ctx.notify(obj, notification_str, msg=None) */
STATIC mp_obj_t ubus_ctx_notify(size_t n_args, const mp_obj_t *args) {
    struct blob_attr *attr = NULL;
    mp_obj_ubus_ctx_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_obj_ubus_obj_t *obj = MP_OBJ_TO_PTR(args[1]);
    if (!obj->obj.has_subscribers)
        return MP_OBJ_NEW_SMALL_INT(0);
    const char *notification = mp_obj_str_get_str(args[2]);
    if (n_args > 3 && args[3] != mp_const_none) {
        mp_buffer_info_t buf_info = {0};
        mp_obj_t msg_in = args[3];
        mp_get_buffer_raise(msg_in, &buf_info, MP_BUFFER_READ);
        attr = alloca(buf_info.len+sizeof(*attr));
        attr->id_len = cpu_to_be32(buf_info.len+sizeof(*attr));
        memcpy(attr->data, buf_info.buf, buf_info.len);
    }
    int r = ubus_notify(&self->ctx, &obj->obj, notification, attr, -1);
    RAISE_ERRNO(r, errno);
    return MP_OBJ_NEW_SMALL_INT(r);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ubus_ctx_notify_obj, 3, 4, ubus_ctx_notify);

STATIC mp_obj_t ubus_ctx_send_event(mp_obj_t self_in, mp_obj_t event_in, mp_obj_t msg_in) {
    mp_buffer_info_t buf_info = {0};
    mp_obj_ubus_ctx_t *self = MP_OBJ_TO_PTR(self_in);
    const char *event = mp_obj_str_get_str(event_in);
    mp_get_buffer_raise(msg_in, &buf_info, MP_BUFFER_READ);
    struct blob_attr *attr = alloca(buf_info.len+sizeof(*attr));
    attr->id_len = cpu_to_be32(buf_info.len+sizeof(*attr));
    memcpy(attr->data, buf_info.buf, buf_info.len);
    int r = ubus_send_event(&self->ctx, event, attr);
    RAISE_ERRNO(r, errno);
    return MP_OBJ_NEW_SMALL_INT(r);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(ubus_ctx_send_event_obj, ubus_ctx_send_event);

STATIC mp_obj_t ubus_ctx_register_event_handler(mp_obj_t self_in, mp_obj_t ev_in, mp_obj_t pattern_in) {
    mp_obj_ubus_ctx_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_ubus_ev_t *ev = MP_OBJ_TO_PTR(ev_in);
    const char *pattern = mp_obj_str_get_str(pattern_in);
    int r = ubus_register_event_handler(&self->ctx, &ev->ev, pattern);
    RAISE_ERRNO(r, errno);
    return MP_OBJ_NEW_SMALL_INT(r);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(ubus_ctx_register_event_handler_obj, ubus_ctx_register_event_handler);

STATIC mp_obj_t ubus_ctx_unregister_event_handler(mp_obj_t self_in, mp_obj_t ev_in) {
    mp_obj_ubus_ctx_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_ubus_ev_t *ev = MP_OBJ_TO_PTR(ev_in);
    int r = ubus_unregister_event_handler(&self->ctx, &ev->ev);
    RAISE_ERRNO(r, errno);
    return MP_OBJ_NEW_SMALL_INT(r);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ubus_ctx_unregister_event_handler_obj, ubus_ctx_unregister_event_handler);

STATIC const mp_rom_map_elem_t ubus_ctx_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___del__), (mp_obj_t)&ubus_ctx_shutdown_obj },

    { MP_ROM_QSTR(MP_QSTR_connect), MP_ROM_PTR(&ubus_ctx_connect_obj) },
    { MP_ROM_QSTR(MP_QSTR_shutdown), MP_ROM_PTR(&ubus_ctx_shutdown_obj) },

    { MP_ROM_QSTR(MP_QSTR_fileno), MP_ROM_PTR(&ubus_ctx_fileno_obj) },
    { MP_ROM_QSTR(MP_QSTR_process_recv_data), MP_ROM_PTR(&ubus_ctx_process_recv_data_obj) },
    { MP_ROM_QSTR(MP_QSTR_process_pending), MP_ROM_PTR(&ubus_ctx_process_pending_obj) },

    { MP_ROM_QSTR(MP_QSTR_lookup), MP_ROM_PTR(&ubus_ctx_lookup_obj) },
    { MP_ROM_QSTR(MP_QSTR_invoke_async), MP_ROM_PTR(&ubus_ctx_invoke_async_obj) },
    { MP_ROM_QSTR(MP_QSTR_complete_request_async), MP_ROM_PTR(&ubus_ctx_complete_request_async_obj) },
    { MP_ROM_QSTR(MP_QSTR_abort_request), MP_ROM_PTR(&ubus_ctx_abort_request_obj) },
    { MP_ROM_QSTR(MP_QSTR_send_reply), MP_ROM_PTR(&ubus_ctx_send_reply_obj) },

    { MP_ROM_QSTR(MP_QSTR_add_object), MP_ROM_PTR(&ubus_ctx_add_object_obj) },
    { MP_ROM_QSTR(MP_QSTR_remove_object), MP_ROM_PTR(&ubus_ctx_remove_object_obj) },

    { MP_ROM_QSTR(MP_QSTR_register_subscriber), MP_ROM_PTR(&ubus_ctx_register_subscriber_obj) },
    { MP_ROM_QSTR(MP_QSTR_unregister_subscriber), MP_ROM_PTR(&ubus_ctx_unregister_subscriber_obj) },
    { MP_ROM_QSTR(MP_QSTR_subscribe), MP_ROM_PTR(&ubus_ctx_subscribe_obj) },
    { MP_ROM_QSTR(MP_QSTR_unsubscribe), MP_ROM_PTR(&ubus_ctx_unsubscribe_obj) },
    { MP_ROM_QSTR(MP_QSTR_notify), MP_ROM_PTR(&ubus_ctx_notify_obj) },

    { MP_ROM_QSTR(MP_QSTR_send_event), MP_ROM_PTR(&ubus_ctx_send_event_obj) },
    { MP_ROM_QSTR(MP_QSTR_register_event_handler), MP_ROM_PTR(&ubus_ctx_register_event_handler_obj) },
    { MP_ROM_QSTR(MP_QSTR_unregister_event_handler), MP_ROM_PTR(&ubus_ctx_unregister_event_handler_obj) },
};
STATIC MP_DEFINE_CONST_DICT(ubus_ctx_dict, ubus_ctx_dict_table);

STATIC mp_int_t ubus_ctx_get_buffer(mp_obj_t obj, mp_buffer_info_t *bufinfo, mp_uint_t flags) {
    mp_obj_ubus_ctx_t *self = MP_OBJ_TO_PTR(obj);
    bufinfo->buf = &self->ctx;
    bufinfo->len = sizeof(self->ctx);
    bufinfo->typecode = BYTEARRAY_TYPECODE;
    return 0;
}

const mp_obj_type_t mp_type_ubus_ctx = {
    { &mp_type_type },
    .name = MP_QSTR_ctx,
    .print = ctx_print,
    .make_new = ctx_make_new,
    .buffer_p = {.get_buffer = ubus_ctx_get_buffer,},
    .locals_dict = (mp_obj_dict_t*)&ubus_ctx_dict,
};


STATIC void subscriber_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_ubus_subscriber_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<_ubus_subscriber %p>", &self->sub);
}

STATIC mp_int_t ubus_subscriber_get_buffer(mp_obj_t obj, mp_buffer_info_t *bufinfo, mp_uint_t flags) {
    mp_obj_ubus_subscriber_t *self = MP_OBJ_TO_PTR(obj);
    bufinfo->buf = &self->sub;
    bufinfo->len = sizeof(self->sub);
    bufinfo->typecode = BYTEARRAY_TYPECODE;
    return 0;
}

STATIC int ubus_notification_handler(struct ubus_context *ctx, struct ubus_object *obj,
                  struct ubus_request_data *req,
                  const char *method, struct blob_attr *msg) {
    mp_obj_ubus_ctx_t *_ctx = container_of(ctx, mp_obj_ubus_ctx_t, ctx);
    mp_obj_ubus_subscriber_t *_sub = container_of(obj, mp_obj_ubus_subscriber_t, sub);
    if (_sub->notification_cb == mp_const_none)
        return 0;
    const mp_obj_t args[4] = {
        MP_OBJ_FROM_PTR(_ctx),
        MP_OBJ_FROM_PTR(_sub),
        mp_obj_new_str(method, strlen(method), false),
        msg ? mp_obj_new_bytes(blob_data(msg), blob_len(msg)) : mp_const_none,
    };
    mp_call_function_n_kw_protected(_sub->notification_cb, 4, 0, args);
    return 0;
}

/* subcriber_obj = ctx.new_subscriber(notification_callback=None) */
/* notification_callback(ctx, subcriber_obj, obj_id, notification_str, msg) */
STATIC mp_obj_t subscriber_make_new(const mp_obj_type_t *type_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void)type_in;
    (void)n_kw;

    mp_obj_ubus_subscriber_t *o = m_new_obj(mp_obj_ubus_subscriber_t);
    memset(&o->sub, 0, sizeof(o->sub));
    o->base.type = &mp_type_ubus_subscriber;
    o->sub.cb = ubus_notification_handler;
    o->notification_cb = mp_const_none;
    if (n_args > 0) {
        o->notification_cb = args[0];
    }
    return MP_OBJ_FROM_PTR(o);
}

const mp_obj_type_t mp_type_ubus_subscriber = {
    { &mp_type_type },
    .name = MP_QSTR_subscriber,
    .print = subscriber_print,
    .make_new = subscriber_make_new,
    .buffer_p = {.get_buffer = ubus_subscriber_get_buffer,},
};

STATIC void ubus_event_handler(struct ubus_context *ctx, struct ubus_event_handler *ev,
                        const char *type, struct blob_attr *msg) {
    mp_obj_ubus_ctx_t *_ctx = container_of(ctx, mp_obj_ubus_ctx_t, ctx);
    mp_obj_ubus_ev_t *_ev = container_of(ev, mp_obj_ubus_ev_t, ev);
    if (_ev->ev_cb == mp_const_none)
        return;
    const mp_obj_t args[4] = {
        MP_OBJ_FROM_PTR(_ctx),
        MP_OBJ_FROM_PTR(_ev),
        mp_obj_new_str(type, strlen(type), false),
        msg ? mp_obj_new_bytes(blob_data(msg), blob_len(msg)) : mp_const_none,
    };
    mp_call_function_n_kw_protected(_ev->ev_cb, 4, 0, args);
}

STATIC mp_obj_t ev_make_new(const mp_obj_type_t *type_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void)type_in;
    (void)n_kw;

    mp_obj_ubus_ev_t *o = m_new_obj(mp_obj_ubus_ev_t);
    memset(&o->ev, 0, sizeof(o->ev));
    o->base.type = &mp_type_ubus_ev;
    o->ev.cb = ubus_event_handler;
    o->ev_cb = mp_const_none;
    if (n_args > 0) {
        o->ev_cb = args[0];
    }
    return MP_OBJ_FROM_PTR(o);
}

STATIC void ev_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_ubus_ev_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<_ubus_ev %p>", &self->ev);
}

STATIC mp_int_t ubus_ev_get_buffer(mp_obj_t obj, mp_buffer_info_t *bufinfo, mp_uint_t flags) {
    mp_obj_ubus_ev_t *self = MP_OBJ_TO_PTR(obj);
    bufinfo->buf = &self->ev;
    bufinfo->len = sizeof(self->ev);
    bufinfo->typecode = BYTEARRAY_TYPECODE;
    return 0;
}

const mp_obj_type_t mp_type_ubus_ev = {
    { &mp_type_type },
    .name = MP_QSTR_event_handler,
    .print = ev_print,
    .make_new = ev_make_new,
    .buffer_p = {.get_buffer = ubus_ev_get_buffer,},
};

STATIC void ubus_data_handler(struct ubus_request *req, int type, struct blob_attr *msg) {
    mp_obj_ubus_req_t *_req = container_of(req, mp_obj_ubus_req_t, req);
    mp_obj_ubus_ctx_t *_ctx = container_of(_req->req.ctx, mp_obj_ubus_ctx_t, ctx);
    if (_req->data_cb == mp_const_none)
        return;
    const mp_obj_t args[4] = {
        MP_OBJ_FROM_PTR(_ctx),
        MP_OBJ_FROM_PTR(_req),
        MP_OBJ_NEW_SMALL_INT(type),
        msg ? mp_obj_new_bytes(blob_data(msg), blob_len(msg)) : mp_const_none,
    };
    mp_call_function_n_kw_protected(_req->data_cb, 4, 0, args);
}

STATIC void ubus_fd_handler(struct ubus_request *req, int fd) {
    mp_obj_ubus_req_t *_req = container_of(req, mp_obj_ubus_req_t, req);
    mp_obj_ubus_ctx_t *_ctx = container_of(_req->req.ctx, mp_obj_ubus_ctx_t, ctx);
    if (_req->fd_cb == mp_const_none)
        return;
    const mp_obj_t args[3] = {
        MP_OBJ_FROM_PTR(_ctx),
        MP_OBJ_FROM_PTR(_req),
        MP_OBJ_NEW_SMALL_INT(fd),
    };
    mp_call_function_n_kw_protected(_req->fd_cb, 3, 0, args);
}

STATIC void ubus_complete_handler(struct ubus_request *req, int ret) {
    mp_obj_ubus_req_t *_req = container_of(req, mp_obj_ubus_req_t, req);
    mp_obj_ubus_ctx_t *_ctx = container_of(_req->req.ctx, mp_obj_ubus_ctx_t, ctx);
    if (_req->complete_cb == mp_const_none)
        return;
    const mp_obj_t args[3] = {
        MP_OBJ_FROM_PTR(_ctx),
        MP_OBJ_FROM_PTR(_req),
        MP_OBJ_NEW_SMALL_INT(ret),
    };
    mp_call_function_n_kw_protected(_req->complete_cb, 3, 0, args);
}

STATIC mp_obj_t req_make_new(const mp_obj_type_t *type_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void)type_in;
    (void)n_kw;

    mp_obj_ubus_req_t *o = m_new_obj(mp_obj_ubus_req_t);
    memset(&o->req, 0, sizeof(o->req));
    o->base.type = &mp_type_ubus_req;
    o->complete_cb = mp_const_none;
    o->data_cb = mp_const_none;
    o->fd_cb = mp_const_none;
    if (n_args > 0)
        o->complete_cb = args[0];
    if (n_args > 1)
        o->data_cb = args[1];
    if (n_args > 2)
        o->fd_cb = args[2];

    return MP_OBJ_FROM_PTR(o);
}

STATIC void req_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_ubus_req_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<_ubus_req %p>", &self->req);
}

STATIC mp_int_t ubus_req_get_buffer(mp_obj_t obj, mp_buffer_info_t *bufinfo, mp_uint_t flags) {
    mp_obj_ubus_req_t *self = MP_OBJ_TO_PTR(obj);
    bufinfo->buf = &self->req;
    bufinfo->len = sizeof(self->req);
    bufinfo->typecode = BYTEARRAY_TYPECODE;
    return 0;
}

const mp_obj_type_t mp_type_ubus_req = {
    { &mp_type_type },
    .name = MP_QSTR_request,
    .print = req_print,
    .make_new = req_make_new,
    .buffer_p = {.get_buffer = ubus_req_get_buffer,},
};

STATIC int obj_method_handler(struct ubus_context *ctx, struct ubus_object *obj,
                  struct ubus_request_data *req,
                  const char *method, struct blob_attr *msg)
{
    mp_obj_ubus_ctx_t *_ctx = container_of(ctx, mp_obj_ubus_ctx_t, ctx);
    mp_obj_ubus_obj_t *_obj = container_of(obj, mp_obj_ubus_obj_t, obj);

    assert(_obj->method_cb != mp_const_none);

    const mp_obj_t args[5] = {
        MP_OBJ_FROM_PTR(_ctx),
        MP_OBJ_FROM_PTR(_obj),
        mp_obj_new_int((intptr_t)req),
        mp_obj_new_str(method, strlen(method), false),
        msg ? mp_obj_new_bytes(blob_data(msg), blob_len(msg)) : mp_const_none,
    };
    mp_obj_t res = mp_call_function_n_kw_protected(_obj->method_cb, 5, 0, args);
    if (res != mp_const_none) {
        return mp_obj_int_get_truncated(res);
    }
    return 0;
}

STATIC mp_obj_t obj_make_new(const mp_obj_type_t *type_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void)type_in;
    (void)n_kw;

    if (n_args < 2)
        nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError, "function expects 3 arguments"));

    /* mp_int_t n_methods = MP_OBJ_SMALL_INT_VALUE(mp_obj_len(args[2])); */
    mp_int_t n_methods = 2;
    mp_obj_ubus_obj_t *o = m_new_obj_var(mp_obj_ubus_obj_t, struct ubus_method, n_methods);
    memset(&o->obj, 0, sizeof(o->obj));
    memset(&o->type, 0, sizeof(o->type));
    memset(o->methods, 0, sizeof(*o->methods)*n_methods);
    o->base.type = &mp_type_ubus_obj;
    o->method_cb = args[1];
    o->obj.type = &o->type;
    o->obj.name = mp_obj_str_get_str(args[0]);
    o->obj.n_methods = 1;
    o->obj.methods = &o->methods[0];
    o->methods[0].name = NULL;
    o->methods[0].handler = obj_method_handler;
    o->type.n_methods = 1;
    o->type.methods = &o->methods[1];
    o->methods[1].name = "*";
    if (n_args > 2)
        o->type.id = mp_obj_int_get_checked(args[2]);

    return MP_OBJ_FROM_PTR(o);
}

STATIC void obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_ubus_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<_ubus_obj %p>", &self->obj);
}

STATIC mp_int_t ubus_obj_get_buffer(mp_obj_t obj, mp_buffer_info_t *bufinfo, mp_uint_t flags) {
    mp_obj_ubus_obj_t *self = MP_OBJ_TO_PTR(obj);
    bufinfo->buf = &self->obj;
    bufinfo->len = sizeof(self->obj);
    bufinfo->typecode = BYTEARRAY_TYPECODE;
    return 0;
}

const mp_obj_type_t mp_type_ubus_obj = {
    { &mp_type_type },
    .name = MP_QSTR_object,
    .print = obj_print,
    .make_new = obj_make_new,
    .buffer_p = {.get_buffer = ubus_obj_get_buffer,},
};

STATIC mp_obj_t blobmsg_to_sequence(struct blob_attr *attr, size_t buflen);

STATIC mp_obj_t append_blobmsg_element(mp_obj_t list, struct blob_attr *attr) {
    bool extended = blob_is_extended(attr);
    uint32_t blob_type = blob_id(attr);
    mp_obj_t elem = mp_const_none;
    void *data;
    size_t len;

    data = blobmsg_data(attr);
    len = blobmsg_data_len(attr);

    if (extended) {
        switch(blob_type) {
        case BLOBMSG_TYPE_UNSPEC:
            elem = mp_obj_new_bytes(data, len);
            break;
        case BLOBMSG_TYPE_BOOL:
            elem = *(uint8_t *)data ? mp_const_true : mp_const_false;
            break;
        case BLOBMSG_TYPE_INT16:
            elem = MP_OBJ_NEW_SMALL_INT(*(uint16_t *)data);
            break;
        case BLOBMSG_TYPE_INT32:
            elem = mp_obj_new_int(*(uint16_t *)data);
            break;
        case BLOBMSG_TYPE_INT64:
            elem = mp_obj_new_int(*(uint64_t *)data);
            break;
        case BLOBMSG_TYPE_STRING:
            elem = mp_obj_new_str(data, len-1, false);
            break;
        case BLOBMSG_TYPE_ARRAY:
            elem = blobmsg_to_sequence(data, len);
            break;
        case BLOBMSG_TYPE_TABLE:
            elem = blobmsg_to_sequence(data, len);
            break;
        }
    } else {
        switch(blob_type) {
        case BLOB_ATTR_UNSPEC:
        case BLOB_ATTR_BINARY:
            elem = mp_obj_new_bytes(data, len);
            break;
        case BLOB_ATTR_NESTED:
            elem = blobmsg_to_sequence(data, len);
            break;
        case BLOB_ATTR_INT8:
            elem = MP_OBJ_NEW_SMALL_INT(*(uint8_t *)data);
            break;
        case BLOB_ATTR_INT16:
            elem = MP_OBJ_NEW_SMALL_INT(*(uint16_t *)data);
            break;
        case BLOB_ATTR_INT32:
            elem = mp_obj_new_int(*(uint16_t *)data);
            break;
        case BLOB_ATTR_INT64:
            elem = mp_obj_new_int(*(uint64_t *)data);
            break;
        case BLOB_ATTR_STRING:
            elem = mp_obj_new_str(data, len-1, false);
            break;
        }
    }

    /* in a tuple with name or just the data */
    const char *name = blobmsg_name(attr);
    if (extended && name) {
        size_t slen = strlen(name);
        if (!slen)
            goto append;
        mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(2, NULL));
        t->items[0] = mp_obj_new_str(name, slen, false);
        t->items[1] = elem;
        elem = MP_OBJ_FROM_PTR(t);
    }

append:
    mp_obj_list_append(list, elem);
    return list;
}

STATIC mp_obj_t blobmsg_to_sequence(struct blob_attr *attr, size_t buflen) {
    struct blob_attr *pos;
    size_t rem = buflen;
    mp_obj_t l = mp_obj_new_list(0, NULL);

    __blob_for_each_attr(pos, attr, rem) {
        append_blobmsg_element(l, pos);
    }
    return l;
}

STATIC mp_obj_t ubus_blob_decode(mp_obj_t blob_in) {
    mp_buffer_info_t buf_info = {0};
    mp_get_buffer_raise(blob_in, &buf_info, MP_BUFFER_READ);
    return blobmsg_to_sequence(buf_info.buf, buf_info.len);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ubus_blob_decode_obj, ubus_blob_decode);


STATIC const mp_rom_map_elem_t mp_module_ubus_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR__ubus) },
    { MP_ROM_QSTR(MP_QSTR_ctx), MP_ROM_PTR(&mp_type_ubus_ctx) },
    { MP_ROM_QSTR(MP_QSTR_subscriber), MP_ROM_PTR(&mp_type_ubus_subscriber) },
    { MP_ROM_QSTR(MP_QSTR_event_handler), MP_ROM_PTR(&mp_type_ubus_ev) },
    { MP_ROM_QSTR(MP_QSTR_request), MP_ROM_PTR(&mp_type_ubus_req) },
    { MP_ROM_QSTR(MP_QSTR_object), MP_ROM_PTR(&mp_type_ubus_obj) },
    { MP_ROM_QSTR(MP_QSTR_blob_decode), MP_ROM_PTR(&ubus_blob_decode_obj) },

#define C(name) { MP_ROM_QSTR(MP_QSTR_ ## name), MP_ROM_INT(name) }
    C(UBUS_STATUS_OK),
    C(UBUS_STATUS_INVALID_COMMAND),
    C(UBUS_STATUS_INVALID_ARGUMENT),
    C(UBUS_STATUS_METHOD_NOT_FOUND),
    C(UBUS_STATUS_NOT_FOUND),
    C(UBUS_STATUS_NO_DATA),
    C(UBUS_STATUS_PERMISSION_DENIED),
    C(UBUS_STATUS_TIMEOUT),
    C(UBUS_STATUS_NOT_SUPPORTED),
    C(UBUS_STATUS_UNKNOWN_ERROR),
    C(UBUS_STATUS_CONNECTION_FAILED),
#undef C
};
STATIC MP_DEFINE_CONST_DICT(mp_module_ubus_globals, mp_module_ubus_globals_table);

const mp_obj_module_t mp_module_ubus = {
    .base = { &mp_type_module },
    .name = MP_QSTR__ubus,
    .globals = (mp_obj_dict_t*)&mp_module_ubus_globals,
};
