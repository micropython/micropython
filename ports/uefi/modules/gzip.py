# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Nicko van Someren

# Minimal CPython-compatible gzip module for the UEFI port, implemented as a thin
# wrapper over the C `deflate` module (frozen into the firmware). Provides
# compress(), decompress(), and GzipFile()/open().

import deflate
import io
from builtins import open as _open

_GZIP = 3  # deflate format selector: gzip (DEFLATEIO_FORMAT_GZIP)


def GzipFile(filename=None, mode="rb", fileobj=None):
    if fileobj is None:
        fileobj = _open(filename, mode if "b" in mode else mode + "b")
    return deflate.DeflateIO(fileobj, _GZIP)


def open(filename, mode="rb"):
    return GzipFile(filename, mode)


def compress(data):
    stream = io.BytesIO()
    d = deflate.DeflateIO(stream, _GZIP)
    d.write(data)
    d.close()
    return stream.getvalue()


def decompress(data):
    return deflate.DeflateIO(io.BytesIO(data), _GZIP).read()
