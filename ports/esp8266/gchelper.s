    .file   "gchelper.s"
    .text

    .align  4
    .global gc_helper_get_regs_and_sp
    .type   gc_helper_get_regs_and_sp, @function
gc_helper_get_regs_and_sp:
    # store regs into given array
    s32i.n  a8, a2, 0
    s32i.n  a9, a2, 4
    s32i.n  a10, a2, 8
    s32i.n  a11, a2, 12
    s32i.n  a12, a2, 16
    s32i.n  a13, a2, 20
    s32i.n  a14, a2, 24
    s32i.n  a15, a2, 28

    # return the sp
    mov     a2, a1
    ret.n

    .size   gc_helper_get_regs_and_sp, .-gc_helper_get_regs_and_sp
