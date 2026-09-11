# Regression test: a native/viper function whose stack frame does not fit the
# 12-bit immediate of SUB (immediate) must still get its frame allocated.
#
# asm_aarch64_entry_common() used to fall back to
#     mov x11, #stack_adjust
#     sub sp, sp, x11
# once stack_adjust >= 4096.  But in ADD/SUB (shifted register) the Rn and Rd
# fields select XZR -- not SP -- when they are 11111, so that pair actually
# encodes as "sub xzr, xzr, x11" and never moves sp.  The frame was therefore
# not allocated, and every local was written on top of the saved fp/lr and
# callee-saved registers instead of into fresh stack space.
#
# The threshold is 511 locals: (511 * 8 + 15) & ~15 == 4096.
# viper_many_locals.py only *compiles* such a function, which is why it did not
# catch this; the functions here are actually called.


def make(n_locals):
    src = "@micropython.viper\ndef f() -> int:\n"
    for i in range(n_locals):
        src += "    v%d = %d\n" % (i, i + 1)
    # v0 is the first local and v(n-1) the last, so this reads both ends of the
    # frame; with no frame allocated they alias the saved registers.
    src += "    return v0 + v%d\n" % (n_locals - 1)
    return src


# Probe with the largest function first.  Some native emitters cap the number of
# locals in a native method, and some targets do not have enough heap to compile
# a function this large.  If the probe fails, skip the whole test with a single
# "SKIP" line so that the output is identical on every target (per-run SKIPs
# would emit a target-dependent number of lines and fail the .exp comparison).
# The probe compiles but does not call the function, so it does not need the
# large C stack frame that calling it would require.
try:
    exec(make(1000), {})
except (MemoryError, NotImplementedError):
    print("SKIP")
    raise SystemExit


def run(n_locals):
    g = {}
    exec(make(n_locals), g)
    got = g["f"]()
    print(n_locals, got == n_locals + 1)


# 510 locals -> 4080 byte frame: still a single "sub sp, sp, #imm"
run(510)
# 511 locals -> 4096 byte frame: the first size that cannot use imm12
run(511)
# 512 locals -> 4096 byte frame (same size, different local count)
run(512)
# 520 locals -> 4160 byte frame, matching viper_many_locals.py
run(520)
# 1000 locals -> 8000 byte frame: high part plus a non-zero low part
run(1000)
