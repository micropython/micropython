/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 MicroPython contributors
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

#include "py/builtin.h"
#include "py/compile.h"
#include "py/gc.h"
#include "py/lexer.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "py/stackctrl.h"
#include "shared/runtime/gchelper.h"

#include "hardware/uart.h"

// Heap and stack bounds defined by the linker script.
extern uint32_t _heap_start;
extern uint32_t _heap_end;
extern uint32_t _stack_top;

#define MICROPY_REPL_UART       (2)
#define MICROPY_REPL_BAUDRATE   (115200)

// Stack guard size matches the linker's reservation minus a small
// safety margin so MP_STACK_CHECK() trips before we hit the heap.
#define STACK_GUARD_BYTES       (1024)

int main(void) {
    // Configure UART2 (PB14 = TX, PB13 = RX) at 115200 8N1.  boot0/boot1
    // left it at 1 Mbaud; this retunes for user-firmware logging.
    uart_init(MICROPY_REPL_UART, MICROPY_REPL_BAUDRATE);

    // MicroPython stack and heap setup.
    mp_stack_set_top(&_stack_top);
    mp_stack_set_limit((char *)&_stack_top - (char *)&_heap_end - STACK_GUARD_BYTES);

    gc_init(&_heap_start, &_heap_end);
    mp_init();

    static const char banner[] = "\r\nMicroPython baochip port (Phase 1.4)\r\n";
    uart_write(MICROPY_REPL_UART, (const uint8_t *)banner, sizeof(banner) - 1);

    mp_deinit();

    // Subsequent commits add the REPL; for now spin so the banner stays
    // visible until reset.
    for (;;) {
    }
}

// GC root collection.
void gc_collect(void) {
    gc_collect_start();
    gc_helper_collect_regs_and_stack();
    gc_collect_end();
}

void nlr_jump_fail(void *val) {
    (void)val;
    for (;;) {
    }
}

// Filesystem stubs (no VFS yet in Phase 1).
mp_import_stat_t mp_import_stat(const char *path) {
    (void)path;
    return MP_IMPORT_STAT_NO_EXIST;
}

mp_lexer_t *mp_lexer_new_from_file(qstr filename) {
    (void)filename;
    mp_raise_OSError(MP_ENOENT);
}

mp_obj_t mp_builtin_open(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    (void)n_args;
    (void)args;
    (void)kwargs;
    mp_raise_OSError(MP_ENOENT);
}
MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);
