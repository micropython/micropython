# test bcc instructions


@micropython.asm_aarch64
def f(x0):
    mov(x1, x0)

    mov(x0, 10)
    cmp(x1, 1)
    beq(end)

    mov(x0, 20)
    cmp(x1, 2)
    beq(end)

    mov(x0, 30)
    cmp(x1, 3)
    beq(end)

    mov(x0, 0)

    label(end)


print(f(0))
print(f(1))
print(f(2))
print(f(3))
