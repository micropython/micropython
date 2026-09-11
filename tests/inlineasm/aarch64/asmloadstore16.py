import array


@micropython.asm_aarch64
def asm_strh(x0, x1):
    strh(x1, [x0, 0])


@micropython.asm_aarch64
def asm_ldrh(x0):
    ldrh(x0, [x0, 0])


@micropython.asm_aarch64
def asm_strh_offset(x0, x1):
    strh(x1, [x0, 4])


@micropython.asm_aarch64
def asm_ldrh_offset(x0):
    ldrh(x0, [x0, 4])


a = array.array("h", (0, 0, 0, 0))
asm_strh(a, 12345)
print(a[0])

a = array.array("h", (99, 0, 0, 0))
print(asm_ldrh(a))

a = array.array("h", (0, 0, 0, 0))
asm_strh_offset(a, 30000)
print(a[2])

a = array.array("h", (0, 0, 54321, 0))
print(asm_ldrh_offset(a))
