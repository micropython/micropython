# test loading from ptr32 type


@micropython.viper
def get(src: ptr32) -> int:
    return src[0]


@micropython.viper
def get1(src: ptr32) -> int:
    return src[1]


@micropython.viper
def memadd(src: ptr32, n: int) -> int:
    sum = 0
    for i in range(n):
        sum += src[i]
    return sum


@micropython.viper
def memadd2(src_in) -> int:
    src = ptr32(src_in)
    n = int(len(src_in)) >> 2
    sum = 0
    for i in range(n):
        sum += src[i]
    return sum


b = bytearray(b"\x12\x12\x12\x12\x34\x34\x34\x34")
print(b)
print(hex(get(b)), hex(get1(b)))
print(hex(memadd(b, 2)))
print(hex(memadd2(b)))
