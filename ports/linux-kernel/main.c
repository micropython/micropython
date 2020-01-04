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


#define pr_fmt(fmt) "MPY: " fmt

#include <linux/module.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 11, 0) // f3ac60671954c
#include <linux/sched/task_stack.h>
#else
#include <linux/sched.h>
#endif

#include <net/sock.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "py/gc.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/stackctrl.h"
#include "py/mperrno.h"
#include "lib/utils/pyexec.h"
#include "lib/mp-readline/readline.h"
#include "genhdr/mpversion.h"

#include "internal.h"

#if MICROPY_PY_THREAD
#include "mpthreadport.h"
#endif


STATIC struct task_struct *server_thread;
STATIC bool need_gc;
STATIC char heap[HEAP_SIZE];

STATIC void get_regs(unsigned long regs[6]) {
    // generated code for this function is a bit weird. it stores these 6 registers
    // on the stack and pops them afterwards. however, it does get the correct values in "regs"
    // so I guess it's okay.

    // those are the callee-saved regs, which may be resident with values *not saved on
    // the stack* when gc_collect is called.
    register long rbx asm ("rbx");
    register long rbp asm ("rbp");
    register long r12 asm ("r12");
    register long r13 asm ("r13");
    register long r14 asm ("r14");
    register long r15 asm ("r15");

    regs[0] = rbx;
    regs[1] = rbp;
    regs[2] = r12;
    regs[3] = r13;
    regs[4] = r14;
    regs[5] = r15;
}

void gc_collect(void) {
    if (in_atomic()) {
        // 1. sholdn't block for too long
        // 2. while the GC itself doesn't perform any non-atomic operations, it calls objects' finalizers,
        //    and they might (for example, the kprobe/ftrace finalizer calls the appropriate unregister
        //    function, and those 2 try to lock mutexes and call synchronize_rcu...)
        // instead, we defer the GC operation to the server thread.
        need_gc = true;
        barrier();
        send_sig(SIGINT, server_thread, 0);
        return;
    }

    gc_collect_start();

    unsigned long regs[6];
    get_regs(regs);
    gc_collect_root((void**)regs, ARRAY_SIZE(regs));

    void *dummy;
    gc_collect_root(&dummy, ((mp_uint_t)MP_STATE_THREAD(stack_top) - (mp_uint_t)&dummy) / sizeof(void *));

#if MICROPY_PY_THREAD
    mp_thread_gc_others();
#endif

    gc_collect_end();
}

STATIC void run_gc_if_requested(void) {
    flush_signals(current);

    if (!need_gc) {
        return;
    }

    need_gc = false;

    pr_info("Deferred GC request detected\n");

    gc_collect();
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

// not inlined - to get a more accurate "reading".
long get_current_stack_limit(void) {
    register unsigned long sp asm("rsp");

    unsigned long end = (unsigned long)end_of_stack(current);

    // only parisc, anyway...
#ifdef CONFIG_STACK_GROWSUP
#error "fix, it's all broken"
#endif

    return sp - end;
}

int DEBUG_printf(const char *fmt, ...) {
    va_list args;

    va_start(args, fmt);
    const int ret = vprintk(fmt, args);
    va_end(args);

    return ret;
}

void NORETURN die(const char *msg) {
    char comm[sizeof(current->comm)];
    get_task_comm(comm, current);
    pr_alert("fatal error '%s', killing current task '%s'\n", msg, current->comm);
    do_exit(0);
}

void nlr_jump_fail(void *val) {
    die(__func__);
}

#ifndef NDEBUG
void MP_WEAK __assert_func(const char *file, int line, const char *func, const char *expr) {
    pr_alert("Assertion '%s' failed, at file %s:%d\n", expr, file, line);
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
        run_gc_if_requested();
        goto retry;
    } else {
        pr_warn("kernel_recvmsg: %d, killing connection\n", ret);
        return CHAR_CTRL_D;
    }
}

void mp_print_printk(void *data, const char *str, size_t len) {
    (void)data;
    static bool newline = true;

    if (newline) {
        pr_info("%*pEp", (int)len, str);
    } else {
        pr_cont("%*pEp", (int)len, str);
    }

    newline = str[len - 1] == '\n';
}

void mp_hal_stdout_tx_strn(const char *str, size_t len) {
    // TODO: if atomic, buffer output, then send from another thread.
    if (NULL == current_peer || in_atomic()) {
        mp_print_printk(NULL, str, len);
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
            pr_warn("kernel_sendmsg: %d, killing connection\n", sent_now);
            goto kill_conn;
        }
    }

    return;

