# test construction of bytes from different objects

# tuple, list
print(bytes((1, 2)))
print(bytes([1, 2]))

# constructor value out of range
try:
    bytes([-1])
except ValueError:
    print('ValueError')

# constructor value out of range
try:
    bytes([256])
except ValueError:
    print('ValueError')

# error in construction
try:
    a = bytes([1, 2, 3], 1)
except TypeError:
    print('TypeError')

# str subclass as the source
class MyStr(str):
    pass

print(bytes(MyStr('abc'), 'utf-8'))
try:
    bytes(MyStr('abc'))
except TypeError:
    print('TypeError')
