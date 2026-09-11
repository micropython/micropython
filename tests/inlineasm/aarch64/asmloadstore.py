# test str and strb instructions


@micropython.asm_aarch64
def asm_str_test(x0, x1):
    str(x1, [x0, 0])


@micropython.asm_aarch64
def asm_strb_test(x0, x1):
    strb(x1, [x0, 0])


@micropython.asm_aarch64
def asm_str_offset(x0, x1):
    str(x1, [x0, 16])


import array

a = array.array("q", (0, 0, 0, 0))
asm_str_test(a, 42)
print(a[0])

a = array.array("b", (0, 0, 0, 0))
asm_strb_test(a, 99)
print(a[0])

a = array.array("q", (0, 0, 0, 0))
asm_str_offset(a, 77)
print(a[2])
