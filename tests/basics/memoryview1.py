# test memoryview
try:
    memoryview
except:
    print("SKIP")
    raise SystemExit
try:
    import array
except ImportError:
    print("SKIP")
    raise SystemExit

# test reading from bytes
b = b'1234'
m = memoryview(b)
print(len(m))
print(m[0], m[1], m[-1])
print(list(m))

# test writing to bytes
try:
    m[0] = 1
except TypeError:
    print("TypeError")
try:
    m[0:2] = b'00'
except TypeError:
    print("TypeError")

# test writing to bytearray
b = bytearray(b)
m = memoryview(b)
m[0] = 1
print(b)
print(list(m))

# test slice
m = memoryview(b'1234')
print(list(m[1:]))
print(list(m[1:-1]))

# this tests get_buffer of memoryview
m = memoryview(bytearray(2))
print(bytearray(m))
print(list(memoryview(memoryview(b'1234')))) # read-only memoryview

a = array.array('i', [1, 2, 3, 4])
m = memoryview(a)
print(list(m))
print(list(m[1:-1]))
m[2] = 6
print(a)

# invalid attribute
try:
    memoryview(b'a').noexist
except AttributeError:
    print('AttributeError')

# equality
print(memoryview(b'abc') == b'abc')
print(memoryview(b'abc') != b'abc')
print(memoryview(b'abc') == b'xyz')
print(memoryview(b'abc') != b'xyz')
print(b'abc' == memoryview(b'abc'))
print(b'abc' != memoryview(b'abc'))
print(b'abc' == memoryview(b'xyz'))
print(b'abc' != memoryview(b'xyz'))
print(memoryview(b'abcdef')[2:4] == b'cd')
print(memoryview(b'abcdef')[2:4] != b'cd')
print(memoryview(b'abcdef')[2:4] == b'xy')
print(memoryview(b'abcdef')[2:4] != b'xy')
print(b'cd' == memoryview(b'abcdef')[2:4])
print(b'cd' != memoryview(b'abcdef')[2:4])
print(b'xy' == memoryview(b'abcdef')[2:4])
print(b'xy' != memoryview(b'abcdef')[2:4])
