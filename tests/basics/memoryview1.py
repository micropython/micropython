# test memoryview
try:
    memoryview
except:
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

import array
a = array.array('i', [1, 2, 3, 4])
m = memoryview(a)
print(list(m))
print(list(m[1:-1]))
m[2] = 6
print(a)

# test slice assignment between memoryviews
b1 = bytearray(b'1234')
b2 = bytearray(b'5678')
b3 = bytearray(b'5678')
m1 = memoryview(b1)
m2 = memoryview(b2)
m3 = memoryview(b3)
m2[1:3] = m1[0:2]
print(b2)
b3[1:3] = m1[0:2]
print(b3)
m1[2:4] = b3[1:3]
print(b1)

try:
    m2[1:3] = b1[0:4]
except ValueError:
    print("ValueError")

try:
    m2[1:3] = m1[0:4]
except ValueError:
    print("ValueError")

try:
    m2[0:4] = m1[1:3]
except ValueError:
    print("ValueError")

# test memoryview of arrays with items sized larger than 1
a1 = array.array('i', [0]*5)
m4 = memoryview(a1)
a2 = array.array('i', [3]*5)
m5 = memoryview(a2)
m4[1:3] = m5[1:3]
print(a1)

try:
    m4[1:3] = m2[1:3]
except ValueError:
    print("ValueError")

# invalid assignment on RHS
try:
    memoryview(array.array('i'))[0:2] = b'1234'
except ValueError:
    print('ValueError')

# invalid attribute
try:
    memoryview(b'a').noexist
except AttributeError:
    print('AttributeError')
