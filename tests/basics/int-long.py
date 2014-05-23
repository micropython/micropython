# This tests long ints for 32-bit machine

a = 0x1ffffffff
b = 0x100000000
print(a)
print(b)
print(a + b)
print(a - b)
print(b - a)
# overflows long long implementation
#print(a * b)
print(a // b)
print(a % b)
print("&", a & b)
print(a | b)
print(a ^ b)
print(a << 3)
print(a >> 1)

a += b
print(a)
a -= 123456
print(a)
a *= 257
print(a)
a //= 257
print(a)
a %= b
print(a)
a ^= b
print(a)
a |= b
print(a)
a &= b
print("&=", a)
a <<= 5
print(a)
a >>= 1
print(a)

# Test referential integrity of long ints
a = 0x1ffffffff
b = a
a += 1
print(a)
print(b)

# Bitwise ops on 64-bit

a = 0x1ffffffffffffffff
b = 0x10000000000000000
print("&", a & b)
print(a | b)
print(a ^ b)
