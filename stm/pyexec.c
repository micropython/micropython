#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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
#include "pyexec.h"
#include "storage.h"
#include "usb.h"
#include "usart.h"

static bool repl_display_debugging_info = 0;

void stdout_tx_str(const char *str) {
    if (pyb_usart_global_debug != PYB_USART_NONE) {
        usart_tx_str(pyb_usart_global_debug, str);
    }
#if defined(USE_HOST_MODE) && MICROPY_HW_HAS_LCD
    lcd_print_str(str);
#endif
    usb_vcp_send_str(str);
}

int stdin_rx_chr(void) {
    for (;;) {
#ifdef USE_HOST_MODE
        pyb_usb_host_process();
        int c = pyb_usb_host_get_keyboard();
        if (c != 0) {
            return c;
        }
#endif
        if (usb_vcp_rx_any() != 0) {
            return usb_vcp_rx_get();
        } else if (pyb_usart_global_debug != PYB_USART_NONE && usart_rx_any(pyb_usart_global_debug)) {
            return usart_rx_char(pyb_usart_global_debug);
        }
        sys_tick_delay_ms(1);
        if (storage_needs_flush()) {
            storage_flush();
        }
    }
}

char *str_dup(const char *str) {
    uint32_t len = strlen(str);
    char *s2 = m_new(char, len + 1);
    memcpy(s2, str, len);
    s2[len] = 0;
    return s2;
}

#define READLINE_HIST_SIZE (8)

static const char *readline_hist[READLINE_HIST_SIZE] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

int readline(vstr_t *line, const char *prompt) {
    stdout_tx_str(prompt);
    int len = vstr_len(line);
    int escape = 0;
    int hist_num = 0;
    for (;;) {
        int c = stdin_rx_chr();
        if (escape == 0) {
            if (VCP_CHAR_CTRL_A <= c && c <= VCP_CHAR_CTRL_D && vstr_len(line) == len) {
                return c;
            } else if (c == '\r') {
                stdout_tx_str("\r\n");
                for (int i = READLINE_HIST_SIZE - 1; i > 0; i--) {
                    readline_hist[i] = readline_hist[i - 1];
                }
                readline_hist[0] = str_dup(vstr_str(line));
                return 0;
            } else if (c == 27) {
                escape = true;
            } else if (c == 127) {
                if (vstr_len(line) > len) {
                    vstr_cut_tail_bytes(line, 1);
                    stdout_tx_str("\b \b");
                }
            } else if (32 <= c && c <= 126) {
                vstr_add_char(line, c);
                stdout_tx_str(line->buf + line->len - 1);
            }
        } else if (escape == 1) {
            if (c == '[') {
                escape = 2;
            } else {
                escape = 0;
            }
        } else if (escape == 2) {
            escape = 0;
            if (c == 'A') {
                // up arrow
                if (hist_num < READLINE_HIST_SIZE && readline_hist[hist_num] != NULL) {
                    // erase line
                    for (int i = line->len - len; i > 0; i--) {
                        stdout_tx_str("\b \b");
                    }
                    // set line to history
                    line->len = len;
                    vstr_add_str(line, readline_hist[hist_num]);
                    // draw line
                    stdout_tx_str(readline_hist[hist_num]);
                    // increase hist num
                    hist_num += 1;
                }
            }
        } else {
            escape = 0;
        }
        sys_tick_delay_ms(1);
    }
}

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
    uint32_t start = sys_tick_counter;
    if (nlr_push(&nlr) == 0) {
        usb_vcp_set_interrupt_char(VCP_CHAR_CTRL_C); // allow ctrl-C to interrupt us
        rt_call_function_0(module_fun);
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
        uint32_t ticks = sys_tick_counter - start; // TODO implement a function that does this properly
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

void pyexec_raw_repl(void) {
    vstr_t line;
    vstr_init(&line, 32);

raw_repl_reset:
    stdout_tx_str("raw REPL; CTRL-C to exit\r\n");

    for (;;) {
        vstr_reset(&line);
        stdout_tx_str(">");
        for (;;) {
            char c = stdin_rx_chr();
            if (c == VCP_CHAR_CTRL_A) {
                goto raw_repl_reset;
            } else if (c == VCP_CHAR_CTRL_C) {
                vstr_reset(&line);
                break;
            } else if (c == VCP_CHAR_CTRL_D) {
                break;
            } else if (c == '\r') {
                vstr_add_char(&line, '\n');
            } else if (32 <= c && c <= 126) {
                vstr_add_char(&line, c);
            }
        }

        stdout_tx_str("OK");

        if (vstr_len(&line) == 0) {
            // finished
            break;
        }

        mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, vstr_str(&line), vstr_len(&line), 0);
        parse_compile_execute(lex, MP_PARSE_FILE_INPUT, false);

        stdout_tx_str("\004");
    }

    vstr_clear(&line);
    stdout_tx_str("\r\n");
}

void pyexec_repl(void) {
#if defined(USE_HOST_MODE) && MICROPY_HW_HAS_LCD
    // in host mode, we enable the LCD for the repl
    mp_obj_t lcd_o = rt_call_function_0(rt_load_name(qstr_from_str("LCD")));
    rt_call_function_1(rt_load_attr(lcd_o, qstr_from_str("light")), mp_const_true);
#endif

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

    vstr_t line;
    vstr_init(&line, 32);

    for (;;) {
        vstr_reset(&line);
        int ret = readline(&line, ">>> ");

        if (ret == VCP_CHAR_CTRL_A) {
            pyexec_raw_repl();
            continue;
        } else if (ret == VCP_CHAR_CTRL_C) {
            stdout_tx_str("\r\n");
            continue;
        } else if (ret == VCP_CHAR_CTRL_D) {
            // EOF
            break;
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

    stdout_tx_str("\r\n");
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
