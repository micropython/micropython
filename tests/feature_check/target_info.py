# Retrieve the native architecture of the target.
# See https://docs.micropython.org/en/latest/reference/mpyfiles.html#versioning-and-compatibility-of-mpy-files
# for more details.

import sys

platform = getattr(sys, "platform", "minimal")
sys_mpy = getattr(sys.implementation, "_mpy", 0)
arch = [
    None,
    "x86",
    "x64",
    "armv6",
    "armv6m",
    "armv7m",
    "armv7em",
    "armv7emsp",
    "armv7emdp",
    "xtensa",
    "xtensawin",
    "rv32imc",
    "rv64imc",
][(sys_mpy >> 10) & 0x0F]
arch_flags = sys_mpy >> 16
build = getattr(sys.implementation, "_build", "unknown")
thread = getattr(sys.implementation, "_thread", None)

# Detect how many bits of precision the floating point implementation has.
try:
    if float("1.0000001") == float("1.0"):
        float_prec = 30
    elif float("1e300") == float("inf"):
        float_prec = 32
    else:
        float_prec = 64
except NameError:
    float_prec = 0

print(platform, arch, arch_flags, build, thread, float_prec, len("α") == 1)
