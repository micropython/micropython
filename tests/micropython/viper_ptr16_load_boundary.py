# Test boundary conditions for various architectures

GET_TEMPLATE = """
@micropython.viper
def get{off}(src: ptr16) -> int:
    return src[{off}]
print(b[{off} * 2:({off} + 1) * 2])
"""


@micropython.viper
def get_index(src: ptr16, i: int) -> int:
    return src[i]


b = bytearray(5000)
b[28:38] = b"0123456789"
b[252:262] = b"ABCDEFGHIJ"
b[4092:4102] = b"KLMNOPQRST"

for pre, idx, post in (15, 16, 17), (127, 128, 129), (2047, 2048, 2049):
    print(get_index(b, pre), get_index(b, idx), get_index(b, post))
    exec(GET_TEMPLATE.format(off=pre))
    exec(GET_TEMPLATE.format(off=idx))
    exec(GET_TEMPLATE.format(off=post))
