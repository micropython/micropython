# Test boundary conditions for various architectures

TEST_DATA = (
    (3, (0x04030201, 0x08070605, 0x0C0B0A09)),
    (63, (0x100F0E0D, 0x14131211, 0x18171615)),
    (1023, (0x1C1B1A19, 0x201F1E1D, 0x24232221)),
)

SET_TEMPLATE = """
@micropython.viper
def set{off}(dest: ptr32):
    dest[{off}] = {val} & 0x3FFFFFFF
set{off}(b)
print(b[{off} * 4:({off} + 1) * 4])
"""


@micropython.viper
def set_index(dest: ptr32, i: int, val: int):
    dest[i] = val


b = bytearray(5000)
for start, vals in TEST_DATA:
    for i, v in enumerate(vals):
        set_index(b, start + i, v)
        print(b[(start + i) * 4 : (start + i + 1) * 4])

for i in range(len(b)):
    b[i] = 0


for start, vals in TEST_DATA:
    for i, v in enumerate(vals):
        exec(SET_TEMPLATE.format(off=start + i, val=v + 0x01010101))
