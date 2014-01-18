#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "mpqstr.h"
#include "obj.h"
#include "objtuple.h"
#include "objarray.h"
#include "runtime.h"
#include "stream.h"

#define MICROPY_SOCKET_EXTRA (0)

typedef struct _mp_obj_socket_t {
    mp_obj_base_t base;
    int fd;
} mp_obj_socket_t;

static const mp_obj_type_t rawsocket_type;

// Helper functions
#define RAISE_ERRNO(err_flag, error_val) \
    { if (err_flag == -1) \
        { nlr_jump(mp_obj_new_exception_msg_varg(MP_QSTR_OSError, "[Errno %d]", error_val)); } }

static void get_buffer(mp_obj_t obj, buffer_info_t *bufinfo) {
    mp_obj_base_t *o = (mp_obj_base_t *)obj;
    if (o->type->buffer_p.get_buffer == NULL) {
        goto error;
    }
    o->type->buffer_p.get_buffer(o, bufinfo, BUFFER_READ);
    if (bufinfo->buf == NULL) {
        goto error;
    }
    return;

error:
    nlr_jump(mp_obj_new_exception_msg(MP_QSTR_TypeError, "Operation not supported"));
}

static mp_obj_socket_t *socket_new(int fd) {
    mp_obj_socket_t *o = m_new_obj(mp_obj_socket_t);
    o->base.type = &rawsocket_type;
    o->fd = fd;
    return o;
}


static void socket_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    mp_obj_socket_t *self = self_in;
    print(env, "<_socket %d>", self->fd);
}

static machine_int_t socket_read(mp_obj_t o_in, void *buf, machine_uint_t size, int *errcode) {
    mp_obj_socket_t *o = o_in;
    machine_int_t r = read(o->fd, buf, size);
    if (r == -1) {
        *errcode = errno;
    }
    return r;
}

static machine_int_t socket_write(mp_obj_t o_in, const void *buf, machine_uint_t size, int *errcode) {
    mp_obj_socket_t *o = o_in;
    machine_int_t r = write(o->fd, buf, size);
    if (r == -1) {
        *errcode = errno;
    }
    return r;
}

