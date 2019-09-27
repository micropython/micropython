# test ustruct and endian specific things

try:
    import ustruct as struct
except:
    try:
        import struct
    except ImportError:
        print("SKIP")
        raise SystemExit

# unpack/unpack_from with unaligned native type
buf = b'0123456789'
print(struct.unpack('h', memoryview(buf)[1:3]))
print(struct.unpack_from('i', buf, 1))
print(struct.unpack_from('@i', buf, 1))
print(struct.unpack_from('@ii', buf, 1))

# pack_into with unaligned native type
buf = bytearray(b'>----<<<<<<<')
struct.pack_into('i', buf, 1, 0x30313233)
print(buf)
struct.pack_into('@ii', buf, 3, 0x34353637, 0x41424344)
print(buf)
