# check if the inline assembler is enabled and enumerate its capabilities.

import sys

fields = []

mpy_arch = (getattr(sys.implementation, "_mpy", 0) >> 10) & 0x0F
if 8 >= mpy_arch >= 3:
    fields.append("thumb")
    if mpy_arch >= 7:
        fields.append("vfp")
elif 10 >= mpy_arch >= 9:
    fields.append("xtensa")
    if mpy_arch == 10:
        fields.append("windowed")
elif mpy_arch == 11:
    fields.append("rv32")
else:
    raise SystemExit

arch = fields[0]
has_asm = False
if arch == "thumb":
    try:
        exec("""
@micropython.asm_thumb
def f():
     add(r0, r0, r0)
""")
        has_asm = True
    except SyntaxError:
        pass
elif arch == "xtensa":
    try:
        exec("""
@micropython.asm_xtensa
def f():
    add(a0, a0, a0)
""")
        has_asm = True
    except SyntaxError:
        pass
elif arch == "rv32":
    try:
        exec("""
@micropython.asm_rv32
def f():
    add(a0, a0, a0)
""")
        has_asm = True
    except SyntaxError:
        pass
if not has_asm:
    raise SystemExit

if arch == "thumb":
    try:
        exec("""
@micropython.asm_thumb
def f():
    it(eq)
    nop()
""")
        fields.append("thumb2")
    except SyntaxError:
        pass
elif arch == "xtensa":
    pass
elif arch == "rv32":
    try:
        exec("""
@micropython.asm_rv32
def f():
    sh1add(a0, a0, a0)
""")
        fields.append("zba")
    except SyntaxError:
        pass
    try:
        exec("""
@micropython.asm_rv32
def f():
    cm_mva01s(s0, s1)
""")
        fields.append("zcmp")
    except SyntaxError:
        pass

print(",".join(fields))
