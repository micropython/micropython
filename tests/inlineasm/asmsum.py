@micropython.asm_thumb
def asm_sum_words(r0, r1):

    # r0 = len
    # r1 = ptr
    # r2 = sum
    # r3 = dummy
    mov(r2, 0)

    b(loop_entry)

    label(loop1)
    ldr(r3, [r1, 0])
    add(r2, r2, r3)

    add(r1, r1, 4)
    sub(r0, r0, 1)

    label(loop_entry)
    cmp(r0, 0)
    bgt(loop1)

    mov(r0, r2)

@micropython.asm_thumb
def asm_sum_bytes(r0, r1):

    # r0 = len
    # r1 = ptr
    # r2 = sum
    # r3 = dummy
    mov(r2, 0)

    b(loop_entry)

    label(loop1)
    ldrb(r3, [r1, 0])
    add(r2, r2, r3)

    add(r1, r1, 1)
    sub(r0, r0, 1)

    label(loop_entry)
    cmp(r0, 0)
    bgt(loop1)

    mov(r0, r2)

import array

b = array.array('l', (100, 200, 300, 400))
n = asm_sum_words(len(b), b)
print(b, n)

b = array.array('b', (10, 20, 30, 40, 50, 60, 70, 80))
n = asm_sum_bytes(len(b), b)
print(b, n)

b = b'\x01\x02\x03\x04'
n = asm_sum_bytes(len(b), b)
print(b, n)
