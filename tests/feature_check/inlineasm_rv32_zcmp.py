# check if RISC-V 32 inline asm supported Zcmp opcodes


@micropython.asm_rv32
def f():
    cm_mvsa01(s0, s1)


f()
print("rv32_zcmp")
