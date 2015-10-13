# test ptr16 type

@micropython.viper
def set(dest:ptr16, val:int):
    dest[0] = val

@micropython.viper
def set1(dest:ptr16, val:int):
    dest[1] = val

@micropython.viper
def memset(dest:ptr16, val:int, n:int):
    for i in range(n):
        dest[i] = val

@micropython.viper
def memset2(dest_in, val:int):
    dest = ptr16(dest_in)
    n = int(len(dest_in)) >> 1
    for i in range(n):
        dest[i] = val

b = bytearray(4)
print(b)

set(b, 0x4242)
print(b)

set1(b, 0x4343)
print(b)

memset(b, 0x4444, len(b) // 2)
print(b)

memset2(b, 0x4545)
print(b)
