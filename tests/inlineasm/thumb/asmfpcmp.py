@micropython.asm_thumb  # test vcmp, vmrs
def f(r0, r1):
    vmov(s0, r0)
    vcvt_f32_s32(s0, s0)
    vmov(s1, r1)
    vcvt_f32_s32(s1, s1)
    vcmp(s1, s0)
    vmrs(r0, FPSCR)
    mov(r1, 28)
    lsr(r0, r1)


print(f(0, 1))
print(f(1, 1))
print(f(1, 0))
