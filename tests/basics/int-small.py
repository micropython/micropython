# This tests small int range for 32-bit machine

a = 0x3fffff
print(a)
a *= 0x10
print(a)
a *= 0x10
print(a)
a += 0xff
print(a)
# This would overflow
#a += 1

a = -0x3fffff
print(a)
a *= 0x10
print(a)
a *= 0x10
print(a)
a -= 0xff
print(a)
# This still doesn't overflow
a -= 1
print(a)
# This would overflow
#a -= 1
