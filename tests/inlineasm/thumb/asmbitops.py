@micropython.asm_thumb
def clz(r0):
    clz(r0, r0)


print(clz(0xF0))
print(clz(0x8000))


@micropython.asm_thumb
def rbit(r0):
    rbit(r0, r0)


print(hex(rbit(0xF0)))
print(hex(rbit(0x8000)))
