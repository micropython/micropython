# test return type of inline asm


@micropython.asm_rv32
def ret_obj(a0) -> object:
    pass


ret_obj(print)(1)


@micropython.asm_rv32
def ret_bool(a0) -> bool:
    pass


print(ret_bool(0), ret_bool(1))


@micropython.asm_rv32
def ret_int(a0) -> int:
    slli(a0, a0, 29)


print(ret_int(0), hex(ret_int(1)), hex(ret_int(2)), hex(ret_int(4)))


@micropython.asm_rv32
def ret_uint(a0) -> uint:
    slli(a0, a0, 29)


print(ret_uint(0), hex(ret_uint(1)), hex(ret_uint(2)), hex(ret_uint(4)))
