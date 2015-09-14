# basic sets

s = {1}
print(s)

s = {3, 4, 3, 1}
print(sorted(s))

# Sets are not hashable
try:
    {s: 1}
except TypeError:
    print("TypeError")
