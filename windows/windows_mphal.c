/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Damien P. George
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


#include "py/mpstate.h"

#include MICROPY_HAL_H
#include <windows.h>
#include <unistd.h>

HANDLE std_in = NULL;
HANDLE con_out = NULL;
DWORD orig_mode = 0;

STATIC void assure_stdin_handle() {
    if (!std_in) {
        std_in = GetStdHandle(STD_INPUT_HANDLE);
        assert(std_in != INVALID_HANDLE_VALUE);
    }
}

STATIC void assure_conout_handle() {
    if (!con_out) {
        con_out = CreateFile("CONOUT$", GENERIC_READ | GENERIC_WRITE,
                      FILE_SHARE_READ | FILE_SHARE_WRITE,
                      NULL, OPEN_EXISTING, 0, 0);
        assert(con_out != INVALID_HANDLE_VALUE);
    }
}

void mp_hal_stdio_mode_raw(void) {
    assure_stdin_handle();
    GetConsoleMode(std_in, &orig_mode);
    DWORD mode = orig_mode;
    mode &= ~ENABLE_ECHO_INPUT;
    mode &= ~ENABLE_LINE_INPUT;
    mode &= ~ENABLE_PROCESSED_INPUT;
    SetConsoleMode(std_in, mode);
}

void mp_hal_stdio_mode_orig(void) {
    assure_stdin_handle();
    SetConsoleMode(std_in, orig_mode);
}

void mp_hal_set_interrupt_char(char c) {
    assure_stdin_handle();
    if (c == CHAR_CTRL_C) {
        DWORD mode;
        GetConsoleMode(std_in, &mode);
        mode |= ENABLE_PROCESSED_INPUT;
        SetConsoleMode(std_in, mode);
    } else {
        DWORD mode;
        GetConsoleMode(std_in, &mode);
        mode &= ~ENABLE_PROCESSED_INPUT;
        SetConsoleMode(std_in, mode);
    }
}

void mp_hal_move_cursor_back(uint pos) {
    assure_conout_handle();
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(con_out, &info);
    info.dwCursorPosition.X -= (short)pos;
    if (info.dwCursorPosition.X < 0) {
        info.dwCursorPosition.X = 0;
    }
    SetConsoleCursorPosition(con_out, info.dwCursorPosition);
}

void mp_hal_erase_line_from_cursor() {
    assure_conout_handle();
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(con_out, &info);
    const short len = info.dwSize.X - info.dwCursorPosition.X;
    DWORD written;
    FillConsoleOutputCharacter(con_out, ' ', len, info.dwCursorPosition, &written);
    FillConsoleOutputAttribute(con_out, info.wAttributes, len, info.dwCursorPosition, &written);
}

typedef struct item_t {
    WORD vkey;
    const char *seq;
} item_t;

// map virtual key codes to VT100 escape sequences
STATIC item_t keyCodeMap[] = {
    {VK_UP, "[A"},
    {VK_DOWN, "[B"},
    {VK_RIGHT, "[C"},
    {VK_LEFT, "[D"},
    {VK_HOME, "[H"},
    {VK_END, "[F"},
    {VK_DELETE, "[3~"},
    {0, ""} //sentinel
};

STATIC const char *cur_esc_seq = NULL;

STATIC int esc_seq_process_vk(int vk) {
    for (item_t *p = keyCodeMap; p->vkey != 0; ++p) {
        if (p->vkey == vk) {
            cur_esc_seq = p->seq;
            return 27; // ESC, start of escape sequence
        }
    }
    return 0; // nothing found
}

STATIC int esc_seq_chr() {
    if (cur_esc_seq) {
        const char c = *cur_esc_seq++;
        if (c) {
            return c;
        }
        cur_esc_seq = NULL;
    }
    return 0;
}

int mp_hal_stdin_rx_chr(void) {
    // currently processing escape seq?
    const int ret = esc_seq_chr();
    if (ret) {
        return ret;
    }

    // poll until key which we handle is pressed
    assure_stdin_handle();
    DWORD num_read;
    INPUT_RECORD rec;
    for (;;) {
      if (!ReadConsoleInput(std_in, &rec, 1, &num_read) || !num_read) {
          return CHAR_CTRL_C; // EOF, ctrl-D
      }
      if (rec.EventType != KEY_EVENT || !rec.Event.KeyEvent.bKeyDown) { // only want key down events
          continue;
      }
      const char c = rec.Event.KeyEvent.uChar.AsciiChar;
      if (c) { // plain ascii char, return it
          return c;
      }
      const int ret = esc_seq_process_vk(rec.Event.KeyEvent.wVirtualKeyCode);
      if (ret) {
          return ret;
      }
    }
}

void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    write(1, str, len);
}

void mp_hal_stdout_tx_strn_cooked(const char *str, mp_uint_t len) {
    mp_hal_stdout_tx_strn(str, len);
}

void mp_hal_stdout_tx_str(const char *str) {
    mp_hal_stdout_tx_strn(str, strlen(str));
}
