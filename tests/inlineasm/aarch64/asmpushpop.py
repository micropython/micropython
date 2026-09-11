# ruff: noqa: F821 - @asm_aarch64 decorator adds names to function scope

import micropython


@micropython.asm_aarch64
def f(x0, x1, x2) -> int:
    push({x0, x1, x2})
    add(x0, x0, x1)
    add(x0, x0, x2)
    pop({x1, x2, x3})


@micropython.asm_aarch64
def g() -> int:
    b(START)
    label(SUB)
    push({lr})
    mov(x0, 7)
    pop({lr})
    ret(lr)
    label(START)
    bl(SUB)


@micropython.asm_aarch64
def h(x0) -> int:
    push({x0, x1, x2, x3})
    mov(x0, 100)
    pop({x1, x2, x3, x4})


print(f(1, 2, 3))
print(g())
print(h(42))
