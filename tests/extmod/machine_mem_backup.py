# Test machine.mem_backup() function.

try:
    import machine

    mem = machine.mem_backup()
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

# Discovery: -1 returns a tuple of all regions.
regions = machine.mem_backup(-1)
print("regions is tuple:", isinstance(regions, tuple))
print("at least one region:", len(regions) >= 1)
print("all memoryviews:", all(isinstance(r, memoryview) for r in regions))
print("all len > 0:", all(len(r) > 0 for r in regions))
print("all valid itemsize:", all(r.itemsize in (1, 4) for r in regions))

# Default region (index 0).
print("memoryview:", isinstance(mem, memoryview))
print("len > 0:", len(mem) > 0)
print("itemsize ok:", mem.itemsize in (1, 4))

# Small-value write/read.
mem[0] = 42
print(mem[0] == 42)

if mem.itemsize == 4:
    # Values >= 0x40000000 are big-ints on 32-bit targets; check that
    # word-only backup registers receive them as aligned word writes.
    for val in (0x40000000, 0x41020304, 0x7FFFFFFF, 0xDEADBEEF):
        mem[0] = val
        print(mem[0] == val)
    mem[0] = -1
    print(mem[0] == 0xFFFFFFFF)
else:
    for val in (0x80, 0xA5, 0xFF, 0xC3):
        mem[0] = val
        print(mem[0] == val)
    mem[0] = 0
    print(mem[0] == 0)

last = len(mem) - 1
mem[last] = 1
print(mem[last] == 1)

# Out-of-range region raises ValueError.
try:
    machine.mem_backup(len(regions))
    print("no error")
except ValueError:
    print("ValueError")

try:
    machine.mem_backup(-2)
    print("no error")
except ValueError:
    print("ValueError")
