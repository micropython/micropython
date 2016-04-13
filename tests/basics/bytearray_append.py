# test bytearray.append method

a = bytearray(4)
print(a)

# append should append a single byte
a.append(2)
print(a)

# a should not be modified if append fails
try:
    a.append(None)
except TypeError:
    print('TypeError')
print(a)
