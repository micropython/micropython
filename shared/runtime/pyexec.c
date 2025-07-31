/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "py/compile.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/gc.h"
#include "py/frozenmod.h"
#include "py/mphal.h"
#if MICROPY_HW_ENABLE_USB
#include "irq.h"
#include "usb.h"
#endif
#include "shared/readline/readline.h"
#include "shared/runtime/pyexec.h"
#include "genhdr/mpversion.h"

pyexec_mode_kind_t pyexec_mode_kind = PYEXEC_MODE_FRIENDLY_REPL;

#if MICROPY_REPL_INFO
static bool repl_display_debugging_info = 0;
#endif

#define EXEC_FLAG_PRINT_EOF             (1 << 0)
#define EXEC_FLAG_ALLOW_DEBUGGING       (1 << 1)
#define EXEC_FLAG_IS_REPL               (1 << 2)
#define EXEC_FLAG_SOURCE_IS_RAW_CODE    (1 << 3)
#define EXEC_FLAG_SOURCE_IS_VSTR        (1 << 4)
#define EXEC_FLAG_SOURCE_IS_FILENAME    (1 << 5)
#define EXEC_FLAG_SOURCE_IS_READER      (1 << 6)
#define EXEC_FLAG_NO_INTERRUPT          (1 << 7)

