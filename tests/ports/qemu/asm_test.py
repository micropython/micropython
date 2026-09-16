import sys

# Boards whose target has no .mpy architecture ID (AArch64 today) cannot freeze
# an inline assembler module, so this test does not apply to them.  Every other
# board is expected to freeze one, so an ImportError there is still reported as
# a failure.
NATIVE_MPY_ARCH = (getattr(sys.implementation, "_mpy", 0) >> 10) & 0x0F
#
# Note: do not use a bare "raise" here.  These tests are also compiled by
# mpy-cross with "-X emit=native" for the ports/qemu "test_full" CI jobs, and
# the native emitter only supports "raise <expr>" -- a bare re-raise makes
# mpy-cross fail with NotImplementedError("native raise"), which run-tests.py
# then reports as "mpy-cross crash".

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
