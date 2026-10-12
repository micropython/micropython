import sys

# Boards without an .mpy architecture ID (AArch64 today) have no frozen
# inline-asm module; other boards must have one, so ImportError = failure.
NATIVE_MPY_ARCH = (getattr(sys.implementation, "_mpy", 0) >> 10) & 0x0F
#
# No bare "raise" here: these tests are also compiled with "-X emit=native"
# for the test_full CI jobs, and the native emitter needs "raise <expr>".

try:
    import frozen_asm_thumb as frozen_asm
except ImportError:
    try:
        import frozen_asm_rv32 as frozen_asm
    except ImportError:
        if NATIVE_MPY_ARCH != 0:
            raise ImportError("no frozen inline assembler module")
        print("SKIP")
        raise SystemExit

print(frozen_asm.asm_add(1, 2))
print(frozen_asm.asm_add1(3))
print(frozen_asm.asm_cast_bool(0), frozen_asm.asm_cast_bool(3))
print(frozen_asm.asm_shift_int(4))
print(frozen_asm.asm_shift_uint(4))
