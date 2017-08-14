try:
    import ustruct as struct
except:
    import struct
print(struct.calcsize("<bI"))
print(struct.unpack("<bI", b"\x80\0\0\x01\0"))
print(struct.calcsize(">bI"))
print(struct.unpack(">bI", b"\x80\0\0\x01\0"))

# 32-bit little-endian specific
#print(struct.unpack("bI", b"\x80\xaa\x55\xaa\0\0\x01\0"))

print(struct.pack("<l", 1))
print(struct.pack(">l", 1))
print(struct.pack("<i", 1))
print(struct.pack(">i", 1))
print(struct.pack("<h", 1))
print(struct.pack(">h", 1))
print(struct.pack("<b", 1))
print(struct.pack(">b", 1))

print(struct.pack("<bI", -128, 256))
print(struct.pack(">bI", -128, 256))

print(struct.calcsize("100sI"))
print(struct.calcsize("97sI"))
print(struct.unpack("<6sH", b"foo\0\0\0\x12\x34"))
print(struct.pack("<6sH", b"foo", 10000))

s = struct.pack("BHBI", 10, 100, 200, 300)
v = struct.unpack("BHBI", s)
print(v == (10, 100, 200, 300))

# check maximum pack on 32-bit machine
print(struct.pack("<I", 2**32 - 1))
print(struct.pack("<I", 0xffffffff))

# long long ints
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

# network byte order
print(struct.pack('!i', 123))

# first arg must be a string
try:
    struct.pack(1, 2)
except TypeError:
    print('TypeError')

# make sure that unknown types are detected
try:
    struct.pack("z", 1)
except:
    print("Unknown type")

# Initially repitition counters were supported only for strings,
# but later were implemented for all.
print(struct.unpack("<3B2h", b"foo\x12\x34\xff\xff"))
print(struct.pack("<3B", 1, 2, 3))

# pack_into
buf = bytearray(b'>>>123<<<')
struct.pack_into('<bbb', buf, 3, 0x41, 0x42, 0x43)
print(buf)
struct.pack_into('<bbb', buf, -6, 0x44, 0x45, 0x46)
print(buf)

try:
    struct.pack_into('<bbb', buf, 7, 0x41, 0x42, 0x43)
except:
    print('struct.error')
try:
    struct.pack_into('<bbb', buf, -10, 0x41, 0x42, 0x43)
except:
    print('struct.error')

# unpack_from
buf = b'0123456789'
print(struct.unpack_from('<b', buf, 4))
print(struct.unpack_from('<b', buf, -4))
try:
    print(struct.unpack_from('<b', buf, 10))
except:
    print('struct.error')
try:
    print(struct.unpack_from('<b', buf, -11))
except:
    print('struct.error')
