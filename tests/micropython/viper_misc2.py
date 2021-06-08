# Miscellaneous viper tests

# Test correct use of registers in load and store
@micropython.viper
def expand(dest: ptr8, source: ptr8, length: int):
    n = 0
    for x in range(0, length, 2):
        c = source[x]
        d = source[x + 1]
        dest[n] = (c & 0xE0) | ((c & 0x1C) >> 1)
        n += 1
        dest[n] = ((c & 3) << 6) | ((d & 0xE0) >> 4)
        n += 1
        dest[n] = ((d & 0x1C) << 3) | ((d & 3) << 2)
        n += 1


source = b"\xaa\xaa\xff\xff"
dest = bytearray(len(source) // 2 * 3)
expand(dest, source, len(source))
print(dest)
