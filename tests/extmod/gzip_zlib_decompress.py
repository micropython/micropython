import sys

try:
    # CPython or zlib from micropython-lib frozen.
    from zlib import decompress
except ImportError:
    try:
        import gzip
        import io
    except ImportError:
        print("SKIP")
        raise SystemExit

    # This is the same as the one provided in micropython-lib.
    def decompress(data, wbits=15):
        f = io.BytesIO(data)
        with gzip.GzipFile(fileobj=f, mode="rb", wbits=wbits) as g:
            return g.read()


PATTERNS = [
    # Packed results produced by CPy's zlib.compress()
    (b"0", b"x\x9c3\x00\x00\x001\x001"),
    (b"a", b"x\x9cK\x04\x00\x00b\x00b"),
    (b"0" * 100, b"x\x9c30\xa0=\x00\x00\xb3q\x12\xc1"),
    (
        bytes(range(64)),
        b"x\x9cc`dbfaec\xe7\xe0\xe4\xe2\xe6\xe1\xe5\xe3\x17\x10\x14\x12\x16\x11\x15\x13\x97\x90\x94\x92\x96\x91\x95\x93WPTRVQUS\xd7\xd0\xd4\xd2\xd6\xd1\xd5\xd370426153\xb7\xb0\xb4\xb2\xb6\xb1\xb5\xb3\x07\x00\xaa\xe0\x07\xe1",
    ),
    (b"hello", b"x\x01\x01\x05\x00\xfa\xffhello\x06,\x02\x15"),  # compression level 0
    # adaptive/dynamic huffman tree
    (
        b"13371813150|13764518736|12345678901",
        b"x\x9c\x05\xc1\x81\x01\x000\x04\x04\xb1\x95\\\x1f\xcfn\x86o\x82d\x06Qq\xc8\x9d\xc5X}<e\xb5g\x83\x0f\x89X\x07\xab",
    ),
    # dynamic Huffman tree with "case 17" (repeat code for 3-10 times)
    (
        b">I}\x00\x951D>I}\x00\x951D>I}\x00\x951D>I}\x00\x951D",
        b"x\x9c\x05\xc11\x01\x00\x00\x00\x010\x95\x14py\x84\x12C_\x9bR\x8cV\x8a\xd1J1Z)F\x1fw`\x089",
    ),
]

for unpacked, packed in PATTERNS:
    assert decompress(packed) == unpacked
    print(unpacked)


# Raw DEFLATE bitstream
v = b"\xcbH\xcd\xc9\xc9\x07\x00"
exp = b"hello"
out = decompress(v, -15)
assert out == exp
print(exp)
# Even when you ask CPython zlib.compress to produce Raw DEFLATE stream,
# it returns it with adler2 and oriignal size appended, as if it was a
# zlib stream. Make sure there're no random issues decompressing such.
v = b"\xcbH\xcd\xc9\xc9\x07\x00\x86\xa6\x106\x05\x00\x00\x00"
out = decompress(v, -15)
assert out == exp

# this should error
try:
    decompress(b"abc")
except Exception:
    print("Exception")

# invalid block type
try:
    decompress(b"\x07", -15)  # final-block, block-type=3 (invalid)
except Exception as er:
    print("Exception")

# Auto-detection of zlib window size.
print(decompress(b"x\xda\xcb\xcdL.\xca/\xa8,\xc9\xc8\xcf\x03\x00\x1c9\x04\xbd", wbits=0))
print(decompress(b"H\x89\xcb\xcdL.\xca/\xa8,\xc9\xc8\xcf\x03\x00\x1c9\x04\xbd", wbits=0))

# Auto-detection of gzip/zlib.
print(decompress(b"\x18\x95\xcb\xcdL.\xca/\xa8,\xc9\xc8\xcf\x03\x00\x1c9\x04\xbd", wbits=41))
print(
    decompress(
        b"\x1f\x8b\x08\x00\x00\x00\x00\x00\x00\x03\xcb\xcdL.\xca/\xa8,\xc9\xc8\xcf\x03\x00\xf2KF>\x0b\x00\x00\x00",
        wbits=40,
    )
)
