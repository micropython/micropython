RESULTS = []

LOAD_TEMPLATE_REG = """
BUFFER = bytearray([1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12])
OFFSET = 1
@micropython.viper
def test_invariants_load{}_reg(buf: ptr{}, offset: int):
    offset_copy1: int = offset
    temporary1 = buf[offset]
    offset_copy2: int = offset
    temporary2 = buf[offset]
    RESULTS.append(["LOAD{}", temporary1 == temporary2, offset == offset_copy1 == offset_copy2])
test_invariants_load{}_reg(BUFFER, OFFSET)
"""


STORE_TEMPLATE_REG = """
BUFFER = bytearray([1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12])
OFFSET = 1
@micropython.viper
def test_invariants_store{}_reg(buf: ptr{}, offset: int):
    offset_copy: int = offset
    value: uint = {}
    buf[offset] = value
    temporary = buf[offset]
    RESULTS.append(["STORE{}", temporary == value, offset == offset_copy])
test_invariants_store{}_reg(BUFFER, OFFSET)
"""

try:
    for width, value in ((8, 0x11), (16, 0x1111), (32, 0x11111111)):
        exec(LOAD_TEMPLATE_REG.format(width, width, width, width, width))
        exec(STORE_TEMPLATE_REG.format(width, width, value, width, width, width))
except MemoryError:
    print("SKIP-TOO-LARGE")
    raise SystemExit

for line in RESULTS:
    print(" ".join([str(i) for i in line]))
