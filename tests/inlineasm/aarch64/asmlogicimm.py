# Exercise the logical (bitmask) immediate encoder over a wide range of
# values: ones that fit the single-instruction AND/ORR/EOR (immediate)
# encoding, and ones that do not and must be materialised in a scratch
# register first (x9, or x10 when x9 is the source operand).


@micropython.asm_aarch64
def a0(x0):
    and_(x0, x0, 0x1)


@micropython.asm_aarch64
def o0(x0):
    orr(x0, x0, 0x1)


@micropython.asm_aarch64
def e0(x0):
    eor(x0, x0, 0x1)


@micropython.asm_aarch64
def a1(x0):
    and_(x0, x0, 0xF)


@micropython.asm_aarch64
def o1(x0):
    orr(x0, x0, 0xF)


@micropython.asm_aarch64
def e1(x0):
    eor(x0, x0, 0xF)


@micropython.asm_aarch64
def a2(x0):
    and_(x0, x0, 0xFF)


@micropython.asm_aarch64
def o2(x0):
    orr(x0, x0, 0xFF)


@micropython.asm_aarch64
def e2(x0):
    eor(x0, x0, 0xFF)


@micropython.asm_aarch64
def a3(x0):
    and_(x0, x0, 0xFFFFFFFF)


@micropython.asm_aarch64
def o3(x0):
    orr(x0, x0, 0xFFFFFFFF)


@micropython.asm_aarch64
def e3(x0):
    eor(x0, x0, 0xFFFFFFFF)


@micropython.asm_aarch64
def a4(x0):
    and_(x0, x0, 0x7FFFFFFFFFFFFFFF)


@micropython.asm_aarch64
def o4(x0):
    orr(x0, x0, 0x7FFFFFFFFFFFFFFF)


@micropython.asm_aarch64
def e4(x0):
    eor(x0, x0, 0x7FFFFFFFFFFFFFFF)


@micropython.asm_aarch64
def a5(x0):
    and_(x0, x0, 0x8000000000000001)


@micropython.asm_aarch64
def o5(x0):
    orr(x0, x0, 0x8000000000000001)


@micropython.asm_aarch64
def e5(x0):
    eor(x0, x0, 0x8000000000000001)


@micropython.asm_aarch64
def a6(x0):
    and_(x0, x0, 0xF00000000000000F)


@micropython.asm_aarch64
def o6(x0):
    orr(x0, x0, 0xF00000000000000F)


@micropython.asm_aarch64
def e6(x0):
    eor(x0, x0, 0xF00000000000000F)


@micropython.asm_aarch64
def a7(x0):
    and_(x0, x0, 0xFF00FF00FF00FF)


@micropython.asm_aarch64
def o7(x0):
    orr(x0, x0, 0xFF00FF00FF00FF)


@micropython.asm_aarch64
def e7(x0):
    eor(x0, x0, 0xFF00FF00FF00FF)


@micropython.asm_aarch64
def a8(x0):
    and_(x0, x0, 0xF0F0F0F0F0F0F0F)


@micropython.asm_aarch64
def o8(x0):
    orr(x0, x0, 0xF0F0F0F0F0F0F0F)


@micropython.asm_aarch64
def e8(x0):
    eor(x0, x0, 0xF0F0F0F0F0F0F0F)


@micropython.asm_aarch64
def a9(x0):
    and_(x0, x0, 0x5555555555555555)


@micropython.asm_aarch64
def o9(x0):
    orr(x0, x0, 0x5555555555555555)


@micropython.asm_aarch64
def e9(x0):
    eor(x0, x0, 0x5555555555555555)


@micropython.asm_aarch64
def a10(x0):
    and_(x0, x0, 0xAAAAAAAAAAAAAAAA)


@micropython.asm_aarch64
def o10(x0):
    orr(x0, x0, 0xAAAAAAAAAAAAAAAA)


