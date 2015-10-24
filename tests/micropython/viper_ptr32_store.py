# test store to ptr32 type

@micropython.viper
def set(dest:ptr32, val:int):
    dest[0] = val

@micropython.viper
def set1(dest:ptr32, val:int):
    dest[1] = val

@micropython.viper
def memset(dest:ptr32, val:int, n:int):
    for i in range(n):
        dest[i] = val

@micropython.viper
def memset2(dest_in, val:int):
    dest = ptr32(dest_in)
    n = int(len(dest_in)) >> 2
    for i in range(n):
        dest[i] = val

b = bytearray(8)
print(b)

set(b, 0x42424242)
print(b)

set1(b, 0x43434343)
print(b)

memset(b, 0x44444444, len(b) // 4)
print(b)

memset2(b, 0x45454545)
print(b)
