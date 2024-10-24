/* This file is part of the MicroPython project, http://micropython.org/
 * The MIT License (MIT)
 * Copyright (c) 2022-2023 Damien P. George
 */

#include <stdio.h>
#include "py/builtin.h"
#include "py/compile.h"
#include "py/mperrno.h"
#if MICROPY_PY_SYS_STDFILES
#include "py/stream.h"
#endif

#if !MICROPY_VFS

mp_lexer_t *mp_lexer_new_from_file(qstr filename) {
    mp_raise_OSError(MP_ENOENT);
}

mp_import_stat_t mp_import_stat(const char *path) {
    (void)path;
    return MP_IMPORT_STAT_NO_EXIST;
}

mp_obj_t mp_builtin_open(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);

#endif

#if MICROPY_PY_ASYNCIO

mp_uint_t mp_hal_ticks_ms(void) {
    return 0;
}

#endif

#if MICROPY_PY_TIME

void mp_hal_delay_ms(mp_uint_t ms) {
}

void mp_hal_delay_us(mp_uint_t us) {
}

mp_uint_t mp_hal_ticks_us(void) {
	return 0;
}

mp_uint_t mp_hal_ticks_cpu(void) {
	return 0;
}

#endif

#if MICROPY_PY_TIME_TIME_TIME_NS

uint64_t mp_hal_time_ns(void) {
	// Nanoseconds since the Epoch.
	return 0;
}

#endif

// Text-mode standard output
// (When MICROPY_PY_SYS_STDFILES && MICROPY_VFS_POSIX, this is only used for
// mp_plat_print (mostly debug output), but not for print() and sys.stdout.)
void mp_hal_stdout_tx_strn_cooked(const char *str, size_t len) {
    // This is a simplistic implementation for demonstration purposes. A real
    // one would probably want to prefix every line, not just at the start of a
    // write operation.
    static int start_of_line = 1;
    if (start_of_line) printf("py: ");
    printf("%.*s", (int)len, str);
    start_of_line = (len > 0 && (str[len-1] == '\n' || str[len-1] == '\r'));
}

#if MICROPY_PY_SYS_STDFILES && !MICROPY_VFS_POSIX

// Binary-mode standard input
// Receive single character, blocking until one is available.
int mp_hal_stdin_rx_chr(void) {
    return 'X';
}

// Binary-mode standard output
// Send the string of given length.
mp_uint_t mp_hal_stdout_tx_strn(const char *str, size_t len) {
    printf("tx: %.*s", (int)len, str);
    return len;
}

uintptr_t mp_hal_stdio_poll(uintptr_t poll_flags) {
    uintptr_t ret = 0;
    if ((poll_flags & MP_STREAM_POLL_RD) /* && can_read */) {
        ret |= MP_STREAM_POLL_RD;
    }
    if ((poll_flags & MP_STREAM_POLL_WR) /* && can_write */) {
        ret |= MP_STREAM_POLL_WR;
    }
    return ret;
}

#endif
