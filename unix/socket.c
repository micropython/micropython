#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
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

static const mp_obj_type_t microsocket_type;

// Helper functions
#define RAISE_ERRNO(err_flag, error_val) \
    { if (err_flag == -1) \
        { nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_OSError, "[Errno %d]", error_val)); } }

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
    nlr_jump(mp_obj_new_exception_msg(&mp_type_TypeError, "Operation not supported"));
}

static mp_obj_socket_t *socket_new(int fd) {
    mp_obj_socket_t *o = m_new_obj(mp_obj_socket_t);
    o->base.type = &microsocket_type;
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

static mp_obj_t socket_fileno(mp_obj_t self_in) {
    mp_obj_socket_t *self = self_in;
    return MP_OBJ_NEW_SMALL_INT((machine_int_t)self->fd);
}
static MP_DEFINE_CONST_FUN_OBJ_1(socket_fileno_obj, socket_fileno);

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

static mp_obj_t socket_recv(uint n_args, const mp_obj_t *args) {
    mp_obj_socket_t *self = args[0];
    int sz = MP_OBJ_SMALL_INT_VALUE(args[1]);
    int flags = 0;

    if (n_args > 2) {
        flags = MP_OBJ_SMALL_INT_VALUE(args[2]);
    }

    char *buf = m_new(char, sz);
    int out_sz = recv(self->fd, buf, sz, flags);
    RAISE_ERRNO(out_sz, errno);

    buf = m_realloc(buf, sz, out_sz);
    return MP_OBJ_NEW_QSTR(qstr_from_strn_take(buf, out_sz, out_sz));
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(socket_recv_obj, 2, 3, socket_recv);

static mp_obj_t socket_send(uint n_args, const mp_obj_t *args) {
    mp_obj_socket_t *self = args[0];
    int flags = 0;

    if (n_args > 2) {
        flags = MP_OBJ_SMALL_INT_VALUE(args[2]);
    }

    uint sz;
    const char *buf = mp_obj_str_get_data(args[1], &sz);
    int out_sz = send(self->fd, buf, sz, flags);
    RAISE_ERRNO(out_sz, errno);

    return MP_OBJ_NEW_SMALL_INT((machine_int_t)out_sz);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(socket_send_obj, 2, 3, socket_send);

static mp_obj_t socket_setsockopt(uint n_args, const mp_obj_t *args) {
    mp_obj_socket_t *self = args[0];
    int level = MP_OBJ_SMALL_INT_VALUE(args[1]);
    int option = mp_obj_get_int(args[2]);

    const void *optval;
    socklen_t optlen;
    if (MP_OBJ_IS_INT(args[3])) {
        int val = mp_obj_int_get(args[3]);
        optval = &val;
        optlen = sizeof(val);
    } else {
        buffer_info_t bufinfo;
        get_buffer(args[3], &bufinfo);
        optval = bufinfo.buf;
        optlen = bufinfo.len;
    }
    int r = setsockopt(self->fd, level, option, optval, optlen);
    RAISE_ERRNO(r, errno);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(socket_setsockopt_obj, 4, 4, socket_setsockopt);

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

static const mp_map_elem_t microsocket_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_fileno), (mp_obj_t)&socket_fileno_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_makefile), (mp_obj_t)&mp_identity_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_read), (mp_obj_t)&mp_stream_read_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_readall), (mp_obj_t)&mp_stream_readall_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_readline), (mp_obj_t)&mp_stream_unbuffered_readline_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_write), (mp_obj_t)&mp_stream_write_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_connect), (mp_obj_t)&socket_connect_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_bind), (mp_obj_t)&socket_bind_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_listen), (mp_obj_t)&socket_listen_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_accept), (mp_obj_t)&socket_accept_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_recv), (mp_obj_t)&socket_recv_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_send), (mp_obj_t)&socket_send_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_setsockopt), (mp_obj_t)&socket_setsockopt_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_close), (mp_obj_t)&socket_close_obj },
#if MICROPY_SOCKET_EXTRA
    { MP_OBJ_NEW_QSTR(MP_QSTR_recv), (mp_obj_t)&mp_stream_read_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_send), (mp_obj_t)&mp_stream_write_obj },
#endif
};

STATIC MP_DEFINE_CONST_DICT(microsocket_locals_dict, microsocket_locals_dict_table);

static const mp_obj_type_t microsocket_type = {
    { &mp_type_type },
    .name = MP_QSTR_socket,
    .print = socket_print,
    .make_new = socket_make_new,
    .getiter = NULL,
    .iternext = NULL,
    .stream_p = {
        .read = socket_read,
        .write = socket_write,
    },
    .locals_dict = (mp_obj_t)&microsocket_locals_dict,
};

static mp_obj_t mod_socket_htons(mp_obj_t arg) {
    return MP_OBJ_NEW_SMALL_INT((machine_int_t)htons(MP_OBJ_SMALL_INT_VALUE(arg)));
}
static MP_DEFINE_CONST_FUN_OBJ_1(mod_socket_htons_obj, mod_socket_htons);

