# test 8/16/32/64 bit signed/unsigned integer arguments and return types for ffi functions
# requires ffi_lib.c to be compiled as: $(CC) -shared -o ffi_lib.so ffi_lib.c

import os, sys

try:
    import ffi
except ImportError:
    print("SKIP")
    raise SystemExit

ffi_lib_filename = "./" + sys.argv[0].rsplit("/", 1)[0] + "/ffi_lib.so"
try:
    os.stat(ffi_lib_filename)
except OSError:
    print("SKIP")
    raise SystemExit

ffi_lib = ffi.open(ffi_lib_filename)

f8i = ffi_lib.func("b", "f8i", "b")
f8u = ffi_lib.func("B", "f8u", "B")
f16i = ffi_lib.func("h", "f16i", "h")
f16u = ffi_lib.func("H", "f16u", "H")
f32i = ffi_lib.func("i", "f32i", "i")
f32u = ffi_lib.func("I", "f32u", "I")
f64i = ffi_lib.func("q", "f64i", "q")
f64u = ffi_lib.func("Q", "f64u", "Q")

for func_name in ("f8i", "f8u", "f16i", "f16u", "f32i", "f32u", "f64i", "f64u"):
    func = globals()[func_name]
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
        print("{}({:x}) = {:x}".format(func_name, val, func(val)))
