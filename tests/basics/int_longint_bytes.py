# Skip if long ints are not supported.
import skip_if
skip_if.no_bigint()

print((2**64).to_bytes(9, "little"))
print((-2**64).to_bytes(9, "little", signed=True))
print(int.from_bytes(b"\x00\x01\0\0\0\0\0\0", "little"))
print(int.from_bytes(b"\x01\0\0\0\0\0\0\0", "little"))
print(int.from_bytes(b"\x00\x01\0\0\0\0\0\0", "little"))
