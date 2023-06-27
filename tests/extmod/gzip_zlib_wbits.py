# test zlib.compress/decompress using the wbits argument.
# With a sufficiently large input, increasing the window size should improve the compression.
# CPython doesn't support the wbits argument untill 3.11 so this test needs a .exp file

try:
    # Check if gzip is available.
    import gzip
    import io
except ImportError:
    print("SKIP")
    raise SystemExit

try:
    # Check that we have enough heap to allocate a big buffer.
    buf = bytearray(40 * 1024)
except MemoryError:
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


# Fill buf with a predictable pseudorandom sequence.
lfsr = 1 << 15 | 1
for i in range(len(buf)):
    bit = (lfsr ^ (lfsr >> 1) ^ (lfsr >> 3) ^ (lfsr >> 12)) & 1
    lfsr = (lfsr >> 1) | (bit << 15)
    buf[i] = lfsr & 0xFF

# Verify that compression improves as the window size increases.
prev_len = len(buf)
for wbits in range(9, 16):
    result = compress(buf, wbits=wbits)
    next_len = len(result)
    print(next_len < prev_len and decompress(result, wbits=wbits) == buf)
    prev_len = next_len

# Verify header generation and handling of the wbits arguments for all possible valid values.
# Note:
all_correct = True
for wbits in range(-20, 40):
    try:
        # CPython incorrectly handles wbits=8 (which should be invalid) the same
        # as wbits=9.
        if wbits == 8:
            continue

        data = compress(b"micropython", wbits=wbits)
        if data[0] == 0x1F and data[1] == 0x8B:
            # CPython sets XFL to 0 (not in gzip spec?) and MicroPython sets
            # it to 4 ("fastest").
            print(wbits, data[0:8] + b"\x00" + data[9:10])
        else:
            print(wbits, data[0:2])
        correct = correct and (decompress(data, wbits=wbits) == b"micropython")
    except Exception:
        print(wbits, "Exception")
print(all_correct)
