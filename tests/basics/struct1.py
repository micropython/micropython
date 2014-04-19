import struct
print(struct.calcsize("<bI"))
print(struct.unpack("<bI", b"\x80\0\0\x01\0"))
print(struct.calcsize(">bI"))
print(struct.unpack(">bI", b"\x80\0\0\x01\0"))

# 32-bit little-endian specific
#print(struct.unpack("bI", b"\x80\xaa\x55\xaa\0\0\x01\0"))

print(struct.pack("<i", 1))
print(struct.pack(">i", 1))
print(struct.pack("<h", 1))
print(struct.pack(">h", 1))
print(struct.pack("<b", 1))
print(struct.pack(">b", 1))

print(struct.pack("<bI", -128, 256))
print(struct.pack(">bI", -128, 256))
