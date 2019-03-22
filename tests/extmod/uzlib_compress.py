try:
    import uio
    import zlib
except ImportError:
    try:
        import uzlib as zlib
    except ImportError:
        print("SKIP")
        raise SystemExit

if not hasattr(zlib, "compress"):
    print("SKIP")
    raise SystemExit

PATTERNS = [
    b"0",
    b"a",
    b"0" * 100,
    bytes(range(64)),
    b"hello",
    # adaptive/dynamic huffman tree
    b"13371813150|13764518736|12345678901",
    # dynamic Huffman tree with "case 17" (repeat code for 3-10 times)
    b">I}\x00\x951D>I}\x00\x951D>I}\x00\x951D>I}\x00\x951D",
]

for unpacked in PATTERNS:
    packed = zlib.compress(unpacked)
    print(packed)
    # Test DecompIO handling of gzip format
    file = uio.BytesIO(packed)
    assert unpacked == zlib.DecompIO(file, 31).read()
    # Test direct decompression of payload
    assert unpacked == zlib.decompress(packed, 31)
