import skip_if

try:
    memoryview
except:
    skip_if.skip()

skip_if.no_slice_assign()

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

import array

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
