# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Nicko van Someren

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
    # Load whenever either is supplied (upstream semantics): the tls layer validates
    # and raises the right error for a bad or missing key/cert, rather than us silently
    # skipping it when only one is given.
    if cert is not None or key is not None:
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
