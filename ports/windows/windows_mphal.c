/*
 * This file is part of the MicroPython project, http://micropython.org/
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


#include "py/runtime.h"
#include "py/mphal.h"
#include "py/mpthread.h"

#include <sys/time.h>
#include <windows.h>
#include <unistd.h>
#include <bcrypt.h>

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

// Handler to be installed by SetConsoleCtrlHandler, currently used only to handle Ctrl-C.
// This handler has to be installed just once (this has to be done elswhere in init code).
// Previous versions of the mp_hal code would install a handler whenever Ctrl-C input is
// allowed and remove the handler again when it is not. That is not necessary though (1),
// and it might introduce problems (2) because console notifications are delivered to the
// application in a separate thread.
// (1) mp_hal_set_interrupt_char effectively enables/disables processing of Ctrl-C via the
// ENABLE_PROCESSED_INPUT flag so in raw mode console_sighandler won't be called.
// (2) if mp_hal_set_interrupt_char would remove the handler while Ctrl-C was issued earlier,
// the thread created for handling it might not be running yet so we'd miss the notification.
BOOL WINAPI console_sighandler(DWORD evt) {
    if (evt == CTRL_C_EVENT) {
        if (MP_STATE_MAIN_THREAD(mp_pending_exception) == MP_OBJ_FROM_PTR(&MP_STATE_VM(mp_kbd_exception))) {
            // this is the second time we are called, so die straight away
            exit(1);
        }
        mp_sched_keyboard_interrupt();
        return TRUE;
    }
    return FALSE;
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
    if (!pos) {
        return;
    }
    assure_conout_handle();
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(con_out, &info);
    info.dwCursorPosition.X -= (short)pos;
    // Move up a line if needed.
    while (info.dwCursorPosition.X < 0) {
        info.dwCursorPosition.X = info.dwSize.X + info.dwCursorPosition.X;
        info.dwCursorPosition.Y -= 1;
    }
    // Caller requested to move out of the screen. That's not possible so just clip,
    // it's the caller's responsibility to not let this happen.
    if (info.dwCursorPosition.Y < 0) {
        info.dwCursorPosition.X = 0;
        info.dwCursorPosition.Y = 0;
    }
    SetConsoleCursorPosition(con_out, info.dwCursorPosition);
}

void mp_hal_erase_line_from_cursor(uint n_chars_to_erase) {
    assure_conout_handle();
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(con_out, &info);
    DWORD written;
    FillConsoleOutputCharacter(con_out, ' ', n_chars_to_erase, info.dwCursorPosition, &written);
    FillConsoleOutputAttribute(con_out, info.wAttributes, n_chars_to_erase, info.dwCursorPosition, &written);
}

typedef struct item_t {
    WORD vkey;
    const char *seq;
} item_t;

// map virtual key codes to key sequences known by MicroPython's readline implementation
STATIC item_t keyCodeMap[] = {
    {VK_UP, "[A"},
    {VK_DOWN, "[B"},
    {VK_RIGHT, "[C"},
    {VK_LEFT, "[D"},
    {VK_HOME, "[H"},
    {VK_END, "[F"},
    {VK_DELETE, "[3~"},
    {0, ""} // sentinel
};

// likewise, but with Ctrl key down
STATIC item_t ctrlKeyCodeMap[] = {
    {VK_LEFT, "b"},
    {VK_RIGHT, "f"},
    {VK_DELETE, "d"},
    {VK_BACK, "\x7F"},
    {0, ""} // sentinel
};

STATIC const char *cur_esc_seq = NULL;

STATIC int esc_seq_process_vk(WORD vk, bool ctrl_key_down) {
    for (item_t *p = (ctrl_key_down ? ctrlKeyCodeMap : keyCodeMap); p->vkey != 0; ++p) {
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
    BOOL status;
    DWORD num_read;
    INPUT_RECORD rec;
    for (;;) {
        MP_THREAD_GIL_EXIT();
        status = ReadConsoleInput(std_in, &rec, 1, &num_read);
        MP_THREAD_GIL_ENTER();
        if (!status || !num_read) {
            return CHAR_CTRL_C; // EOF, ctrl-D
        }
        if (rec.EventType != KEY_EVENT || !rec.Event.KeyEvent.bKeyDown) { // only want key down events
            continue;
        }
        const bool ctrl_key_down = (rec.Event.KeyEvent.dwControlKeyState & LEFT_CTRL_PRESSED) ||
            (rec.Event.KeyEvent.dwControlKeyState & RIGHT_CTRL_PRESSED);
        const int ret = esc_seq_process_vk(rec.Event.KeyEvent.wVirtualKeyCode, ctrl_key_down);
        if (ret) {
            return ret;
        }
        const char c = rec.Event.KeyEvent.uChar.AsciiChar;
        if (c) { // plain ascii char, return it
            return c;
        }
    }
}

mp_uint_t mp_hal_stdout_tx_strn(const char *str, size_t len) {
    MP_THREAD_GIL_EXIT();
    int ret = write(STDOUT_FILENO, str, len);
    MP_THREAD_GIL_ENTER();
    return ret < 0 ? 0 : ret; // return the number of bytes written, so in case of an error in the syscall, return 0
}

void mp_hal_stdout_tx_strn_cooked(const char *str, size_t len) {
    mp_hal_stdout_tx_strn(str, len);
}

void mp_hal_stdout_tx_str(const char *str) {
    mp_hal_stdout_tx_strn(str, strlen(str));
}

mp_uint_t mp_hal_ticks_ms(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

mp_uint_t mp_hal_ticks_us(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000 + tv.tv_usec;
}

mp_uint_t mp_hal_ticks_cpu(void) {
    LARGE_INTEGER value;
    QueryPerformanceCounter(&value);
    #ifdef _WIN64
    return value.QuadPart;
    #else
    return value.LowPart;
    #endif
}

uint64_t mp_hal_time_ns(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000000000ULL + (uint64_t)tv.tv_usec * 1000ULL;
}

void msec_sleep(double msec) {
    if (msec < 0.0) {
        msec = 0.0;
    }
    SleepEx((DWORD)msec, TRUE);
}

#ifdef _MSC_VER
int usleep(__int64 usec) {
    msec_sleep((double)usec / 1000.0);
    return 0;
}
#endif

void mp_hal_delay_ms(mp_uint_t ms) {
    #if MICROPY_ENABLE_SCHEDULER
    mp_uint_t start = mp_hal_ticks_ms();
    while (mp_hal_ticks_ms() - start < ms) {
        mp_event_wait_ms(1);
    }
    #else
    msec_sleep((double)ms);
    #endif
}

void mp_hal_get_random(size_t n, void *buf) {
    NTSTATUS result = BCryptGenRandom(NULL, (unsigned char *)buf, n, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
    if (!BCRYPT_SUCCESS(result)) {
        mp_raise_OSError(errno);
    }
}
