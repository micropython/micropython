print(0 & (1 << 80))
print(0 & (1 << 80) == 0)
print(bool(0 & (1 << 80)))

#a = 0xfffffffffffffffffffffffffffff
#print(a & (1 << 80))
#print((a & (1 << 80)) >> 80)
#print((a & (1 << 80)) >> 80 == 1)