// parses, compiles and executes the code in the lexer
// frees the lexer before returning
// EXEC_FLAG_PRINT_EOF prints 2 EOF chars: 1 after normal output, 1 after exception output
// EXEC_FLAG_ALLOW_DEBUGGING allows debugging info to be printed after executing the code
// EXEC_FLAG_IS_REPL is used for REPL inputs (flag passed on to mp_compile)
static int parse_compile_execute(const void *source, mp_parse_input_kind_t input_kind, mp_uint_t exec_flags) {
    int ret = 0;
    #if MICROPY_REPL_INFO
    uint32_t start = 0;
    #endif

    #ifdef MICROPY_BOARD_BEFORE_PYTHON_EXEC
    MICROPY_BOARD_BEFORE_PYTHON_EXEC(input_kind, exec_flags);
    #endif

    nlr_buf_t nlr;
    nlr.ret_val = NULL;
    if (nlr_push(&nlr) == 0) {
        mp_obj_t module_fun;
        #if MICROPY_MODULE_FROZEN_MPY
        if (exec_flags & EXEC_FLAG_SOURCE_IS_RAW_CODE) {
            // source is a raw_code object, create the function
            const mp_frozen_module_t *frozen = source;
            mp_module_context_t *ctx = m_new_obj(mp_module_context_t);
            ctx->module.globals = mp_globals_get();
            ctx->constants = frozen->constants;
            module_fun = mp_make_function_from_proto_fun(frozen->proto_fun, ctx, NULL);
        } else
        #endif
        {
            #if MICROPY_ENABLE_COMPILER
            mp_lexer_t *lex;
            if (exec_flags & EXEC_FLAG_SOURCE_IS_VSTR) {
                const vstr_t *vstr = source;
                lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, vstr->buf, vstr->len, 0);
            } else if (exec_flags & EXEC_FLAG_SOURCE_IS_READER) {
                lex = mp_lexer_new(MP_QSTR__lt_stdin_gt_, *(mp_reader_t *)source);
            } else if (exec_flags & EXEC_FLAG_SOURCE_IS_FILENAME) {
                lex = mp_lexer_new_from_file(qstr_from_str(source));
            } else {
                lex = (mp_lexer_t *)source;
            }
            // source is a lexer, parse and compile the script
            qstr source_name = lex->source_name;
            mp_parse_tree_t parse_tree = mp_parse(lex, input_kind);
            module_fun = mp_compile(&parse_tree, source_name, exec_flags & EXEC_FLAG_IS_REPL);
            #else
            mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("script compilation not supported"));
            #endif
        }

        // execute code
        if (!(exec_flags & EXEC_FLAG_NO_INTERRUPT)) {
            mp_hal_set_interrupt_char(CHAR_CTRL_C);
        }
        #if MICROPY_REPL_INFO
        start = mp_hal_ticks_ms();
        #endif
        mp_call_function_0(module_fun);
        mp_hal_set_interrupt_char(-1); // disable interrupt
        mp_handle_pending(true); // handle any pending exceptions (and any callbacks)
        nlr_pop();
        ret = 1;
        if (exec_flags & EXEC_FLAG_PRINT_EOF) {
            mp_hal_stdout_tx_strn("\x04", 1);
        }
    } else {
        // uncaught exception
        mp_hal_set_interrupt_char(-1); // disable interrupt
        mp_handle_pending(false); // clear any pending exceptions (and run any callbacks)

        if (exec_flags & EXEC_FLAG_SOURCE_IS_READER) {
            const mp_reader_t *reader = source;
            reader->close(reader->data);
        }

        // print EOF after normal output
        if (exec_flags & EXEC_FLAG_PRINT_EOF) {
            mp_hal_stdout_tx_strn("\x04", 1);
        }

        // check for SystemExit
        if (mp_obj_is_subclass_fast(MP_OBJ_FROM_PTR(((mp_obj_base_t *)nlr.ret_val)->type), MP_OBJ_FROM_PTR(&mp_type_SystemExit))) {
            // at the moment, the value of SystemExit is unused
            ret = PYEXEC_FORCED_EXIT;
        } else {
            mp_obj_print_exception(&mp_plat_print, MP_OBJ_FROM_PTR(nlr.ret_val));
            ret = 0;
        }
    }

    #if MICROPY_REPL_INFO
    // display debugging info if wanted
    if ((exec_flags & EXEC_FLAG_ALLOW_DEBUGGING) && repl_display_debugging_info) {
        mp_uint_t ticks = mp_hal_ticks_ms() - start; // TODO implement a function that does this properly
        mp_printf(&mp_plat_print, "took " UINT_FMT " ms\n", ticks);
        // qstr info
        {
            size_t n_pool, n_qstr, n_str_data_bytes, n_total_bytes;
            qstr_pool_info(&n_pool, &n_qstr, &n_str_data_bytes, &n_total_bytes);
            mp_printf(&mp_plat_print, "qstr:\n  n_pool=%u\n  n_qstr=%u\n  "
                "n_str_data_bytes=%u\n  n_total_bytes=%u\n",
                (unsigned)n_pool, (unsigned)n_qstr, (unsigned)n_str_data_bytes, (unsigned)n_total_bytes);
        }

        #if MICROPY_ENABLE_GC
        // run collection and print GC info
        gc_collect();
        gc_dump_info(&mp_plat_print);
        #endif
    }
    #endif

    if (exec_flags & EXEC_FLAG_PRINT_EOF) {
        mp_hal_stdout_tx_strn("\x04", 1);
    }

    #ifdef MICROPY_BOARD_AFTER_PYTHON_EXEC
    MICROPY_BOARD_AFTER_PYTHON_EXEC(input_kind, exec_flags, nlr.ret_val, &ret);
    #endif

    return ret;
}

#if MICROPY_ENABLE_COMPILER

// This can be configured by a port (and even configured to a function to be
// computed dynamically) to indicate the maximum number of bytes that can be
// held in the stdin buffer.
#ifndef MICROPY_REPL_STDIN_BUFFER_MAX
#define MICROPY_REPL_STDIN_BUFFER_MAX (256)
#endif

typedef struct _mp_reader_stdin_t {
    bool eof;
    uint16_t window_max;
    uint16_t window_remain;
} mp_reader_stdin_t;

