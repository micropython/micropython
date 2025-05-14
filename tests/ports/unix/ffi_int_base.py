# common tests for ffi_int_types/long32/long64
# requires ffi_lib.c to be compiled as: $(CC) -shared -o ffi_lib.so ffi_lib.c

import os, sys

try:
    import ffi
except ImportError:
    print("SKIP")
    raise SystemExit

ffi_lib_filename = sys.argv[0].rsplit("/", 1)[0] + "/ffi_lib.so"
try:
    os.stat(ffi_lib_filename)
except OSError:
    print("SKIP")
    raise SystemExit

ffi_lib = ffi.open(ffi_lib_filename)


def test(funcs):
    for type, name in funcs:
        func = ffi_lib.func(type, name, type)
        for val in (
            0,
            0x7F,
            0x80,
            0xFF,
            0x100,
            0x7FFF,
            0x8000,
            0xFFFF,
            0x10000,
            0x7FFFFFFF,
            0x80000000,
            0xFFFFFFFF,
            0x100000000,
            0x7FFF_FFFF_FFFF_FFFF,
            0x8000_0000_0000_0000,
            0xFFFF_FFFF_FFFF_FFFF,
            0x1_0000_0000_0000_0000,
        ):
            print("{}({:x}) = {:x}".format(name, val, func(val)))


if __name__ == "__main__":
    print("SKIP")
    raise SystemExit
