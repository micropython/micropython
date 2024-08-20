# test return type of inline asm


@micropython.asm_thumb
def ret_obj(r0) -> object:
    pass


ret_obj(print)(1)


@micropython.asm_thumb
def ret_bool(r0) -> bool:
    pass


print(ret_bool(0), ret_bool(1))


@micropython.asm_thumb
def ret_int(r0) -> int:
    lsl(r0, r0, 29)


print(ret_int(0), hex(ret_int(1)), hex(ret_int(2)), hex(ret_int(4)))


@micropython.asm_thumb
def ret_uint(r0) -> uint:
    lsl(r0, r0, 29)


print(ret_uint(0), hex(ret_uint(1)), hex(ret_uint(2)), hex(ret_uint(4)))
