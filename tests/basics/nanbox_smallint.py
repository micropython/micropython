# Test creating small integers without heap allocation in nan-boxing mode.

import micropython

try:
    # Test for nan-box build by allocating a float while heap is locked.
    # This should pass on nan-box builds.
    micropython.heap_lock()
    float(123)
    micropython.heap_unlock()
except:
    print("SKIP")
    raise SystemExit

# Check that nan-boxing uses 64-bit floats (eg it's not object representation C).
if float("1e100") == float("inf"):
    print("SKIP")
    raise SystemExit

micropython.heap_lock()
print(int("0x80000000"))
micropython.heap_unlock()

# This is the most positive small integer.
micropython.heap_lock()
print(int("0x3fffffffffff"))
micropython.heap_unlock()

# This is the most negative small integer.
micropython.heap_lock()
print(int("-0x3fffffffffff") - 1)
micropython.heap_unlock()

x = 1
micropython.heap_lock()
print((x << 31) + 1)
micropython.heap_unlock()

x = 1
micropython.heap_lock()
print((x << 45) + 1)
micropython.heap_unlock()