static mp_uint_t mp_reader_stdin_readbyte(void *data) {
    mp_reader_stdin_t *reader = (mp_reader_stdin_t *)data;

    if (reader->eof) {
        return MP_READER_EOF;
    }

    int c = mp_hal_stdin_rx_chr();

    if (c == CHAR_CTRL_C || c == CHAR_CTRL_D) {
        reader->eof = true;
        mp_hal_stdout_tx_strn("\x04", 1); // indicate end to host
        if (c == CHAR_CTRL_C) {
            #if MICROPY_KBD_EXCEPTION
            MP_STATE_VM(mp_kbd_exception).traceback_data = NULL;
            nlr_raise(MP_OBJ_FROM_PTR(&MP_STATE_VM(mp_kbd_exception)));
            #else
            mp_raise_type(&mp_type_KeyboardInterrupt);
            #endif
        } else {
            return MP_READER_EOF;
        }
    }

    if (--reader->window_remain == 0) {
        mp_hal_stdout_tx_strn("\x01", 1); // indicate window available to host
        reader->window_remain = reader->window_max;
    }

    return c;
}

static void mp_reader_stdin_close(void *data) {
    mp_reader_stdin_t *reader = (mp_reader_stdin_t *)data;
    if (!reader->eof) {
        reader->eof = true;
        mp_hal_stdout_tx_strn("\x04", 1); // indicate end to host
        for (;;) {
            int c = mp_hal_stdin_rx_chr();
            if (c == CHAR_CTRL_C || c == CHAR_CTRL_D) {
                break;
            }
        }
    }
}

static void mp_reader_new_stdin(mp_reader_t *reader, mp_reader_stdin_t *reader_stdin, uint16_t buf_max) {
    // Make flow-control window half the buffer size, and indicate to the host that 2x windows are
    // free (sending the window size implicitly indicates that a window is free, and then the 0x01
    // indicates that another window is free).
    size_t window = buf_max / 2;
    char reply[3] = { window & 0xff, window >> 8, 0x01 };
    mp_hal_stdout_tx_strn(reply, sizeof(reply));

    reader_stdin->eof = false;
    reader_stdin->window_max = window;
    reader_stdin->window_remain = window;
    reader->data = reader_stdin;
    reader->readbyte = mp_reader_stdin_readbyte;
    reader->close = mp_reader_stdin_close;
}

static int do_reader_stdin(int c) {
    if (c != 'A') {
        // Unsupported command.
        mp_hal_stdout_tx_strn("R\x00", 2);
        return 0;
    }

    // Indicate reception of command.
    mp_hal_stdout_tx_strn("R\x01", 2);

    mp_reader_t reader;
    mp_reader_stdin_t reader_stdin;
    mp_reader_new_stdin(&reader, &reader_stdin, MICROPY_REPL_STDIN_BUFFER_MAX);
    int exec_flags = EXEC_FLAG_PRINT_EOF | EXEC_FLAG_SOURCE_IS_READER;
    return parse_compile_execute(&reader, MP_PARSE_FILE_INPUT, exec_flags);
}

#if MICROPY_REPL_EVENT_DRIVEN

typedef struct _repl_t {
    // This structure originally also held current REPL line,
    // but it was moved to MP_STATE_VM(repl_line) as containing
    // root pointer. Still keep structure in case more state
    // will be added later.
    // vstr_t line;
    bool cont_line;
    bool paste_mode;
} repl_t;

repl_t repl;

static int pyexec_raw_repl_process_char(int c);
static int pyexec_friendly_repl_process_char(int c);

void pyexec_event_repl_init(void) {
    MP_STATE_VM(repl_line) = vstr_new(32);
    repl.cont_line = false;
    repl.paste_mode = false;
    // no prompt before printing friendly REPL banner or entering raw REPL
    readline_init(MP_STATE_VM(repl_line), "");
    if (pyexec_mode_kind == PYEXEC_MODE_RAW_REPL) {
        pyexec_raw_repl_process_char(CHAR_CTRL_A);
    } else {
        pyexec_friendly_repl_process_char(CHAR_CTRL_B);
    }
}

