# test 8/16/32/64 bit signed/unsigned integer arguments and return types for ffi functions

import ffi_int_base

ffi_int_base.test(
    [
        ("b", "f8i"),
        ("B", "f8u"),
        ("h", "f16i"),
        ("H", "f16u"),
        ("i", "f32i"),
        ("I", "f32u"),
        ("q", "f64i"),
        ("Q", "f64u"),
    ]
)
