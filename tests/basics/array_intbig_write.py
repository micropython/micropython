# Test writing big-int values to array/memoryview elements via subscript
# and struct.pack_into. Exercises the mp_binary_set_val_array and
# mp_binary_set_val code paths for big-int values in py/binary.c.

try:
    from array import array
    import struct
except ImportError:
    print("SKIP")
    raise SystemExit

# Write big-int values to array('I') via subscript.
a = array("I", [0])
for val in (0x80000000, 0xA5A5A5A5, 0xDEADBEEF, 2**32 - 1):
    a[0] = val
    print(hex(a[0]))

# Write big-int values to array('L') via subscript.
a = array("L", [0])
for val in (0x80000000, 0xFFFFFFFF):
    a[0] = val
    print(hex(a[0]))

# Write big-int values via struct.pack_into to a bytearray.
buf = bytearray(4)
for val in (0x80000000, 0xDEADBEEF, 2**32 - 1):
    struct.pack_into("<I", buf, 0, val)
    (result,) = struct.unpack_from("<I", buf, 0)
    print(hex(result))
