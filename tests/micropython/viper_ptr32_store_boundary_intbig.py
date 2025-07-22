# Test boundary conditions for various architectures

SET_TEMPLATE = """
@micropython.viper
def set{off}(dest: ptr32):
    dest[{off}] = {val}
set{off}(buffer)
print(hex(get_index(buffer, {off})))
"""

BIT_THRESHOLDS = (5, 8, 11, 12)
SIZE = 4
MASK = (1 << (8 * SIZE)) - 1


@micropython.viper
def set_index(dest: ptr32, i: int, val: uint):
    dest[i] = val


def get_index(src, i):
    return (
        src[i * SIZE]
        + (src[(i * SIZE) + 1] << 8)
        + (src[(i * SIZE) + 2] << 16)
        + (src[(i * SIZE) + 3] << 24)
    )


buffer = bytearray(((1 << max(BIT_THRESHOLDS) + 1) // 1024) * 1024)
next = 1
val = 0
for bit in BIT_THRESHOLDS:
    print("---", bit)
    pre, idx, post = (
        (((1 << bit) - (2 * SIZE)) // SIZE),
        (((1 << bit) - (1 * SIZE)) // SIZE),
        ((1 << bit) // SIZE),
    )
    val = (val << 8) + next
    next += 1
    set_index(buffer, pre, val & MASK)
    val = (val << 8) + next
    next += 1
    set_index(buffer, idx, val & MASK)
    val = (val << 8) + next
    next += 1
    set_index(buffer, post, val & MASK)
    val = (val << 8) + next
    next += 1
    print(hex(get_index(buffer, pre)), hex(get_index(buffer, idx)), hex(get_index(buffer, post)))
    exec(SET_TEMPLATE.format(off=pre, val=val & MASK))
    val = (val << 8) + next
    next += 1
    exec(SET_TEMPLATE.format(off=idx, val=val & MASK))
    val = (val << 8) + next
    next += 1
    exec(SET_TEMPLATE.format(off=post, val=val & MASK))
