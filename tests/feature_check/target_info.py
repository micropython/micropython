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
][sys_mpy >> 10]
print(platform, arch)
