# Test boundary conditions for various architectures

GET_TEMPLATE = """
@micropython.viper
def get{off}(src: ptr32) -> int:
    return src[{off}]
print(b[{off} * 4:({off} + 1) * 4])
"""


@micropython.viper
def get_index(src: ptr32, i: int) -> int:
    return src[i]


b = bytearray(5000)
b[24:43] = b"0123456789ABCDEFGHIJ"
b[248:268] = b"KLMNOPQRSTUVWXYZabcd"
b[4088:4108] = b"efghijklmnopqrstuvwx"

for pre, idx, post in (7, 8, 9), (63, 64, 65), (1023, 1024, 1025):
    print(get_index(b, pre), get_index(b, idx), get_index(b, post))
    exec(GET_TEMPLATE.format(off=pre))
    exec(GET_TEMPLATE.format(off=idx))
    exec(GET_TEMPLATE.format(off=post))
