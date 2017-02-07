try:
    import zlib
except ImportError:
    import uzlib as zlib

PATTERNS = [
    # Packed results produced by CPy's zlib.compress()
    (b'0', b'x\x9c3\x00\x00\x001\x001'),
    (b'a', b'x\x9cK\x04\x00\x00b\x00b'),
    (b'0' * 100, b'x\x9c30\xa0=\x00\x00\xb3q\x12\xc1'),
    (bytes(range(64)), b'x\x9cc`dbfaec\xe7\xe0\xe4\xe2\xe6\xe1\xe5\xe3\x17\x10\x14\x12\x16\x11\x15\x13\x97\x90\x94\x92\x96\x91\x95\x93WPTRVQUS\xd7\xd0\xd4\xd2\xd6\xd1\xd5\xd370426153\xb7\xb0\xb4\xb2\xb6\xb1\xb5\xb3\x07\x00\xaa\xe0\x07\xe1'),
    (b'hello', b'x\x01\x01\x05\x00\xfa\xffhello\x06,\x02\x15'), # compression level 0
    # adaptive/dynamic huffman tree
    (b'13371813150|13764518736|12345678901', b'x\x9c\x05\xc1\x81\x01\x000\x04\x04\xb1\x95\\\x1f\xcfn\x86o\x82d\x06Qq\xc8\x9d\xc5X}<e\xb5g\x83\x0f\x89X\x07\xab')
]

for unpacked, packed in PATTERNS:
    assert zlib.decompress(packed) == unpacked
    print(unpacked)


# Raw DEFLATE bitstream
v = b'\xcbH\xcd\xc9\xc9\x07\x00'
exp = b"hello"
out = zlib.decompress(v, -15)
assert(out == exp)
print(exp)
# Even when you ask CPython zlib.compress to produce Raw DEFLATE stream,
# it returns it with adler2 and oriignal size appended, as if it was a
# zlib stream. Make sure there're no random issues decompressing such.
v = b'\xcbH\xcd\xc9\xc9\x07\x00\x86\xa6\x106\x05\x00\x00\x00'
out = zlib.decompress(v, -15)
assert(out == exp)

# this should error
try:
    zlib.decompress(b'abc')
except Exception:
    print("Exception")
