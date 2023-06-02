/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Damien P. George
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
#ifndef AXTLS_OS_PORT_H
#define AXTLS_OS_PORT_H

#ifndef __ets__
#include <arpa/inet.h>
#endif
#include <errno.h>
#include <sys/time.h>
#include "py/stream.h"
#include "lib/crypto-algorithms/sha256.h"

#define SSL_CTX_MUTEX_INIT(mutex)
#define SSL_CTX_MUTEX_DESTROY(mutex)
#define SSL_CTX_LOCK(mutex)
#define SSL_CTX_UNLOCK(mutex)

#define SOCKET_READ(s, buf, size)       mp_stream_posix_read((void *)s, buf, size)
#define SOCKET_WRITE(s, buf, size)      mp_stream_posix_write((void *)s, buf, size)
#define SOCKET_CLOSE(A)                 UNUSED
#define SOCKET_ERRNO()                  errno

#define SHA256_CTX                      CRYAL_SHA256_CTX
#define SHA256_Init(ctx)                sha256_init(ctx)
#define SHA256_Update(ctx, buf, size)   sha256_update(ctx, buf, size)
#define SHA256_Final(hash, ctx)         sha256_final(ctx, hash)

#define TTY_FLUSH()

#ifdef WDEV_HWRNG
// For esp8266 port: use the hardware RNG.
#define PLATFORM_RNG_U8()               (*WDEV_HWRNG)
#endif

#endif // AXTLS_OS_PORT_H