static mp_obj_t mod_socket_inet_aton(mp_obj_t arg) {
    assert(MP_OBJ_IS_TYPE(arg, &mp_type_str));
    const char *s = mp_obj_str_get_str(arg);
    struct in_addr addr;
    if (!inet_aton(s, &addr)) {
        nlr_jump(mp_obj_new_exception_msg(&mp_type_OSError, "Invalid IP address"));
    }

    return mp_obj_new_int(addr.s_addr);
}
static MP_DEFINE_CONST_FUN_OBJ_1(mod_socket_inet_aton_obj, mod_socket_inet_aton);

#if MICROPY_SOCKET_EXTRA
static mp_obj_t mod_socket_gethostbyname(mp_obj_t arg) {
    assert(MP_OBJ_IS_TYPE(arg, &mp_type_str));
    const char *s = mp_obj_str_get_str(arg);
    struct hostent *h = gethostbyname(s);
    if (h == NULL) {
        nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_OSError, "[Errno %d]", errno));
    }
    assert(h->h_length == 4);
    return mp_obj_new_int(*(int*)*h->h_addr_list);
}
static MP_DEFINE_CONST_FUN_OBJ_1(mod_socket_gethostbyname_obj, mod_socket_gethostbyname);
#endif

static mp_obj_t mod_socket_getaddrinfo(uint n_args, const mp_obj_t *args) {
    // TODO: Implement all args
    assert(n_args == 2);
    assert(MP_OBJ_IS_STR(args[0]));

    const char *host = mp_obj_str_get_str(args[0]);
    const char *serv = NULL;
    // getaddrinfo accepts port in string notation, so however
    // it may seem stupid, we need to convert int to str
    if (MP_OBJ_IS_SMALL_INT(args[1])) {
        int port = (short)MP_OBJ_SMALL_INT_VALUE(args[1]);
        char buf[6];
        sprintf(buf, "%d", port);
        serv = buf;
    } else {
        serv = mp_obj_str_get_str(args[1]);
    }

    struct addrinfo hints;
    struct addrinfo *addr;
    memset(&hints, 0, sizeof(hints));
    int res = getaddrinfo(host, serv, NULL/*&hints*/, &addr);

    if (res != 0) {
        nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_OSError, "[addrinfo error %d]", res));
    }
    assert(addr);

    mp_obj_t list = mp_obj_new_list(0, NULL);
    for (; addr; addr = addr->ai_next) {
        mp_obj_tuple_t *t = mp_obj_new_tuple(5, NULL);
        t->items[0] = MP_OBJ_NEW_SMALL_INT((machine_int_t)addr->ai_family);
        t->items[1] = MP_OBJ_NEW_SMALL_INT((machine_int_t)addr->ai_socktype);
        t->items[2] = MP_OBJ_NEW_SMALL_INT((machine_int_t)addr->ai_protocol);
        // "canonname will be a string representing the canonical name of the host
        // if AI_CANONNAME is part of the flags argument; else canonname will be empty." ??
        if (addr->ai_canonname) {
            t->items[3] = MP_OBJ_NEW_QSTR(qstr_from_str(addr->ai_canonname));
        } else {
            t->items[3] = mp_const_none;
        }
        t->items[4] = mp_obj_new_bytearray(addr->ai_addrlen, addr->ai_addr);
        mp_obj_list_append(list, t);
    }
    return list;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_socket_getaddrinfo_obj, 2, 6, mod_socket_getaddrinfo);

extern mp_obj_type_t sockaddr_in_type;

#define C(name) { #name, name }

static const struct sym_entry {
    const char *sym;
    int val;
} constants[] = {
    C(AF_UNIX),
    C(AF_INET),
    C(AF_INET6),
    C(SOCK_STREAM),
    C(SOCK_DGRAM),
    C(SOCK_RAW),

    C(MSG_DONTROUTE),
    C(MSG_DONTWAIT),

    C(SOL_SOCKET),
    C(SO_BROADCAST),
    C(SO_ERROR),
    C(SO_KEEPALIVE),
    C(SO_LINGER),
    C(SO_REUSEADDR),

    {NULL}
};

#undef C

void microsocket_init() {
    mp_obj_t m = mp_obj_new_module(MP_QSTR_microsocket);
    mp_store_attr(m, MP_QSTR_socket, (mp_obj_t)&microsocket_type);
#if MICROPY_SOCKET_EXTRA
    mp_store_attr(m, MP_QSTR_sockaddr_in, (mp_obj_t)&sockaddr_in_type);
    mp_store_attr(m, MP_QSTR_htons, (mp_obj_t)&mod_socket_htons_obj);
    mp_store_attr(m, MP_QSTR_inet_aton, (mp_obj_t)&mod_socket_inet_aton_obj);
    mp_store_attr(m, MP_QSTR_gethostbyname, (mp_obj_t)&mod_socket_gethostbyname_obj);
#endif
    mp_store_attr(m, MP_QSTR_getaddrinfo, (mp_obj_t)&mod_socket_getaddrinfo_obj);
    for (const struct sym_entry *p = constants; p->sym != NULL; p++) {
        mp_store_attr(m, QSTR_FROM_STR_STATIC(p->sym), MP_OBJ_NEW_SMALL_INT((machine_int_t)p->val));
    }
}
