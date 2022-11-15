# Test freezing inline-asm code.

import micropython


@micropython.asm_thumb
def asm_add(r0, r1):
    add(r0, r0, r1)


@micropython.asm_thumb
def asm_add1(r0) -> object:
    lsl(r0, r0, 1)
    add(r0, r0, 3)


@micropython.asm_thumb
def asm_cast_bool(r0) -> bool:
    pass


@micropython.asm_thumb
def asm_shift_int(r0) -> int:
    lsl(r0, r0, 29)


@micropython.asm_thumb
def asm_shift_uint(r0) -> uint:
    lsl(r0, r0, 29)
