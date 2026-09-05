// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Nicko van Someren

// Minimal <errno.h> for the freestanding UEFI port. MicroPython uses its own error
// codes (MP_Exxx, py/mperrno.h) internally; this exists only so third-party code
// that #includes <errno.h> (e.g. extmod/modtls_mbedtls.c) compiles. Values mirror
// the common Linux numbering used elsewhere in the port.
#ifndef _UEFI_LIBC_ERRNO_H
#define _UEFI_LIBC_ERRNO_H

#define EPERM        1
#define EIO          5
#define EBADF        9
#define EAGAIN       11
#define ENOMEM       12
#define EACCES       13
#define EINVAL       22
#define EWOULDBLOCK  EAGAIN
#define EINPROGRESS  115

#endif // _UEFI_LIBC_ERRNO_H
