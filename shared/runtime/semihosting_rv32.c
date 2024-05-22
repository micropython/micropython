/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Alessandro Gatti
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

#include <stdbool.h>

#include "semihosting_rv32.h"

#if !defined(__riscv) || !defined(__riscv_xlen) || (__riscv_xlen != 32)
#error "This semihosting support code is only available for RV32 targets."
#endif

// Features file magic header.
#define MAGIC_SIZE 4
#define MAGIC_0 0x53 // 'S'
#define MAGIC_1 0x48 // 'H'
#define MAGIC_2 0x46 // 'B'
#define MAGIC_3 0x42 // 'F'

#define CMDLINE_MIN_BUFFER_SIZE 80

#define SYS_OPEN          0x01
#define SYS_CLOSE         0x02
#define SYS_WRITEC        0x03
#define SYS_WRITE0        0x04
#define SYS_WRITE         0x05
#define SYS_READ          0x06
#define SYS_READC         0x07
#define SYS_ISERROR       0x08
#define SYS_ISTTY         0x09
#define SYS_SEEK          0x0A
#define SYS_FLEN          0x0C
#define SYS_TMPNAM        0x0D
#define SYS_REMOVE        0x0E
#define SYS_RENAME        0x0F
#define SYS_CLOCK         0x10
#define SYS_TIME          0x11
#define SYS_SYSTEM        0x12
#define SYS_ERRNO         0x13
#define SYS_GET_CMDLINE   0x15
#define SYS_HEAPINFO      0x16
#define SYS_EXIT          0x18
#define SYS_EXIT_EXTENDED 0x20
#define SYS_ELAPSED       0x30
#define SYS_TICKFREQ      0x31

// Extended features availability flags.
static bool exit_extended_available = false;
static bool split_stdout_stderr = false;

// Perform a semihosting call with the given call number and using the given
// parameters block.
int mp_semihosting_call(uint32_t num, void *arg);

// Convert the given fopen(3) open mode string into the appropriate integer
// value required by SYS_OPEN.  If the mode is invalid, it will return -1.
static int mp_lookup_open_mode(const char *mode);

// Computes the length of the given string.  If it gets passed a NULL pointer
// it will return -1.
static int mp_strlen(const char *string);

// Check which extended features are advertised by the host system.
static void mp_check_extended_features_availability(void);

// Write the given string to the host system's debug console.
static int mp_write_to_debug_console(const char *string, size_t length);

// The host system's STDOUT file handle.
int mp_semihosting_stdout = -1;

// The host system's STDERR file handle.
int mp_semihosting_stderr = -1;

int mp_semihosting_open(const char *file_name, const char *file_mode) {
    if (file_name == NULL || file_mode == NULL) {
        return -1;
    }

    int file_name_length = mp_strlen(file_name);
    if (file_name_length <= 0) {
        return -1;
    }
    int file_open_mode = mp_lookup_open_mode(file_mode);
    if (file_open_mode < 0) {
        return -1;
    }

    uint32_t arguments[3] = { (uintptr_t)file_name, file_open_mode, file_name_length };
    return mp_semihosting_call(SYS_OPEN, arguments);
}

int mp_semihosting_close(int handle) {
    uint32_t arguments[] = { handle };
    return mp_semihosting_call(SYS_CLOSE, arguments);
}

void mp_semihosting_writec(char character) {
    uint32_t arguments[] = { character };
    mp_semihosting_call(SYS_WRITEC, arguments);
}

void mp_semihosting_write0(const char *string) {
    if (string == NULL) {
        return;
    }
    uint32_t arguments[] = { (uintptr_t)string };
    mp_semihosting_call(SYS_WRITE0, arguments);
}

int mp_semihosting_write(int handle, const void *data, size_t length) {
    if (data == NULL) {
        return length;
    }
    if (length == 0) {
        return 0;
    }

    uint32_t arguments[] = { handle, (uintptr_t)data, length };
    return mp_semihosting_call(SYS_WRITE, arguments);
}

int mp_semihosting_read(int handle, void *data, size_t length) {
    if (data == NULL) {
        return -1;
    }
    if (length == 0) {
        return 0;
    }

    uint32_t arguments[] = { handle, (uintptr_t)data, length };
    return mp_semihosting_call(SYS_READ, arguments);
}

inline int mp_semihosting_readc(void) {
    return mp_semihosting_call(SYS_READC, NULL);
}

