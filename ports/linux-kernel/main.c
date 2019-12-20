/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Yonatan Goldschmidt
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

#include <linux/module.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/version.h>

#include <net/sock.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "py/gc.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/mperrno.h"
#include "lib/utils/pyexec.h"
#include "lib/mp-readline/readline.h"


STATIC char *stack_top;
STATIC char heap[1 << 23];

void gc_collect(void) {
    // WARNING: This gc_collect implementation doesn't try to get root
    // pointers from CPU registers, and thus may function incorrectly.
    // TODO collect from regs
    void *dummy;
    gc_collect_start();
    gc_collect_root(&dummy, ((mp_uint_t)stack_top - (mp_uint_t)&dummy) / sizeof(mp_uint_t));
    gc_collect_end();
}

struct kernel_reader {
    struct file *filp;
    loff_t pos;
};

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 13, 0) // bdd1d2d3d251c
static inline ssize_t _kernel_read(struct file *file, void *buf, size_t count, loff_t *pos) {
    return kernel_read(file, buf, count, pos);
}
#else
static inline ssize_t _kernel_read(struct file *file, void *buf, size_t count, loff_t *pos) {
    ssize_t ret = (ssize_t)kernel_read(file, *pos, buf, count);
    if (ret > 0) {
        *pos += ret;
    }
    return ret;
}
#endif

STATIC mp_uint_t kernel_reader_readbyte(void *data) {
    struct kernel_reader *kr = (struct kernel_reader*)data;
    uint8_t buf;

    ssize_t ret = _kernel_read(kr->filp, &buf, sizeof(buf), &kr->pos);
    if (1 == ret) {
        return buf;
    } else if (0 == ret) {
        return MP_READER_EOF;
    } else {
        // ?
        return MP_READER_EOF;
    }
}

STATIC void kernel_reader_close(void *data) {
    struct kernel_reader *kr = (struct kernel_reader*)data;

    filp_close(kr->filp, NULL);
    gc_free(kr);
}

mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
    struct file *filp = filp_open(filename, O_RDONLY, 0);
    if (IS_ERR(filp)) {
        mp_raise_OSError(PTR_ERR(filp));
    }

    struct kernel_reader *kr = (struct kernel_reader*)m_malloc(sizeof(struct kernel_reader));
    kr->pos = 0;
    kr->filp = filp;

    mp_reader_t reader;
    reader.data = kr;
    reader.readbyte = kernel_reader_readbyte;
    reader.close = kernel_reader_close;

    return mp_lexer_new(qstr_from_str(filename), reader);
}

mp_import_stat_t mp_import_stat(const char *path) {
    struct file *f = filp_open(path, O_RDONLY|O_DIRECTORY, 0);
    if (IS_ERR(f)) {
        if (-ENOTDIR == PTR_ERR(f)) {
            return MP_IMPORT_STAT_FILE;
        }

        return MP_IMPORT_STAT_NO_EXIST;
    }
    filp_close(f, NULL);
    return MP_IMPORT_STAT_DIR;
}

mp_obj_t mp_builtin_open(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    // TODO implement using filp_open and friends
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);

STATIC void NORETURN die(const char *msg) {
    char comm[sizeof(current->comm)];
    get_task_comm(comm, current);
    printk(KERN_ALERT "fatal error '%s', killing current task '%s'\n", msg, current->comm);
    do_exit(0);
}

void nlr_jump_fail(void *val) {
    die(__func__);
}