static int pyexec_raw_repl_process_char(int c) {
    if (c == CHAR_CTRL_A) {
        // reset raw REPL
        if (vstr_len(MP_STATE_VM(repl_line)) == 2 && vstr_str(MP_STATE_VM(repl_line))[0] == CHAR_CTRL_E) {
            int ret = do_reader_stdin(vstr_str(MP_STATE_VM(repl_line))[1]);
            if (ret & PYEXEC_FORCED_EXIT) {
                return ret;
            }
            goto reset;
        }
        mp_hal_stdout_tx_str("raw REPL; CTRL-B to exit\r\n");
        goto reset;
    } else if (c == CHAR_CTRL_B) {
        // change to friendly REPL
        pyexec_mode_kind = PYEXEC_MODE_FRIENDLY_REPL;
        vstr_reset(MP_STATE_VM(repl_line));
        repl.cont_line = false;
        repl.paste_mode = false;
        pyexec_friendly_repl_process_char(CHAR_CTRL_B);
        return 0;
    } else if (c == CHAR_CTRL_C) {
        // clear line
        vstr_reset(MP_STATE_VM(repl_line));
        return 0;
    } else if (c == CHAR_CTRL_D) {
        // input finished
    } else {
        // let through any other raw 8-bit value
        vstr_add_byte(MP_STATE_VM(repl_line), c);
        return 0;
    }

    // indicate reception of command
    mp_hal_stdout_tx_str("OK");

    if (MP_STATE_VM(repl_line)->len == 0) {
        // exit for a soft reset
        mp_hal_stdout_tx_str("\r\n");
        vstr_clear(MP_STATE_VM(repl_line));
        return PYEXEC_FORCED_EXIT;
    }

    int ret = parse_compile_execute(MP_STATE_VM(repl_line), MP_PARSE_FILE_INPUT, EXEC_FLAG_PRINT_EOF | EXEC_FLAG_SOURCE_IS_VSTR);
    if (ret & PYEXEC_FORCED_EXIT) {
        return ret;
    }

reset:
    vstr_reset(MP_STATE_VM(repl_line));
    mp_hal_stdout_tx_str(">");

    return 0;
}

