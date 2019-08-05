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
