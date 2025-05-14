try:
    import struct
except ImportError:
    print("SKIP")
    raise SystemExit

# check maximum pack on 32-bit machine
print(struct.pack("<I", 2**32 - 1))
print(struct.pack("<I", 0xffffffff))

# long long ints
print(struct.pack("<Q", 1))
print(struct.pack(">Q", 1))
print(struct.pack("<Q", 2**64 - 1))
print(struct.pack(">Q", 2**64 - 1))
print(struct.pack("<Q", 0xffffffffffffffff))
print(struct.pack(">Q", 0xffffffffffffffff))
print(struct.pack("<q", -1))
print(struct.pack(">q", -1))
print(struct.pack("<Q", 1234567890123456789))
print(struct.pack("<q", -1234567890123456789))
print(struct.pack(">Q", 1234567890123456789))
print(struct.pack(">q", -1234567890123456789))
print(struct.unpack("<Q", b"\x12\x34\x56\x78\x90\x12\x34\x56"))
print(struct.unpack(">Q", b"\x12\x34\x56\x78\x90\x12\x34\x56"))
print(struct.unpack("<q", b"\x12\x34\x56\x78\x90\x12\x34\xf6"))
print(struct.unpack(">q", b"\xf2\x34\x56\x78\x90\x12\x34\x56"))

# check maximum unpack
print(struct.unpack("<I", b"\xff\xff\xff\xff"))
print(struct.unpack("<Q", b"\xff\xff\xff\xff\xff\xff\xff\xff"))

# check small int overflow
print(struct.unpack("<i", b'\xff\xff\xff\x7f'))
print(struct.unpack("<q", b'\xff\xff\xff\xff\xff\xff\xff\x7f'))

# test with negative big integers that are actually small in magnitude
bigzero = (1 << 70) - (1 << 70)
for endian in "<>":
    for type_ in "bhiq":
        fmt = endian + type_
        b = struct.pack(fmt, -2 + bigzero)
        print(fmt, b, struct.unpack(fmt, b))
