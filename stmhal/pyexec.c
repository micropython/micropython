#include <stdlib.h>
#include <stdio.h>

#include <stm32f4xx_hal.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "misc.h"
#include "lexer.h"
#include "parse.h"
#include "obj.h"
#include "parsehelper.h"
#include "compile.h"
#include "runtime.h"
#include "repl.h"
#include "gc.h"
#include "gccollect.h"
#include "systick.h"
#include "pybstdio.h"
#include "readline.h"
#include "pyexec.h"
#include "storage.h"
#include "usb.h"

pyexec_mode_kind_t pyexec_mode_kind = PYEXEC_MODE_FRIENDLY_REPL;
STATIC bool repl_display_debugging_info = 0;

// parses, compiles and executes the code in the lexer
// frees the lexer before returning
bool parse_compile_execute(mp_lexer_t *lex, mp_parse_input_kind_t input_kind, bool is_repl) {
    mp_parse_error_kind_t parse_error_kind;
    mp_parse_node_t pn = mp_parse(lex, input_kind, &parse_error_kind);
    qstr source_name = mp_lexer_source_name(lex);

    if (pn == MP_PARSE_NODE_NULL) {
        // parse error
        mp_parse_show_exception(lex, parse_error_kind);
        mp_lexer_free(lex);
        return false;
    }

    mp_lexer_free(lex);

    mp_obj_t module_fun = mp_compile(pn, source_name, is_repl);
    mp_parse_node_free(pn);

    if (module_fun == mp_const_none) {
        return false;
    }

    nlr_buf_t nlr;
    bool ret;
    uint32_t start = HAL_GetTick();
    if (nlr_push(&nlr) == 0) {
        usb_vcp_set_interrupt_char(VCP_CHAR_CTRL_C); // allow ctrl-C to interrupt us
        mp_call_function_0(module_fun);
        usb_vcp_set_interrupt_char(VCP_CHAR_NONE); // disable interrupt
        nlr_pop();
        ret = true;
    } else {
        // uncaught exception
        // FIXME it could be that an interrupt happens just before we disable it here
        usb_vcp_set_interrupt_char(VCP_CHAR_NONE); // disable interrupt
        mp_obj_print_exception((mp_obj_t)nlr.ret_val);
        ret = false;
    }

    // display debugging info if wanted
    if (is_repl && repl_display_debugging_info) {
        uint32_t ticks = HAL_GetTick() - start; // TODO implement a function that does this properly
        printf("took %lu ms\n", ticks);
        gc_collect();
        // qstr info
        {
            uint n_pool, n_qstr, n_str_data_bytes, n_total_bytes;
            qstr_pool_info(&n_pool, &n_qstr, &n_str_data_bytes, &n_total_bytes);
            printf("qstr:\n  n_pool=%u\n  n_qstr=%u\n  n_str_data_bytes=%u\n  n_total_bytes=%u\n", n_pool, n_qstr, n_str_data_bytes, n_total_bytes);
        }

        // GC info
        {
            gc_info_t info;
            gc_info(&info);
            printf("GC:\n");
            printf("  %lu total\n", info.total);
            printf("  %lu : %lu\n", info.used, info.free);
            printf("  1=%lu 2=%lu m=%lu\n", info.num_1block, info.num_2block, info.max_block);
        }
    }

    return ret;
}

