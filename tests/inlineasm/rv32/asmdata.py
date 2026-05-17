# test the "data" directive


@micropython.asm_rv32
def ret_num(a0) -> uint:
    slli(a0, a0, 2)
    addi(a0, a0, 16)
    auipc(a1, 0)
    add(a1, a1, a0)
    lw(a0, 0(a1))
    jal(zero, HERE)
    data(4, 0x12345678, 0x20000000, 0x40000000, 0x7FFFFFFF + 1, (1 << 32) - 2)
    label(HERE)


for i in range(5):
    print(hex(ret_num(i)))


@micropython.asm_rv32
def ret_num_la(a0) -> uint:
    slli(a0, a0, 2)
    la(a1, DATA)
    add(a1, a1, a0)
    lw(a0, 0(a1))
    jal(zero, HERE)
    label(DATA)
    data(4, 0x12345678, 0x20000000, 0x40000000, 0x7FFFFFFF + 1, (1 << 32) - 2)
    label(HERE)


for i in range(5):
    print(hex(ret_num_la(i)))
