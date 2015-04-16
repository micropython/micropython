@micropython.asm_thumb      # r0 = (int)(r0*r1/r2)
def muldiv(r0, r1, r2):
    vmov(s0, r0)
    vcvt_f32_s32(s0, s0)
    vmov(s1, r1)
    vcvt_f32_s32(s1, s1)
    vmov(s2, r2)
    vcvt_f32_s32(s2, s2)
    vmul(s7, s0, s1)
    vdiv(s8, s7, s2)
    vcvt_s32_f32(s31, s8)
    vmov(r0, s31)

print(muldiv(100, 10, 50))

