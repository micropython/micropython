/* This file is part of the MicroPython project, http://micropython.org/
 * The MIT License (MIT)
 * Copyright (c) 2022-2023 Damien P. George
 */

#include <stdio.h>
#include "py/builtin.h"
#include "py/compile.h"
#include "py/mperrno.h"

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

// Text-mode standard output
void mp_hal_stdout_tx_strn_cooked(const char *str, size_t len) {
    // This is a simplistic implementation for demonstration purposes. A real
    // one would probably want to prefix every line, not just at the start of a
    // write operation.
    static int start_of_line = 1;
    if (start_of_line) printf("py: ");
    printf("%.*s", (int)len, str);
    start_of_line = (len > 0 && (str[len-1] == '\n' || str[len-1] == '\r'));
}
