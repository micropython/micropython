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

#ifndef MICROPY_INCLUDED_SHARED_RUNTIME_SEMIHOSTING_RV32_H
#define MICROPY_INCLUDED_SHARED_RUNTIME_SEMIHOSTING_RV32_H

/*
 * To integrate semihosting, make sure to call mp_semihosting_init() first.
 * Then, if the host system's STDOUT should be used instead of a UART, replace
 * mp_hal_stdin_rx_chr and similar calls in mphalport.c with the semihosting
 * equivalents.
 *
 * At runtime, make sure that the debugger is attached and semihosting is
 * enabled on its end.  The terminal must be configured in raw mode with local
 * echo disabled, on Linux this can be done with "stty raw -echo" on the
 * command line.
 */

/*
 * This follows the RISC-V Semihosting specification version 0.3.
 *
 * That document can be downloaded from
 * https://github.com/riscv-non-isa/riscv-semihosting/releases/
 *
 * Version 0.3 claims that the current RISC-V Semihosting implementation
 * should follow Arm's, and more precisely the "Semihosting for AArch32
 * and AArch64" document, revision 2023Q3.
 */

#include <stddef.h>
#include <stdint.h>
#include <stdnoreturn.h>

// A container for heap and stack pointers as returned by SYS_HEAPINFO.
typedef struct {
    void *heap_base;
    void *heap_limit;
    void *stack_base;
    void *stack_limit;
} mp_semihosting_heap_info_t;

// A 64-bits value indicating how many ticks were counted since the target
// image's execution started.
typedef struct {
    uint32_t low;
    uint32_t high;
} mp_semihosting_elapsed_ticks_t;

// The host system's STDOUT file handle.
extern int mp_semihosting_stdout;

// The host system's STDERR file handle.  If the host system does not support
// explicit STDOUT and STDERR handles, this handle will be aliased to STDOUT
// instead.
extern int mp_semihosting_stderr;

/*
 * Even though exit codes from 0x20000 to 0x20007 are part of the original Arm
 * specification document, they are omitted due to them being tied to hardware
 * events.  Whilst some of them may still have a meaning on the RISC-V
 * platform, it is not yet clear which ones are available and which ones are
 * not.  Thus, only "soft" error codes are provided here although the SYS_EXIT
 * and SYS_EXIT_EXTENDED semihosting calls accept any 32-bits integer as an
 * exit code.
 */

enum {
    MP_SEMIHOSTING_EXIT_BREAKPOINT = 0x20020,
    MP_SEMIHOSTING_EXIT_WATCHPOINT,
    MP_SEMIHOSTING_EXIT_STEP_COMPLETE,
    MP_SEMIHOSTING_EXIT_RUNTIME_ERROR_UNKNOWN,
    MP_SEMIHOSTING_EXIT_INTERNAL_ERROR,
    MP_SEMIHOSTING_EXIT_USER_INTERRUPTION,
    MP_SEMIHOSTING_EXIT_APPLICATION_EXIT,
    MP_SEMIHOSTING_EXIT_STACK_OVERFLOW,
    MP_SEMIHOSTING_EXIT_DIVISION_BY_ZERO,
    MP_SEMIHOSTING_EXIT_OS_SPECIFIC
};

// Initialises semihosting support.
void mp_semihosting_init();

// Read a character from the host system's STDIN stream.
int mp_semihosting_rx_char();

// Write the given string to the host system's STDOUT stream.
int mp_semihosting_tx_strn(const char *string, size_t length);

// Write the given string to the host system's STDOUT stream, writing a CR byte
// before each LF byte to be written.
int mp_semihosting_tx_strn_cooked(const char *string, size_t length);

// Terminates execution with the given code and an optional subcode.  This
// will choose the appropriate semihosting call (either SYS_EXIT or
// SYS_EXIT_EXTENDED) depending on the host system's reported capabilities.
noreturn void mp_semihosting_terminate(uint32_t code, uint32_t subcode);

// Direct semihosting calls access.

// Open a file on the host system with the given name and file mode.
// The file mode follows fopen(3)'s syntax.  The function will return -1 if it
// failed to open the required file, or a file handle number if the operation
// succeeded.  To see why the operation failed, call mp_semihosting_errno.
int mp_semihosting_open(const char *file_name, const char *file_mode);

// Close a file previously opened with mp_semihosting_open.  If the file cannot
// be closed, the function will return -1, otherwise it will return 0.  To see
// why the operation failed, call mp_semihosting_errno.
int mp_semihosting_close(int handle);

// Write the given character to the host system's STDOUT file handle.
void mp_semihosting_writec(char character);

// Write the given NULL-terminated string to the host system's STDOUT file
// handle.
void mp_semihosting_write0(const char *string);

