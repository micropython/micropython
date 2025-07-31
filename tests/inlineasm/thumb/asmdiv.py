@micropython.asm_thumb
def sdiv(r0, r1):
    sdiv(r0, r0, r1)


@micropython.asm_thumb
def udiv(r0, r1):
    udiv(r0, r0, r1)


print(sdiv(1234, 3))
print(sdiv(-1234, 3))
print(sdiv(1234, -3))
print(sdiv(-1234, -3))

print(udiv(1234, 3))
print(udiv(0xFFFFFFFF, 0x7FFFFFFF))
print(udiv(0xFFFFFFFF, 0xFFFFFFFF))
