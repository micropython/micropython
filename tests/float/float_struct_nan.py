# test struct pack/unpack of inf and nan with 'f' and 'd' typecodes

try:
    import struct
except ImportError:
    print("SKIP")
    raise SystemExit

inf = float("inf")
nan = float("nan")

# pack/unpack must round-trip inf and nan
for fmt in ("f", "d", ">f", ">d", "<f", "<d"):
    for val in (inf, -inf, nan, -nan):
        v = struct.unpack(fmt, struct.pack(fmt, val))[0]
        print(fmt, val, v)

# unpacking raw inf/nan bit patterns must give inf/nan
print(struct.unpack(">d", b"\x7f\xf0\x00\x00\x00\x00\x00\x00")[0])
print(struct.unpack(">d", b"\xff\xf0\x00\x00\x00\x00\x00\x00")[0])
print(struct.unpack(">d", b"\x7f\xf8\x00\x00\x00\x00\x00\x00")[0])
print(struct.unpack(">d", b"\xff\xf8\x00\x00\x00\x00\x00\x00")[0])
print(struct.unpack("<d", b"\x00\x00\x00\x00\x00\x00\xf8\x7f")[0])
print(struct.unpack(">f", b"\x7f\x80\x00\x00")[0])
print(struct.unpack(">f", b"\x7f\xc0\x00\x00")[0])
print(struct.unpack("<f", b"\x00\x00\xc0\x7f")[0])
