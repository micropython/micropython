# test large constant offsets for ptr8/ptr16/ptr32 in viper mode
# these trigger the "else" branch in asm_aarch64 load/store offset functions
# where the offset is too large for the immediate field


@micropython.viper
def load8_large(src: ptr8) -> int:
    return src[4096]


@micropython.viper
def load8_index(src: ptr8, i: int) -> int:
    return src[i]


@micropython.viper
def store8_large(dest: ptr8, val: int):
    dest[4096] = val


@micropython.viper
def store8_index(dest: ptr8, i: int, val: int):
    dest[i] = val


@micropython.viper
def load16_large(src: ptr16) -> int:
    return src[4096]


@micropython.viper
def load16_index(src: ptr16, i: int) -> int:
    return src[i]


@micropython.viper
def store16_large(dest: ptr16, val: int):
    dest[4096] = val


@micropython.viper
def store16_index(dest: ptr16, i: int, val: int):
    dest[i] = val


@micropython.viper
def load32_large(src: ptr32) -> int:
    return src[4096]


@micropython.viper
def load32_index(src: ptr32, i: int) -> int:
    return src[i]


@micropython.viper
def store32_large(dest: ptr32, val: int):
    dest[4096] = val


@micropython.viper
def store32_index(dest: ptr32, i: int, val: int):
    dest[i] = val


buf8 = bytearray(4096 + 16)
for i in range(len(buf8)):
    buf8[i] = i & 0xFF

print(hex(load8_large(buf8)))
print(hex(load8_index(buf8, 4096)))
store8_large(buf8, 0xAB)
print(hex(buf8[4096]))
store8_index(buf8, 4097, 0xCD)
print(hex(buf8[4097]))

buf16 = bytearray(4096 * 2 + 16)
for i in range(len(buf16)):
    buf16[i] = i & 0xFF

print(hex(load16_large(buf16)))
print(hex(load16_index(buf16, 4096)))
store16_large(buf16, 0xBEEF)
print(hex(buf16[4096 * 2]), hex(buf16[4096 * 2 + 1]))
store16_index(buf16, 4097, 0xDEAD)
print(hex(buf16[4097 * 2]), hex(buf16[4097 * 2 + 1]))

buf32 = bytearray(4096 * 4 + 16)
for i in range(len(buf32)):
    buf32[i] = i & 0xFF

print(hex(load32_large(buf32)))
print(hex(load32_index(buf32, 4096)))
store32_large(buf32, 0xDEADBEEF)
print(hex(buf32[4096 * 4]), hex(buf32[4096 * 4 + 1]), hex(buf32[4096 * 4 + 2]), hex(buf32[4096 * 4 + 3]))
store32_index(buf32, 4097, 0xCAFEBABE)
print(hex(buf32[4097 * 4]), hex(buf32[4097 * 4 + 1]), hex(buf32[4097 * 4 + 2]), hex(buf32[4097 * 4 + 3]))
