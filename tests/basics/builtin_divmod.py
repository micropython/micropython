# test builtin divmod

print(divmod(0, 2))
print(divmod(3, 4))
print(divmod(20, 3))

try:
    divmod(1, 0)
except ZeroDivisionError:
    print("ZeroDivisionError")

try:
    divmod('a', 'b')
except TypeError:
    print("TypeError")
