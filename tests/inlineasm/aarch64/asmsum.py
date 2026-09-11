@micropython.asm_aarch64
def asm_sum_words(x0, x1):
    mov(x2, 0)

    b(loop_entry)

    label(loop1)
    ldr(x3, [x1, 0])
    add(x2, x2, x3)

    add(x1, x1, 8)
    sub(x0, x0, 1)

    label(loop_entry)
    cmp(x0, 0)
    bgt(loop1)

    mov(x0, x2)


@micropython.asm_aarch64
def asm_sum_bytes(x0, x1):
    mov(x2, 0)

    b(loop_entry)

    label(loop1)
    ldrb(x3, [x1, 0])
    add(x2, x2, x3)

    add(x1, x1, 1)
    sub(x0, x0, 1)

    label(loop_entry)
    cmp(x0, 0)
    bgt(loop1)

    mov(x0, x2)


import array

b = array.array("q", (100, 200, 300, 400))
n = asm_sum_words(len(b), b)
print(b, n)

b = array.array("b", (10, 20, 30, 40, 50, 60, 70, 80))
n = asm_sum_bytes(len(b), b)
print(b, n)

b = b"\x01\x02\x03\x04"
n = asm_sum_bytes(len(b), b)
print(b, n)
