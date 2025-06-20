@micropython.asm_thumb
def f(r0, r1, r2):
    push({r0})
    push({r1, r2})
    pop({r0})
    pop({r1, r2})


@micropython.asm_thumb
def g():
    b(START)
    label(SUBROUTINE)
    push({lr})  # push return address
    mov(r0, 7)
    pop({pc})  # return
    label(START)
    bl(SUBROUTINE)


print(f(0, 1, 2))
print(g())
