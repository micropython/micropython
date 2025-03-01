# check if Thumb2/ARMV7M instructions are supported


@micropython.asm_thumb
def f():
    it(eq)
    nop()


print("thumb2")
