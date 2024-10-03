import sys

arch = getattr(sys.implementation, "_mpy", 0) >> 10
if arch >= 3 and arch <= 8:
    import frozen_asm_thumb as frozen_asm
elif arch == 11:
    import frozen_asm_rv32imc as frozen_asm
else:
    raise ImportError


print(frozen_asm.asm_add(1, 2))
print(frozen_asm.asm_add1(3))
print(frozen_asm.asm_cast_bool(0), frozen_asm.asm_cast_bool(3))
print(frozen_asm.asm_shift_int(4))
print(frozen_asm.asm_shift_uint(4))
