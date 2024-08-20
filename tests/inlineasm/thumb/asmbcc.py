# test bcc instructions
# at the moment only tests beq, narrow and wide versions


@micropython.asm_thumb
def f(r0):
    mov(r1, r0)

    mov(r0, 10)
    cmp(r1, 1)
    beq(end)

    mov(r0, 20)
    cmp(r1, 2)
    beq_n(end)

    mov(r0, 30)
    cmp(r1, 3)
    beq_w(end)

    mov(r0, 0)

    label(end)


print(f(0))
print(f(1))
print(f(2))
print(f(3))
