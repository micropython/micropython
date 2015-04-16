@micropython.asm_thumb      # test vcmp, vmrs
def comptest():
    mov(r0, 5)
    vmov(s0, r0)
    vcvt_f32_s32(s0, s0)
    mov(r0, 0)
    vmov(s1, r0)
    vcvt_f32_s32(s1, s1)
    mov(r0, 1)
    vmov(s2, r0)
    vcvt_f32_s32(s2, s2)
    label(LOOP)
    vadd(s1, s1, s2)
    vcmp(s1, s0)
    vmrs()
    ble(LOOP)
    vcvt_s32_f32(s31, s1)
    vmov(r0, s31) # should return 6

print(comptest())

