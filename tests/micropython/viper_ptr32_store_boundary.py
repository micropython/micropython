# Test boundary conditions for various architectures

SET_TEMPLATE = """
@micropython.viper
def set{off}(dest: ptr32):
    saved = dest
    dest[{off}] = {val}
    assert int(saved) == int(dest)
"""

BIT_THRESHOLDS = (5, 8, 11, 12)
SIZE = 4
MASK = (1 << (8 * SIZE)) - 1

next_int = 1
test_buffer = bytearray(SIZE)


def next_value() -> uint:
    global next_int
    global test_buffer
    for index in range(1, SIZE):
        test_buffer[index - 1] = test_buffer[index]
    test_buffer[SIZE - 1] = next_int
    next_int += 1
    output = 0
    for byte in test_buffer:
        output = (output << 8) | byte
    return output & MASK


def get_index(src, i):
    return (
        src[i * SIZE]
        + (src[(i * SIZE) + 1] << 8)
        + (src[(i * SIZE) + 2] << 16)
        + (src[(i * SIZE) + 3] << 24)
    )


@micropython.viper
def set_index(dest: ptr32, i: int, val: uint):
    saved = dest
    dest[i] = val
    assert int(dest) == int(saved)


try:
    buffer = bytearray((((1 << max(BIT_THRESHOLDS)) // 1024) + 1) * 1024)

    for bit in BIT_THRESHOLDS:
        offset = (1 << bit) - (2 * SIZE)
        for index in range(0, 3 * SIZE, SIZE):
            exec(SET_TEMPLATE.format(off=(offset + index) // SIZE, val=next_value()))
except MemoryError:
    print("SKIP-TOO-LARGE")
    raise SystemExit


for bit in BIT_THRESHOLDS:
    print("---", bit)
    offset = (1 << bit) - (2 * SIZE)
    for index in range(0, 3 * SIZE, SIZE):
        globals()["set{}".format((offset + index) // SIZE)](buffer)
        print(hex(get_index(buffer, (offset + index) // SIZE)))
    for index in range(0, 3 * SIZE, SIZE):
        set_index(buffer, (offset + index) // SIZE, next_value())
        print(hex(get_index(buffer, (offset + index) // SIZE)))
