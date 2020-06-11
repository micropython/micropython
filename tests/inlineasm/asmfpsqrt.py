# test vsqrt, vneg
@micropython.asm_thumb  # r0 = -(int)(sqrt(r0)*r1)
def sqrt_test(r0, r1):
    vmov(s1, r0)
    vcvt_f32_s32(s1, s1)
    vsqrt(s1, s1)
    vmov(s2, r1)
    vcvt_f32_s32(s2, s2)
    vmul(s0, s1, s2)
    vneg(s7, s0)
    vcvt_s32_f32(s31, s7)
    vmov(r0, s31)


print(sqrt_test(256, 10))
