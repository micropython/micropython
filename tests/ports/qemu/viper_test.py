import sys

# Boards without an .mpy architecture ID (AArch64 today) have no frozen
# native module; other boards must have one, so ImportError = failure there.
NATIVE_MPY_ARCH = (getattr(sys.implementation, "_mpy", 0) >> 10) & 0x0F
#
# No bare "raise" here: these tests are also compiled with "-X emit=native"
# for the test_full CI jobs, and the native emitter needs "raise <expr>".

try:
    import frozen_viper
except ImportError:
    if NATIVE_MPY_ARCH != 0:
        raise ImportError("frozen_viper is not frozen into this firmware")
    print("SKIP")
    raise SystemExit

frozen_viper.viper_add(1, 2)
