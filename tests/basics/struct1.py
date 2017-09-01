try:
    import ustruct as struct
except:
    try:
        import struct
    except ImportError:
        print("SKIP")
        raise SystemExit

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

# network byte order
print(struct.pack('!i', 123))

# check that we get an error if the buffer is too small
try:
    struct.unpack('I', b'\x00\x00\x00')
except:
    print('struct.error')

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