static int pyexec_friendly_repl_process_char(int c) {
    if (repl.paste_mode) {
        if (c == CHAR_CTRL_C) {
            // cancel everything
            mp_hal_stdout_tx_str("\r\n");
            goto input_restart;
        } else if (c == CHAR_CTRL_D) {
            // end of input
            mp_hal_stdout_tx_str("\r\n");
            int ret = parse_compile_execute(MP_STATE_VM(repl_line), MP_PARSE_FILE_INPUT, EXEC_FLAG_ALLOW_DEBUGGING | EXEC_FLAG_IS_REPL | EXEC_FLAG_SOURCE_IS_VSTR);
            if (ret & PYEXEC_FORCED_EXIT) {
                return ret;
            }
            goto input_restart;
        } else {
            // add char to buffer and echo
            vstr_add_byte(MP_STATE_VM(repl_line), c);
            if (c == '\r') {
                mp_hal_stdout_tx_str("\r\n=== ");
            } else {
                char buf[1] = {c};
                mp_hal_stdout_tx_strn(buf, 1);
            }
            return 0;
        }
    }

    int ret = readline_process_char(c);

    if (!repl.cont_line) {

        if (ret == CHAR_CTRL_A) {
            // change to raw REPL
            pyexec_mode_kind = PYEXEC_MODE_RAW_REPL;
            mp_hal_stdout_tx_str("\r\n");
            pyexec_raw_repl_process_char(CHAR_CTRL_A);
            return 0;
        } else if (ret == CHAR_CTRL_B) {
            // reset friendly REPL
            mp_hal_stdout_tx_str("\r\n");
            mp_hal_stdout_tx_str(MICROPY_BANNER_NAME_AND_VERSION);
            mp_hal_stdout_tx_str("; " MICROPY_BANNER_MACHINE);
            mp_hal_stdout_tx_str("\r\n");
            #if MICROPY_PY_BUILTINS_HELP
            mp_hal_stdout_tx_str("Type \"help()\" for more information.\r\n");
            #endif
            goto input_restart;
        } else if (ret == CHAR_CTRL_C) {
            // break
            mp_hal_stdout_tx_str("\r\n");
            goto input_restart;
        } else if (ret == CHAR_CTRL_D) {
            // exit for a soft reset
            mp_hal_stdout_tx_str("\r\n");
            vstr_clear(MP_STATE_VM(repl_line));
            return PYEXEC_FORCED_EXIT;
        } else if (ret == CHAR_CTRL_E) {
            // paste mode
            mp_hal_stdout_tx_str("\r\npaste mode; Ctrl-C to cancel, Ctrl-D to finish\r\n=== ");
            vstr_reset(MP_STATE_VM(repl_line));
            repl.paste_mode = true;
            return 0;
        }

        if (ret < 0) {
            return 0;
        }

        if (!mp_repl_continue_with_input(vstr_null_terminated_str(MP_STATE_VM(repl_line)))) {
            goto exec;
        }

        vstr_add_byte(MP_STATE_VM(repl_line), '\n');
        repl.cont_line = true;
        readline_note_newline(mp_repl_get_ps2());
        return 0;

    } else {

        if (ret == CHAR_CTRL_C) {
            // cancel everything
            mp_hal_stdout_tx_str("\r\n");
            repl.cont_line = false;
            goto input_restart;
        } else if (ret == CHAR_CTRL_D) {
            // stop entering compound statement
            goto exec;
        }

        if (ret < 0) {
            return 0;
        }

        if (mp_repl_continue_with_input(vstr_null_terminated_str(MP_STATE_VM(repl_line)))) {
            vstr_add_byte(MP_STATE_VM(repl_line), '\n');
            readline_note_newline(mp_repl_get_ps2());
            return 0;
        }

    exec:;
        int ret = parse_compile_execute(MP_STATE_VM(repl_line), MP_PARSE_SINGLE_INPUT, EXEC_FLAG_ALLOW_DEBUGGING | EXEC_FLAG_IS_REPL | EXEC_FLAG_SOURCE_IS_VSTR);
        if (ret & PYEXEC_FORCED_EXIT) {
            return ret;
        }

    input_restart:
        vstr_reset(MP_STATE_VM(repl_line));
        repl.cont_line = false;
        repl.paste_mode = false;
        readline_init(MP_STATE_VM(repl_line), mp_repl_get_ps1());
        return 0;
    }
}

uint8_t pyexec_repl_active;
int pyexec_event_repl_process_char(int c) {
    pyexec_repl_active = 1;
    int res;
    if (pyexec_mode_kind == PYEXEC_MODE_RAW_REPL) {
        res = pyexec_raw_repl_process_char(c);
    } else {
        res = pyexec_friendly_repl_process_char(c);
    }
    pyexec_repl_active = 0;
    return res;
}

MP_REGISTER_ROOT_POINTER(vstr_t * repl_line);

#else // MICROPY_REPL_EVENT_DRIVEN

int pyexec_raw_repl(void) {
    vstr_t line;
    vstr_init(&line, 32);

raw_repl_reset:
    mp_hal_stdout_tx_str("raw REPL; CTRL-B to exit\r\n");

    for (;;) {
        vstr_reset(&line);
        mp_hal_stdout_tx_str(">");
        for (;;) {
            int c = mp_hal_stdin_rx_chr();
            if (c == CHAR_CTRL_A) {
                // reset raw REPL
                if (vstr_len(&line) == 2 && vstr_str(&line)[0] == CHAR_CTRL_E) {
                    int ret = do_reader_stdin(vstr_str(&line)[1]);
                    if (ret & PYEXEC_FORCED_EXIT) {
                        return ret;
                    }
                    vstr_reset(&line);
                    mp_hal_stdout_tx_str(">");
                    continue;
                }
                goto raw_repl_reset;
            } else if (c == CHAR_CTRL_B) {
                // change to friendly REPL
                mp_hal_stdout_tx_str("\r\n");
                vstr_clear(&line);
                pyexec_mode_kind = PYEXEC_MODE_FRIENDLY_REPL;
                return 0;
            } else if (c == CHAR_CTRL_C) {
                // clear line
                vstr_reset(&line);
            } else if (c == CHAR_CTRL_D) {
                // input finished
                break;
            } else {
                // let through any other raw 8-bit value
                vstr_add_byte(&line, c);
            }
        }

        // indicate reception of command
        mp_hal_stdout_tx_str("OK");

        if (line.len == 0) {
            // exit for a soft reset
            mp_hal_stdout_tx_str("\r\n");
            vstr_clear(&line);
            return PYEXEC_FORCED_EXIT;
        }

        int ret = parse_compile_execute(&line, MP_PARSE_FILE_INPUT, EXEC_FLAG_PRINT_EOF | EXEC_FLAG_SOURCE_IS_VSTR);
        if (ret & PYEXEC_FORCED_EXIT) {
            return ret;
        }
    }
}

