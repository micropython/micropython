# Test boundary conditions for various architectures

SET_TEMPLATE = """
@micropython.viper
def set{off}(dest: ptr8):
    dest[{off}] = {val}
set{off}(buffer)
print(hex(get_index(buffer, {off})))
"""

BIT_THRESHOLDS = (5, 8, 11, 12)
SIZE = 1
MASK = (1 << (8 * SIZE)) - 1


@micropython.viper
def set_index(dest: ptr8, i: int, val: uint):
    dest[i] = val


def get_index(src: ptr8, i: int):
    return src[i]


buffer = bytearray(((1 << max(BIT_THRESHOLDS) + 1) // 1024) * 1024)
next = 1
val = 0
for bit in BIT_THRESHOLDS:
    print("---", bit)
    pre, idx, post = (((1 << bit) - (2 * SIZE)), ((1 << bit) - (1 * SIZE)), (1 << bit))
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
