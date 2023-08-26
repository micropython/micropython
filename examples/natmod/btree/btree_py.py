# Implemented in Python to support keyword arguments

# ruff: noqa: F821 - this file is evaluated with C-defined names in scope


def open(stream, *, flags=0, cachesize=0, pagesize=0, minkeypage=0):
    return _open(stream, flags, cachesize, pagesize, minkeypage)
