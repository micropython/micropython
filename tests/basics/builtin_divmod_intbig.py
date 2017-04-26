# test builtin divmod

try:
    divmod(1 << 65, 0)
except ZeroDivisionError:
    print("ZeroDivisionError")

# bignum
l = (1 << 65) + 123
print(divmod(3, l))
print(divmod(l, 5))
print(divmod(l + 3, l))
print(divmod(l * 20, l + 2))
