import array

# On the 8266 the generated code gets put into the IRAM segment, which is only
# word-addressable.  Therefore, to test byte and halfword load/store opcodes
# some memory must be reserved in the DRAM segment.

BYTE_DATA = array.array("B", (0x11, 0x22, 0x33, 0x44))
WORD_DATA = array.array("h", (100, 200, -100, -200))
DWORD_DATA = array.array("i", (100_000, -200_000, 300_000, -400_000))


@micropython.asm_xtensa
def tl32r() -> int:
    nop()
    j(CODE)
    align(4)
    label(DATA)
    data(1, 1, 2, 3, 4, 5, 6, 7)
    align(4)
    label(CODE)
    nop_n()
    nop_n()
    l32r(a2, DATA)


print(hex(tl32r()))


@micropython.asm_xtensa
def tl32i() -> uint:
    call0(ENTRY)
    label(ENTRY)
    l32i(a2, a0, 0)


print(hex(tl32i()))


@micropython.asm_xtensa
def tl8ui(a2) -> uint:
    mov(a3, a2)
    l8ui(a2, a3, 1)


print(hex(tl8ui(BYTE_DATA)))


@micropython.asm_xtensa
def tl16ui(a2) -> uint:
    mov(a3, a2)
    l16ui(a2, a3, 2)


print(tl16ui(WORD_DATA))


@micropython.asm_xtensa
def tl16si(a2) -> int:
    mov(a3, a2)
    l16si(a2, a3, 6)


print(tl16si(WORD_DATA))


@micropython.asm_xtensa
def ts8i(a2, a3):
    s8i(a3, a2, 1)


ts8i(BYTE_DATA, 0xFF)
print(BYTE_DATA)


@micropython.asm_xtensa
def ts16i(a2, a3):
    s16i(a3, a2, 2)


ts16i(WORD_DATA, -123)
print(WORD_DATA)


@micropython.asm_xtensa
def ts32i(a2, a3) -> uint:
    s32i(a3, a2, 4)


ts32i(DWORD_DATA, -123456)
print(DWORD_DATA)


@micropython.asm_xtensa
def tl32i_n(a2) -> uint:
    l32i_n(a2, a2, 8)


print(tl32i_n(DWORD_DATA))
