@micropython.asm_thumb
def lsl1(r0):
    lsl(r0, r0, 1)
print(hex(lsl1(0x123)))

@micropython.asm_thumb
def lsl23(r0):
    lsl(r0, r0, 23)
print(hex(lsl23(1)))

@micropython.asm_thumb
def lsr1(r0):
    lsr(r0, r0, 1)
print(hex(lsr1(0x123)))

@micropython.asm_thumb
def lsr31(r0):
    lsr(r0, r0, 31)
print(hex(lsr31(0x80000000)))

@micropython.asm_thumb
def asr1(r0):
    asr(r0, r0, 1)
print(hex(asr1(0x123)))

@micropython.asm_thumb
def asr31(r0):
    asr(r0, r0, 31)
print(hex(asr31(0x80000000)))
