x = [1, 2]

print(x[1])

try:
    print(x[1.0])
except TypeError:
    print("TypeError")
