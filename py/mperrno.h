/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Damien P. George
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
#ifndef MICROPY_INCLUDED_PY_MPERRNO_H
#define MICROPY_INCLUDED_PY_MPERRNO_H

#include "py/mpconfig.h"

#if MICROPY_USE_INTERNAL_ERRNO

// MP_Exxx errno's are defined directly as numeric values
// (Linux constants are used as a reference)

#define MP_EPERM              (1) // Operation not permitted
#define MP_ENOENT             (2) // No such file or directory
#define MP_ESRCH              (3) // No such process
#define MP_EINTR              (4) // Interrupted system call
#define MP_EIO                (5) // I/O error
#define MP_ENXIO              (6) // No such device or address
#define MP_E2BIG              (7) // Argument list too long
#define MP_ENOEXEC            (8) // Exec format error
#define MP_EBADF              (9) // Bad file number
#define MP_ECHILD            (10) // No child processes
#define MP_EAGAIN            (11) // Try again
#define MP_ENOMEM            (12) // Out of memory
#define MP_EACCES            (13) // Permission denied
#define MP_EFAULT            (14) // Bad address
#define MP_ENOTBLK           (15) // Block device required
#define MP_EBUSY             (16) // Device or resource busy
#define MP_EEXIST            (17) // File exists
#define MP_EXDEV             (18) // Cross-device link
#define MP_ENODEV            (19) // No such device
#define MP_ENOTDIR           (20) // Not a directory
#define MP_EISDIR            (21) // Is a directory
#define MP_EINVAL            (22) // Invalid argument
#define MP_ENFILE            (23) // File table overflow
#define MP_EMFILE            (24) // Too many open files
#define MP_ENOTTY            (25) // Not a typewriter
#define MP_ETXTBSY           (26) // Text file busy
#define MP_EFBIG             (27) // File too large
#define MP_ENOSPC            (28) // No space left on device
#define MP_ESPIPE            (29) // Illegal seek
#define MP_EROFS             (30) // Read-only file system
#define MP_EMLINK            (31) // Too many links
#define MP_EPIPE             (32) // Broken pipe
#define MP_EDOM              (33) // Math argument out of domain of func
#define MP_ERANGE            (34) // Math result not representable
#define MP_EWOULDBLOCK  MP_EAGAIN // Operation would block
#define MP_EOPNOTSUPP        (95) // Operation not supported on transport endpoint
#define MP_EAFNOSUPPORT      (97) // Address family not supported by protocol
#define MP_EADDRINUSE        (98) // Address already in use
#define MP_ECONNABORTED     (103) // Software caused connection abort
#define MP_ECONNRESET       (104) // Connection reset by peer
#define MP_ENOBUFS          (105) // No buffer space available
#define MP_EISCONN          (106) // Transport endpoint is already connected
#define MP_ENOTCONN         (107) // Transport endpoint is not connected
#define MP_ETIMEDOUT        (110) // Connection timed out
#define MP_ECONNREFUSED     (111) // Connection refused
#define MP_EHOSTUNREACH     (113) // No route to host
#define MP_EALREADY         (114) // Operation already in progress
#define MP_EINPROGRESS      (115) // Operation now in progress
#define MP_ECANCELED        (125) // Operation canceled

#else

// MP_Exxx errno's are defined in terms of system supplied ones

#include <errno.h>

#define MP_EPERM            EPERM
#define MP_ENOENT           ENOENT
#define MP_ESRCH            ESRCH
#define MP_EINTR            EINTR
#define MP_EIO              EIO
#define MP_ENXIO            ENXIO
#define MP_E2BIG            E2BIG
#define MP_ENOEXEC          ENOEXEC
#define MP_EBADF            EBADF
#define MP_ECHILD           ECHILD
#define MP_EAGAIN           EAGAIN
#define MP_ENOMEM           ENOMEM
#define MP_EACCES           EACCES
#define MP_EFAULT           EFAULT
#define MP_ENOTBLK          ENOTBLK
#define MP_EBUSY            EBUSY
#define MP_EEXIST           EEXIST
#define MP_EXDEV            EXDEV
#define MP_ENODEV           ENODEV
#define MP_ENOTDIR          ENOTDIR
#define MP_EISDIR           EISDIR
#define MP_EINVAL           EINVAL
#define MP_ENFILE           ENFILE
#define MP_EMFILE           EMFILE
#define MP_ENOTTY           ENOTTY
#define MP_ETXTBSY          ETXTBSY
#define MP_EFBIG            EFBIG
#define MP_ENOSPC           ENOSPC
#define MP_ESPIPE           ESPIPE
#define MP_EROFS            EROFS
#define MP_EMLINK           EMLINK
#define MP_EPIPE            EPIPE
#define MP_EDOM             EDOM
#define MP_ERANGE           ERANGE
#define MP_EWOULDBLOCK      EWOULDBLOCK
#define MP_EOPNOTSUPP       EOPNOTSUPP
#define MP_EAFNOSUPPORT     EAFNOSUPPORT
#define MP_EADDRINUSE       EADDRINUSE
#define MP_ECONNABORTED     ECONNABORTED
#define MP_ECONNRESET       ECONNRESET
#define MP_ENOBUFS          ENOBUFS
#define MP_EISCONN          EISCONN
#define MP_ENOTCONN         ENOTCONN
#define MP_ETIMEDOUT        ETIMEDOUT
#define MP_ECONNREFUSED     ECONNREFUSED
#define MP_EHOSTUNREACH     EHOSTUNREACH
#define MP_EALREADY         EALREADY
#define MP_EINPROGRESS      EINPROGRESS
#define MP_ECANCELED        ECANCELED

#endif

#if MICROPY_PY_UERRNO

#include "py/obj.h"

qstr mp_errno_to_str(mp_obj_t errno_val);

#endif

#endif // MICROPY_INCLUDED_PY_MPERRNO_H
