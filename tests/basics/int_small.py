# This tests small int range for 32-bit machine

# Small ints are variable-length encoded in MicroPython, so first
# test that encoding works as expected.

print(0)
print(1)
print(-1)
# Value is split in 7-bit "subwords", and taking into account that all
# ints in Python are signed, there're 6 bits of magnitude. So, around 2^6
# there's "turning point"
print(63)
print(64)
print(65)
print(-63)
print(-64)
print(-65)
# Maximum values of small ints on 32-bit platform
print(1073741823)
# Per python semantics, lexical integer is without a sign (i.e. positive)
# and '-' is unary minus operation applied to it. That's why -1073741824
# (min two-complement's negative value) is not allowed.
print(-1073741823)

# Operations tests

# compile-time constexprs
print(1 + 3)
print(3 - 2)
print(2 * 3)
print(1 & 3)
print(1 | 2)
print(1 ^ 3)
print(+3)
print(-3)
print(~3)

a = 0x3fffff
print(a)
a *= 0x10
print(a)
a *= 0x10
print(a)
a += 0xff
print(a)
# This would overflow
#a += 1

a = -0x3fffff
print(a)
a *= 0x10
print(a)
a *= 0x10
print(a)
a -= 0xff
print(a)
# This still doesn't overflow
a -= 1
print(a)
# This would overflow
#a -= 1

# negative shifts are not allowed
try:
    a << -1
except ValueError:
    print("ValueError")
try:
    a >> -1
except ValueError:
    print("ValueError")

# Shifts to big amounts are undefined behavior in C and is CPU-specific

# These are compile-time constexprs
print(1 >> 32)
print(1 >> 64)
print(1 >> 128)

# These are runtime calcs
a = 1
print(a >> 32)
print(a >> 64)
print(a >> 128)
