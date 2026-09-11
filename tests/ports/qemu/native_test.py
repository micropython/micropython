import sys

# Boards without an .mpy architecture ID (AArch64 today) have no frozen
# native module; other boards must have one, so ImportError = failure there.
NATIVE_MPY_ARCH = (getattr(sys.implementation, "_mpy", 0) >> 10) & 0x0F
#
# No bare "raise" here: these tests are also compiled with "-X emit=native"
# for the test_full CI jobs, and the native emitter needs "raise <expr>".

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
