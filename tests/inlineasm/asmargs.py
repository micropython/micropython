# test passing arguments

@micropython.asm_thumb
def arg0():
    mov(r0, 1)
print(arg0())

@micropython.asm_thumb
def arg1(r0):
    add(r0, r0, 1)
print(arg1(1))

@micropython.asm_thumb
def arg2(r0, r1):
    add(r0, r0, r1)
print(arg2(1, 2))

@micropython.asm_thumb
def arg3(r0, r1, r2):
    add(r0, r0, r1)
    add(r0, r0, r2)
print(arg3(1, 2, 3))

@micropython.asm_thumb
def arg4(r0, r1, r2, r3):
    add(r0, r0, r1)
    add(r0, r0, r2)
    add(r0, r0, r3)
print(arg4(1, 2, 3, 4))
