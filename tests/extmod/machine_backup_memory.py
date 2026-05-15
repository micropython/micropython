# Test machine.backup_memory memoryview.

try:
    import machine

    mem = machine.backup_memory
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

# Basic properties.
print("memoryview:", isinstance(mem, memoryview))
print("len > 0:", len(mem) > 0)
print("itemsize ok:", mem.itemsize in (1, 4))

# Small-value write/read (fits in small-int on all targets).
mem[0] = 42
print(mem[0] == 42)

if mem.itemsize == 4:
    # Values above 0x3FFFFFFF are big-ints on 32-bit targets and exercise
    # the mp_binary_set_val_array long-int code path in py/binary.c.
    for val in (0x40000000, 0x41020304, 0x7FFFFFFF):
        mem[0] = val
        print(mem[0] == val)
    # Negative value maps to upper uint32 range.
    mem[0] = -1
    print(mem[0] == 0xFFFFFFFF)
else:
    # Byte-level write/read (same number of output lines).
    for val in (0x80, 0xA5, 0xFF):
        mem[0] = val
        print(mem[0] == val)
    mem[0] = 0
    print(mem[0] == 0)

# Boundary: last valid index.
last = len(mem) - 1
mem[last] = 1
print(mem[last] == 1)
