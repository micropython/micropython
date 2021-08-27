try:
    try:
        import ubinascii as binascii
    except ImportError:
        import binascii
except ImportError:
    print("SKIP")
    raise SystemExit

try:
    binascii.crc32
except AttributeError:
    print("SKIP")
    raise SystemExit

print(hex(binascii.crc32(b"The quick brown fox jumps over the lazy dog")))
print(hex(binascii.crc32(b"\x00" * 32)))
print(hex(binascii.crc32(b"\xff" * 32)))
print(hex(binascii.crc32(bytes(range(32)))))

print(hex(binascii.crc32(b" over the lazy dog", binascii.crc32(b"The quick brown fox jumps"))))
print(hex(binascii.crc32(b"\x00" * 16, binascii.crc32(b"\x00" * 16))))
print(hex(binascii.crc32(b"\xff" * 16, binascii.crc32(b"\xff" * 16))))
print(hex(binascii.crc32(bytes(range(16, 32)), binascii.crc32(bytes(range(16))))))
try:
    binascii.crc32("")
except TypeError:
    print("TypeError")
