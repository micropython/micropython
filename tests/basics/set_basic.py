# basic sets

s = {1}
print(s)

s = {3, 4, 3, 1}
print(sorted(s))

# expression in constructor
s = {1 + len(s)}
print(s)

# bools mixed with integers
s = {False, True, 0, 1, 2}
print(len(s))

# Sets are not hashable
try:
    {s: 1}
except TypeError:
    print("TypeError")
