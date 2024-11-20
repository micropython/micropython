# check if Xtensa inline asm is supported


@micropython.asm_xtensa
def f():
    ret_n()


print("xtensa")
