# Regression test: a native/viper function whose frame exceeds the 12-bit SUB
# immediate must still get its frame allocated.  asm_aarch64_entry() used to
# fall back to "sub sp, sp, x11", but R31 in SUB (shifted register) is XZR,
# not SP, so sp never moved and the locals overwrote the saved registers.
#
# The threshold is 511 locals: (511 * 8 + 15) & ~15 == 4096.  Unlike
# viper_many_locals.py (compile only), the functions here are actually called.


def make(n_locals):
    src = "@micropython.viper\ndef f() -> int:\n"
    for i in range(n_locals):
        src += "    v%d = %d\n" % (i, i + 1)
    # Read both ends of the frame; with no frame allocated they alias the
    # saved registers.
    src += "    return v0 + v%d\n" % (n_locals - 1)
    return src


# Probe with the largest function first: emitters may cap the locals count or
# lack the heap for it.  On failure, skip the whole test with one "SKIP" line
# so the output is identical on every target.  The probe only compiles, so it
# does not need the large C stack frame.
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
