#include <stdio.h>

#include <stm32f4xx_hal.h>

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "misc.h"
#include "obj.h"
#include "stream.h"
#include "pybstdio.h"
#include "usb.h"
#include "usart.h"

void stdout_tx_str(const char *str) {
    if (pyb_usart_global_debug != PYB_USART_NONE) {
        usart_tx_str(pyb_usart_global_debug, str);
    }
#if defined(USE_HOST_MODE) && MICROPY_HW_HAS_LCD
    lcd_print_str(str);
#endif
    usb_vcp_send_str(str);
}

void stdout_tx_strn(const char *str, uint len) {
    if (pyb_usart_global_debug != PYB_USART_NONE) {
        usart_tx_strn(pyb_usart_global_debug, str, len);
    }
#if defined(USE_HOST_MODE) && MICROPY_HW_HAS_LCD
    lcd_print_strn(str, len);
#endif
    usb_vcp_send_strn(str, len);
}

int stdin_rx_chr(void) {
    for (;;) {
#if 0
#ifdef USE_HOST_MODE
        pyb_usb_host_process();
        int c = pyb_usb_host_get_keyboard();
        if (c != 0) {
            return c;
        }
#endif
#endif
        if (usb_vcp_rx_num() != 0) {
            return usb_vcp_rx_get();
        } else if (pyb_usart_global_debug != PYB_USART_NONE && usart_rx_any(pyb_usart_global_debug)) {
            return usart_rx_char(pyb_usart_global_debug);
        }
        __WFI();
    }
}


/******************************************************************************/
// Micro Python bindings

#define STDIO_FD_IN  (0)
#define STDIO_FD_OUT (1)
#define STDIO_FD_ERR (2)

typedef struct _pyb_stdio_obj_t {
    mp_obj_base_t base;
    int fd;
} pyb_stdio_obj_t;

void stdio_obj_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_stdio_obj_t *self = self_in;
    printf("<io.FileIO %d>", self->fd);
}

STATIC machine_int_t stdio_read(mp_obj_t self_in, void *buf, machine_uint_t size, int *errcode) {
    pyb_stdio_obj_t *self = self_in;
    if (self->fd == STDIO_FD_IN) {
        for (uint i = 0; i < size; i++) {
            int c = stdin_rx_chr();
            if (c == '\r') {
                c = '\n';
            }
            ((byte*)buf)[i] = c;
        }
        *errcode = 0;
        return size;
    } else {
        *errcode = 1;
        return 0;
    }
}

STATIC machine_int_t stdio_write(mp_obj_t self_in, const void *buf, machine_uint_t size, int *errcode) {
    pyb_stdio_obj_t *self = self_in;
    if (self->fd == STDIO_FD_OUT || self->fd == STDIO_FD_ERR) {
        stdout_tx_strn(buf, size);
        *errcode = 0;
        return size;
    } else {
        *errcode = 1;
        return 0;
    }
}

mp_obj_t stdio_obj___exit__(uint n_args, const mp_obj_t *args) {
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(stdio_obj___exit___obj, 4, 4, stdio_obj___exit__);

// TODO gc hook to close the file if not already closed

STATIC const mp_map_elem_t stdio_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_read), (mp_obj_t)&mp_stream_read_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_readall), (mp_obj_t)&mp_stream_readall_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_readline), (mp_obj_t)&mp_stream_unbuffered_readline_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_write), (mp_obj_t)&mp_stream_write_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_close), (mp_obj_t)&mp_identity_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR___del__), (mp_obj_t)&mp_identity_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR___enter__), (mp_obj_t)&mp_identity_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR___exit__), (mp_obj_t)&stdio_obj___exit___obj },
};

STATIC MP_DEFINE_CONST_DICT(stdio_locals_dict, stdio_locals_dict_table);

STATIC const mp_stream_p_t stdio_obj_stream_p = {
    .read = stdio_read,
    .write = stdio_write,
};

STATIC const mp_obj_type_t stdio_obj_type = {
    { &mp_type_type },
    .name = MP_QSTR_FileIO,
    // TODO .make_new?
    .print = stdio_obj_print,
    .getiter = mp_identity,
    .iternext = mp_stream_unbuffered_iter,
    .stream_p = &stdio_obj_stream_p,
    .locals_dict = (mp_obj_t)&stdio_locals_dict,
};

const pyb_stdio_obj_t mp_sys_stdin_obj = {{&stdio_obj_type}, .fd = STDIO_FD_IN};
const pyb_stdio_obj_t mp_sys_stdout_obj = {{&stdio_obj_type}, .fd = STDIO_FD_OUT};
const pyb_stdio_obj_t mp_sys_stderr_obj = {{&stdio_obj_type}, .fd = STDIO_FD_ERR};
