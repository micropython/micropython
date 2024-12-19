# test the "data" directive


@micropython.asm_thumb
def ret_num(r0) -> uint:
    lsl(r0, r0, 2)
    mov(r1, pc)
    add(r0, r0, r1)
    ldr(r0, [r0, 4])
    b(HERE)
    data(4, 0x12345678, 0x20000000, 0x40000000, 0x7FFFFFFF + 1, (1 << 32) - 2)
    label(HERE)


for i in range(5):
    print(hex(ret_num(i)))
