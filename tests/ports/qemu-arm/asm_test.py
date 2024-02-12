import frozen_asm

print(frozen_asm.asm_add(1, 2))
print(frozen_asm.asm_add1(3))
print(frozen_asm.asm_cast_bool(0), frozen_asm.asm_cast_bool(3))
print(frozen_asm.asm_shift_int(4))
print(frozen_asm.asm_shift_uint(4))