#ifndef NDEBUG
void MP_WEAK __assert_func(const char *file, int line, const char *func, const char *expr) {
    printk(KERN_INFO "Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    die("Assertion failed");
}
#endif

STATIC struct socket *current_peer;

static inline bool is_retry_errno(int err) {
    return -EAGAIN == err || -EINTR == err || -ERESTARTSYS == err;
}

int mp_hal_stdin_rx_chr(void) {
    if (NULL == current_peer) {
        return CHAR_CTRL_D;
    }

    struct msghdr msg = { };
    uint8_t c;
    struct kvec iov = { .iov_base = &c, .iov_len = sizeof(c) };
    int ret;

retry:
    ret = kernel_recvmsg(current_peer, &msg, &iov, 1, iov.iov_len, msg.msg_flags);
    if (MP_LIKELY(1 == ret)) {
        return (int)c;
    } else if (0 == ret) {
        // EOF
        return CHAR_CTRL_D;
    } else if(is_retry_errno(ret)) {
        goto retry;
    } else {
        printk(KERN_WARNING "kernel_recvmsg: %d, killing connection\n", ret);
        return CHAR_CTRL_D;
    }
}

void mp_hal_stdout_tx_strn(const char *str, size_t len) {
    if (NULL == current_peer) {
        static char line_buf[512];
        static size_t idx;
        for (size_t i = 0; i < len; ++i) {
            line_buf[idx++] = str[i];

            if (str[i] == '\n') {
                goto print;
            }

            if (idx == sizeof(line_buf) - 2) {
                line_buf[idx++] = '\n';
                goto print;
            }

            continue;

        print:
            line_buf[idx] = '\0';
            printk("mpy: %s", line_buf);
            idx = 0;
        }

        return;
    }

    struct msghdr msg = {};
    struct kvec iov = { .iov_base = (uint8_t*)str, .iov_len = len };

    size_t sent = 0;
    while (sent < len) {
        const int sent_now = kernel_sendmsg(current_peer, &msg, &iov, 1, iov.iov_len);
        if (sent_now > 0) {
            sent += sent_now;
        } else if (0 == sent_now) {
            // we already sent a FIN and this function was called again.
            goto kill_conn;
        } else if (is_retry_errno(sent_now)) {
            continue;
        } else {
            printk(KERN_WARNING "kernel_sendmsg: %d, killing connection\n", sent_now);
            goto kill_conn;
        }
    }

    return;

kill_conn:
    // this will cause next mp_hal_stdin_rx_chr() call to exit.
    kernel_sock_shutdown(current_peer, SHUT_RDWR);
}

STATIC int run_python(struct socket *peer) {
    int stack_dummy;
    stack_top = (char*)&stack_dummy;

    current_peer = peer;

    (void)pyexec_friendly_repl();

    current_peer = NULL;

    return 0;
}

STATIC int run_server(void *data) {
    (void)data;

    int err;
    struct socket *sock;

    err = sock_create_kern(&init_net, AF_INET, SOCK_STREAM, 0, &sock);
    if (err < 0) {
        printk(KERN_WARNING "sock_create_kern: %d\n", err);
        goto out;
    }

    struct sockaddr_in bind_addr = {
        .sin_family = AF_INET,
        .sin_addr = {0},
        .sin_port = htons(9999),
    };
    err = kernel_bind(sock, (struct sockaddr *)&bind_addr,
            sizeof(bind_addr));
    if (err < 0) {
        printk(KERN_WARNING "kernel_bind: %d\n", err);
        goto out_sock;
    }

    err = kernel_listen(sock, 1);
    if (err < 0) {
        printk(KERN_WARNING "kernel_listen: %d\n", err);
        goto out_sock;
    }

    printk(KERN_INFO "mpy: server is up\n");

    struct socket *peer;
    while (1) {
        err = kernel_accept(sock, &peer, 0);
        if (0 == err) {
            run_python(peer);
            sock_release(peer);
            peer = NULL;
        } else if (is_retry_errno(err)) {
            continue;
        } else {
            printk(KERN_WARNING "kernel_accept: %d\n", err);
            goto out_sock;
        }

        // TODO exit on rmmod request
    }

out_sock:
    sock_release(sock);
out:
    return 0;
}

STATIC int __init mpy_init_module(void) {
    gc_init(heap, heap + sizeof(heap));
    mp_init();
    mp_obj_list_init(mp_sys_argv, 0);
    mp_obj_list_init(mp_sys_path, 0);

#ifdef INCLUDE_STRUCT_LAYOUT
    printk("mpy: calling struct access initializer\n");
    pyexec_frozen_module("structs.py");
#endif

    printk("mpy: starting server\n");
    kthread_run(run_server, NULL, "kmp");

    return 0;
}

STATIC void __exit mpy_exit_module(void) {
    // TODO implement
    mp_deinit();
}

module_init(mpy_init_module);
module_exit(mpy_exit_module);
MODULE_LICENSE("GPL");
