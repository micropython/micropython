# test ptr16 type

@micropython.viper
def set(dest:ptr16, val:int):
    dest[0] = val

@micropython.viper
def memset(dest:ptr16, val:int, n:int):
    for i in range(n):
        dest[i] = val

b = bytearray(4)
print(b)

set(b, 0x4242)
print(b)

memset(b, 0x4343, len(b) // 2)
print(b)
