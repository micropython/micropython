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

#include "py/mpstate.h"
#include "readline.h"
#ifdef MICROPY_HAL_H
#include MICROPY_HAL_H
#endif

#if 0 // print debugging info
#define DEBUG_PRINT (1)
#define DEBUG_printf printf
#else // don't print debugging info
#define DEBUG_printf(...) (void)0
#endif

#define READLINE_HIST_SIZE (MP_ARRAY_SIZE(MP_STATE_PORT(readline_hist)))

enum { ESEQ_NONE, ESEQ_ESC, ESEQ_ESC_BRACKET, ESEQ_ESC_BRACKET_DIGIT, ESEQ_ESC_O };

void readline_init0(void) {
    memset(MP_STATE_PORT(readline_hist), 0, READLINE_HIST_SIZE * sizeof(const char*));
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

STATIC void move_cursor_back(uint pos) {
    if (pos <= 4) {
        // fast path for most common case of 1 step back
        mp_hal_stdout_tx_strn("\b\b\b\b", pos);
    } else {
        char vt100_command[6];
        // snprintf needs space for the terminating null character
        int n = snprintf(&vt100_command[0], sizeof(vt100_command), "\x1b[%u", pos);
        if (n > 0) {
            vt100_command[n] = 'D'; // replace null char
            mp_hal_stdout_tx_strn(vt100_command, n + 1);
        }
    }
}

STATIC void erase_line_from_cursor(void) {
    mp_hal_stdout_tx_strn("\x1b[K", 3);
}

typedef struct _readline_t {
    vstr_t *line;
    int orig_line_len;
    int escape_seq;
    int hist_cur;
    int cursor_pos;
    char escape_seq_buf[1];
} readline_t;

STATIC readline_t rl;

int readline_process_char(int c) {
    int last_line_len = rl.line->len;
    int redraw_step_back = 0;
    bool redraw_from_cursor = false;
    int redraw_step_forward = 0;
    if (rl.escape_seq == ESEQ_NONE) {
        if (CHAR_CTRL_A <= c && c <= CHAR_CTRL_D && vstr_len(rl.line) == rl.orig_line_len) {
            // control character with empty line
            return c;
        } else if (c == CHAR_CTRL_A) {
            // CTRL-A with non-empty line is go-to-start-of-line
            goto home_key;
        } else if (c == CHAR_CTRL_C) {
            // CTRL-C with non-empty line is cancel
            return c;
        } else if (c == CHAR_CTRL_E) {
            // CTRL-E is go-to-end-of-line
            goto end_key;
        } else if (c == '\r') {
            // newline
            mp_hal_stdout_tx_str("\r\n");
            if (rl.line->len > rl.orig_line_len && (MP_STATE_PORT(readline_hist)[0] == NULL || strcmp(MP_STATE_PORT(readline_hist)[0], rl.line->buf + rl.orig_line_len) != 0)) {
                // a line which is not empty and different from the last one
                // so update the history
                char *most_recent_hist = str_dup_maybe(vstr_null_terminated_str(rl.line) + rl.orig_line_len);
                if (most_recent_hist != NULL) {
                    for (int i = READLINE_HIST_SIZE - 1; i > 0; i--) {
                        MP_STATE_PORT(readline_hist)[i] = MP_STATE_PORT(readline_hist)[i - 1];
                    }
                    MP_STATE_PORT(readline_hist)[0] = most_recent_hist;
                }
            }
            return 0;
        } else if (c == 27) {
            // escape sequence
            rl.escape_seq = ESEQ_ESC;
        } else if (c == 8 || c == 127) {
            // backspace/delete
            if (rl.cursor_pos > rl.orig_line_len) {
                vstr_cut_out_bytes(rl.line, rl.cursor_pos - 1, 1);
                // set redraw parameters
                redraw_step_back = 1;
                redraw_from_cursor = true;
            }
        } else if (32 <= c && c <= 126) {
            // printable character
            vstr_ins_char(rl.line, rl.cursor_pos, c);
            // set redraw parameters
            redraw_from_cursor = true;
            redraw_step_forward = 1;
        }
    } else if (rl.escape_seq == ESEQ_ESC) {
        switch (c) {
            case '[':
                rl.escape_seq = ESEQ_ESC_BRACKET;
                break;
            case 'O':
                rl.escape_seq = ESEQ_ESC_O;
                break;
            default:
                DEBUG_printf("(ESC %d)", c);
                rl.escape_seq = ESEQ_NONE;
        }
    } else if (rl.escape_seq == ESEQ_ESC_BRACKET) {
        if ('0' <= c && c <= '9') {
            rl.escape_seq = ESEQ_ESC_BRACKET_DIGIT;
            rl.escape_seq_buf[0] = c;
        } else {
            rl.escape_seq = ESEQ_NONE;
            if (c == 'A') {
                // up arrow
                if (rl.hist_cur + 1 < READLINE_HIST_SIZE && MP_STATE_PORT(readline_hist)[rl.hist_cur + 1] != NULL) {
                    // increase hist num
                    rl.hist_cur += 1;
                    // set line to history
                    rl.line->len = rl.orig_line_len;
                    vstr_add_str(rl.line, MP_STATE_PORT(readline_hist)[rl.hist_cur]);
                    // set redraw parameters
                    redraw_step_back = rl.cursor_pos - rl.orig_line_len;
                    redraw_from_cursor = true;
                    redraw_step_forward = rl.line->len - rl.orig_line_len;
                }
            } else if (c == 'B') {
                // down arrow
                if (rl.hist_cur >= 0) {
                    // decrease hist num
                    rl.hist_cur -= 1;
                    // set line to history
                    vstr_cut_tail_bytes(rl.line, rl.line->len - rl.orig_line_len);
                    if (rl.hist_cur >= 0) {
                        vstr_add_str(rl.line, MP_STATE_PORT(readline_hist)[rl.hist_cur]);
                    }
                    // set redraw parameters
                    redraw_step_back = rl.cursor_pos - rl.orig_line_len;
                    redraw_from_cursor = true;
                    redraw_step_forward = rl.line->len - rl.orig_line_len;
                }
            } else if (c == 'C') {
                // right arrow
                if (rl.cursor_pos < rl.line->len) {
                    redraw_step_forward = 1;
                }
            } else if (c == 'D') {
                // left arrow
                if (rl.cursor_pos > rl.orig_line_len) {
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
    } else if (rl.escape_seq == ESEQ_ESC_BRACKET_DIGIT) {
        if (c == '~') {
            if (rl.escape_seq_buf[0] == '1' || rl.escape_seq_buf[0] == '7') {
home_key:
                redraw_step_back = rl.cursor_pos - rl.orig_line_len;
            } else if (rl.escape_seq_buf[0] == '4' || rl.escape_seq_buf[0] == '8') {
end_key:
                redraw_step_forward = rl.line->len - rl.cursor_pos;
            } else {
                DEBUG_printf("(ESC [ %c %d)", rl.escape_seq_buf[0], c);
            }
        } else {
            DEBUG_printf("(ESC [ %c %d)", rl.escape_seq_buf[0], c);
        }
        rl.escape_seq = ESEQ_NONE;
    } else if (rl.escape_seq == ESEQ_ESC_O) {
        switch (c) {
            case 'H':
                goto home_key;
            case 'F':
                goto end_key;
            default:
                DEBUG_printf("(ESC O %d)", c);
                rl.escape_seq = ESEQ_NONE;
        }
    } else {
        rl.escape_seq = ESEQ_NONE;
    }

    // redraw command prompt, efficiently
    if (redraw_step_back > 0) {
        move_cursor_back(redraw_step_back);
        rl.cursor_pos -= redraw_step_back;
    }
    if (redraw_from_cursor) {
        if (rl.line->len < last_line_len) {
            // erase old chars
            // (number of chars to erase: last_line_len - rl.cursor_pos)
            erase_line_from_cursor();
        }
        // draw new chars
        mp_hal_stdout_tx_strn(rl.line->buf + rl.cursor_pos, rl.line->len - rl.cursor_pos);
        // move cursor forward if needed (already moved forward by length of line, so move it back)
        move_cursor_back(rl.line->len - (rl.cursor_pos + redraw_step_forward));
        rl.cursor_pos += redraw_step_forward;
    } else if (redraw_step_forward > 0) {
        // draw over old chars to move cursor forwards
        mp_hal_stdout_tx_strn(rl.line->buf + rl.cursor_pos, redraw_step_forward);
        rl.cursor_pos += redraw_step_forward;
    }

    return -1;
}

void readline_note_newline(void) {
    rl.orig_line_len = rl.line->len;
    rl.cursor_pos = rl.orig_line_len;
}

void readline_init(vstr_t *line) {
    rl.line = line;
    rl.orig_line_len = line->len;
    rl.escape_seq = ESEQ_NONE;
    rl.escape_seq_buf[0] = 0;
    rl.hist_cur = -1;
    rl.cursor_pos = rl.orig_line_len;
}

int readline(vstr_t *line, const char *prompt) {
    mp_hal_stdout_tx_str(prompt);
    readline_init(line);
    for (;;) {
        int c = mp_hal_stdin_rx_chr();
        int r = readline_process_char(c);
        if (r >= 0) {
            return r;
        }
    }
}
