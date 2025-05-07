# Test boundary conditions for various architectures

GET_TEMPLATE = """
@micropython.viper
def get{off}(src: ptr8) -> int:
    return src[{off}]
print(get{off}(b))
"""


@micropython.viper
def get_index(src: ptr8, i: int) -> int:
    return src[i]


b = bytearray(5000)
b[30:32] = b"123"
b[254:256] = b"456"
b[4094:4096] = b"789"

for pre, idx, post in (30, 31, 32), (254, 255, 256), (4094, 4095, 4096):
    print(get_index(b, pre), get_index(b, idx), get_index(b, post))
    exec(GET_TEMPLATE.format(off=pre))
    exec(GET_TEMPLATE.format(off=idx))
    exec(GET_TEMPLATE.format(off=post))
