# test zlib.compress/decompress using the wbits argument with MicroPython-only small values for wbits.

try:
    # Check if gzip is available.
    import gzip
    import io
except ImportError:
    print("SKIP")
    raise SystemExit

try:
    # Check if compression is enabled.
    b = io.BytesIO()
    g = gzip.GzipFile(fileobj=b, mode="wb")
except ValueError:
    print("SKIP")
    raise SystemExit

try:
    # CPython or zlib from micropython-lib frozen.
    from zlib import compress, decompress
except ImportError:
    try:
        import gzip
        import io
    except ImportError:
        print("SKIP")
        raise SystemExit

    # These are the same as the ones provided in micropython-lib.
    def compress(data, level=-1, wbits=15):
        f = io.BytesIO()
        with gzip.GzipFile(fileobj=f, mode="wb", wbits=wbits) as g:
            g.write(data)
        return f.getvalue()

    def decompress(data, wbits=15):
        f = io.BytesIO(data)
        with gzip.GzipFile(fileobj=f, mode="rb", wbits=wbits) as g:
            return g.read()


# Verify header generation and handling of the wbits arguments for all possible CPython valid values.
all_correct = True
for wbits in range(-15, 48):
    try:
        data = compress(b"micropython", wbits=wbits)
    except Exception:
        print(wbits, "Exception")
        continue

    all_correct = all_correct and (decompress(data, wbits=wbits) == b"micropython")

    if data[0] == 0x1F and data[1] == 0x8B:
        # CPython sets XFL to 0 (not in gzip spec?) and MicroPython sets
        # it to 4 ("fastest").
        print(wbits, data[0:8] + b"\x00" + data[9:10])
    else:
        print(wbits, data[0:2])
print(all_correct)
