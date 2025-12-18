# Test uctypes.addressof().

try:
    from sys import maxsize
    import uctypes
except ImportError:
    print("SKIP")
    raise SystemExit

# Test small addresses.
for i in range(8):
    print(uctypes.addressof(uctypes.bytearray_at(1 << i, 8)))

# Test address that is bigger than the greatest small-int but still within the address range.
try:
    large_addr = maxsize + 1
    print(uctypes.addressof(uctypes.bytearray_at(large_addr, 8)) == large_addr)
except OverflowError:
    print(True)  # systems with 64-bit bigints will overflow on the above operation
