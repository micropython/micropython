@micropython.asm_aarch64
def lsl_reg(x0, x1):
    lsl(x0, x0, x1)


@micropython.asm_aarch64
def lsr_reg(x0, x1):
    lsr(x0, x0, x1)


@micropython.asm_aarch64
def asr_reg(x0, x1):
    asr(x0, x0, x1)


print(hex(lsl_reg(1, 4)))
print(hex(lsl_reg(3, 8)))
print(hex(lsr_reg(0x100, 4)))
print(hex(lsr_reg(0xFF00, 8)))
print(hex(asr_reg(0x100, 4)))
print(hex(asr_reg(-0x100, 4)))
