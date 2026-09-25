# ruff: noqa: F821 - @asm_aarch64 decorator adds names to function scope

# Boundary tests for push/pop.
#
# asm_aarch64_pop() used to emit its final pair as LDP (post-indexed).  That
# form always loads from [sp + 0] -- its imm7 field is the writeback amount, not
# a load displacement -- so with two or more pairs the last pair re-read slot 0.
# sp stayed balanced, which made the corruption silent.
#
# Every function below therefore *observes the values restored by pop()*: it
# sets x1..xN to 1..N, pushes, zeroes them, pops, and returns the sum.  The
# expected result is N*(N+1)/2, and any mis-restored register changes it.
#
# Note that x19-x28 cannot be used for this: the generated epilogue restores
# them from the stack frame, hiding a bad pop.  The "callee_saved_sp" case uses
# them anyway, to check that sp is left balanced.

import micropython


# --- num_pairs == 1 (1 and 2 registers): always took the correct path ---


@micropython.asm_aarch64
def one(x0) -> int:
    mov(x1, 1)
    push({x1})
    mov(x1, 0)
    pop({x1})
    mov(x0, x1)


@micropython.asm_aarch64
def two(x0) -> int:
    mov(x1, 1)
    mov(x2, 2)
    push({x1, x2})
    mov(x1, 0)
    mov(x2, 0)
    pop({x1, x2})
    add(x0, x1, x2)


# --- num_pairs >= 2: the previously broken cases ---
# Odd counts also exercise the XZR padding slot that keeps sp 16-byte aligned.


@micropython.asm_aarch64
def three(x0) -> int:  # 3 regs -> 4 slots, 2 pairs (odd, padded)
    mov(x1, 1)
    mov(x2, 2)
    mov(x3, 3)
    push({x1, x2, x3})
    mov(x1, 0)
    mov(x2, 0)
    mov(x3, 0)
    pop({x1, x2, x3})
    add(x0, x1, x2)
    add(x0, x0, x3)


@micropython.asm_aarch64
def four(x0) -> int:  # 4 regs -> 2 pairs
    mov(x1, 1)
    mov(x2, 2)
    mov(x3, 3)
    mov(x4, 4)
    push({x1, x2, x3, x4})
    mov(x1, 0)
    mov(x2, 0)
    mov(x3, 0)
    mov(x4, 0)
    pop({x1, x2, x3, x4})
    add(x0, x1, x2)
    add(x0, x0, x3)
    add(x0, x0, x4)


@micropython.asm_aarch64
def five(x0) -> int:  # 5 regs -> 6 slots, 3 pairs (odd, padded)
    mov(x1, 1)
    mov(x2, 2)
    mov(x3, 3)
    mov(x4, 4)
    mov(x5, 5)
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


@micropython.asm_aarch64
def six(x0) -> int:  # 6 regs -> 3 pairs
    mov(x1, 1)
    mov(x2, 2)
    mov(x3, 3)
    mov(x4, 4)
    mov(x5, 5)
    mov(x6, 6)
    push({x1, x2, x3, x4, x5, x6})
    mov(x1, 0)
    mov(x2, 0)
    mov(x3, 0)
    mov(x4, 0)
    mov(x5, 0)
    mov(x6, 0)
    pop({x1, x2, x3, x4, x5, x6})
    add(x0, x1, x2)
    add(x0, x0, x3)
    add(x0, x0, x4)
    add(x0, x0, x5)
    add(x0, x0, x6)


@micropython.asm_aarch64
def seven(x0) -> int:  # 7 regs -> 8 slots, 4 pairs (odd, padded)
    mov(x1, 1)
    mov(x2, 2)
    mov(x3, 3)
    mov(x4, 4)
    mov(x5, 5)
    mov(x6, 6)
    mov(x7, 7)
    push({x1, x2, x3, x4, x5, x6, x7})
    mov(x1, 0)
    mov(x2, 0)
    mov(x3, 0)
    mov(x4, 0)
    mov(x5, 0)
    mov(x6, 0)
    mov(x7, 0)
    pop({x1, x2, x3, x4, x5, x6, x7})
    add(x0, x1, x2)
    add(x0, x0, x3)
    add(x0, x0, x4)
    add(x0, x0, x5)
    add(x0, x0, x6)
    add(x0, x0, x7)


@micropython.asm_aarch64
def eight(x0) -> int:  # 8 regs -> 4 pairs
    mov(x1, 1)
    mov(x2, 2)
    mov(x3, 3)
    mov(x4, 4)
    mov(x5, 5)
    mov(x6, 6)
    mov(x7, 7)
    mov(x8, 8)
    push({x1, x2, x3, x4, x5, x6, x7, x8})
    mov(x1, 0)
    mov(x2, 0)
    mov(x3, 0)
    mov(x4, 0)
    mov(x5, 0)
    mov(x6, 0)
    mov(x7, 0)
    mov(x8, 0)
    pop({x1, x2, x3, x4, x5, x6, x7, x8})
    add(x0, x1, x2)
    add(x0, x0, x3)
    add(x0, x0, x4)
    add(x0, x0, x5)
    add(x0, x0, x6)
    add(x0, x0, x7)
    add(x0, x0, x8)


# --- non-contiguous register list: the set is sorted ascending, so the pairs
# --- are (x1,x3) and (x5,x7); a bad final pair shows up as a wrong sum ---


@micropython.asm_aarch64
def noncontig(x0) -> int:
    mov(x1, 1)
    mov(x3, 3)
    mov(x5, 5)
    mov(x7, 7)
    push({x7, x1, x5, x3})
    mov(x1, 0)
    mov(x3, 0)
    mov(x5, 0)
    mov(x7, 0)
    pop({x3, x7, x1, x5})
    add(x0, x1, x3)
    add(x0, x0, x5)
    add(x0, x0, x7)


# --- nested push/pop, different arities at each level ---


@micropython.asm_aarch64
def nested(x0) -> int:
    mov(x1, 1)
    mov(x2, 2)
    mov(x3, 3)
    push({x1, x2, x3})
    mov(x4, 40)
    mov(x5, 50)
    push({x4, x5})
    mov(x4, 0)
    mov(x5, 0)
    mov(x1, 0)
    mov(x2, 0)
    mov(x3, 0)
    pop({x4, x5})
    pop({x1, x2, x3})
    add(x0, x1, x2)
    add(x0, x0, x3)
    add(x0, x0, x4)
    add(x0, x0, x5)


# --- sp must be left exactly where it was.  If it is not, the generated
# --- epilogue restores x19-x28/fp/lr from the wrong slots and the call
# --- returns garbage or crashes, so returning a constant is the assertion. ---


@micropython.asm_aarch64
def callee_saved_sp(x0) -> int:
    push({x19, x20, x21, x22, x23, x24, x25, x26})
    pop({x19, x20, x21, x22, x23, x24, x25, x26})
    push({x19, x20, x21, x22, x23})
    pop({x19, x20, x21, x22, x23})
    mov(x0, 42)


print(one(0))
print(two(0))
print(three(0))
print(four(0))
print(five(0))
print(six(0))
print(seven(0))
print(eight(0))
print(noncontig(0))
print(nested(0))
print(callee_saved_sp(0))
