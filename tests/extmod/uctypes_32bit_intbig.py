# This test checks previously known problem values for 32-bit ports.
# It's less useful for 64-bit ports.
try:
    import uctypes
except ImportError:
    print("SKIP")
    raise SystemExit

buf = bytearray(b"12345678abcd")
struct = uctypes.struct(
    uctypes.addressof(buf),
    {"f32": uctypes.UINT32 | 0, "f64": uctypes.UINT64 | 4},
    uctypes.LITTLE_ENDIAN,
)

struct.f32 = 0x7FFFFFFF
print(buf)

struct.f32 = 0x80000000
print(buf)

struct.f32 = 0xFF010203
print(buf)

struct.f64 = 0x80000000
print(buf)

struct.f64 = 0x80000000 * 2
print(buf)

print("=")

buf = bytearray(b"12345678abcd")
struct = uctypes.struct(
    uctypes.addressof(buf),
    {"f32": uctypes.UINT32 | 0, "f64": uctypes.UINT64 | 4},
    uctypes.BIG_ENDIAN,
)

struct.f32 = 0x7FFFFFFF
print(buf)

struct.f32 = 0x80000000
print(buf)

struct.f32 = 0xFF010203
print(buf)

struct.f64 = 0x80000000
print(buf)

struct.f64 = 0x80000000 * 2
print(buf)
