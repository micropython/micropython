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
#ifndef MICROPY_INCLUDED_LIB_MP_READLINE_READLINE_H
#define MICROPY_INCLUDED_LIB_MP_READLINE_READLINE_H

#define CHAR_CTRL_A (1)
#define CHAR_CTRL_B (2)
#define CHAR_CTRL_C (3)
#define CHAR_CTRL_D (4)
#define CHAR_CTRL_E (5)
#define CHAR_CTRL_F (6)
#define CHAR_CTRL_K (11)
#define CHAR_CTRL_N (14)
#define CHAR_CTRL_P (16)
#define CHAR_CTRL_U (21)

void readline_init0(void);
int readline(vstr_t *line, const char *prompt);
void readline_push_history(const char *line);

void readline_init(vstr_t *line, const char *prompt);
void readline_note_newline(const char *prompt);
int readline_process_char(int c);

#endif // MICROPY_INCLUDED_LIB_MP_READLINE_READLINE_H
