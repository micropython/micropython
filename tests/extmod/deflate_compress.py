try:
    # Check if deflate is available.
    import deflate
    import io
except ImportError:
    print("SKIP")
    raise SystemExit

# Check if compression is enabled.
if not hasattr(deflate.DeflateIO, "write"):
    print("SKIP")
    raise SystemExit

# Simple compression & decompression.
b = io.BytesIO()
g = deflate.DeflateIO(b, deflate.RAW)
data = b"micropython"
N = 10
for i in range(N):
    g.write(data)
g.close()
result_raw = b.getvalue()
print(len(result_raw) < len(data) * N)
b = io.BytesIO(result_raw)
g = deflate.DeflateIO(b, deflate.RAW)
print(g.read())

# Same, but using a context manager.
b = io.BytesIO()
with deflate.DeflateIO(b, deflate.RAW) as g:
    for i in range(N):
        g.write(data)
result_raw = b.getvalue()
print(len(result_raw) < len(data) * N)
b = io.BytesIO(result_raw)
with deflate.DeflateIO(b, deflate.RAW) as g:
    print(g.read())

# Writing to a closed underlying stream.
b = io.BytesIO()
g = deflate.DeflateIO(b, deflate.RAW)
g.write(b"micropython")
b.close()
try:
    g.write(b"micropython")
except ValueError:
    print("ValueError")

# Writing to a closed DeflateIO.
b = io.BytesIO()
g = deflate.DeflateIO(b, deflate.RAW)
g.write(b"micropython")
g.close()
try:
    g.write(b"micropython")
except OSError:
    print("OSError")


def decompress(data, *args):
    buf = io.BytesIO(data)
    with deflate.DeflateIO(buf, *args) as g:
        return g.read()


def compress(data, *args):
    b = io.BytesIO()
    with deflate.DeflateIO(b, *args) as g:
        g.write(data)
    return b.getvalue()


def compress_error(data, *args):
    try:
        compress(data, *args)
    except OSError:
        print("OSError")
    except ValueError:
        print("ValueError")


# More test patterns.
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
    print(compress(unpacked) == packed)
    print(compress(unpacked, deflate.RAW) == packed)

# Verify header and checksum format.
unpacked = b"hello"
packed = b"\xcbH\xcd\xc9\xc9\x07\x00"


def check_header(n, a, b):
    if a == b:
        print(n)
    else:
        print(n, a, b)


check_header("RAW", compress(unpacked, deflate.RAW), packed)
check_header(
    "ZLIB(9)", compress(unpacked, deflate.ZLIB, 9), b"\x18\x95" + packed + b"\x06,\x02\x15"
)
check_header(
    "ZLIB(15)", compress(unpacked, deflate.ZLIB, 15), b"\x78\x9c" + packed + b"\x06,\x02\x15"
)
check_header(
    "GZIP",
    compress(unpacked, deflate.GZIP, 9),
    b"\x1f\x8b\x08\x00\x00\x00\x00\x00\x04\x03" + packed + b"\x86\xa6\x106\x05\x00\x00\x00",
)

# Valid wbits values.
compress_error(unpacked, deflate.RAW, -1)
print(len(compress(unpacked, deflate.RAW, 0)))
compress_error(unpacked, deflate.RAW, 1)
compress_error(unpacked, deflate.RAW, 4)
for i in range(5, 16):
    print(len(compress(unpacked, deflate.RAW, i)))
compress_error(unpacked, deflate.RAW, 16)

# Invalid values for format.
compress_error(unpacked, -1)
compress_error(unpacked, 5)

# Fill buf with a predictable pseudorandom sequence.
buf = bytearray(1024)
lfsr = 1 << 15 | 1
for i in range(len(buf)):
    bit = (lfsr ^ (lfsr >> 1) ^ (lfsr >> 3) ^ (lfsr >> 12)) & 1
    lfsr = (lfsr >> 1) | (bit << 15)
    buf[i] = lfsr & 0xFF

# Verify that compression improves as the window size increases.
prev_len = len(buf)
for wbits in range(5, 10):
    result = compress(buf, deflate.RAW, wbits)
    next_len = len(result)
    print(next_len < prev_len and decompress(result, deflate.RAW, wbits) == buf)
    prev_len = next_len

# Verify that compression is optimal: in the bytes below, the final "123" should be
# compressed by referencing the "123" just before it, and not the one all the way back
# at the start of the bytes.
compressed = compress(b"1234567890abcdefghijklmnopqrstuvwxyz123123", deflate.RAW)
print(len(compressed), compressed)
