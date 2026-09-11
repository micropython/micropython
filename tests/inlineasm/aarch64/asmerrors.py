# ruff: noqa: F821

# Test that SyntaxError is raised for invalid AArch64 inline assembly.
# These errors occur at compile time when the @asm_aarch64 decorator
# processes the function body, so they cannot be caught with try/except
# at runtime. Instead we verify the error messages via .exp file.

# 1. Too many parameters (max 4: x0-x3)
try:
    exec("@micropython.asm_aarch64\ndef f(x0, x1, x2, x3, x4):\n    add(x0, x0, x1)\n")
except SyntaxError:
    print("SyntaxError: too many params")

# 2. Wrong parameter order
try:
    exec("@micropython.asm_aarch64\ndef f(x0, x2):\n    add(x0, x0, x2)\n")
except SyntaxError:
    print("SyntaxError: wrong param order")

# 3. Unknown instruction
try:
    exec("@micropython.asm_aarch64\ndef f(x0):\n    fake_instr(x0, x0)\n")
except SyntaxError:
    print("SyntaxError: unknown op")

# 4. Wrong argument count
try:
    exec("@micropython.asm_aarch64\ndef f(x0):\n    add(x0, x0)\n")
except SyntaxError:
    print("SyntaxError: wrong arg count")

# 5. Bad register name
try:
    exec("@micropython.asm_aarch64\ndef f(x0):\n    mov(x0, y0)\n")
except SyntaxError:
    print("SyntaxError: bad register")

# 6. Non-integer where integer expected
try:
    exec('@micropython.asm_aarch64\ndef f(x0):\n    mov(x0, "not_an_int")\n')
except SyntaxError:
    print("SyntaxError: not integer")

# 7. Register out of range
try:
    exec("@micropython.asm_aarch64\ndef f(x0):\n    mov(x0, x31)\n")
except SyntaxError:
    print("SyntaxError: register out of range")

# 8. Integer out of range for shift
try:
    exec("@micropython.asm_aarch64\ndef f(x0):\n    lsl(x0, x0, 64)\n")
except SyntaxError:
    print("SyntaxError: shift out of range")

# 9. Bad address format (not [base, offset])
try:
    exec("@micropython.asm_aarch64\ndef f(x0):\n    ldr(x0, x1)\n")
except SyntaxError:
    print("SyntaxError: bad address")

# 10. Wrong number of elements in address
try:
    exec("@micropython.asm_aarch64\ndef f(x0):\n    ldr(x0, [x1])\n")
except SyntaxError:
    print("SyntaxError: wrong address count")

# 11. Unknown 1-arg instruction
try:
    exec("@micropython.asm_aarch64\ndef f(x0):\n    bxyz(x0)\n")
except SyntaxError:
    print("SyntaxError: unknown 1-arg op")

# 12. Unknown 2-arg instruction
try:
    exec("@micropython.asm_aarch64\ndef f(x0, x1):\n    xyz(x0, x1)\n")
except SyntaxError:
    print("SyntaxError: unknown 2-arg op")

# 13. Unknown 3-arg instruction
try:
    exec("@micropython.asm_aarch64\ndef f(x0, x1):\n    xyz(x0, x1, x0)\n")
except SyntaxError:
    print("SyntaxError: unknown 3-arg op")

# 14. Too many arguments (4+)
try:
    exec("@micropython.asm_aarch64\ndef f(x0, x1):\n    add(x0, x1, x0, x1)\n")
except SyntaxError:
    print("SyntaxError: too many args")

# 15. Register exceeds max_reg (sp = reg 31, max is x30)
try:
    exec("@micropython.asm_aarch64\ndef f(x0):\n    mov(x0, sp)\n")
except SyntaxError:
    print("SyntaxError: reg exceeds max")

# 16. Non-ID parameter (*x0)
try:
    exec("@micropython.asm_aarch64\ndef f(*x0):\n    mov(x0, x0)\n")
except SyntaxError:
    print("SyntaxError: param not id")

# 17. Non-identifier label argument (with label defined to avoid assert)
try:
    exec("@micropython.asm_aarch64\ndef f(x0):\n    label(1)\n    b(123)\n")
except SyntaxError:
    print("SyntaxError: label not id")

# 18. Undefined label (label defined first to avoid assert)
try:
    exec("@micropython.asm_aarch64\ndef f(x0):\n    label(1)\n    b(NOT_DEF)\n")
except SyntaxError:
    print("SyntaxError: label undefined")
