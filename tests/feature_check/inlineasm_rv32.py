# check if RISC-V 32 inline asm is supported


@micropython.asm_rv32
def f():
    add(a0, a0, a0)


print("rv32")
