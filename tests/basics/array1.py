try:
    import uarray as array
except ImportError:
    try:
        import array
    except ImportError:
        print("SKIP")
        raise SystemExit

a = array.array('B', [1, 2, 3])
print(a, len(a))
i = array.array('I', [1, 2, 3])
print(i, len(i))
print(a[0])
print(i[-1])
a = array.array('l', [-1])
print(len(a), a[0])
a1 = array.array('l', [1, 2, 3])
a2 = array.array('L', [1, 2, 3])
print(a2[1])
print(a1 == a2)

# Empty arrays
print(len(array.array('h')))
print(array.array('i'))

# bool operator acting on arrays
print(bool(array.array('i')))
print(bool(array.array('i', [1])))

# containment, with incorrect type
print('12' in array.array('B', b'12'))
print([] in array.array('B', b'12'))

# bad typecode
try:
    array.array('X')
except ValueError:
    print("ValueError")

# equality (CPython requires both sides are array)
print(bytes(array.array('b', [0x61, 0x62, 0x63])) == b'abc')
print(array.array('b', [0x61, 0x62, 0x63]) == b'abc')
print(array.array('b', [0x61, 0x62, 0x63]) != b'abc')
print(array.array('b', [0x61, 0x62, 0x63]) == b'xyz')
print(array.array('b', [0x61, 0x62, 0x63]) != b'xyz')
print(b'abc' == array.array('b', [0x61, 0x62, 0x63]))
print(b'abc' != array.array('b', [0x61, 0x62, 0x63]))
print(b'xyz' == array.array('b', [0x61, 0x62, 0x63]))
print(b'xyz' != array.array('b', [0x61, 0x62, 0x63]))

class X(array.array):
    pass

print(bytes(X('b', [0x61, 0x62, 0x63])) == b'abc')
print(X('b', [0x61, 0x62, 0x63]) == b'abc')
print(X('b', [0x61, 0x62, 0x63]) != b'abc')
print(X('b', [0x61, 0x62, 0x63]) == array.array('b', [0x61, 0x62, 0x63]))
print(X('b', [0x61, 0x62, 0x63]) != array.array('b', [0x61, 0x62, 0x63]))
