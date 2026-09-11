@micropython.asm_aarch64
def and_reg(x0, x1):
    and_(x0, x0, x1)


@micropython.asm_aarch64
def and_imm(x0):
    and_(x0, x0, 0xF0)


@micropython.asm_aarch64
def eor_reg(x0, x1):
    eor(x0, x0, x1)


@micropython.asm_aarch64
def eor_imm(x0):
    eor(x0, x0, 0xFF)


@micropython.asm_aarch64
def orr_reg(x0, x1):
    orr(x0, x0, x1)


@micropython.asm_aarch64
def orr_imm(x0):
    orr(x0, x0, 0x100)


print(hex(and_reg(0xFF, 0x0F)))
print(hex(and_imm(0xFF)))
print(hex(eor_reg(0xAA, 0x55)))
print(hex(eor_imm(0xAA)))
print(hex(orr_reg(0xF0, 0x0F)))
print(hex(orr_imm(0xFF)))
