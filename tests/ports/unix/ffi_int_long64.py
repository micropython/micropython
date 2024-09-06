# test 64-bit long arguments and return types for ffi functions

import struct

import ffi_int_base

if struct.calcsize("l") != 8:
    print("SKIP")
    raise SystemExit

ffi_int_base.test(
    [
        ("l", "fli"),
        ("L", "flu"),
    ]
)
