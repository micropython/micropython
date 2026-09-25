# ruff: noqa: F821 - @asm_aarch64 decorator adds names to function scope

# Regression test: pop() must be the exact inverse of push() for register
# lists of every length.  asm_aarch64_pop() used to emit the final pair as
# LDP (post-indexed), which always loads from [sp + 0] -- so for lists with
# three or more registers the last pair re-read slot 0 instead of its own
# slot.  Each function below returns the sum of the restored registers, so a
# mis-restored register changes the printed value.

import micropython


# Three registers -> two stack slots pairs, exercises the multi-pair path.
@micropython.asm_aarch64
def f(x0) -> int:
    mov(x1, 111)
    mov(x2, 222)
    mov(x3, 333)
    push({x1, x2, x3})
    mov(x1, 0)
    mov(x2, 0)
    mov(x3, 0)
    pop({x1, x2, x3})
    add(x0, x1, x2)
    add(x0, x0, x3)


# Four registers -> two full pairs.
@micropython.asm_aarch64
def g(x0) -> int:
    mov(x1, 10)
    mov(x2, 20)
    mov(x3, 30)
    mov(x4, 40)
    push({x1, x2, x3, x4})
    mov(x1, 0)
    mov(x2, 0)
    mov(x3, 0)
    mov(x4, 0)
    pop({x1, x2, x3, x4})
    add(x0, x1, x2)
    add(x0, x0, x3)
    add(x0, x0, x4)


# Five registers -> odd count, so the last slot is XZR padding.
@micropython.asm_aarch64
def h(x0) -> int:
    mov(x1, 1)
    mov(x2, 2)
    mov(x3, 4)
    mov(x4, 8)
    mov(x5, 16)
    push({x1, x2, x3, x4, x5})
    mov(x1, 0)
    mov(x2, 0)
    mov(x3, 0)
    mov(x4, 0)
    mov(x5, 0)
    pop({x1, x2, x3, x4, x5})
    add(x0, x1, x2)
    add(x0, x0, x3)
    add(x0, x0, x4)
    add(x0, x0, x5)


# Control: one and two registers always took the correct single-pair path.
@micropython.asm_aarch64
def one(x0) -> int:
    mov(x1, 7)
    push({x1})
    mov(x1, 0)
    pop({x1})
    mov(x0, x1)


@micropython.asm_aarch64
def two(x0) -> int:
    mov(x1, 7)
    mov(x2, 9)
    push({x1, x2})
    mov(x1, 0)
    mov(x2, 0)
    pop({x1, x2})
    add(x0, x1, x2)


# push/pop must also leave sp exactly where it was, otherwise the generated
# epilogue restores x19-x28/fp/lr from the wrong addresses.
@micropython.asm_aarch64
def sp_balanced(x0) -> int:
    mov(x1, 1)
    mov(x2, 2)
    mov(x3, 3)
    mov(x4, 4)
    mov(x5, 5)
    push({x1, x2, x3, x4, x5})
    pop({x1, x2, x3, x4, x5})
    mov(x0, 42)


print(f(0))
print(g(0))
print(h(0))
print(one(0))
print(two(0))
print(sp_balanced(0))
