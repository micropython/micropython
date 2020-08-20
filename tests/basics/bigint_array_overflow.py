import skip_if
skip_if.no_bigint()

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

# big int -1
test_array_overflow('Q', -2**64 // 2**64)
test_array_overflow('L', -2**64 // 2**64)
test_array_overflow('I', -2**64 // 2**64)
test_array_overflow('H', -2**64 // 2**64)
test_array_overflow('B', -2**64 // 2**64)

# big int 2**63
test_array_overflow('q', 2**63)
test_array_overflow('l', 2**63)
test_array_overflow('i', 2**63)
test_array_overflow('h', 2**63)
test_array_overflow('b', 2**63)
