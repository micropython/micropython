# check that interned strings are compared against non-interned strings

di = {"key1": "value"}

# lookup interned string
k = "key1"
print(k in di)

# lookup non-interned string
k2 = "key" + "1"
print(k == k2)
print(k2 in di)

# lookup non-interned string
print("".join(['k', 'e', 'y', '1']) in di)
