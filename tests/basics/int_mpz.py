# to test arbitrariy precision integers

x = 1000000000000000000000000000000
y = 2000000000000000000000000000000

# printing
print(x)
print(y)

# addition
print(x + 1)
print(x + y)

# subtraction
print(x - 1)
print(x - y)
print(y - x)

# multiplication
print(x * 2)
print(x * y)

# integer division
print(x // 2)
print(y // x)

# bit inversion
print(~x)
print(~(-x))

# left shift
x = 0x10000000000000000000000
for i in range(32):
    x = x << 1
    print(x)

# right shift
x = 0x10000000000000000000000
for i in range(32):
    x = x >> 1
    print(x)

# left shift of a negative number
for i in range(8):
    print(-10000000000000000000000000 << i)
    print(-10000000000000000000000001 << i)
    print(-10000000000000000000000002 << i)
    print(-10000000000000000000000003 << i)
    print(-10000000000000000000000004 << i)

# right shift of a negative number
for i in range(8):
    print(-10000000000000000000000000 >> i)
    print(-10000000000000000000000001 >> i)
    print(-10000000000000000000000002 >> i)
    print(-10000000000000000000000003 >> i)
    print(-10000000000000000000000004 >> i)
