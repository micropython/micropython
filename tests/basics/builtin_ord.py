# test builtin ord (whether or not we support unicode)

print(ord('a'))

try:
    ord('')
except TypeError:
    print("TypeError")

# bytes also work in ord

print(ord(b'a'))
print(ord(b'\x00'))
print(ord(b'\x01'))
print(ord(b'\x7f'))
print(ord(b'\x80'))
print(ord(b'\xff'))

try:
    ord(b'')
except TypeError:
    print("TypeError")

# argument must be a string
try:
    ord(1)
except TypeError:
    print('TypeError')