int pyexec_raw_repl(void) {
    vstr_t line;
    vstr_init(&line, 32);

raw_repl_reset:
    stdout_tx_str("raw REPL; CTRL-B to exit\r\n");

    for (;;) {
        vstr_reset(&line);
        stdout_tx_str(">");
        for (;;) {
            char c = stdin_rx_chr();
            if (c == VCP_CHAR_CTRL_A) {
                // reset raw REPL
                goto raw_repl_reset;
            } else if (c == VCP_CHAR_CTRL_B) {
                // change to friendly REPL
                stdout_tx_str("\r\n");
                vstr_clear(&line);
                pyexec_mode_kind = PYEXEC_MODE_FRIENDLY_REPL;
                return 0;
            } else if (c == VCP_CHAR_CTRL_C) {
                // clear line
                vstr_reset(&line);
            } else if (c == VCP_CHAR_CTRL_D) {
                // input finished
                break;
            } else if (c <= 127) {
                // let through any other ASCII character
                vstr_add_char(&line, c);
            }
        }

        // indicate reception of command
        stdout_tx_str("OK");

        if (line.len == 0) {
            // exit for a soft reset
            stdout_tx_str("\r\n");
            vstr_clear(&line);
            return 1;
        }

        mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, line.buf, line.len, 0);
        parse_compile_execute(lex, MP_PARSE_FILE_INPUT, false);

        // indicate end of output with EOF character
        stdout_tx_str("\004");
    }
}

int pyexec_friendly_repl(void) {
    vstr_t line;
    vstr_init(&line, 32);

#if defined(USE_HOST_MODE) && MICROPY_HW_HAS_LCD
    // in host mode, we enable the LCD for the repl
    mp_obj_t lcd_o = mp_call_function_0(mp_load_name(qstr_from_str("LCD")));
    mp_call_function_1(mp_load_attr(lcd_o, qstr_from_str("light")), mp_const_true);
#endif

friendly_repl_reset:
    stdout_tx_str("Micro Python build <git hash> on 25/1/2014; " MICROPY_HW_BOARD_NAME " with STM32F405RG\r\n");
    stdout_tx_str("Type \"help()\" for more information.\r\n");

    // to test ctrl-C
    /*
    {
        uint32_t x[4] = {0x424242, 0xdeaddead, 0x242424, 0xdeadbeef};
        for (;;) {
            nlr_buf_t nlr;
            printf("pyexec_repl: %p\n", x);
            usb_vcp_set_interrupt_char(VCP_CHAR_CTRL_C);
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
        vstr_reset(&line);
        int ret = readline(&line, ">>> ");

        if (ret == VCP_CHAR_CTRL_A) {
            // change to raw REPL
            stdout_tx_str("\r\n");
            vstr_clear(&line);
            pyexec_mode_kind = PYEXEC_MODE_RAW_REPL;
            return 0;
        } else if (ret == VCP_CHAR_CTRL_B) {
            // reset friendly REPL
            stdout_tx_str("\r\n");
            goto friendly_repl_reset;
        } else if (ret == VCP_CHAR_CTRL_C) {
            // break
            stdout_tx_str("\r\n");
            continue;
        } else if (ret == VCP_CHAR_CTRL_D) {
            // exit for a soft reset
            stdout_tx_str("\r\n");
            vstr_clear(&line);
            return 1;
        } else if (vstr_len(&line) == 0) {
            continue;
        }

        if (mp_repl_is_compound_stmt(vstr_str(&line))) {
            for (;;) {
                vstr_add_char(&line, '\n');
                int len = vstr_len(&line);
                int ret = readline(&line, "... ");
                if (ret == VCP_CHAR_CTRL_D || vstr_len(&line) == len) {
                    // done entering compound statement
                    break;
                }
            }
        }

        mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, vstr_str(&line), vstr_len(&line), 0);
        parse_compile_execute(lex, MP_PARSE_SINGLE_INPUT, true);
    }
}

bool pyexec_file(const char *filename) {
    mp_lexer_t *lex = mp_lexer_new_from_file(filename);

    if (lex == NULL) {
        printf("could not open file '%s' for reading\n", filename);
        return false;
    }

    return parse_compile_execute(lex, MP_PARSE_FILE_INPUT, false);
}

mp_obj_t pyb_set_repl_info(mp_obj_t o_value) {
    repl_display_debugging_info = mp_obj_get_int(o_value);
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_1(pyb_set_repl_info_obj, pyb_set_repl_info);
