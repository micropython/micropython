# tests int.bit_length

try:
    int.bit_length
except AttributeError:
    print('SKIP')
    raise SystemExit

n = -37
print(n.bit_length())

n = 1024
print(n.bit_length())

n = -1024
print(n.bit_length())

print((2048).bit_length())

print((-2048).bit_length())