int mp_semihosting_iserror(int code) {
    uint32_t arguments[] = { code };
    return mp_semihosting_call(SYS_ISERROR, arguments);
}

int mp_semihosting_istty(int handle) {
    uint32_t arguments[] = { handle };
    return mp_semihosting_call(SYS_ISTTY, arguments);
}

int mp_semihosting_seek(int handle, uint32_t offset) {
    uint32_t arguments[] = { handle, offset };
    return mp_semihosting_call(SYS_SEEK, arguments);
}

int mp_semihosting_flen(int handle) {
    uint32_t arguments[] = { handle };
    return mp_semihosting_call(SYS_FLEN, arguments);
}

int mp_semihosting_tmpnam(uint8_t identifier, void *buffer, size_t buffer_length) {
    if (buffer == NULL || buffer_length == 0) {
        return -1;
    }

    uint32_t arguments[] = { (uintptr_t)buffer, identifier, buffer_length };
    return mp_semihosting_call(SYS_TMPNAM, arguments);
}

int mp_semihosting_remove(const char *file_name) {
    if (file_name == NULL) {
        return -1;
    }

    int file_name_length = mp_strlen(file_name);
    if (file_name_length <= 0) {
        return -1;
    }

    uint32_t arguments[] = { (uintptr_t)file_name, file_name_length };
    return mp_semihosting_call(SYS_REMOVE, arguments);
}

int mp_semihosting_rename(const char *old_name, const char *new_name) {
    if (old_name == NULL || new_name == NULL) {
        return -1;
    }

    int old_name_length = mp_strlen(old_name);
    if (old_name_length <= 0) {
        return -1;
    }

    int new_name_length = mp_strlen(new_name);
    if (new_name_length <= 0) {
        return -1;
    }

    uint32_t arguments[] = {
        (uintptr_t)old_name, old_name_length, (uintptr_t)new_name, new_name_length
    };
    return mp_semihosting_call(SYS_RENAME, arguments);
}

inline int mp_semihosting_clock(void) {
    return mp_semihosting_call(SYS_CLOCK, NULL);
}

inline int mp_semihosting_time(void) {
    return mp_semihosting_call(SYS_TIME, NULL);
}

int mp_semihosting_system(const char *command) {
    if (command == NULL) {
        return -1;
    }

    int command_length = mp_strlen(command);
    if (command_length <= 0) {
        return -1;
    }

    uint32_t arguments[] = { (uintptr_t)command, command_length };
    return mp_semihosting_call(SYS_SYSTEM, arguments);
}

inline int mp_semihosting_errno(void) {
    return mp_semihosting_call(SYS_ERRNO, NULL);
}

int mp_semihosting_get_cmdline(void *buffer, size_t buffer_length) {
    if (buffer == NULL || buffer_length < CMDLINE_MIN_BUFFER_SIZE) {
        return -1;
    }

    uint32_t arguments[] = { (uintptr_t)buffer, buffer_length };
    return mp_semihosting_call(SYS_GET_CMDLINE, arguments);
}

void mp_semihosting_heapinfo(mp_semihosting_heap_info_t *block) {
    if (block == NULL) {
        return;
    }

    uint32_t arguments[] = { (uintptr_t)block };
    mp_semihosting_call(SYS_HEAPINFO, arguments);
}

void mp_semihosting_exit(uint32_t code, uint32_t subcode) {
    uint32_t arguments[] = { code, subcode };
    mp_semihosting_call(SYS_EXIT, arguments);
    for (;;) {}
}

void mp_semihosting_exit_extended(uint32_t code, uint32_t subcode) {
    uint32_t arguments[] = { code, subcode };
    mp_semihosting_call(SYS_EXIT_EXTENDED, arguments);
    for (;;) {}
}

int mp_semihosting_elapsed(mp_semihosting_elapsed_ticks_t *ticks) {
    if (ticks == NULL) {
        return -1;
    }

    uint32_t arguments[] = { (uintptr_t)ticks };
    return mp_semihosting_call(SYS_ELAPSED, arguments);
}

inline int mp_semihosting_tickfreq(void) {
    return mp_semihosting_call(SYS_TICKFREQ, NULL);
}

void mp_semihosting_init() {
    mp_check_extended_features_availability();
    mp_semihosting_stdout = mp_semihosting_open(":tt", "w");
    if (split_stdout_stderr) {
        mp_semihosting_stderr = mp_semihosting_open(":tt", "a");
    } else {
        mp_semihosting_stderr = mp_semihosting_stdout;
    }
}

