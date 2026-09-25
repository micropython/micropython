import sys

# Boards whose target has no .mpy architecture ID (AArch64 today, and any board
# without a native emitter) cannot freeze native/viper/inline-asm modules, so
# this test does not apply to them.  Every other board is expected to freeze
# the module, so an ImportError there is still reported as a failure.
NATIVE_MPY_ARCH = (getattr(sys.implementation, "_mpy", 0) >> 10) & 0x0F
#
# Note: do not use a bare "raise" here.  These tests are also compiled by
# mpy-cross with "-X emit=native" for the ports/qemu "test_full" CI jobs, and
# the native emitter only supports "raise <expr>" -- a bare re-raise makes
# mpy-cross fail with NotImplementedError("native raise"), which run-tests.py
# then reports as "mpy-cross crash".

try:
    import native_frozen_align
except ImportError:
    if NATIVE_MPY_ARCH != 0:
        raise ImportError("native_frozen_align is not frozen into this firmware")
    print("SKIP")
    raise SystemExit

native_frozen_align.native_x(1)
native_frozen_align.native_y(2)
native_frozen_align.native_z(3)
