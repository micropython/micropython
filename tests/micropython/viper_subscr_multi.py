# test viper with multiple subscripts in a single expression


@micropython.viper
def f1(b: ptr8):
    b[0] += b[1]


@micropython.viper
def f2(b: ptr8, i: int):
    b[0] += b[i]


b = bytearray(b"\x01\x02")
f1(b)
print(b)

b = bytearray(b"\x01\x02")
f2(b, 1)
print(b)
