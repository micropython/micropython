@micropython.asm_aarch64
def f_nop(x0):
    nop()
    nop()
    add(x0, x0, 1)
    nop()


# wfi is compiled but not called: without an enabled interrupt source it may
# stall forever on bare-metal targets.
@micropython.asm_aarch64
def f_wfi(x0):
    wfi()
    add(x0, x0, 1)


print(f_nop(41))