// Write the given buffer to the given host system file handle.  The function
// will return how many characters were left to be written (0 if the operation
// wrote the whole buffer), or -1 if the input buffer pointer is NULL.
int mp_semihosting_write(int handle, const void *data, size_t length);

// Read from the given host system file handle into the given buffer. The
// function will return how many characters were left to be read (0 if the
// operation read whole buffer), or -1 if the input buffer pointer is NULL.
int mp_semihosting_read(int handle, void *data, size_t length);

// Read a single character from the host system's STDIN file handle.
int mp_semihosting_readc(void);

// Check whether the given result code represents an error.  The function will
// return a non-zero value if the code is indeed an error, or zero otherwise.
int mp_semihosting_iserror(int code);

// Check whether the given host system file handle is mapped to an interactive
// device.  The function will return 1 if the handle is mapped to an
// interactive device, 0 if it is mapped to a regular file, and anything else
// if an error occurred.
int mp_semihosting_istty(int handle);

// Move the file pointer on the given host system's file handle to the given
// absolute offset (in bytes).  The function will return 0 if the file pointer
// was moved to the requested position, or a negative value if it was not
// possible to do so.  To see why the operation failed, call
// mp_semihosting_errno.
int mp_semihosting_seek(int handle, uint32_t offset);

// Get the length (in bytes) of the host system's file mapped to the given
// handle.  The function will return a negative value if an error occurred, or
// the requested file's length (in bytes).
int mp_semihosting_flen(int handle);

// Create a temporary file on the host system.  The function requires a file
// identifier between 0 and 255 (inclusive) that will be bound to the requested
// temporary file.  Subsequent calls to mp_semihosting_tmpnam with the same
// identifier will always return the same host system file name.  On success,
// the function will fill the given buffer with the host system's file name,
// and will return 0.  If the buffer pointer is NULL, the buffer name area is
// too small, or the operation failed on the host system's end, the function
// will return -1 instead.  Make sure that the buffer is big enough to contain
// a host system's full path name.
int mp_semihosting_tmpnam(uint8_t identifier, void *buffer, size_t buffer_length);

// Delete a file on the host system's matching the given file name.  The
// function will return 0 if the deletion operation succeeded, or a host system
// dependent error code instead.
int mp_semihosting_remove(const char *file_name);

// Rename a file on the host system's name matching the given file name to the
// new chosen name.  The function will return 0 if the rename operation
// succeeded, or a host system dependent error code instead.
int mp_semihosting_rename(const char *old_name, const char *new_name);

// Get how many hundredths of a second passed since execution started.  If an
// error occurred whilst retrieving clock value, the function will return -1.
int mp_semihosting_clock(void);

// Get the host system's clock in seconds since midnight of January 1st, 1970
// at UTC.
int mp_semihosting_time(void);

// Execute the given command on the host system.  The function will return the
// command's result code retrieved on the host system.
int mp_semihosting_system(const char *command);

// Get the last operation's status code.  The function will return the host
// system's errno variable contents, and can be used to see the exact result
// code for failed I/O operations.
int mp_semihosting_errno(void);

// Get the host system's command line that started execution of the target
// image.  The function will fill the given buffer with the command line
// arguments passed to the target executable.  The function will return 0 on
// success, or -1 if it failed.  Make sure that the buffer can contain at
// least 80 bytes, as it is the minimum supported size defined by the
// specifications document.
int mp_semihosting_get_cmdline(void *buffer, size_t buffer_length);

// Fill the given heap info structure with the system's stack and heap
// start/end addresses.
void mp_semihosting_heapinfo(mp_semihosting_heap_info_t *block);

// Terminate the execution with the given reason code and optional subcode.
// This should be preferred over mp_semihosting_exit_extended if the host
// system does not support the SYS_EXIT_EXTENDED semihosting call.  In doubt
// use mp_semihosting_terminate instead.
noreturn void mp_semihosting_exit(uint32_t code, uint32_t subcode);

// Terminate the execution with the given reason code and optional subcode.
// This should be preferred over mp_semihosting_exit if the host system
// supports this semihosting call.  In doubt use mp_semihosting_terminate
// instead.
noreturn void mp_semihosting_exit_extended(uint32_t code, uint32_t subcode);

// Fill the given structure with how many ticks were counted since execution
// started.  On success, the function will return 0, or -1 if it was not
// possible to compute the ticks count.
int mp_semihosting_elapsed(mp_semihosting_elapsed_ticks_t *ticks);

// Get the system's tick frequency.  If this value is not known, the function
// will return -1 instead.
int mp_semihosting_tickfreq(void);

#endif // MICROPY_INCLUDED_SHARED_SEMIHOSTING_RUNTIME_RV32_H
