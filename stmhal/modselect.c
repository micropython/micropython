#include <std.h>
#include "stm32f4xx_hal.h"
#include "mpconfig.h"
#include "nlr.h"
#include "misc.h"
#include "qstr.h"
#include "obj.h"
#include "objtuple.h"
#include "runtime.h"

#include "hci.h"
#include "ccspi.h"
#include "wlan.h"
#include "socket.h"
#include "nvmem.h"
#include "netapp.h"
#include "evnt_handler.h"
#include "patch_prog.h"

#include "modwlan.h"
#include "modsocket.h"
#include "modselect.h"

#if MICROPY_HW_ENABLE_CC3K

#define MP_ASSERT_TYPE(obj, type)                       \
    do {                                                \
        __typeof__ (obj) _a = (obj);                    \
        __typeof__ (type) _b = (type);                  \
        if (!MP_OBJ_IS_TYPE(_a, _b)) {                  \
            nlr_jump(mp_obj_new_exception_msg_varg(     \
                        &mp_type_TypeError,             \
                        "can't convert %s to %s",       \
                        mp_obj_get_type_str(_a),        \
                        _b->name));                     \
        }                                               \
    } while(0)

// select helper functions
STATIC void set_fds(int *nfds, mp_obj_t *fdlist, uint fdlist_len, fd_set *fdset) {

    // clear fd set
    FD_ZERO(fdset);

    // add sockets to fd set
    for (int i=0; i<fdlist_len; i++) {
        socket_t *s = fdlist[i];

        // check arg type
        MP_ASSERT_TYPE(s, &socket_type);

        // add to fd set
        FD_SET(s->fd, fdset);

        if (s->fd > (*nfds)) {
            *nfds = s->fd;
        }
    }
}

STATIC void get_fds(mp_obj_t *fdlist, uint fdlist_len, mp_obj_t *fdlist_out, fd_set *fdset) {
    for (int i=0; i<fdlist_len; i++) {
        socket_t *s = fdlist[i];
        if (FD_ISSET(s->fd, fdset)) {
            socket_t *socket_obj = m_new_obj_with_finaliser(socket_t);
            socket_obj->base.type = (mp_obj_t)&socket_type;
            socket_obj->fd  = s->fd;
            mp_obj_list_append(fdlist_out, socket_obj);
        }
    }
}

STATIC mp_obj_t modselect_select(uint n_args, const mp_obj_t *args) {
    int nfds=0; //highest-numbered fd plus 1
    timeval tv={0};
    fd_set rfds, wfds, xfds;

    mp_obj_t *rlist, *wlist, *xlist;
    uint rlist_len, wlist_len, xlist_len;

    // read args
    mp_obj_get_array(args[0], &rlist_len, &rlist);
    mp_obj_get_array(args[1], &wlist_len, &wlist);
    mp_obj_get_array(args[2], &xlist_len, &xlist);

    if (n_args == 4) {
        float timeout = mp_obj_get_float(args[3]);
        tv.tv_sec = (int)timeout;
        tv.tv_usec = (timeout-(int)timeout)*1000*1000;
    }

    // add fds to their respective sets
    set_fds(&nfds, rlist, rlist_len, &rfds);
    set_fds(&nfds, wlist, wlist_len, &wfds);
    set_fds(&nfds, xlist, xlist_len, &xfds);

    // call select
    nfds = select(nfds+1, &rfds, &wfds, &xfds, &tv);

    // if any of the read sockets is closed, we add it to the read fd set,
    // a subsequent call to recv() returns 0. This behavior is consistent with BSD.
    for (int i=0; i<rlist_len; i++) {
        socket_t *s = rlist[i];
        if (wlan_get_fd_state(s->fd)) {
            FD_SET(s->fd, &rfds);
            nfds = (nfds > s->fd)? nfds:s->fd;
        }
    }

    // return value; a tuple of 3 lists
    mp_obj_t fds[3] = {
        mp_obj_new_list(0, NULL),
        mp_obj_new_list(0, NULL),
        mp_obj_new_list(0, NULL)
    };

    // On success, select() returns the number of file descriptors contained
    // in the three returned descriptor sets which may be zero if the timeout
    // expires before anything interesting happens, -1 is returned on error.
    if (nfds == -1) {   // select failed
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "select failed"));
    } else if (nfds) {  // an fd is ready
        get_fds(rlist, rlist_len, fds[0], &rfds);
        get_fds(wlist, wlist_len, fds[1], &wfds);
        get_fds(xlist, xlist_len, fds[2], &xfds);
    } // select timedout

    return mp_obj_new_tuple(3, fds);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(modselect_select_obj, 3, 4, modselect_select);

void modselect_init0() {
    mp_obj_t m = mp_obj_new_module(QSTR_FROM_STR_STATIC("select"));
    mp_store_attr(m, QSTR_FROM_STR_STATIC("select"),    (mp_obj_t)&modselect_select_obj);
    mp_store_name(QSTR_FROM_STR_STATIC("select"), m);
}
#endif // MICROPY_HW_ENABLE_CC3K
