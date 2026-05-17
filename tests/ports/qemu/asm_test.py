try:
    import frozen_asm_thumb as frozen_asm
except ImportError:
    try:
        import frozen_asm_rv32 as frozen_asm
    except ImportError:
        raise ImportError

print(frozen_asm.asm_add(1, 2))
print(frozen_asm.asm_add1(3))
print(frozen_asm.asm_cast_bool(0), frozen_asm.asm_cast_bool(3))
print(frozen_asm.asm_shift_int(4))
print(frozen_asm.asm_shift_uint(4))
