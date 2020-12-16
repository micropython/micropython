# test constants in assembler

@micropython.asm_thumb
def c1():
    movwt(r0, 0xffffffff)
    movwt(r1, 0xf0000000)
    sub(r0, r0, r1)
print(hex(c1()))
