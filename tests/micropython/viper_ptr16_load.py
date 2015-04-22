# test loading from ptr16 type
# only works on little endian machines

@micropython.viper
def get(src:ptr16) -> int:
    return src[0]

@micropython.viper
def get1(src:ptr16) -> int:
    return src[1]

@micropython.viper
def memadd(src:ptr16, n:int) -> int:
    sum = 0
    for i in range(n):
        sum += src[i]
    return sum

b = bytearray(b'1234')
print(b)
print(get(b), get1(b))
print(memadd(b, 2))