void mp_check_extended_features_availability(void) {
    int features_handle = mp_semihosting_open(":semihosting-features", "r");
    if (features_handle < 0) {
        return;
    }

    uint8_t magic_buffer[MAGIC_SIZE];
    if (mp_semihosting_flen(features_handle) < sizeof(magic_buffer)) {
        mp_semihosting_close(features_handle);
        return;
    }

    if (mp_semihosting_read(features_handle, magic_buffer, sizeof(magic_buffer)) != 0) {
        mp_semihosting_close(features_handle);
        return;
    }

    if (magic_buffer[0] != MAGIC_0 ||
        magic_buffer[1] != MAGIC_1 ||
        magic_buffer[2] != MAGIC_2 ||
        magic_buffer[3] != MAGIC_3) {
        mp_semihosting_close(features_handle);
        return;
    }

    uint8_t features_byte = 0;
    if (mp_semihosting_read(features_handle, &features_byte, sizeof(features_byte)) != 0) {
        mp_semihosting_close(features_handle);
        return;
    }

    mp_semihosting_close(features_handle);

    exit_extended_available = (features_byte & 0x01) != 0;
    split_stdout_stderr = (features_byte & 0x02) != 0;
}

int mp_strlen(const char *string) {
    int length = 0;
    while (*string++ != 0) {
        length += 1;
    }
    return length;
}

int mp_lookup_open_mode(const char *mode) {
    if (mode == NULL) {
        return -1;
    }

    int mode_found;

    switch (mode[0]) {
        case 'r':
            mode_found = 0x00;
            break;
        case 'w':
            mode_found = 0x04;
            break;
        case 'a':
            mode_found = 0x08;
            break;
        default:
            return -1;
    }

    switch (mode[1]) {
        case 'b':
            mode_found |= 0x01;
            break;
        case '+':
            mode_found |= 0x02;
            break;
        case '\0':
            return mode_found;
        default:
            return -1;
    }

    switch (mode[2]) {
        case 'b':
            if (mode_found & 0x01) {
                // 'b' was already seen.
                return -1;
            }
            mode_found |= 1;
            break;
        case '+':
            if (mode_found & 0x02) {
                // '+' was already seen.
                return -1;
            }
            mode_found |= 2;
            break;
        case '\0':
            return mode_found;
        default:
            return -1;
    }

    return mode[3] == '\0' ? mode_found : -1;
}

int mp_semihosting_call(uint32_t num, void *arg) {
    register uint32_t call_number_register __asm__ ("x10") = num;
    register void *arguments_register __asm__ ("x11") = arg;

    __asm volatile (
        ".option push            \n" // Transient options
        ".option norvc           \n" // Do not emit compressed instructions
        ".align 4                \n" // 16 bytes alignment
        "slli   zero, zero, 0x1F \n" // Entry NOP
        "ebreak                  \n" // Give control to the debugger
        "srai   zero, zero, 7    \n" // Semihosting call
        ".option pop             \n" // Restore previous options set
        : "+r" (call_number_register)
        : "r" (arguments_register)
        : "memory"
        );

    return call_number_register;
}

inline int mp_semihosting_rx_char() {
    return mp_semihosting_call(SYS_READC, NULL);
}

int mp_write_to_debug_console(const char *string, size_t length) {
    if (length == 0) {
        return 0;
    }

    if (length == 1) {
        mp_semihosting_writec(*string);
        return 0;
    }

    return mp_semihosting_write(mp_semihosting_stdout, string, length);
}

void mp_semihosting_terminate(uint32_t code, uint32_t subcode) {
    if (exit_extended_available) {
        mp_semihosting_exit_extended(code, subcode);
    } else {
        mp_semihosting_exit(code, subcode);
    }
}

int mp_semihosting_tx_strn(const char *string, size_t length) {
    if (string == NULL) {
        return -1;
    }

    return mp_write_to_debug_console(string, length);
}

int mp_semihosting_tx_strn_cooked(const char *string, size_t length) {
    if (string == NULL) {
        return -1;
    }

    if (length == 0) {
        return 0;
    }

    size_t current_offset = 0;
    for (size_t index = 0; index < length; index++) {
        if (string[index] != '\n') {
            continue;
        }

        mp_write_to_debug_console(string + current_offset, index - current_offset);
        mp_semihosting_writec('\r');
        current_offset = index;
    }

    return mp_write_to_debug_console(string + current_offset, length - current_offset);
}
