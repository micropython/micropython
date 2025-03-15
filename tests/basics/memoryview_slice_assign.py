# test slice assignment to memoryview

try:
    memoryview(bytearray(1))[:] = memoryview(bytearray(1))
except (NameError, TypeError):
    print("SKIP")
    raise SystemExit

try:
    import array
except ImportError:
    print("SKIP")
    raise SystemExit

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

# invalid slice assignments
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

# test shift left of bytearray
b = bytearray(range(10))
mv = memoryview(b)
mv[1:] = mv[:-1]
print(b)

# test shift right of bytearray
b = bytearray(range(10))
mv = memoryview(b)
mv[:-1] = mv[1:]
print(b)

# test shift left of array
a = array.array('I', range(10))
mv = memoryview(a)
mv[1:] = mv[:-1]
print(a)

# test shift right of array
a = array.array('I', range(10))
mv = memoryview(a)
mv[:-1] = mv[1:]
print(a)
