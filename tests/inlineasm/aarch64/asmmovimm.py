# test mov immediate encodings: halfword placement for MOVZ/MOVN/MOVK
#
# A 64-bit immediate is built from 16-bit halfwords; these tests exercise
# each halfword position of the MOVZ, MOVN and MOVK encodings, and the
# MOVK sequence with a zero halfword in the middle.


@micropython.asm_aarch64
def movz_hw1():
    # single non-zero halfword at position 1
    mov(x0, 0x12340000)


print(hex(movz_hw1()))


@micropython.asm_aarch64
def movz_hw2():
    # single non-zero halfword at position 2
    mov(x0, 0x123400000000)


print(hex(movz_hw2()))


@micropython.asm_aarch64
def movz_hw3():
    # single non-zero halfword at position 3
    mov(x0, 0x1234000000000000)


print(hex(movz_hw3()))


@micropython.asm_aarch64
def movz_hw3_msb():
    # only the most significant bit set: MOVZ with hw=3, imm=0x8000
    mov(x0, 0x8000000000000000)


print(hex(movz_hw3_msb()))


@micropython.asm_aarch64
def movn_hw1():
    # inverse has a single non-zero halfword at position 1: MOVN with hw=1
    mov(x0, 0xFFFFFFFFEDCBFFFF)


print(hex(movn_hw1()))


@micropython.asm_aarch64
def movn_hw2():
    # inverse has a single non-zero halfword at position 2: MOVN with hw=2
    mov(x0, 0xFFFFEDCBFFFFFFFF)


print(hex(movn_hw2()))


@micropython.asm_aarch64
def movn_hw3():
    # inverse has a single non-zero halfword at position 3: MOVN with hw=3
    mov(x0, 0xEDCBFFFFFFFFFFFF)


print(hex(movn_hw3()))


@micropython.asm_aarch64
def movk_gap():
    # non-zero halfwords at positions 0 and 2, zero at position 1
    mov(x0, 0x567800001234)


print(hex(movk_gap()))


@micropython.asm_aarch64
def movk_full():
    # all four halfwords non-zero: MOVZ followed by three MOVK
    mov(x0, 0x0123456789ABCDEF)


print(hex(movk_full()))


@micropython.asm_aarch64
def movn_hw0():
    # inverse has a single non-zero halfword at position 0: MOVN with hw=0
    mov(x0, 0xFFFFFFFFFFFFFF01)


print(hex(movn_hw0()))


@micropython.asm_aarch64
def movz_base_hw1():
    # hw0 and hw2 are zero: the MOVZ base moves up to position 1
    mov(x0, 0xCDEF0000ABCD0000)


print(hex(movz_base_hw1()))


@micropython.asm_aarch64
def movz_base_hw2():
    # hw0 and hw1 are zero: the MOVZ base moves up to position 2
    mov(x0, 0xFFFFFFFF00000000)


print(hex(movz_base_hw2()))


@micropython.asm_aarch64
def movn_base():
    # two halfwords need patching either way, but only two of them are
    # non-zero vs two that are not 0xffff: MOVN base wins the tie-break
    # against the four non-zero halfwords (MOVN + MOVK instead of 4)
    mov(x0, 0xFFF0FFF0FFFFFFFF)


print(hex(movn_base()))


@micropython.asm_aarch64
def movn_base_movk2():
    # three halfwords are not 0xffff vs four non-zero: MOVN base plus two
    # MOVK, three instructions instead of four
    mov(x0, 0xAAAABBBBFFFFCCCC)


print(hex(movn_base_movk2()))