@micropython.asm_aarch64
def e10(x0):
    eor(x0, x0, 0xAAAAAAAAAAAAAAAA)


@micropython.asm_aarch64
def a11(x0):
    and_(x0, x0, 0x101010101010101)


@micropython.asm_aarch64
def o11(x0):
    orr(x0, x0, 0x101010101010101)


@micropython.asm_aarch64
def e11(x0):
    eor(x0, x0, 0x101010101010101)


@micropython.asm_aarch64
def a12(x0):
    and_(x0, x0, 0xFFFF0000)


@micropython.asm_aarch64
def o12(x0):
    orr(x0, x0, 0xFFFF0000)


@micropython.asm_aarch64
def e12(x0):
    eor(x0, x0, 0xFFFF0000)


@micropython.asm_aarch64
def a13(x0):
    and_(x0, x0, -16)


@micropython.asm_aarch64
def o13(x0):
    orr(x0, x0, -16)


@micropython.asm_aarch64
def e13(x0):
    eor(x0, x0, -16)


@micropython.asm_aarch64
def a14(x0):
    and_(x0, x0, 0x10001)


@micropython.asm_aarch64
def o14(x0):
    orr(x0, x0, 0x10001)


@micropython.asm_aarch64
def e14(x0):
    eor(x0, x0, 0x10001)


@micropython.asm_aarch64
def a15(x0):
    and_(x0, x0, 0xF0F)


@micropython.asm_aarch64
def o15(x0):
    orr(x0, x0, 0xF0F)


@micropython.asm_aarch64
def e15(x0):
    eor(x0, x0, 0xF0F)


@micropython.asm_aarch64
def a16(x0):
    and_(x0, x0, 0x1234)


@micropython.asm_aarch64
def o16(x0):
    orr(x0, x0, 0x1234)


@micropython.asm_aarch64
def e16(x0):
    eor(x0, x0, 0x1234)


@micropython.asm_aarch64
def a17(x0):
    and_(x0, x0, 0xDEADBEEF)


@micropython.asm_aarch64
def o17(x0):
    orr(x0, x0, 0xDEADBEEF)


@micropython.asm_aarch64
def e17(x0):
    eor(x0, x0, 0xDEADBEEF)


@micropython.asm_aarch64
def a18(x0):
    and_(x0, x0, 0x123456789ABCDEF0)


@micropython.asm_aarch64
def o18(x0):
    orr(x0, x0, 0x123456789ABCDEF0)


@micropython.asm_aarch64
def e18(x0):
    eor(x0, x0, 0x123456789ABCDEF0)


@micropython.asm_aarch64
def a19(x0):
    and_(x0, x0, -1)


@micropython.asm_aarch64
def o19(x0):
    orr(x0, x0, -1)


@micropython.asm_aarch64
def e19(x0):
    eor(x0, x0, -1)


@micropython.asm_aarch64
def a20(x0):
    and_(x0, x0, 0xF0F0000FFFF)


@micropython.asm_aarch64
def o20(x0):
    orr(x0, x0, 0xF0F0000FFFF)


@micropython.asm_aarch64
def e20(x0):
    eor(x0, x0, 0xF0F0000FFFF)


# The scratch register must not collide with the source operand.
@micropython.asm_aarch64
def scratch_x9(x0):
    mov(x9, x0)
    and_(x9, x9, 0x10001)
    mov(x0, x9)


@micropython.asm_aarch64
def scratch_x10(x0):
    mov(x10, x0)
    and_(x10, x10, 0x10001)
    mov(x0, x10)


# x9 is the scratch when the source is some other register; a live value
# in x10 must survive.
@micropython.asm_aarch64
def keep_x10(x0):
    mov(x10, 0x5A5A)
    and_(x0, x0, 0x10001)
    mov(x0, x10)


