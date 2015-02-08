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

# test constant integer with more than 255 chars
x = 0x84ce72aa8699df436059f052ac51b6398d2511e49631bcb7e71f89c499b9ee425dfbc13a5f6d408471b054f2655617cbbaf7937b7c80cd8865cf02c8487d30d2b0fbd8b2c4e102e16d828374bbc47b93852f212d5043c3ea720f086178ff798cc4f63f787b9c2e419efa033e7644ea7936f54462dc21a6c4580725f7f0e7d1aaaaaaa
print(x)