static mp_obj_t socket_close(mp_obj_t self_in) {
    mp_obj_socket_t *self = self_in;
    close(self->fd);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(socket_close_obj, socket_close);

static mp_obj_t socket_connect(mp_obj_t self_in, mp_obj_t addr_in) {
    mp_obj_socket_t *self = self_in;
    buffer_info_t bufinfo;
    get_buffer(addr_in, &bufinfo);
    int r = connect(self->fd, (const struct sockaddr *)bufinfo.buf, bufinfo.len);
    RAISE_ERRNO(r, errno);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(socket_connect_obj, socket_connect);

static mp_obj_t socket_bind(mp_obj_t self_in, mp_obj_t addr_in) {
    mp_obj_socket_t *self = self_in;
    buffer_info_t bufinfo;
    get_buffer(addr_in, &bufinfo);
    int r = bind(self->fd, (const struct sockaddr *)bufinfo.buf, bufinfo.len);
    RAISE_ERRNO(r, errno);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(socket_bind_obj, socket_bind);

static mp_obj_t socket_listen(mp_obj_t self_in, mp_obj_t backlog_in) {
    mp_obj_socket_t *self = self_in;
    int r = listen(self->fd, MP_OBJ_SMALL_INT_VALUE(backlog_in));
    RAISE_ERRNO(r, errno);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(socket_listen_obj, socket_listen);

static mp_obj_t socket_accept(mp_obj_t self_in) {
    mp_obj_socket_t *self = self_in;
    struct sockaddr addr;
    socklen_t addr_len = sizeof(addr);
    int fd = accept(self->fd, &addr, &addr_len);
    RAISE_ERRNO(fd, errno);

    mp_obj_tuple_t *t = mp_obj_new_tuple(2, NULL);
    t->items[0] = socket_new(fd);
    t->items[1] = mp_obj_new_bytearray(addr_len, &addr);

    return t;
}
static MP_DEFINE_CONST_FUN_OBJ_1(socket_accept_obj, socket_accept);

static mp_obj_t socket_make_new(mp_obj_t type_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    int family = AF_INET;
    int type = SOCK_STREAM;
    int proto = 0;

    if (n_args > 0) {
        assert(MP_OBJ_IS_SMALL_INT(args[0]));
        family = MP_OBJ_SMALL_INT_VALUE(args[0]);
        if (n_args > 1) {
            assert(MP_OBJ_IS_SMALL_INT(args[1]));
            type = MP_OBJ_SMALL_INT_VALUE(args[1]);
            if (n_args > 2) {
                assert(MP_OBJ_IS_SMALL_INT(args[2]));
                proto = MP_OBJ_SMALL_INT_VALUE(args[2]);
            }
        }
    }

    int fd = socket(family, type, proto);
    RAISE_ERRNO(fd, errno);
    return socket_new(fd);
}

static const mp_method_t rawsocket_type_methods[] = {
        { "read", &mp_stream_read_obj },
        { "readall", &mp_stream_readall_obj },
        { "readline", &mp_stream_unbuffered_readline_obj},
        { "write", &mp_stream_write_obj },
        { "connect", &socket_connect_obj },
        { "bind", &socket_bind_obj },
        { "listen", &socket_listen_obj },
        { "accept", &socket_accept_obj },
        { "close", &socket_close_obj },
#if MICROPY_SOCKET_EXTRA
        { "recv", &mp_stream_read_obj },
        { "send", &mp_stream_write_obj },
#endif
        { NULL, NULL },
};

static const mp_obj_type_t rawsocket_type = {
    { &mp_const_type },
    "socket",
    .print = socket_print,
    .make_new = socket_make_new,
    .getiter = NULL,
    .iternext = NULL,
    .stream_p = {
        .read = socket_read,
        .write = socket_write,
    },
    .methods = rawsocket_type_methods,
};

static mp_obj_t mod_socket_htons(mp_obj_t arg) {
    return MP_OBJ_NEW_SMALL_INT(htons(MP_OBJ_SMALL_INT_VALUE(arg)));
}
static MP_DEFINE_CONST_FUN_OBJ_1(mod_socket_htons_obj, mod_socket_htons);

static mp_obj_t mod_socket_inet_aton(mp_obj_t arg) {
    assert(MP_OBJ_IS_TYPE(arg, &str_type));
    const char *s = qstr_str(mp_obj_str_get(arg));
    struct in_addr addr;
    if (!inet_aton(s, &addr)) {
        nlr_jump(mp_obj_new_exception_msg(MP_QSTR_OSError, "Invalid IP address"));
    }

    return mp_obj_new_int(addr.s_addr);
}
static MP_DEFINE_CONST_FUN_OBJ_1(mod_socket_inet_aton_obj, mod_socket_inet_aton);

#if MICROPY_SOCKET_EXTRA
static mp_obj_t mod_socket_gethostbyname(mp_obj_t arg) {
    assert(MP_OBJ_IS_TYPE(arg, &str_type));
    const char *s = qstr_str(mp_obj_str_get(arg));
    struct hostent *h = gethostbyname(s);
    if (h == NULL) {
        nlr_jump(mp_obj_new_exception_msg_varg(MP_QSTR_OSError, "[Errno %d]", errno));
    }
    assert(h->h_length == 4);
    return mp_obj_new_int(*(int*)*h->h_addr_list);
}
static MP_DEFINE_CONST_FUN_OBJ_1(mod_socket_gethostbyname_obj, mod_socket_gethostbyname);
#endif

static mp_obj_t mod_socket_getaddrinfo(int n_args, const mp_obj_t *args) {
    // TODO: Implement all args
    assert(n_args == 2);
    assert(MP_OBJ_IS_TYPE(args[0], &str_type));

    const char *host = qstr_str(mp_obj_str_get(args[0]));
    const char *serv = NULL;
    // getaddrinfo accepts port in string notation, so however
    // it may seem stupid, we need to convert int to str
    if (MP_OBJ_IS_SMALL_INT(args[1])) {
        int port = MP_OBJ_SMALL_INT_VALUE(args[1]);
        static char buf[20];
        sprintf(buf, "%d", port);
        serv = buf;
    } else {
        serv = qstr_str(mp_obj_str_get(args[1]));
    }

    struct addrinfo hints;
    struct addrinfo *addr;
    memset(&hints, 0, sizeof(hints));
    int res = getaddrinfo(host, serv, NULL/*&hints*/, &addr);

    if (res != 0) {
        nlr_jump(mp_obj_new_exception_msg_varg(MP_QSTR_OSError, "[addrinfo error %d]", res));
    }
    assert(addr);

    mp_obj_t list = rt_build_list(0, NULL);
    for (; addr; addr = addr->ai_next) {
        mp_obj_tuple_t *t = mp_obj_new_tuple(5, NULL);
        t->items[0] = MP_OBJ_NEW_SMALL_INT(addr->ai_family);
        t->items[1] = MP_OBJ_NEW_SMALL_INT(addr->ai_socktype);
        t->items[2] = MP_OBJ_NEW_SMALL_INT(addr->ai_protocol);
        // "canonname will be a string representing the canonical name of the host
        // if AI_CANONNAME is part of the flags argument; else canonname will be empty." ??
        if (addr->ai_canonname) {
            t->items[3] = MP_OBJ_NEW_QSTR(qstr_from_strn_copy(addr->ai_canonname, strlen(addr->ai_canonname)));
        } else {
            t->items[3] = mp_const_none;
        }
        t->items[4] = mp_obj_new_bytearray(addr->ai_addrlen, addr->ai_addr);
        rt_list_append(list, t);
    }
    return list;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_socket_getaddrinfo_obj, 2, 6, mod_socket_getaddrinfo);

extern mp_obj_type_t sockaddr_in_type;

#define STORE_INT_CONST(m, name) rt_store_attr(m, qstr_from_str_static(#name), MP_OBJ_NEW_SMALL_INT(name))

void socket_init() {
    mp_obj_t m = mp_obj_new_module(qstr_from_str_static("rawsocket"));
    rt_store_attr(m, qstr_from_str_static("socket"), (mp_obj_t)&rawsocket_type);
#if MICROPY_SOCKET_EXTRA
    rt_store_attr(m, qstr_from_str_static("sockaddr_in"), (mp_obj_t)&sockaddr_in_type);
    rt_store_attr(m, qstr_from_str_static("htons"), (mp_obj_t)&mod_socket_htons_obj);
    rt_store_attr(m, qstr_from_str_static("inet_aton"), (mp_obj_t)&mod_socket_inet_aton_obj);
    rt_store_attr(m, qstr_from_str_static("gethostbyname"), (mp_obj_t)&mod_socket_gethostbyname_obj);
#endif
    rt_store_attr(m, qstr_from_str_static("getaddrinfo"), (mp_obj_t)&mod_socket_getaddrinfo_obj);
    STORE_INT_CONST(m, AF_UNIX);
    STORE_INT_CONST(m, AF_INET);
    STORE_INT_CONST(m, AF_INET6);
    STORE_INT_CONST(m, SOCK_STREAM);
    STORE_INT_CONST(m, SOCK_DGRAM);
    STORE_INT_CONST(m, SOCK_RAW);
    rt_store_name(qstr_from_str_static("rawsocket"), m);
}
