try:
    import uio as io
except ImportError:
    import io

a = io.BytesIO(b"foobar")
try:
    a.seek(-10)
except Exception as e:
    # CPython throws ValueError, but MicroPython has consistent stream
    # interface, so BytesIO raises the same error as a real file, which
    # is OSError(EINVAL).
    print(repr(e))
