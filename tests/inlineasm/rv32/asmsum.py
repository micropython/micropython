@micropython.asm_rv32
def asm_sum_words(a0, a1):
    # a0 = len
    # a1 = ptr
    # a2 = sum
    # a3 = dummy
    li(a2, 0)

    jal(zero, loop_entry)

    label(loop1)
    lw(a3, 0(a1))
    add(a2, a2, a3)

    addi(a1, a1, 4)
    addi(a0, a0, -1)

    label(loop_entry)
    bne(a0, zero, loop1)

    mv(a0, a2)


@micropython.asm_rv32
def asm_sum_bytes(a0, a1):
    # a0 = len
    # a1 = ptr
    # a2 = sum
    # a3 = dummy
    li(a2, 0)

    jal(zero, loop_entry)

    label(loop1)
    lbu(a3, 0(a1))
    add(a2, a2, a3)

    addi(a1, a1, 1)
    addi(a0, a0, -1)

    label(loop_entry)
    bne(a0, zero, loop1)

    mv(a0, a2)


import array

b = array.array("l", (100, 200, 300, 400))
n = asm_sum_words(len(b), b)
print(b, n)

b = array.array("b", (10, 20, 30, 40, 50, 60, 70, 80))
n = asm_sum_bytes(len(b), b)
print(b, n)

b = b"\x01\x02\x03\x04"
n = asm_sum_bytes(len(b), b)
print(b, n)
