# test [...] of bytes

print(b'123'[0])
print(b'123'[1])
print(b'123'[-1])

try:
    b'123'[1] = 4
except TypeError:
    print('TypeError')

try:
    del b'123'[1]
except TypeError:
    print('TypeError')
