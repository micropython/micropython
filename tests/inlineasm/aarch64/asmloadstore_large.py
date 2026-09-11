# test large constant offsets in inline assembler
# these test the boundary of immediate offset encoding


import array


@micropython.asm_aarch64
def ldr_max(x0) -> int:
    ldr(x0, [x0, 32760])


@micropython.asm_aarch64
def str_max(x0, x1):
    str(x1, [x0, 32760])


@micropython.asm_aarch64
def ldrb_max(x0) -> int:
    ldrb(x0, [x0, 4095])


@micropython.asm_aarch64
def strb_max(x0, x1):
    strb(x1, [x0, 4095])


@micropython.asm_aarch64
def ldrh_max(x0) -> int:
    ldrh(x0, [x0, 4094])


@micropython.asm_aarch64
def strh_max(x0, x1):
    strh(x1, [x0, 4094])


a = array.array("q", [0] * 4096)
a[4095] = 0x4242424242424242
print(hex(ldr_max(a)))

a = array.array("q", [0] * 4096)
str_max(a, 0x4343434343434343)
print(hex(a[4095]))

a = array.array("b", list(range(256)) * 17)
print(hex(ldrb_max(a)))

a = array.array("b", [0] * 4096)
strb_max(a, 0xFF)
print(hex(a[4095]))

a = array.array("h", list(range(256)) * 17)
print(hex(ldrh_max(a)))

a = array.array("h", [0] * 2048)
strh_max(a, 0xBEEF)
print(hex(a[2047]))
