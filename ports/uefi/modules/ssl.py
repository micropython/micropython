# This file is part of the MicroPython project, http://micropython.org/
#
# The MIT License (MIT)
#
# Copyright (c) 2026 Nicko van Someren
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

# CPython-compatible `ssl` facade over the built-in `tls` module (either backend).
# Mirrors the upstream micropython-lib `ssl` package.

from tls import *
from tls import (
    CERT_NONE,
    CERT_OPTIONAL,
    CERT_REQUIRED,
    PROTOCOL_TLS_CLIENT,
    PROTOCOL_TLS_SERVER,
)

import tls as _tls

# `tls` reports TLS failures as OSError; expose the CPython spelling.
SSLError = OSError


def wrap_socket(
    sock,
    server_side=False,
    key=None,
    cert=None,
    cert_reqs=CERT_NONE,
    cadata=None,
    server_hostname=None,
    do_handshake=True,
):
    ctx = _tls.SSLContext(PROTOCOL_TLS_SERVER if server_side else PROTOCOL_TLS_CLIENT)
    if key is not None and cert is not None:
        ctx.load_cert_chain(cert, key)
    if cadata is not None:
        ctx.load_verify_locations(cadata)
    ctx.verify_mode = cert_reqs
    return ctx.wrap_socket(
        sock,
        server_side=server_side,
        do_handshake_on_connect=do_handshake,
        server_hostname=server_hostname,
    )
