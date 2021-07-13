# test loading from ptr8 type


@micropython.viper
def get(src: ptr8) -> int:
    return src[0]


@micropython.viper
def get1(src: ptr8) -> int:
    return src[1]


@micropython.viper
def memadd(src: ptr8, n: int) -> int:
    sum = 0
    for i in range(n):
        sum += src[i]
    return sum


@micropython.viper
def memadd2(src_in) -> int:
    src = ptr8(src_in)
    n = int(len(src_in))
    sum = 0
    for i in range(n):
        sum += src[i]
    return sum


b = bytearray(b"1234")
print(b)
print(get(b), get1(b))
print(memadd(b, 4))
print(memadd2(b))
