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
print(array.array('B', [0x61, 0x62, 0x63]) == b'abc')
print(array.array('b', [0x61, 0x62, 0x63]) != b'abc')
print(array.array('b', [0x61, 0x62, 0x63]) == b'xyz')
print(array.array('b', [0x61, 0x62, 0x63]) != b'xyz')
print(b'abc' == array.array('b', [0x61, 0x62, 0x63]))
print(b'abc' == array.array('B', [0x61, 0x62, 0x63]))
print(b'abc' != array.array('b', [0x61, 0x62, 0x63]))
print(b'xyz' == array.array('b', [0x61, 0x62, 0x63]))
print(b'xyz' != array.array('b', [0x61, 0x62, 0x63]))

compatible_typecodes = []
for t in ["b", "h", "i", "l", "q"]:
  compatible_typecodes.append((t, t))
  compatible_typecodes.append((t, t.upper()))
for a, b in compatible_typecodes:
  print(array.array(a, [1, 2]) == array.array(b, [1, 2]))

class X(array.array):
    pass

print(bytes(X('b', [0x61, 0x62, 0x63])) == b'abc')
print(X('b', [0x61, 0x62, 0x63]) == b'abc')
print(X('b', [0x61, 0x62, 0x63]) != b'abc')
print(X('b', [0x61, 0x62, 0x63]) == array.array('b', [0x61, 0x62, 0x63]))
print(X('b', [0x61, 0x62, 0x63]) != array.array('b', [0x61, 0x62, 0x63]))

# other comparisons
for typecode in ["B", "H", "I", "L", "Q"]:
    a = array.array(typecode, [1, 1])
    print(a < a)
    print(a <= a)
    print(a > a)
    print(a >= a)

    al = array.array(typecode, [1, 0])
    ab = array.array(typecode, [1, 2])

    print(a < al)
    print(a <= al)
    print(a > al)
    print(a >= al)

    print(a < ab)
    print(a <= ab)
    print(a > ab)
    print(a >= ab)