kill_conn:
    // this will cause next mp_hal_stdin_rx_chr() call to exit.
    kernel_sock_shutdown(current_peer, SHUT_RDWR);
}

void set_stack_limit(void) {
    // -500: have a safe zone. recursion errors are raised correctly.
    mp_stack_set_limit(get_current_stack_limit() - 500);
}

STATIC int run_python(struct socket *peer) {
    current_peer = peer;

    (void)pyexec_friendly_repl();

    current_peer = NULL;

    return 0;
}

STATIC int run_server(void *data) {
    struct socket *sock = (struct socket*)data;
    int err;

    allow_signal(SIGINT);

    // called first!
    mp_thread_init();
    mp_stack_ctrl_init();
    set_stack_limit();

    gc_init(heap, heap + sizeof(heap));
    mp_init();

    mp_obj_list_init(mp_sys_argv, 0);
    mp_obj_list_init(mp_sys_path, 0);

#ifdef INCLUDE_STRUCT_LAYOUT
    pr_info("calling struct access initializer\n");
    pyexec_frozen_module("structs.py");
#endif

    pr_info("server is up\n");

    struct socket *peer;
    while (!kthread_should_stop()) {
        err = kernel_accept(sock, &peer, 0);
        if (0 == err) {
            run_python(peer);
            sock_release(peer);
            peer = NULL;
        } else if (is_retry_errno(err)) {
            run_gc_if_requested();
            continue;
        } else if (err == -EINVAL) {
            // we'll get EINVAL after module exit shuts down the socket.
            pr_info("stop requested identified\n");
            // technically we can "continue" here and let kthread_should_stop() return true,
            // however it's a bit of a race here (because kernel_sock_shutdown might've been
            // called, but kthread_stop wasn't yet).
            // so just go out...
            goto out;
        } else {
            pr_warn("kernel_accept: %d\n", err);
            goto out;
        }
    }

    pr_info("done!\n");

out:
    // module exit function will kill the socket

    pr_info("server stopped\n");

    mp_deinit();
    mp_thread_finish();

    return 0;
}

STATIC struct socket *listener;

STATIC int __init mpy_init_module(void) {
    int err;
    struct socket *sock;

    pr_info("MicroPython " MICROPY_GIT_TAG " on " MICROPY_BUILD_DATE "; " MICROPY_HW_BOARD_NAME " with " MICROPY_HW_MCU_NAME "\n");
#ifndef CONFIG_KPROBES
    pr_info("CONFIG_KPROBES missing, kernel_ffi.kprobe will not be availale\n");
#endif
#ifndef CONFIG_FUNCTION_TRACER
    pr_info("CONFIG_FUNCTION_TRACER missing, kernel_ffi.ftrace will not be availale\n");
#endif

    err = sock_create_kern(&init_net, AF_INET, SOCK_STREAM, 0, &sock);
    if (err < 0) {
        pr_warn("sock_create_kern: %d\n", err);
        goto out;
    }

    int one = 1;
    err = kernel_setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&one, sizeof(one));
    if (err < 0) {
        pr_warn("kernel_setsockopt(SO_REUSEADDR): %d\n", err);
    }

    struct sockaddr_in bind_addr = {
        .sin_family = AF_INET,
        .sin_addr = {0},
        .sin_port = htons(9999),
    };
    err = kernel_bind(sock, (struct sockaddr *)&bind_addr,
            sizeof(bind_addr));
    if (err < 0) {
        pr_warn("kernel_bind: %d\n", err);
        goto out_sock;
    }

    err = kernel_listen(sock, 1);
    if (err < 0) {
        pr_warn("kernel_listen: %d\n", err);
        goto out_sock;
    }

    listener = sock;

    pr_info("starting server\n");
    server_thread = kthread_run(run_server, sock, "kmpy-server");
    if (IS_ERR(server_thread)) {
        err = PTR_ERR(server_thread);
        goto out_sock;
    }

    return 0;

out_sock:
    sock_release(sock);
out:
    return err;
}

STATIC void __exit mpy_exit_module(void) {
    kernel_sock_shutdown(listener, SHUT_RDWR); // breaks out from kernel_accept
    // kill the socket, it will make existing python session to die, if running.
    if (current_peer) {
        kernel_sock_shutdown(current_peer, SHUT_RDWR);
    }
    kthread_stop(server_thread);
    // now we can safely close this
    sock_release(listener);
}

module_init(mpy_init_module);
module_exit(mpy_exit_module);
MODULE_LICENSE("GPL");
