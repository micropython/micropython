# Exercise addressing a deep value-stack slot in a native function: with ~530
# pending tuple items, the trailing inner(1) call builds a frame address
# beyond the 12-bit scaled ADD immediate -- on AArch64 the shifted-ADD-pair
# path of asm_aarch64_mov_reg_local_addr().  inner() needs >=1 argument:
# zero-arg calls skip stack-pointer materialisation.  All targets (including
# bytecode fallback) print the same tuple length.

NUM_ITEMS = 530  # deep enough that the slot address exceeds 4095 bytes


def make(n):
    src = "@micropython.native\ndef inner(x):\n    return x\n\n"
    src += "@micropython.native\ndef f():\n"
    src += "    return len((" + ", ".join(str(i) for i in range(n)) + ", inner(1)))\n"
    return src


try:
    exec(make(NUM_ITEMS))
except (MemoryError, NotImplementedError):
    # Some targets do not have enough heap to compile a function this large.
    print("SKIP")
    raise SystemExit

print(f())
