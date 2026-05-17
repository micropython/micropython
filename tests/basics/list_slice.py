# test list slices, getting values

x = list(range(10))

print(x[1:])
print(x[:-1])
print(x[2:3])

a = 2
b = 4
c = 3
print(x[:])
print(x[::])
print(x[::c])
print(x[:b])
print(x[:b:])
print(x[:b:c])
print(x[a])
print(x[a:])
print(x[a::])
print(x[a::c])
print(x[a:b])
print(x[a:b:])
print(x[a:b:c])

# these should not raise IndexError
print([][1:])
print([][-1:])

try:
    [][::0]
except ValueError:
    print('ValueError')
