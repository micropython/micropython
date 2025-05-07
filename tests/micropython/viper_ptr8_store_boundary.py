# Test boundary conditions for various architectures

TEST_DATA = ((49, 30, 3), (52, 254, 3), (55, 4094, 3))

SET_TEMPLATE = """
@micropython.viper
def set{off}(dest: ptr8):
    dest[{off}] = {val}
set{off}(b)
print(b[{off}])
"""


@micropython.viper
def set_index(dest: ptr8, i: int, val: int):
    dest[i] = val


b = bytearray(5000)
for val, start, count in TEST_DATA:
    for i in range(count):
        set_index(b, start + i, val + i)
    print(b[start : start + count])

for i in range(len(b)):
    b[i] = 0

for val, start, count in TEST_DATA:
    for i in range(count):
        exec(SET_TEMPLATE.format(off=start + i, val=val + i + 16))
