# Test boundary conditions for various architectures

GET_TEMPLATE = """
@micropython.viper
def get{off}(src: ptr16) -> uint:
    return uint(src[{off}])
print(hex(get{off}(buffer)))
"""


BIT_THRESHOLDS = (5, 8, 11, 12)
SIZE = 2


@micropython.viper
def get_index(src: ptr16, i: int) -> int:
    return src[i]


def data(start, len):
    output = bytearray(len)
    for idx in range(len):
        output[idx] = (start + idx) & 0xFF
    return output


buffer = bytearray((((1 << max(BIT_THRESHOLDS)) + 1) // 1024) * 1024)
val = 0
for bit in BIT_THRESHOLDS:
    print("---", bit)
    pre, idx, post = ((1 << bit) - (2 * SIZE), (1 << bit) - (1 * SIZE), 1 << bit)
    buffer[pre:post] = data(val, 3 * SIZE)
    val = val + (3 * SIZE)

    pre, idx, post = pre // SIZE, idx // SIZE, post // SIZE
    print(hex(get_index(buffer, pre)), hex(get_index(buffer, idx)), hex(get_index(buffer, post)))
    exec(GET_TEMPLATE.format(off=pre))
    exec(GET_TEMPLATE.format(off=idx))
    exec(GET_TEMPLATE.format(off=post))
