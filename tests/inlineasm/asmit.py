# test it instruction

@micropython.asm_thumb
def f(r0, r1):
    cmp(r0, r1)
    it(eq)
    mov(r0, 100)
print(f(0, 0), f(1, 2))

@micropython.asm_thumb
def g(r0, r1):
    cmp(r0, r1)
    ite(eq)
    mov(r0, 100)
    mov(r0, 200)
print(g(0, 0), g(0, 1))