int pyexec_friendly_repl(void) {
    vstr_t line;
    vstr_init(&line, 32);

friendly_repl_reset:
    mp_hal_stdout_tx_str(MICROPY_BANNER_NAME_AND_VERSION);
    mp_hal_stdout_tx_str("; " MICROPY_BANNER_MACHINE);
    mp_hal_stdout_tx_str("\r\n");
    #if MICROPY_PY_BUILTINS_HELP
    mp_hal_stdout_tx_str("Type \"help()\" for more information.\r\n");
    #endif

    // to test ctrl-C
    /*
    {
        uint32_t x[4] = {0x424242, 0xdeaddead, 0x242424, 0xdeadbeef};
        for (;;) {
            nlr_buf_t nlr;
            printf("pyexec_repl: %p\n", x);
            mp_hal_set_interrupt_char(CHAR_CTRL_C);
            if (nlr_push(&nlr) == 0) {
                for (;;) {
                }
            } else {
                printf("break\n");
            }
        }
    }
    */

    for (;;) {
    input_restart:

        #if MICROPY_HW_ENABLE_USB
        if (usb_vcp_is_enabled()) {
            // If the user gets to here and interrupts are disabled then
            // they'll never see the prompt, traceback etc. The USB REPL needs
            // interrupts to be enabled or no transfers occur. So we try to
            // do the user a favor and re-enable interrupts.
            if (query_irq() == IRQ_STATE_DISABLED) {
                enable_irq(IRQ_STATE_ENABLED);
                mp_hal_stdout_tx_str("MPY: enabling IRQs\r\n");
            }
        }
        #endif

        // If the GC is locked at this point there is no way out except a reset,
        // so force the GC to be unlocked to help the user debug what went wrong.
        if (MP_STATE_THREAD(gc_lock_depth) != 0) {
            MP_STATE_THREAD(gc_lock_depth) = 0;
        }

        vstr_reset(&line);
        int ret = readline(&line, mp_repl_get_ps1());
        mp_parse_input_kind_t parse_input_kind = MP_PARSE_SINGLE_INPUT;

        if (ret == CHAR_CTRL_A) {
            // change to raw REPL
            mp_hal_stdout_tx_str("\r\n");
            vstr_clear(&line);
            pyexec_mode_kind = PYEXEC_MODE_RAW_REPL;
            return 0;
        } else if (ret == CHAR_CTRL_B) {
            // reset friendly REPL
            mp_hal_stdout_tx_str("\r\n");
            goto friendly_repl_reset;
        } else if (ret == CHAR_CTRL_C) {
            // break
            mp_hal_stdout_tx_str("\r\n");
            continue;
        } else if (ret == CHAR_CTRL_D) {
            // exit for a soft reset
            mp_hal_stdout_tx_str("\r\n");
            vstr_clear(&line);
            return PYEXEC_FORCED_EXIT;
        } else if (ret == CHAR_CTRL_E) {
            // paste mode
            mp_hal_stdout_tx_str("\r\npaste mode; Ctrl-C to cancel, Ctrl-D to finish\r\n=== ");
            vstr_reset(&line);
            for (;;) {
                char c = mp_hal_stdin_rx_chr();
                if (c == CHAR_CTRL_C) {
                    // cancel everything
                    mp_hal_stdout_tx_str("\r\n");
                    goto input_restart;
                } else if (c == CHAR_CTRL_D) {
                    // end of input
                    mp_hal_stdout_tx_str("\r\n");
                    break;
                } else {
                    // add char to buffer and echo
                    vstr_add_byte(&line, c);
                    if (c == '\r') {
                        mp_hal_stdout_tx_str("\r\n=== ");
                    } else {
                        mp_hal_stdout_tx_strn(&c, 1);
                    }
                }
            }
            parse_input_kind = MP_PARSE_FILE_INPUT;
        } else if (vstr_len(&line) == 0) {
            continue;
        } else {
            // got a line with non-zero length, see if it needs continuing
            while (mp_repl_continue_with_input(vstr_null_terminated_str(&line))) {
                vstr_add_byte(&line, '\n');
                ret = readline(&line, mp_repl_get_ps2());
                if (ret == CHAR_CTRL_C) {
                    // cancel everything
                    mp_hal_stdout_tx_str("\r\n");
                    goto input_restart;
                } else if (ret == CHAR_CTRL_D) {
                    // stop entering compound statement
                    break;
                }
            }
        }

        ret = parse_compile_execute(&line, parse_input_kind, EXEC_FLAG_ALLOW_DEBUGGING | EXEC_FLAG_IS_REPL | EXEC_FLAG_SOURCE_IS_VSTR);
        if (ret & PYEXEC_FORCED_EXIT) {
            return ret;
        }
    }
}

