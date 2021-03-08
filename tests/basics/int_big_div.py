for lhs in (1000000000000000000000000, 10000000000100000000000000, 10012003400000000000000007, 12349083434598210349871029923874109871234789):
    for rhs in range(1, 555):
        print(lhs // rhs)

# these check an edge case on 64-bit machines where two mpz limbs
# are used and the most significant one has the MSB set
x = 0x8000000000000000
print((x + 1) // x)
x = 0x86c60128feff5330
print((x + 1) // x)

# these check edge cases where borrow overflows
print((2 ** 48 - 1) ** 2 // (2 ** 48 - 1))
print((2 ** 256 - 2 ** 32) ** 2 // (2 ** 256 - 2 ** 32))
