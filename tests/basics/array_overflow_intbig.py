# This test currently requires an .exp file but in MP 2 it shouldn't.
#
# This is because MicroPython 1.x truncates integers written into arrays
# (see also cpydiff/module_array_constructor), but MicroPython 2 will change to
# match CPython.

try:
    from array import array
except ImportError:
    print("SKIP")
    raise SystemExit


def test_array_overflow(typecode, val):
    try:
        print(array(typecode, [val]))
    except OverflowError:
        print("OverflowError")


# big int -1
test_array_overflow("Q", -(2**64) // 2**64)
# test_array_overflow("L", -(2**64) // 2**64)  # 32-bit vs 64-bit results differ
test_array_overflow("I", -(2**64) // 2**64)
test_array_overflow("H", -(2**64) // 2**64)
test_array_overflow("B", -(2**64) // 2**64)

# big int 2**63
test_array_overflow("q", 2**63)
# test_array_overflow("l", 2**63)  # 32-bit vs 64-bit results differ
test_array_overflow("i", 2**63)
test_array_overflow("h", 2**63)
test_array_overflow("b", 2**63)