#endif // MICROPY_REPL_EVENT_DRIVEN
#endif // MICROPY_ENABLE_COMPILER

int pyexec_file(const char *filename) {
    return parse_compile_execute(filename, MP_PARSE_FILE_INPUT, EXEC_FLAG_SOURCE_IS_FILENAME);
}

int pyexec_file_if_exists(const char *filename) {
    #if MICROPY_MODULE_FROZEN
    if (mp_find_frozen_module(filename, NULL, NULL) == MP_IMPORT_STAT_FILE) {
        return pyexec_frozen_module(filename, true);
    }
    #endif
    if (mp_import_stat(filename) != MP_IMPORT_STAT_FILE) {
        return 1; // success (no file is the same as an empty file executing without fail)
    }
    return pyexec_file(filename);
}

#if MICROPY_MODULE_FROZEN
int pyexec_frozen_module(const char *name, bool allow_keyboard_interrupt) {
    void *frozen_data;
    int frozen_type;
    mp_find_frozen_module(name, &frozen_type, &frozen_data);
    mp_uint_t exec_flags = allow_keyboard_interrupt ? 0 : EXEC_FLAG_NO_INTERRUPT;

    switch (frozen_type) {
        #if MICROPY_MODULE_FROZEN_STR
        case MP_FROZEN_STR:
            return parse_compile_execute(frozen_data, MP_PARSE_FILE_INPUT, exec_flags);
        #endif

        #if MICROPY_MODULE_FROZEN_MPY
        case MP_FROZEN_MPY:
            return parse_compile_execute(frozen_data, MP_PARSE_FILE_INPUT, exec_flags |
                EXEC_FLAG_SOURCE_IS_RAW_CODE);
        #endif

        default:
            mp_printf(MICROPY_ERROR_PRINTER, "could not find module '%s'\n", name);
            return false;
    }
}
#endif

int pyexec_vstr(vstr_t *str, bool allow_keyboard_interrupt) {
    mp_uint_t exec_flags = allow_keyboard_interrupt ? 0 : EXEC_FLAG_NO_INTERRUPT;
    return parse_compile_execute(str, MP_PARSE_FILE_INPUT, exec_flags | EXEC_FLAG_SOURCE_IS_VSTR);
}

#if MICROPY_REPL_INFO
mp_obj_t pyb_set_repl_info(mp_obj_t o_value) {
    repl_display_debugging_info = mp_obj_get_int(o_value);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(pyb_set_repl_info_obj, pyb_set_repl_info);
#endif
