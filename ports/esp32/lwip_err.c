//
// Micropython note: this is a copy of ESP-IDFv4.0's components/lwip/lwip/src/api/err.c with
// the error EXXX constants replaced by MicroPython's to avoid getting the wrong values because
// ESP-IDFv4.0 uses ones from newlib (in components/newlib) that have a bunch of different values.
// It is assumed that ESP-IDFv4.1 fixes this discrepancy.

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

#include "lwip/def.h"
#include "lwip/sys.h"
#include "py/mperrno.h"

/** Table to quickly map an lwIP error (err_t) to a socket error
  * by using -err as an index */
static const int err_to_errno_table[] = {
    0,              /* ERR_OK          0      No error, everything OK. */
    MP_ENOMEM,      /* ERR_MEM        -1      Out of memory error.     */
    MP_ENOBUFS,     /* ERR_BUF        -2      Buffer error.            */
    MP_EWOULDBLOCK, /* ERR_TIMEOUT    -3      Timeout                  */
    MP_EHOSTUNREACH, /* ERR_RTE        -4      Routing problem.         */
    MP_EINPROGRESS, /* ERR_INPROGRESS -5      Operation in progress    */
    MP_EINVAL,      /* ERR_VAL        -6      Illegal value.           */
    MP_EWOULDBLOCK, /* ERR_WOULDBLOCK -7      Operation would block.   */
    MP_EADDRINUSE,  /* ERR_USE        -8      Address in use.          */
    MP_EALREADY,    /* ERR_ALREADY    -9      Already connecting.      */
    MP_EISCONN,     /* ERR_ISCONN     -10     Conn already established.*/
    MP_ENOTCONN,    /* ERR_CONN       -11     Not connected.           */
    -1,             /* ERR_IF         -12     Low-level netif error    */
    MP_ECONNABORTED, /* ERR_ABRT       -13     Connection aborted.      */
    MP_ECONNRESET,  /* ERR_RST        -14     Connection reset.        */
    MP_ENOTCONN,    /* ERR_CLSD       -15     Connection closed.       */
    MP_EIO          /* ERR_ARG        -16     Illegal argument.        */
};

int
err_to_errno(err_t err) {
    if ((err > 0) || (-err >= (err_t)LWIP_ARRAYSIZE(err_to_errno_table))) {
        return EIO;
    }
    // printf("lwip err_to_errno %d->%d\n", err, err_to_errno_table[-err]);
    return err_to_errno_table[-err];
}
