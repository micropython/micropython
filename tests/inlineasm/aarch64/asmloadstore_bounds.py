# ruff: noqa: F821 - @asm_aarch64 decorator adds names to function scope

# Boundary tests for constant load/store offsets.
#
# asm_aarch64_{ldr,ldrh,ldrb,str,strh,strb}_reg_reg_offset() have three tiers:
#   1. unsigned scaled imm12 -- offset in 0 .. 4095 * scale, correctly aligned
#   2. LDUR/STUR imm9        -- offset in -256 .. 255
#   3. mov x11, #offset, then the register-offset form
#
# Tier 3 clobbers x11, which inline assembler code is free to use, so the inline
# assembler must never reach it: get_arg_offset()/get_arg_i() clamp to tier 1's
# range.  The ASM_AARCH64_INLINE_ASM_MAX_OFFSET* macros in py/asmaarch64.h are
# the single source of truth for those bounds, and the MP_STATIC_ASSERTs in
# py/asmaarch64.c prove they stay inside tier 1.
#
# asmloadstore_large.py covers the in-range maximums; this file covers the
# rejection side of the boundary, plus direct evidence that tier 3 is not used.

import micropython

PREAMBLE = "@micropython.asm_aarch64\ndef f(x0, x1):\n"


def rejects(src, why):
    try:
        exec(PREAMBLE + src + "\n")
    except SyntaxError:
        print("SyntaxError:", why)
        return
    print("FAIL: no SyntaxError for", why)


# --- one past the maximum: must be rejected, never silently truncated ---
rejects("    ldr(x0, [x1, 32768])", "ldr past imm12 max")
rejects("    str(x0, [x1, 32768])", "str past imm12 max")
rejects("    ldrh(x0, [x1, 8192])", "ldrh past imm12 max")
rejects("    strh(x0, [x1, 8192])", "strh past imm12 max")
rejects("    ldrb(x0, [x1, 4096])", "ldrb past imm12 max")
rejects("    strb(x0, [x1, 4096])", "strb past imm12 max")

# --- misaligned offsets, including one just below the maximum ---
rejects("    ldr(x0, [x1, 4])", "ldr not a multiple of 8")
rejects("    ldr(x0, [x1, 32756])", "ldr near max not a multiple of 8")
rejects("    str(x0, [x1, 32756])", "str near max not a multiple of 8")
rejects("    ldrh(x0, [x1, 3])", "ldrh not a multiple of 2")
rejects("    strh(x0, [x1, 8189])", "strh near max not a multiple of 2")

# --- negative offsets: LDUR/STUR could encode these, but the inline assembler
# --- clamps the lower bound to 0 so that tier 3 stays unreachable ---
rejects("    ldr(x0, [x1, -8])", "negative ldr offset")
rejects("    ldrb(x0, [x1, -1])", "negative ldrb offset")
rejects("    str(x0, [x1, -256])", "negative str offset")

# --- direct evidence that tier 3 is not used: x11 must survive a maximal
# --- offset load and store of every width.  If the fallback had been reached,
# --- x11 would hold the offset instead of 7. ---


@micropython.asm_aarch64
def keeps_x11(x0, x1) -> int:
    mov(x11, 7)
    ldr(x0, [x1, 32760])
    ldrh(x0, [x1, 8190])
    ldrb(x0, [x1, 4095])
    str(x0, [x1, 0])
    strh(x0, [x1, 2])
    strb(x0, [x1, 4])
    mov(x0, x11)


import array

buf = array.array("q", [0] * 4097)
print("x11 preserved at max offsets:", keeps_x11(0, buf) == 7)
