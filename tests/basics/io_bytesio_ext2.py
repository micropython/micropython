try:
    import io
except ImportError:
    print("SKIP")
    raise SystemExit

a = io.BytesIO(b"foobar")
try:
    a.seek(-10)
except Exception as e:
    # CPython throws ValueError, but MicroPython has consistent stream
    # interface, so BytesIO raises the same error as a real file, which
    # is OSError(EINVAL).
    print(type(e), e.args[0] > 0)
