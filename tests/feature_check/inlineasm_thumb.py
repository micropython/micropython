# check if Thumb inline asm is supported


@micropython.asm_thumb
def f():
    nop()


print("thumb")
