# test zlib.compress function
# CPython doesn't support the wbits argument untill 3.11 so this test needs a .exp file

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
    from zlib import compress
except ImportError:
    try:
        import gzip
        import io
    except ImportError:
        print("SKIP")
        raise SystemExit

    # This is the same as the one provided in micropython-lib.
    def compress(data, level=-1, wbits=15):
        f = io.BytesIO()
        with gzip.GzipFile(fileobj=f, mode="wb", wbits=wbits) as g:
            g.write(data)
        return f.getvalue()


unpacked = b"hello"
packed = b"\xcbH\xcd\xc9\xc9\x07\x00"

out = packed
print("RAW", unpacked, compress(unpacked, -1, -9) == out)

out = b"\x78\x9c" + packed + b"\x06,\x02\x15"
print("ZLIB", unpacked, compress(unpacked) == out)

out = b"\x1f\x8b\x08\x00\x00\x00\x00\x00\x04\x03" + packed + b"\x86\xa6\x106\x05\x00\x00\x00"
print("GZIP", unpacked, compress(unpacked, -1, 16 + 9) == out)

PATTERNS_RAW = (
    (b"0", b"3\x00\x00"),
    (b"a", b"K\x04\x00"),
    (b"0" * 100, b"3\xa0\x03\x00\x00"),
    (
        bytes(range(64)),
        b"c`dbfaec\xe7\xe0\xe4\xe2\xe6\xe1\xe5\xe3\x17\x10\x14\x12\x16\x11\x15\x13\x97\x90\x94\x92\x96\x91\x95\x93WPTRVQUS\xd7\xd0\xd4\xd2\xd6\xd1\xd5\xd370426153\xb7\xb0\xb4\xb2\xb6\xb1\xb5\xb3\x07\x00",
    ),
)

for unpacked, packed in PATTERNS_RAW:
    print(unpacked, compress(unpacked, -1, -9) == packed)
