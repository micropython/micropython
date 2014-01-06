#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "Arduino.h"

extern "C"
{
#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "lexer.h"
#include "lexerteensy.h"
#include "parse.h"
#include "obj.h"
#include "compile.h"
#include "runtime0.h"
#include "runtime.h"
#include "repl.h"
#include "usb.h"
}

#ifdef USE_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif

#if 0
static char *str_join(const char *s1, int sep_char, const char *s2) {
    int l1 = strlen(s1);
    int l2 = strlen(s2);
    char *s = m_new(char, l1 + l2 + 2);
    memcpy(s, s1, l1);
    if (sep_char != 0) {
        s[l1] = sep_char;
        l1 += 1;
    }
    memcpy(s + l1, s2, l2);
    s[l1 + l2] = 0;
    return s;
}

static char *prompt(char *p) {
#ifdef USE_READLINE
    char *line = readline(p);
    if (line) {
        add_history(line);
    }
#else
    static char buf[256];
    fputs(p, stdout);
    char *s = fgets(buf, sizeof(buf), stdin);
    if (!s) {
        return NULL;
    }
    int l = strlen(buf);
    if (buf[l - 1] == '\n') {
        buf[l - 1] = 0;
    } else {
        l++;
    }
    char *line = m_new(char, l);
    memcpy(line, buf, l);
#endif
    return line;
}
#endif

#define READLINE_HIST_SIZE (8)

static const char *readline_hist[READLINE_HIST_SIZE] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

void stdout_tx_str(const char *str) {
//    usart_tx_str(str);
    usb_vcp_send_str(str);
}

static elapsedMillis ledTime;
static uint8_t ledState;

int readline(vstr_t *line, const char *prompt) {
    stdout_tx_str(prompt);
    int len = vstr_len(line);
    int escape = 0;
    int hist_num = 0;
    for (;;) {
        char c;
        ledState = 1;
        ledTime = 0;
        digitalWrite(LED_BUILTIN, ledState);
        for (;;) {
            if (ledTime > 200) {
                ledState = !ledState;
                digitalWrite(LED_BUILTIN, ledState);
                ledTime = 0;
            }
            if (usb_vcp_rx_any() != 0) {
                c = usb_vcp_rx_get();
                break;
#if 0
            } else if (usart_rx_any()) {
                c = usart_rx_char();
                break;
#endif
            }
            //delay(1);
            //if (storage_needs_flush()) {
            //    storage_flush();
            //}
        }
        if (escape == 0) {
            if (c == 4 && vstr_len(line) == len) {
                return 0;
            } else if (c == '\r') {
                stdout_tx_str("\r\n");
                for (int i = READLINE_HIST_SIZE - 1; i > 0; i--) {
                    readline_hist[i] = readline_hist[i - 1];
                }
                readline_hist[0] = strdup(vstr_str(line));
                return 1;
            } else if (c == 27) {
                escape = true;
            } else if (c == 127) {
                if (vstr_len(line) > len) {
                    vstr_cut_tail(line, 1);
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
        delay(10);
    }
}

void setup(void) {
    pinMode(LED_BUILTIN, OUTPUT);    
    ledState = 1;
    digitalWrite(LED_BUILTIN, ledState);
    ledTime = 0;
    // Wait for host side to get connected
    while (!usb_vcp_is_connected()) {
        if (ledTime > 100) {
            ledState = !ledState;
            digitalWrite(LED_BUILTIN, ledState);
            ledTime = 0;
        }
    }
    digitalWrite(LED_BUILTIN, 0);

    qstr_init();
    rt_init();

    stdout_tx_str("Micro Python for Teensy 3.1\r\n");
    stdout_tx_str("Type \"help()\" for more information.\r\n");
}

void loop(void) {
    vstr_t line;
    vstr_init(&line);

    vstr_reset(&line);
    int ret = readline(&line, ">>> ");
    if (ret == 0) {
        // EOF
        return;
    }

    if (vstr_len(&line) == 0) {
        return;
    }

    if (mp_repl_is_compound_stmt(vstr_str(&line))) {
        for (;;) {
            vstr_add_char(&line, '\n');
            int len = vstr_len(&line);
            int ret = readline(&line, "... ");
            if (ret == 0 || vstr_len(&line) == len) {
                // done entering compound statement
                break;
            }
        }
    }

    mp_lexer_str_buf_t sb;
    mp_lexer_t *lex = mp_lexer_new_from_str_len("<stdin>", vstr_str(&line), vstr_len(&line), false, &sb);
    mp_parse_node_t pn = mp_parse(lex, MP_PARSE_SINGLE_INPUT);
    mp_lexer_free(lex);

    if (pn != MP_PARSE_NODE_NULL) {
        mp_obj_t module_fun = mp_compile(pn, true);
        if (module_fun != mp_const_none) {
            nlr_buf_t nlr;
            uint32_t start = micros();
            if (nlr_push(&nlr) == 0) {
                rt_call_function_0(module_fun);
                nlr_pop();
                // optional timing
                if (0) {
                    uint32_t ticks = micros() - start; // TODO implement a function that does this properly
                    printf("(took %lu us)\n", ticks);
                }
            } else {
                // uncaught exception
                mp_obj_print((mp_obj_t)nlr.ret_val);
                printf("\r\n");
            }
        }
    }
}

// for sqrt
#include <math.h>
machine_float_t machine_sqrt(machine_float_t x) {
    return sqrt(x);
}

