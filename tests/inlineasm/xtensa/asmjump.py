@micropython.asm_xtensa
def jump() -> int:
    movi(a2, 0)
    j(NEXT)
    addi(a2, a2, 1)
    j(DONE)
    label(NEXT)
    addi(a2, a2, 2)
    label(DONE)


print(jump())


@micropython.asm_xtensa
def jumpx() -> int:
    call0(ENTRY)
    label(ENTRY)
    movi(a2, 0)
    addi(a3, a0, 12)
    jx(a3)
    movi(a2, 1)
    movi(a2, 2)


print(jumpx())
