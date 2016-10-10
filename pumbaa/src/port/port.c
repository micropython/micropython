/**
 * @file junk.c
 *
 * @section License
 * Copyright (C) 2016, Erik Moqvist
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * This file is part of the Pumbaa project.
 */

#include "pumbaa.h"

char *stack_top_p;

/* The character that raises a keyboard exception. */
static char interrupt_char = -1;

/**
 * The write filter callback that raises the keyboard exception when
 * the interrupt character is read.
 */
static int write_filter_isr(void *self_p, const void *buf_p, size_t size)
{
    const char *char_p;

    char_p = buf_p;

    if (*char_p == interrupt_char) {
        MP_STATE_VM(mp_pending_exception) = MP_STATE_VM(keyboard_interrupt_obj);

        return (1);
    }

    return (0);
}

mp_import_stat_t mp_import_stat(const char *path_p)
{
    struct fs_stat_t stat;

    if (fs_stat(path_p, &stat) != 0) {
        return (MP_IMPORT_STAT_NO_EXIST);
    }

    switch (stat.type) {

    case FS_TYPE_FILE:
    case FS_TYPE_SOFT_LINK:
    case FS_TYPE_HARD_LINK:
        return (MP_IMPORT_STAT_FILE);

    case FS_TYPE_DIR:
        return (MP_IMPORT_STAT_DIR);

    default:
        return (MP_IMPORT_STAT_NO_EXIST);
    }
}

void nlr_jump_fail(void *val_p)
{
    std_printf(FSTR("FATAL: uncaught NLR 0x%x\r\n"), (long)val_p);
    sys_stop(1);
}

void mp_hal_stdout_tx_strn_cooked(const char *str_p, size_t len)
{
    char c;

    while (len--) {
        if (*str_p == '\n') {
            c = '\r';
            chan_write(sys_get_stdout(), &c, 1);
        }

        c = *str_p++;
        chan_write(sys_get_stdout(), &c, 1);
    }
}

void mp_hal_stdout_tx_strn(const char *str_p, mp_uint_t len)
{
    mp_hal_stdout_tx_strn_cooked(str_p, len);
}

void mp_hal_stdout_tx_str(const char *str_p)
{
    mp_hal_stdout_tx_strn_cooked(str_p, strlen(str_p));
}

/* Receive single character. */
int mp_hal_stdin_rx_chr(void)
{
    unsigned char c = 0;

    chan_read(sys_get_stdin(), &c, 1);

    return (c);
}

void mp_hal_set_interrupt_char(char c)
{
    /* Save the interrupt character in a global variable. */
    interrupt_char = c;

    /* Raise the keyboard exception from the write filter function. */
    if (c == -1) {
        chan_set_write_filter_cb(sys_get_stdin(), NULL);
    } else {
        mp_obj_exception_clear_traceback(MP_STATE_VM(keyboard_interrupt_obj));
        chan_set_write_filter_isr_cb(sys_get_stdin(), write_filter_isr);
    }
}
