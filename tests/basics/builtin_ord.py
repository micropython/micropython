# test builtin ord (whether or not we support unicode)

print(ord('a'))

try:
    ord('')
except TypeError:
    print("TypeError")

