@micropython.asm_thumb
def f(r0, r1, r2):
    push({r0})
    push({r1, r2})
    pop({r0})
    pop({r1, r2})

print(f(0, 1, 2))
