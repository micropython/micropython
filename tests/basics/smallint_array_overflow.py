try:
    from array import array
except ImportError:
    print("SKIP")
    raise SystemExit

def test_array_overflow(typecode, val):
    try:
        print(array(typecode, [val]))
    except OverflowError:
        print('OverflowError')

def test_bytearray_overflow(val):
    try:
        print(bytearray([val]))
    except (OverflowError, ValueError):
        # CircuitPython always does OverflowError
        print('(OverflowError, ValueError)')


# small int -1
test_array_overflow('Q', -1)
test_array_overflow('L', -1)
test_array_overflow('I', -1)
test_array_overflow('H', -1)
test_array_overflow('B', -1)

# 0 ok
test_array_overflow('Q', 0)
test_array_overflow('L', 0)
test_array_overflow('I', 0)
test_array_overflow('H', 0)
test_array_overflow('B', 0)

# 1 ok
test_array_overflow('Q', 1)
test_array_overflow('L', 1)
test_array_overflow('I', 1)
test_array_overflow('H', 1)
test_array_overflow('B', 1)

# truth value conversions
test_array_overflow('b', True)
test_array_overflow('b', False)

# similar tests for bytearrays
test_bytearray_overflow(0)
test_bytearray_overflow(1)
test_bytearray_overflow(-1)
test_bytearray_overflow(256)
test_bytearray_overflow(True)
test_bytearray_overflow(False)
