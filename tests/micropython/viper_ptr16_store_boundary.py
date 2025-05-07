# Test boundary conditions for various architectures

import gc


@micropython.viper
def set_index(dest: ptr16, i: int, val: int):
    dest[i] = val


b = bytearray(5000)
test_data = (
    (15, (0x4241, 0x4443, 0x4645)),
    (127, (0x4847, 0x4A49, 0x4C4B)),
    (2047, (0x4E4D, 0x504F, 0x5251)),
)

for start, vals in test_data:
    for i, v in enumerate(vals):
        set_index(b, start + i, v)
        print(b[(start + i) * 2 : (start + i + 1) * 2])

gc.collect()

b = bytearray(5000)

SET_TEMPLATE = """
@micropython.viper
def set{off}(dest: ptr16):
    dest[{off}] = {val}
set{off}(b)
print(b[{off} * 2:({off} + 1) * 2])
"""

for start, vals in test_data:
    for i, v in enumerate(vals):
        exec(SET_TEMPLATE.format(off=start + i, val=v))
