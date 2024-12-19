@micropython.asm_thumb  # r0 = r0+r1-r2
def add_sub(r0, r1, r2):
    vmov(s0, r0)
    vcvt_f32_s32(s0, s0)
    vmov(s1, r1)
    vcvt_f32_s32(s1, s1)
    vmov(s2, r2)
    vcvt_f32_s32(s2, s2)
    vadd(s0, s0, s1)
    vsub(s0, s0, s2)
    vcvt_s32_f32(s31, s0)
    vmov(r0, s31)


print(add_sub(100, 20, 30))
