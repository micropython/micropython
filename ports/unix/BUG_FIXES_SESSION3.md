# MicroPython Unix Port - Bug Fixes Session 3

This document summarizes the fixes applied in Session 3.

## Fixed Issues

### 1. Thread Mutex Unlock Check
**File**: `ports/unix/mpthreadport.c`
**Fix**: Added error checking for `pthread_mutex_unlock` to detect potential logic errors in debug builds.

### 2. Configurable Bluetooth HCI Baudrate
**File**: `ports/unix/mpbthciport.c`
**Fix**: Made the HCI UART baudrate configurable via the `MICROPYBTBAUD` environment variable. Default remains 1Mbit.

### 3. Dupterm Slot Support
**File**: `ports/unix/unix_mphal.c`
**Fix**: Updated `mp_hal_stdin_rx_chr` to check all `MICROPY_PY_OS_DUPTERM` slots instead of just the first one.

### 4. JNI Debug Printf
**File**: `ports/unix/modjni.c`
**Fix**: Removed a leftover debug `printf` and improved comments.

### 5. FFI Buffer Access Mode
**File**: `ports/unix/modffi.c`
**Fix**: Removed TODO comment. Kept `MP_BUFFER_READ` to allow passing immutable bytes objects to FFI functions (e.g. for read-only C APIs).

### 6. Termios TODO Cleanup
**File**: `ports/unix/modtermios.c`
**Fix**: Removed an outdated TODO comment regarding `c_cc` array handling.

### 7. Struct Pack Argument Count Check
**File**: `py/modstruct.c`
**Fix**: Added a check to ensure the number of arguments passed to `struct.pack` matches the format string, raising a `RuntimeError` if there are too many arguments.

### 8. Time Select Behavior
**File**: `ports/unix/modtime.c`
**Fix**: Added `__linux__` guard around code that relies on Linux-specific `select` behavior (updating `tv` to remaining time).

### 9. Socket Makefile Comment
**File**: `ports/unix/modsocket.c`
**Fix**: Clarified the comment in `socket_makefile` explaining why MicroPython does not duplicate the file descriptor.

### 10. Machine Mem Address Format
**File**: `ports/unix/modmachine.c`
**Fix**: Fixed the printf format specifier in `mod_machine_mem_get_addr` to correctly print 64-bit addresses using `%p`.