print(hex(0x1), hex(0x0), hex(a0(0x0)), hex(o0(0x0)), hex(e0(0x0)))
print(
    hex(0x1),
    hex(0x123456789ABCDEF0),
    hex(a0(0x123456789ABCDEF0)),
    hex(o0(0x123456789ABCDEF0)),
    hex(e0(0x123456789ABCDEF0)),
)
print(hex(0x1), hex(-0x1), hex(a0(-0x1)), hex(o0(-0x1)), hex(e0(-0x1)))
print(hex(0xF), hex(0x0), hex(a1(0x0)), hex(o1(0x0)), hex(e1(0x0)))
print(
    hex(0xF),
    hex(0x123456789ABCDEF0),
    hex(a1(0x123456789ABCDEF0)),
    hex(o1(0x123456789ABCDEF0)),
    hex(e1(0x123456789ABCDEF0)),
)
print(hex(0xF), hex(-0x1), hex(a1(-0x1)), hex(o1(-0x1)), hex(e1(-0x1)))
print(hex(0xFF), hex(0x0), hex(a2(0x0)), hex(o2(0x0)), hex(e2(0x0)))
print(
    hex(0xFF),
    hex(0x123456789ABCDEF0),
    hex(a2(0x123456789ABCDEF0)),
    hex(o2(0x123456789ABCDEF0)),
    hex(e2(0x123456789ABCDEF0)),
)
print(hex(0xFF), hex(-0x1), hex(a2(-0x1)), hex(o2(-0x1)), hex(e2(-0x1)))
print(hex(0xFFFFFFFF), hex(0x0), hex(a3(0x0)), hex(o3(0x0)), hex(e3(0x0)))
print(
    hex(0xFFFFFFFF),
    hex(0x123456789ABCDEF0),
    hex(a3(0x123456789ABCDEF0)),
    hex(o3(0x123456789ABCDEF0)),
    hex(e3(0x123456789ABCDEF0)),
)
print(hex(0xFFFFFFFF), hex(-0x1), hex(a3(-0x1)), hex(o3(-0x1)), hex(e3(-0x1)))
print(hex(0x7FFFFFFFFFFFFFFF), hex(0x0), hex(a4(0x0)), hex(o4(0x0)), hex(e4(0x0)))
print(
    hex(0x7FFFFFFFFFFFFFFF),
    hex(0x123456789ABCDEF0),
    hex(a4(0x123456789ABCDEF0)),
    hex(o4(0x123456789ABCDEF0)),
    hex(e4(0x123456789ABCDEF0)),
)
print(hex(0x7FFFFFFFFFFFFFFF), hex(-0x1), hex(a4(-0x1)), hex(o4(-0x1)), hex(e4(-0x1)))
print(hex(-0x7FFFFFFFFFFFFFFF), hex(0x0), hex(a5(0x0)), hex(o5(0x0)), hex(e5(0x0)))
print(
    hex(-0x7FFFFFFFFFFFFFFF),
    hex(0x123456789ABCDEF0),
    hex(a5(0x123456789ABCDEF0)),
    hex(o5(0x123456789ABCDEF0)),
    hex(e5(0x123456789ABCDEF0)),
)
print(hex(-0x7FFFFFFFFFFFFFFF), hex(-0x1), hex(a5(-0x1)), hex(o5(-0x1)), hex(e5(-0x1)))
print(hex(-0xFFFFFFFFFFFFFF1), hex(0x0), hex(a6(0x0)), hex(o6(0x0)), hex(e6(0x0)))
print(
    hex(-0xFFFFFFFFFFFFFF1),
    hex(0x123456789ABCDEF0),
    hex(a6(0x123456789ABCDEF0)),
    hex(o6(0x123456789ABCDEF0)),
    hex(e6(0x123456789ABCDEF0)),
)
print(hex(-0xFFFFFFFFFFFFFF1), hex(-0x1), hex(a6(-0x1)), hex(o6(-0x1)), hex(e6(-0x1)))
print(hex(0xFF00FF00FF00FF), hex(0x0), hex(a7(0x0)), hex(o7(0x0)), hex(e7(0x0)))
print(
    hex(0xFF00FF00FF00FF),
    hex(0x123456789ABCDEF0),
    hex(a7(0x123456789ABCDEF0)),
    hex(o7(0x123456789ABCDEF0)),
    hex(e7(0x123456789ABCDEF0)),
)
print(hex(0xFF00FF00FF00FF), hex(-0x1), hex(a7(-0x1)), hex(o7(-0x1)), hex(e7(-0x1)))
print(hex(0xF0F0F0F0F0F0F0F), hex(0x0), hex(a8(0x0)), hex(o8(0x0)), hex(e8(0x0)))
print(
    hex(0xF0F0F0F0F0F0F0F),
    hex(0x123456789ABCDEF0),
    hex(a8(0x123456789ABCDEF0)),
    hex(o8(0x123456789ABCDEF0)),
    hex(e8(0x123456789ABCDEF0)),
)
print(hex(0xF0F0F0F0F0F0F0F), hex(-0x1), hex(a8(-0x1)), hex(o8(-0x1)), hex(e8(-0x1)))
print(hex(0x5555555555555555), hex(0x0), hex(a9(0x0)), hex(o9(0x0)), hex(e9(0x0)))
print(
    hex(0x5555555555555555),
    hex(0x123456789ABCDEF0),
    hex(a9(0x123456789ABCDEF0)),
    hex(o9(0x123456789ABCDEF0)),
    hex(e9(0x123456789ABCDEF0)),
)
print(hex(0x5555555555555555), hex(-0x1), hex(a9(-0x1)), hex(o9(-0x1)), hex(e9(-0x1)))
print(hex(-0x5555555555555556), hex(0x0), hex(a10(0x0)), hex(o10(0x0)), hex(e10(0x0)))
print(
    hex(-0x5555555555555556),
    hex(0x123456789ABCDEF0),
    hex(a10(0x123456789ABCDEF0)),
    hex(o10(0x123456789ABCDEF0)),
    hex(e10(0x123456789ABCDEF0)),
)
print(hex(-0x5555555555555556), hex(-0x1), hex(a10(-0x1)), hex(o10(-0x1)), hex(e10(-0x1)))
print(hex(0x101010101010101), hex(0x0), hex(a11(0x0)), hex(o11(0x0)), hex(e11(0x0)))
print(
    hex(0x101010101010101),
    hex(0x123456789ABCDEF0),
    hex(a11(0x123456789ABCDEF0)),
    hex(o11(0x123456789ABCDEF0)),
    hex(e11(0x123456789ABCDEF0)),
)
print(hex(0x101010101010101), hex(-0x1), hex(a11(-0x1)), hex(o11(-0x1)), hex(e11(-0x1)))
print(hex(0xFFFF0000), hex(0x0), hex(a12(0x0)), hex(o12(0x0)), hex(e12(0x0)))
print(
    hex(0xFFFF0000),
    hex(0x123456789ABCDEF0),
    hex(a12(0x123456789ABCDEF0)),
    hex(o12(0x123456789ABCDEF0)),
    hex(e12(0x123456789ABCDEF0)),
)
print(hex(0xFFFF0000), hex(-0x1), hex(a12(-0x1)), hex(o12(-0x1)), hex(e12(-0x1)))
print(hex(-0x10), hex(0x0), hex(a13(0x0)), hex(o13(0x0)), hex(e13(0x0)))
print(
    hex(-0x10),
    hex(0x123456789ABCDEF0),
    hex(a13(0x123456789ABCDEF0)),
    hex(o13(0x123456789ABCDEF0)),
    hex(e13(0x123456789ABCDEF0)),
)
print(hex(-0x10), hex(-0x1), hex(a13(-0x1)), hex(o13(-0x1)), hex(e13(-0x1)))
print(hex(0x10001), hex(0x0), hex(a14(0x0)), hex(o14(0x0)), hex(e14(0x0)))
print(
    hex(0x10001),
    hex(0x123456789ABCDEF0),
    hex(a14(0x123456789ABCDEF0)),
    hex(o14(0x123456789ABCDEF0)),
    hex(e14(0x123456789ABCDEF0)),
)
print(hex(0x10001), hex(-0x1), hex(a14(-0x1)), hex(o14(-0x1)), hex(e14(-0x1)))
print(hex(0xF0F), hex(0x0), hex(a15(0x0)), hex(o15(0x0)), hex(e15(0x0)))
print(
    hex(0xF0F),
    hex(0x123456789ABCDEF0),
    hex(a15(0x123456789ABCDEF0)),
    hex(o15(0x123456789ABCDEF0)),
    hex(e15(0x123456789ABCDEF0)),
)
print(hex(0xF0F), hex(-0x1), hex(a15(-0x1)), hex(o15(-0x1)), hex(e15(-0x1)))
print(hex(0x1234), hex(0x0), hex(a16(0x0)), hex(o16(0x0)), hex(e16(0x0)))
print(
    hex(0x1234),
    hex(0x123456789ABCDEF0),
    hex(a16(0x123456789ABCDEF0)),
    hex(o16(0x123456789ABCDEF0)),
    hex(e16(0x123456789ABCDEF0)),
)
print(hex(0x1234), hex(-0x1), hex(a16(-0x1)), hex(o16(-0x1)), hex(e16(-0x1)))
print(hex(0xDEADBEEF), hex(0x0), hex(a17(0x0)), hex(o17(0x0)), hex(e17(0x0)))
print(
    hex(0xDEADBEEF),
    hex(0x123456789ABCDEF0),
    hex(a17(0x123456789ABCDEF0)),
    hex(o17(0x123456789ABCDEF0)),
    hex(e17(0x123456789ABCDEF0)),
)
print(hex(0xDEADBEEF), hex(-0x1), hex(a17(-0x1)), hex(o17(-0x1)), hex(e17(-0x1)))
print(hex(0x123456789ABCDEF0), hex(0x0), hex(a18(0x0)), hex(o18(0x0)), hex(e18(0x0)))
print(
    hex(0x123456789ABCDEF0),
    hex(0x123456789ABCDEF0),
    hex(a18(0x123456789ABCDEF0)),
    hex(o18(0x123456789ABCDEF0)),
    hex(e18(0x123456789ABCDEF0)),
)
print(hex(0x123456789ABCDEF0), hex(-0x1), hex(a18(-0x1)), hex(o18(-0x1)), hex(e18(-0x1)))
print(hex(-0x1), hex(0x0), hex(a19(0x0)), hex(o19(0x0)), hex(e19(0x0)))
print(
    hex(-0x1),
    hex(0x123456789ABCDEF0),
    hex(a19(0x123456789ABCDEF0)),
    hex(o19(0x123456789ABCDEF0)),
    hex(e19(0x123456789ABCDEF0)),
)
print(hex(-0x1), hex(-0x1), hex(a19(-0x1)), hex(o19(-0x1)), hex(e19(-0x1)))
print(hex(0xF0F0000FFFF), hex(0x0), hex(a20(0x0)), hex(o20(0x0)), hex(e20(0x0)))
print(
    hex(0xF0F0000FFFF),
    hex(0x123456789ABCDEF0),
    hex(a20(0x123456789ABCDEF0)),
    hex(o20(0x123456789ABCDEF0)),
    hex(e20(0x123456789ABCDEF0)),
)
print(hex(0xF0F0000FFFF), hex(-0x1), hex(a20(-0x1)), hex(o20(-0x1)), hex(e20(-0x1)))

print(hex(scratch_x9(0x1FFFF)))
print(hex(scratch_x10(0x1FFFF)))
print(hex(keep_x10(0xFFFFFFFF)))
