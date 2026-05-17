# check if RISC-V 32 inline asm supported Zba opcodes


@micropython.asm_rv32
def f():
    sh1add(a0, a0, a0)


f()
print("rv32_zba")
