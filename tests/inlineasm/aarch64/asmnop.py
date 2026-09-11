@micropython.asm_aarch64
def f_nop(x0):
    nop()
    nop()
    add(x0, x0, 1)
    nop()


print(f_nop(41))
