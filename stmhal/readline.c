/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "mpconfig.h"
#include "misc.h"
#include "qstr.h"
#include "misc.h"
#include "obj.h"
#include MICROPY_HAL_H
#include "readline.h"
#include "usb.h"
#include "uart.h"
#include "pybstdio.h"

#if 0 // print debugging info
#define DEBUG_PRINT (1)
#define DEBUG_printf printf
#else // don't print debugging info
#define DEBUG_printf(...) (void)0
#endif

#define READLINE_HIST_SIZE (8)

static const char *readline_hist[READLINE_HIST_SIZE] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

enum { ESEQ_NONE, ESEQ_ESC, ESEQ_ESC_BRACKET, ESEQ_ESC_BRACKET_DIGIT, ESEQ_ESC_O };

void readline_init0(void) {
    memset(readline_hist, 0, READLINE_HIST_SIZE * sizeof(const char*));
}

STATIC char *str_dup_maybe(const char *str) {
    uint32_t len = strlen(str);
    char *s2 = m_new_maybe(char, len + 1);
    if (s2 == NULL) {
        return NULL;
    }
    memcpy(s2, str, len + 1);
    return s2;
}

int readline(vstr_t *line, const char *prompt) {
    stdout_tx_str(prompt);
    int orig_line_len = line->len;
    int escape_seq = ESEQ_NONE;
    char escape_seq_buf[1] = {0};
    int hist_cur = -1;
    int cursor_pos = orig_line_len;
    for (;;) {
        int c = stdin_rx_chr();
        int last_line_len = line->len;
        int redraw_step_back = 0;
        bool redraw_from_cursor = false;
        int redraw_step_forward = 0;
        if (escape_seq == ESEQ_NONE) {
            if (VCP_CHAR_CTRL_A <= c && c <= VCP_CHAR_CTRL_D && vstr_len(line) == orig_line_len) {
                // control character with empty line
                return c;
            } else if (c == VCP_CHAR_CTRL_A) {
                // CTRL-A with non-empty line is go-to-start-of-line
                goto home_key;
            } else if (c == VCP_CHAR_CTRL_E) {
                // CTRL-E is go-to-end-of-line
                goto end_key;
            } else if (c == '\r') {
                // newline
                stdout_tx_str("\r\n");
                if (line->len > orig_line_len && (readline_hist[0] == NULL || strcmp(readline_hist[0], line->buf + orig_line_len) != 0)) {
                    // a line which is not empty and different from the last one
                    // so update the history
                    char *most_recent_hist = str_dup_maybe(line->buf + orig_line_len);
                    if (most_recent_hist != NULL) {
                        for (int i = READLINE_HIST_SIZE - 1; i > 0; i--) {
                            readline_hist[i] = readline_hist[i - 1];
                        }
                        readline_hist[0] = most_recent_hist;
                    }
                }
                return 0;
            } else if (c == 27) {
                // escape sequence
                escape_seq = ESEQ_ESC;
            } else if (c == 8 || c == 127) {
                // backspace/delete
                if (cursor_pos > orig_line_len) {
                    vstr_cut_out_bytes(line, cursor_pos - 1, 1);
                    // set redraw parameters
                    redraw_step_back = 1;
                    redraw_from_cursor = true;
                }
            } else if (32 <= c && c <= 126) {
                // printable character
                vstr_ins_char(line, cursor_pos, c);
                // set redraw parameters
                redraw_from_cursor = true;
                redraw_step_forward = 1;
            }
        } else if (escape_seq == ESEQ_ESC) {
            switch (c) {
                case '[':
                    escape_seq = ESEQ_ESC_BRACKET;
                    break;
                case 'O':
                    escape_seq = ESEQ_ESC_O;
                    break;
                default:
                    DEBUG_printf("(ESC %d)", c);
                    escape_seq = ESEQ_NONE;
            }
        } else if (escape_seq == ESEQ_ESC_BRACKET) {
            if ('0' <= c && c <= '9') {
                escape_seq = ESEQ_ESC_BRACKET_DIGIT;
                escape_seq_buf[0] = c;
            } else {
                escape_seq = ESEQ_NONE;
                if (c == 'A') {
                    // up arrow
                    if (hist_cur + 1 < READLINE_HIST_SIZE && readline_hist[hist_cur + 1] != NULL) {
                        // increase hist num
                        hist_cur += 1;
                        // set line to history
                        line->len = orig_line_len;
                        vstr_add_str(line, readline_hist[hist_cur]);
                        // set redraw parameters
                        redraw_step_back = cursor_pos - orig_line_len;
                        redraw_from_cursor = true;
                        redraw_step_forward = line->len - orig_line_len;
                    }
                } else if (c == 'B') {
                    // down arrow
                    if (hist_cur >= 0) {
                        // decrease hist num
                        hist_cur -= 1;
                        // set line to history
                        vstr_cut_tail_bytes(line, line->len - orig_line_len);
                        if (hist_cur >= 0) {
                            vstr_add_str(line, readline_hist[hist_cur]);
                        }
                        // set redraw parameters
                        redraw_step_back = cursor_pos - orig_line_len;
                        redraw_from_cursor = true;
                        redraw_step_forward = line->len - orig_line_len;
                    }
                } else if (c == 'C') {
                    // right arrow
                    if (cursor_pos < line->len) {
                        redraw_step_forward = 1;
                    }
                } else if (c == 'D') {
                    // left arrow
                    if (cursor_pos > orig_line_len) {
                        redraw_step_back = 1;
                    }
                } else if (c == 'H') {
                    // home
                    goto home_key;
                } else if (c == 'F') {
                    // end
                    goto end_key;
                } else {
                    DEBUG_printf("(ESC [ %d)", c);
                }
            }
        } else if (escape_seq == ESEQ_ESC_BRACKET_DIGIT) {
            if (c == '~') {
                if (escape_seq_buf[0] == '1' || escape_seq_buf[0] == '7') {
home_key:
                    redraw_step_back = cursor_pos - orig_line_len;
                } else if (escape_seq_buf[0] == '4' || escape_seq_buf[0] == '8') {
end_key:
                    redraw_step_forward = line->len - cursor_pos;
                } else {
                    DEBUG_printf("(ESC [ %c %d)", escape_seq_buf[0], c);
                }
            } else {
                DEBUG_printf("(ESC [ %c %d)", escape_seq_buf[0], c);
            }
            escape_seq = ESEQ_NONE;
        } else if (escape_seq == ESEQ_ESC_O) {
            switch (c) {
                case 'H':
                    goto home_key;
                case 'F':
                    goto end_key;
                default:
                    DEBUG_printf("(ESC O %d)", c);
                    escape_seq = ESEQ_NONE;
            }
        } else {
            escape_seq = ESEQ_NONE;
        }

        // redraw command prompt, efficiently
        // TODO we can probably use some more sophisticated VT100 commands here
        if (redraw_step_back > 0) {
            for (int i = 0; i < redraw_step_back; i++) {
                stdout_tx_str("\b");
            }
            cursor_pos -= redraw_step_back;
        }
        if (redraw_from_cursor) {
            if (line->len < last_line_len) {
                // erase old chars
                for (int i = cursor_pos; i < last_line_len; i++) {
                    stdout_tx_str(" ");
                }
                // step back
                for (int i = cursor_pos; i < last_line_len; i++) {
                    stdout_tx_str("\b");
                }
            }
            // draw new chars
            stdout_tx_strn(line->buf + cursor_pos, line->len - cursor_pos);
            // move cursor forward if needed (already moved forward by length of line, so move it back)
            for (int i = cursor_pos + redraw_step_forward; i < line->len; i++) {
                stdout_tx_str("\b");
            }
            cursor_pos += redraw_step_forward;
        } else if (redraw_step_forward > 0) {
            // draw over old chars to move cursor forwards
            stdout_tx_strn(line->buf + cursor_pos, redraw_step_forward);
            cursor_pos += redraw_step_forward;
        }

        HAL_Delay(1);
    }
}
