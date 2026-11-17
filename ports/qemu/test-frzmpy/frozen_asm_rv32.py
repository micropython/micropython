# Test freezing inline-asm code.

# ruff: noqa: F821 - @asm_rv32 decorator adds names to function scope

import micropython


@micropython.asm_rv32
def asm_add(a0, a1):
    add(a0, a0, a1)


@micropython.asm_rv32
def asm_add1(a0) -> object:
    slli(a0, a0, 1)
    addi(a0, a0, 3)


@micropython.asm_rv32
def asm_cast_bool(a0) -> bool:
    pass


@micropython.asm_rv32
def asm_shift_int(a0) -> int:
    slli(a0, a0, 29)


@micropython.asm_rv32
def asm_shift_uint(a0) -> uint:
    slli(a0, a0, 29)
