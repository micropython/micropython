# test bcc instructions, narrow and wide versions


RESULT = []


TEMPLATE = """
@micropython.asm_thumb
def t(r0):
    mov(r1, r0)

    mov(r0, 10)
    cmp(r1, 1)
    b{}(next1)

    b(end)

    label(next1)

    mov(r0, 20)
    cmp(r1, 2)
    b{}_n(next2)

    b(end)

    label(next2)
    mov(r0, 30)
    cmp(r1, 3)
    b{}_w(next3)

    b(end)

    label(next3)
    mov(r0, 0)

    label(end)
"""

try:
    for code in (
        "eq",
        "ne",
        "cs",
        "cc",
        "mi",
        "pl",
        "vs",
        "vc",
        "hi",
        "ls",
        "ge",
        "lt",
        "gt",
        "le",
    ):
        exec(TEMPLATE.format(code, code, code))
        RESULT.append("B" + code.upper())
        for i in range(4):
            RESULT.append(t(i))
except MemoryError:
    print("SKIP-TOO-LARGE")
    raise SystemExit

for line in RESULT:
    print(line)
