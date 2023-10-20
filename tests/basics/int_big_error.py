# test errors operating on bignum

i = 1 << 65

try:
    i << -1
except ValueError:
    print("ValueError")

try:
    i @ 0
except TypeError:
    print("TypeError")

try:
    i @= 0
except TypeError:
    print("TypeError")

try:
    len(i)
except TypeError:
    print("TypeError")

try:
    1 in i
except TypeError:
    print("TypeError")

# overflow because arg of bytearray is being converted to machine int
try:
    bytearray(i)
except OverflowError:
    print('OverflowError')

# to test conversion of negative mpz to machine int
# (we know << will convert to machine int, even though it fails to do the shift)
try:
    i << (-(i >> 40))
except ValueError:
    print('ValueError')

try:
    i // 0
except ZeroDivisionError:
    print('ZeroDivisionError')

try:
    i % 0
except ZeroDivisionError:
    print('ZeroDivisionError')
